#ifndef RTSP_INTERNALS_H
#define RTSP_INTERNALS_H

#ifndef _WIN32
// Define constants that are predefined in Winsock
#define INVALID_SOCKET (-1)
#define SOCKET_ERROR (-1)
#endif

// Internally used functions, data types and constant values
void _AddCallback(RTSP& rtsp, void (*f)(void), void* p, uchar index);
bool _SendMessage(RTSP& rtsp, void* p, uchar index, uchar rcv_index);

enum _ElementType
{
    Char,
    Uchar,
    Short,
    Ushort,
    Int,
    Uint,
    Float,
    Double
};

struct _ElementInfo
{
    _ElementType type;
    uint offset;
};

struct _MsgContents
{
    char *ip_addr;
    uint port;
    uint size;
    uint time_tag_offset;
    uint sequence_num_offset;
    uint sender_index_offset;
    uint rcvr_index_offset;
    uint num_elements;
    const _ElementInfo *element_info;
    ushort* cur_sequence_num;
    char *msg_name;
    void *rcv_addr;
    void (*callback)(void);
    int file_descr;
    struct sockaddr_in sock_addr;
};

struct _ControllerInfo
{
    const char* ip_addr;
    uint port;
    int file_descr;
    struct sockaddr_in sock_addr;
};

struct _FederateInfo
{
    const char* name;
    uint dimension;
    uint num_outgoing_msgs;
    const uint* outgoing_msgs;
};

#ifdef _RTSP_DATA_MODIFIABLE
extern uint _NumMsgs, _NumFederateTypes, _MulticastTtl, _NumFederates;
extern uint _MaxMessageSize;
#else
extern const uint _NumMsgs, _NumFederateTypes, _MulticastTtl, _NumFederates;
extern const uint _MaxMessageSize;
#endif

// Controller messages

enum _ControlMsgType
{
    PingCmd,
    PingResponse,
    InitializeCmd,
    InitializeResponse,
    StartCmd,
    StartResponse,
    HaltCmd,
    HaltResponse,
    HaltRequest
};

enum _ControlMsgResult
{
    Success,
    UserRoutineFailure,
    InitFailed_Running,
    StartFailed_Running,
    StartFailed_NoInit,
    HaltFailed_NotRunning
};

const uint _AllFederates = 0xFFFFFFFF;

struct _ControlMsg
{
    uint msg_type;      // enum _MsgType goes here
    uint result;        // enum _ControlMsgResult goes here
    uint fed_id;        // sending or receiving fed ID or _AllFederates
    uint array_index;   // index of sending/receiving fed, 0 if scalar
};

extern _MsgContents _MsgContentsList[];
extern _ControllerInfo _ControllerData[2];
extern _FederateInfo _FederateInfoList[];
extern char _MsgSwapBuf[];
extern bool _FederatePinged[];
extern bool _FederateInitialized[];
extern uint _ResponseTime[];
extern bool _FederateRunning[];
extern bool _FederateHalted[];
extern _ControlMsgResult _FederateMsgResult[];

// Limiting sizes for dynamically loading federation data
const uint _MaxMessages         = 100;
const uint _MaxFederateTypes    = 100;
const uint _MaxFederates        = 1000;
const uint _MessageBufSize      = 10000;

// Wildcard identifier for message receive index
const uchar _AllIndexes         = 255;

#endif
