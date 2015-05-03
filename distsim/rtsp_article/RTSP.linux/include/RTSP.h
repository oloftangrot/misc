#ifndef RTSP_H
#define RTSP_H

// Preprocessor symbols used:
//   Windows NT:        _WIN32
//   Unix:              UNIX
//   VxWorks:           VXWORKS

// Define FD_SETSIZE as the maximum number of incoming sockets for a federate.
#undef FD_SETSIZE
#define FD_SETSIZE 128

#include <stdio.h>

// Windows NT includes:
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#endif

// Unix includes:
#ifdef UNIX
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#ifdef __GNUG__
#else
enum bool { false, true }; // For a compiler that doesn't support bool type
#endif
#endif

// VxWorks/Tornado includes:
#ifdef VXWORKS
#include <VxWorks.h>
#include <selectLib.h>
#include <inetLib.h>
#include <sockLib.h>
#include <ioLib.h>
#include <taskLib.h>
#include <sysLib.h>
#include <intLib.h>
#include <tickLib.h>
#endif

// Definitions for unsigned integer types
typedef unsigned char  uchar;
typedef unsigned short ushort;
typedef unsigned int   uint;

class RTSP
{
public:
    // Possible states for each federate
    enum StateType
    {
        Stopped,
        Initialized,
        Running
    };

    // Constructor for a controller
    RTSP(const char* interface_addr=NULL);

    // Constructor for a federate
    RTSP(const char* fed_name, uint fed_index, const char* interface_addr=NULL);

    // Destructor (closes sockets)
    ~RTSP();

    // Enable logging of received messages to the current directory
    void EnableMessageLogging();

    // Disable logging of received messages
    void DisableMessageLogging();

    // Override the auto-detected byte ordering
    void SetEndianness(bool is_big_endian);

    // Process and call callbacks for any incoming messages that are available
    // then return
    void PollRcv(bool use_timeout, uint timeout_usec=0);

    // Block until a message is received, process it, then block til next message
    void RcvMainLoop(void);

    // Start the timer at zero microseconds counting up
    void StartTimer(void);

    // Stop the timer
    void StopTimer(void);

    // Return the current timer value in microseconds since timer start
    // or zero if the timer hasn't been started or the stop time if
    // the timer has been started and then stopped.
    uint ReadTimer(void);

    // Return the current clock time returning microseconds past the hour
    uint ReadClock(void);

    // Add two time values while accounting for hourly wrap-around
    uint AddTime(uint time1, uint time2);

    // Subtract two time values while accounting for hourly wrap-around
    uint DeltaTime(uint earlier_time, uint later_time);

    // CmdFederation can only be called by the controller
    enum MsgType
    {
        Ping,
        Initialize,
        Start,
        Halt
    };

    bool CmdFederation(MsgType cmd, double timeout_secs=2.0);

    // These can only be called by non-controllers
    void WaitForInit(void);
    void WaitForStart(void);
    void WaitForHalt(void);
    void RequestHalt(void);

    // Display an error message and optionally exit the program
    static void Error(bool abort, const char* msg);

    // Federate Data member access
    bool IsController(void) const { return m_IsController; }
    uchar FederateIndex(void) const { return m_FederateIndex; }

    // Federation Data member access
    bool IsBigEndian(void) { return m_IsBigEndian; }
    const char* InterfaceAddr(void) { return m_InterfaceAddr; }

    // Control message callbacks (non-controllers only)
    void SetPingCallback(bool (*f)(void)) { m_PingCallback = f; }
    void SetInitializeCallback(bool (*f)(void)) { m_InitializeCallback = f; }
    void SetStartCallback(bool (*f)(void)) { m_StartCallback = f; }
    void SetHaltCallback(bool (*f)(void)) { m_HaltCallback = f; }

    // Federate and federation state
    StateType FederateState();
    StateType FederationState();

    // Information about lost messages
    struct LostMsgRecord
    {
        uint msg_index;
        uchar send_index;
        uchar rcv_index;
        ushort lost_seq_start;
        ushort lost_seq_end;
        uint time_tag;
    };

    enum { m_LostMsgLogSize = 50 };
    uint NumLostMsgs() { return m_NumLostMsgs; }
    LostMsgRecord GetLostMsgRecord(uint index)
    { index = (index < m_LostMsgLogSize) ? index : m_LostMsgLogSize-1;
      return m_LostMsg[index]; }

    // Header added to each message going into a log file
    struct MsgLogHeader
    {
        ushort rec_size;    // total size of header + message in bytes
        ushort port;        // IP port number for this message
        uint log_time_tag;  // Time tag when message received
    };

    // Used by static function to set up select() socket set
    void AddSocketToSet(uint fd) { FD_SET(fd, &m_SocketSet); }

protected:
    void Setup(const char* fed_name, uint fed_index, 
               const char* interface_addr);
    char* GetLogFileName();

    bool m_IsController, m_TimerStarted;
    uint m_FederateNum;
    uchar m_FederateIndex;
    enum StateType m_State, m_FederationState;

    uint m_NumLostMsgs;
    LostMsgRecord m_LostMsg[m_LostMsgLogSize];

    bool (*m_PingCallback)(void), (*m_InitializeCallback)(void),
        (*m_StartCallback)(void), (*m_HaltCallback)(void);

    fd_set m_SocketSet;

    bool m_LogMsgs;
    FILE* m_MsgLogIov;

    uint m_TimerStartValue;

    bool m_IsBigEndian;
    char m_InterfaceAddr[40];

};

#endif
