%{
/*
 * Parser for the RTSP Message Definition File (MDF) format
 *
 */

#include <Parser.h>
#include <Element.h>
#include <Struct.h>
#include <Federate.h>
#include <Message.h>

#include "RTSP.tab.h"

#include <string>
#include <stack>
#include <map>
#include <cctype>

int LineNumber;
std::string CurFileName;

void c_comment();
void cpp_comment();
void yyerror(std::string s);

extern ConstMapType ConstMap;

#define YY_NEVER_INTERACTIVE 1
%}

letter              [a-zA-Z]
digit               [0-9]
underscore          "_"

identifier          {letter}({letter}|{digit}|{underscore})*
string_literal      (\"([^\"])*\")

integer             {digit}+
c_comment           "/*"
cpp_comment         "//"

%%

[\n\f\v]                { LineNumber++; }
[ \t\r]                 ;
{c_comment}             { c_comment(); }
{cpp_comment}           { cpp_comment(); }

multicast_base_addr     { return MULTICAST_BASE; }
multicast_base_port     { return MULTICAST_PORT; }
multicast_time_to_live  { return MULTICAST_TTL; }
constant                { return CONSTANT; }
federate                { return FEDERATE; }
struct                  { return STRUCT; }
message                 { return MESSAGE; }
subscribe               { return SUBSCRIBE; }

char                    { yylval.basic_type = Element::Char;
                          return BASIC_TYPE; }
uchar                   { yylval.basic_type = Element::Uchar;
                          return BASIC_TYPE; }
short                   { yylval.basic_type = Element::Short;
                          return BASIC_TYPE; }
ushort                  { yylval.basic_type = Element::Ushort;
                          return BASIC_TYPE; }
int                     { yylval.basic_type = Element::Int;
                          return BASIC_TYPE; }
uint                    { yylval.basic_type = Element::Uint;
                          return BASIC_TYPE; }
float                   { yylval.basic_type = Element::Float;
                          return BASIC_TYPE; }
double                  { yylval.basic_type = Element::Double;
                          return BASIC_TYPE; }

#include                { return INCLUDE; }
{string_literal}        { yylval.string_val = new std::string(yytext);
                          return STRING; 
                        }

{integer}               { yylval.int_val = atoi(yytext);
                          return NUMBER; 
                        }

{identifier}            { ConstMapType::iterator ci = ConstMap.find(yytext);
                          if (ci != ConstMap.end())
                          {
                              yylval.int_val = (*ci).second;
                              return NUMBER;
                          }

                          StructMapType::iterator si = StructMap.find(yytext);
                          if (si != StructMap.end())
                          {
                              yylval.struct_type = (*si).second;
                              return STRUCT_NAME;
                          }

                          FederateMapType::iterator fi = FederateMap.find(yytext);
                          if (fi != FederateMap.end())
                          {
                              yylval.federate_type = (*fi).second;
                              return FEDERATE_NAME;
                          }

                          yylval.string_val = new std::string(yytext);
                          return IDENTIFIER; 
                        }

.                       { return *yytext; }
%%

struct include_info
{
    YY_BUFFER_STATE include_buf;
    int line_num;
    std::string file_name;
};

bool operator<(const include_info& left, const include_info& right)
{
    return (left.include_buf < right.include_buf);
}

bool operator==(const include_info& left, const include_info& right)
{
    return (left.include_buf == right.include_buf);
}

static std::stack<include_info> include_stack;

void enter_include_file(std::string file_name)
{
    // copy file name and remove quotes from each end
    std::string new_file = std::string(file_name, 1, file_name.size()-2);

    // open the new file
    yyin = fopen(new_file.c_str(), "r");
    if (!yyin)
    {
        char buf[250];
        sprintf(buf, "can't open include file \"%s\"", new_file.c_str());
        yyerror(buf);
        return;
    }

    // push current state onto include stack
    include_info info;
    info.include_buf = YY_CURRENT_BUFFER;
    info.line_num = LineNumber;
    info.file_name = CurFileName;
    include_stack.push(info);

    CurFileName = new_file;

    yy_switch_to_buffer(yy_create_buffer(yyin, YY_BUF_SIZE));

    LineNumber = 1;
    BEGIN(INITIAL);
}

int yywrap(void)
{
    if (include_stack.size() == 0)
        return 1;
    else
    {
        yy_delete_buffer( YY_CURRENT_BUFFER );

        include_info info = include_stack.top();
        include_stack.pop();
        yy_switch_to_buffer(info.include_buf);
        LineNumber = info.line_num;
        CurFileName = info.file_name;
    
        return 0;
    }
}

void yyerror(std::string s) {
    extern int yychar;

    ErrorCount++;

    char buf[500];
    sprintf(buf,"\nERROR: %s on line %d of \"%s\"\n", s.c_str(),
         LineNumber, CurFileName.c_str());

    if (strlen(yytext) > 0)
    {
        char buf2[200];
        sprintf(buf2, "Offending text: %s\n", yytext);
        strcat(buf, buf2);
    }

    ErrorMsg(buf);
}

void c_comment() {
    int c = '\0', prev_c;

    do {
        prev_c = c;
        c = yyinput();

        if (c == '\n')
            LineNumber++;
    } while ((prev_c != '*' || c != '/') && c != EOF);

    if (c == EOF)
        yyerror("unterminated comment");

    return;
}

void cpp_comment() {
    int c;

    do {
        c = yyinput();
    } while (c != '\n' && c != EOF);

    LineNumber++;
    return;
}
