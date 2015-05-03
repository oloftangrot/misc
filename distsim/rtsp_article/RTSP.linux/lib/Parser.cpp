#include <RTSP.h>
#define _RTSP_DATA_MODIFIABLE
#include <RTSP_Internals.h>

#include "Parser.h"

#include <time.h>
#include <assert.h>

struct in_addr IP_BaseAddr;
uint IP_BasePort, MulticastTtl, ErrorCount;
FederateMapType FederateMap;
StructMapType StructMap;
std::string MDF_File;
void (*ErrorMsg)(const char* msg);

static const char* Default_IP_BaseAddr = "225.0.0.0";
static const uint  Default_IP_BasePort = 20000;

static void default_error_msg(const char* msg)
{
    fprintf(stderr, "%s", msg);
}

void ParseMDFFile(std::string file_name,
                 void (*error_msg_func)(const char* msg))
{
    extern int LineNumber;
    extern int yyparse();
    extern std::string CurFileName;
    extern ConstMapType ConstMap;
    FILE *flptr;

    if (error_msg_func)
        ErrorMsg = error_msg_func;
    else
        ErrorMsg = default_error_msg;

    Struct::Initialize();
    Message::Initialize();
    Federate::Initialize();

    ConstMap.clear();

    MDF_File = file_name;

    if ((flptr = freopen(file_name.c_str(), "r", stdin)) == NULL) {
        char buf[200];
        sprintf(buf, "ERROR: Can't open %s for input\n", file_name.c_str());
        ErrorMsg(buf);
        return;
    }

    ErrorCount = 0;
    LineNumber = 1;
    CurFileName = file_name;
    yyparse();

    if (ErrorCount > 0) {
        char buf[200];
        sprintf(buf, "%d error%s detected\n", ErrorCount,
            ErrorCount == 1? "": "s");
        ErrorMsg(buf);
        return;
    }
}

void GenerateCppCode( void )
{
    if (IP_BaseAddr.s_addr == 0)
        IP_BaseAddr.s_addr = inet_addr(Default_IP_BaseAddr);

    if (IP_BasePort == 0)
        IP_BasePort = Default_IP_BasePort;

    FederateMapType::iterator it = FederateMap.begin();
    for (; it != FederateMap.end(); it++)
    {
        std::string fed_name = (*it).first;
        Federate *fed = (*it).second;

        if (fed->MessageMap().size() > 0)
        {
            std::string file = fed_name + "Send.h";
            printf("Generating %s\n", file.c_str());

            FILE *iov = fopen(file.c_str(), "w");
            assert(iov);
            fed->GenerateCppCode(iov, true);
            fclose(iov);
        }

        if (fed->SubscriptionMap().size() > 0)
        {
            std::string file = fed_name + "Rcv.h";
            printf("Generating %s\n", file.c_str());

            FILE *iov = fopen(file.c_str(), "w");
            assert(iov);
            fed->GenerateCppCode(iov, false);
            fclose(iov);
        }
    }

    printf("Generating _MsgData.cpp\n");
    FILE *iov = fopen("_MsgData.cpp", "w");
    assert(iov);

    // get date and time of code gen and remove trailing '\n'.
    time_t long_time;
    time(&long_time);
    char *cp = asctime(localtime(&long_time));
    cp[strlen(cp) - 1] = '\0';

    // print file header
    fprintf(iov, "// Message internal definitions\n");
    fprintf(iov, "// Generated %s from %s\n", cp, MDF_File.c_str());
    fprintf(iov, "// *** This file was automatically generated. "
        "Do not edit it! ***\n\n");
    fprintf(iov, "#include <RTSP.h>\n#include <RTSP_Internals.h>\n\n");

    fprintf(iov, "const uint _NumMsgs = %d;\n", Message::NumMsgs());
    fprintf(iov, "const uint _NumFederateTypes = %lu;\n", FederateMap.size());
    fprintf(iov, "const uint _MulticastTtl = %d;\n", MulticastTtl);

    int num_feds = 0;
    FederateMapType::const_iterator fi = FederateMap.begin();
    for (; fi != FederateMap.end(); fi++)
        num_feds += (*fi).second->Dimension();

    fprintf(iov, "const uint _NumFederates = %d;\n", num_feds);

    fprintf(iov, "const uint _MaxMessageSize = %d;\n\n",
        Message::MaxMessageSize());

    const MessageVecType& msg_vec = Message::MessageVec();

    MessageVecType::const_iterator mi = msg_vec.begin();
    for (int i=0; mi != msg_vec.end(); mi++, i++)
    {
        (*mi)->OutputElementInfo(iov, i);
        fprintf(iov, "\n");
    }

    mi = msg_vec.begin();
    for (int i=0; mi != msg_vec.end(); mi++, i++)
        fprintf(iov, "ushort _seq%d[_NumFederates];\n", i);

    fprintf(iov, "\n_MsgContents _MsgContentsList[_NumMsgs] =\n{\n");

    mi = msg_vec.begin();
    for (int i=0; mi != msg_vec.end(); mi++, i++)
    {
        (*mi)->OutputMessageInfo(iov, i);
    }

    fprintf(iov, "};\n\n");

    fi = FederateMap.begin();
    for (int i=0; fi != FederateMap.end(); fi++, i++)
    {
        (*fi).second->OutputOutgoingMsgInfo(iov, i);
    }

    fprintf(iov,
        "\n_FederateInfo _FederateInfoList[_NumFederateTypes] =\n{\n");

    fi = FederateMap.begin();
    for (int i=0; fi != FederateMap.end(); fi++, i++)
    {
        (*fi).second->OutputFederateInfo(iov, i);
    }

    fprintf(iov, "};\n");

    fprintf(iov, "\n_ControllerInfo _ControllerData[2] =\n{\n");
    uint ip_addr = ntohl(IP_BaseAddr.s_addr);
    struct in_addr net_addr;
    net_addr.s_addr = htonl(ip_addr);
    fprintf(iov, "    {\"%s\", %3d}, // Controller output IP addr and port\n",
        inet_ntoa(net_addr), IP_BasePort);

    ip_addr++;
    net_addr.s_addr = htonl(ip_addr);
    fprintf(iov, "    {\"%s\", %3d}  // Controller input IP addr and port\n",
        inet_ntoa(net_addr), IP_BasePort+1);
    fprintf(iov, "};\n\n");

    fprintf(iov, "char _MsgSwapBuf[_MaxMessageSize];\n");
    fprintf(iov, "bool _FederatePinged[_NumFederates];\n");
    fprintf(iov, "bool _FederateInitialized[_NumFederates];\n");
    fprintf(iov, "uint _ResponseTime[_NumFederates];\n");
    fprintf(iov, "bool _FederateRunning[_NumFederates];\n");
    fprintf(iov, "bool _FederateHalted[_NumFederates];\n");
	fprintf(iov, "_ControlMsgResult _FederateMsgResult[_NumFederates];\n");

    fclose(iov);
}

void GenerateMDFData( void )
{
	// Generate constants
    if (IP_BaseAddr.s_addr == 0)
        IP_BaseAddr.s_addr = inet_addr(Default_IP_BaseAddr);

    if (IP_BasePort == 0)
        IP_BasePort = Default_IP_BasePort;

    _NumMsgs = Message::NumMsgs();
    _NumFederateTypes = FederateMap.size();
    _MulticastTtl = MulticastTtl;

    int num_feds = 0;
    FederateMapType::const_iterator fi = FederateMap.begin();
    for (; fi != FederateMap.end(); fi++)
        num_feds += (*fi).second->Dimension();

    _NumFederates = num_feds;

    _MaxMessageSize = Message::MaxMessageSize();

	// Generate message contents
    const MessageVecType& msg_vec = Message::MessageVec();

    MessageVecType::const_iterator mi = msg_vec.begin();
    for (int i=0; mi != msg_vec.end(); mi++, i++)
		_MsgContentsList[i] = (*mi)->GenerateMessageData(i, num_feds);

	// Generate federate description data
    fi = FederateMap.begin();
    for (int i=0; fi != FederateMap.end(); fi++, i++)
        (*fi).second->GenerateFederateData(i);

	// Generate controller data
    uint ip_addr = ntohl(IP_BaseAddr.s_addr);
    for (int i=0; i<2; i++)
    {
        char* cp = const_cast<char*>(_ControllerData[i].ip_addr);
        if (cp)
            delete[] cp;

        struct in_addr net_addr;
        net_addr.s_addr = htonl(ip_addr);

        char* sp = inet_ntoa(net_addr);
        cp = new char[strlen(sp)+1];
        strcpy(cp, sp);

        _ControllerData[i].ip_addr = cp;
        _ControllerData[i].port = IP_BasePort+i;

        ip_addr++;
    }
}
