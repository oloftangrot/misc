/* YACC grammar for the RTSP Message Definition File (MDF) format */

%token INCLUDE STRING IDENTIFIER NUMBER BASIC_TYPE MULTICAST_BASE
%token MULTICAST_PORT MULTICAST_TTL CONSTANT FEDERATE STRUCT MESSAGE
%token SUBSCRIBE STRUCT_NAME FEDERATE_NAME MESSAGE_NAME

%type <int_val>         NUMBER
%type <string_val>      STRING IDENTIFIER
%type <basic_type>      BASIC_TYPE
%type <struct_type>     STRUCT_NAME
%type <federate_type>   FEDERATE_NAME
%type <message_type>    MESSAGE_NAME

%{
#include <Parser.h>
#include <Element.h>
#include <Struct.h>
#include <Message.h>

#include <string>
#include <map>
#include <vector>

int yylex(void);
void yyerror(std::string s);
void enter_include_file(std::string file_name);

ConstMapType ConstMap;

static MessageMapType CurSubscriptionMap;
static Struct* CurStruct = 0;
static Element::ElType CurElType = Element::Undefined;
static Element* CurElement = new Element;
static ElementMapType* CurElementMap = new ElementMapType();
static Federate* CurFederate = new Federate;
static std::vector<uint> MsgDimensions;

%}

%union {
    std::string* string_val;
    uint int_val;
    Element::ElType basic_type;
    Struct* struct_type;
    Federate* federate_type;
    Message* message_type;
}

%%

stmt_list :
      stmt
    | stmt_list stmt
    ;

stmt :
      preprocessor_stmt
    | const_decl
    | multicast_stmt
    | multicast_port_stmt
    | multicast_ttl_stmt
    | struct_decl
    | message_decl
    | federate_decl
    | subscribe_decl
    ;

const_assign :
      IDENTIFIER '=' NUMBER
      { ConstMap[*$1] = $3; 
        delete $1; }
    ;

const_assign_list :
      const_assign
    | const_assign_list ',' const_assign
    ;

const_decl :
      CONSTANT const_assign_list ';'
    ;

multicast_stmt :
      MULTICAST_BASE '=' NUMBER '.' NUMBER '.' NUMBER '.' NUMBER ';'
      { 
        if (($3 < 225) || ($3 > 239) || ($5 > 255) || ($7 > 255) || ($9 > 255))
            yyerror("Invalid Multicast group base address\n"
            "(must be in the range 225.0.0.0 to 239.255.255.255)");
        else
            IP_BaseAddr.s_addr =
                htonl(($3 << 24) | ($5 << 16) | ($7 << 8) | ($9));
      }
    ;

multicast_port_stmt :
      MULTICAST_PORT '=' NUMBER ';'
      { IP_BasePort = $3; }
    ;

multicast_ttl_stmt :
      MULTICAST_TTL '=' NUMBER ';'
      { MulticastTtl = $3; }
    ;

elem_index_list :
      /* empty */
    | elem_index_list '[' NUMBER ']'
      { CurElement->AddDimension($3); }
    ;

elem_name :
      IDENTIFIER elem_index_list
      { CurElement->SetName(*$1);
        CurElement->SetType(CurElType);
        if (CurElType == Element::Struct)
            CurElement->SetStruct(CurStruct);

        CurElementMap->insert(ElementMapType::value_type(*$1, CurElement));

        CurElement = new Element(Element::Undefined);
        delete $1; 
      }
    ;

elem_list :
      elem_name
    | elem_list ',' elem_name
    ;

elem_type :
      BASIC_TYPE
      { CurElType = $1; }
    | STRUCT_NAME
      { CurStruct = $1;
        CurElType = Element::Struct; }
    ;

elem_decl :
      elem_type elem_list ';'
    ;

decl_list :
      /* empty */
    | decl_list elem_decl
    ;

struct_decl :
      STRUCT IDENTIFIER '{' decl_list '}' ';'
      { Struct *s = new Struct(CurElementMap);
        s->SetName(*$2);
        CurElementMap = new ElementMapType;
        StructMap[*$2] = s;
        delete $2;
      }
    ;

message_decl :
      MESSAGE FEDERATE_NAME '.' IDENTIFIER '{' decl_list '}' ';'
      { Element* e = new Element();
        e->SetName("_TimeTag");
        e->SetType(Element::Uint);
        CurElementMap->insert(ElementMapType::value_type(e->Name(), e));

        e = new Element();
        e->SetName("_SequenceNum");
        e->SetType(Element::Ushort);
        CurElementMap->insert(ElementMapType::value_type(e->Name(), e));
        
        e = new Element();
        e->SetName("_SenderIndex");
        e->SetType(Element::Uchar);
        CurElementMap->insert(ElementMapType::value_type(e->Name(), e));
        
        e = new Element();
        e->SetName("_ReceiverIndex");
        e->SetType(Element::Uchar);
        CurElementMap->insert(ElementMapType::value_type(e->Name(), e));
        
        Message* m = new Message(CurElementMap);
        m->SetOwner($2);
        m->SetName(*$4);
        $2->AddMessage(*$4, m);

        MsgDimensions.clear();
        CurElementMap = new ElementMapType;
        delete $4;
      }
    ;

fed_index :
      /* empty */
    | '[' NUMBER ']'
      { CurFederate->SetDimension($2); }
    ;

fed_name :
      IDENTIFIER fed_index
      { FederateMap[*$1] = CurFederate;
        CurFederate->SetName(*$1);
        CurFederate = new Federate; 
        delete $1;
      }
    ;

fed_list :
      fed_name
    | fed_list ',' fed_name
    ;

federate_decl :
      FEDERATE fed_list ';'
    ;

msg_name :
      FEDERATE_NAME '.' IDENTIFIER
      { Message *m = $1->FindMessage(*$3);
        if (!m)
            yyerror("invalid message name");
        else
            CurSubscriptionMap[$1->Name()+"."+*$3] = m;
      }
    | FEDERATE_NAME '.' '*'
      { const MessageMapType& map = $1->MessageMap();
        MessageMapType::const_iterator it = map.begin();
        for (; it != map.end(); it++)
            CurSubscriptionMap[$1->Name()+"."+(*it).first] = (*it).second;
      }
    ;

msg_list :
      msg_name
    | msg_list ',' msg_name
    | '*' '.' '*'
      { FederateMapType::iterator fit = FederateMap.begin();
        for (; fit != FederateMap.end(); fit++)
        {
            Federate *f = (*fit).second;
            const MessageMapType& map = f->MessageMap();
            MessageMapType::const_iterator it = map.begin();
            for (; it != map.end(); it++)
                CurSubscriptionMap[f->Name()+"."+(*it).first] = (*it).second;
        }
      }
    ;
      
subscribe_decl :
      SUBSCRIBE FEDERATE_NAME ':' msg_list ';'
      { $2->SetSubscriptionMap(CurSubscriptionMap);
        CurSubscriptionMap.clear();
      }
    ;

preprocessor_stmt :
      INCLUDE STRING
      { enter_include_file(*$2);
        delete $2;
      }
    ;
%%
