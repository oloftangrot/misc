#include <RTSP.h>
#include <RTSP_Internals.h>

#include <stdio.h>
#include <time.h>
#include <assert.h>

static const uint usecPerHour = 60u * 60u * 1000000u;

static inline void Swap2Byte(void *p)
{
    uchar* cp = (uchar*) p;

    uchar tmp = cp[0];
    cp[0] = cp[1];
    cp[1] = tmp;
}

static inline void Swap4Byte(void *p)
{
    uchar* cp = (uchar*) p;

    uchar tmp = cp[0];
    cp[0] = cp[3];
    cp[3] = tmp;

    tmp   = cp[1];
    cp[1] = cp[2];
    cp[2] = tmp;
}

static inline void Swap8Byte(void *p)
{
    uchar* cp = (uchar*) p;

    uchar tmp = cp[0];
    cp[0] = cp[7];
    cp[7] = tmp;

    tmp   = cp[1];
    cp[1] = cp[6];
    cp[6] = tmp;

    tmp   = cp[2];
    cp[2] = cp[5];
    cp[5] = tmp;

    tmp   = cp[3];
    cp[3] = cp[4];
    cp[4] = tmp;
}

static inline void SendControlMsg(_ControlMsg msg, bool is_controller,
                                  bool is_big_endian)
{
    if (!is_big_endian)
    {
        Swap4Byte(&msg.msg_type);
        Swap4Byte(&msg.result);
        Swap4Byte(&msg.fed_id);
        Swap4Byte(&msg.array_index);
    }

    int send_index = is_controller ? 0 : 1;
    int fd = _ControllerData[send_index].file_descr;
    struct sockaddr_in addr = _ControllerData[send_index].sock_addr;

    int result = sendto(fd, (char*) &msg, sizeof(msg), 0,
        (struct sockaddr *) &addr, sizeof(struct sockaddr_in));
    assert(result >= 0);
}

static inline void ReadControlMsg(_ControlMsg& msg, bool is_controller,
                                  bool is_big_endian)
{
    int rcv_index = is_controller ? 1 : 0;
    int fd = _ControllerData[rcv_index].file_descr;
    struct sockaddr_in addr = _ControllerData[rcv_index].sock_addr;

    socklen_t addrlen = sizeof(struct sockaddr_in);
    int result = recvfrom(fd, (char*) &msg, sizeof(msg), 0,
        (struct sockaddr *) &addr, &addrlen);

    assert(result == sizeof(msg));

    if (!is_big_endian)
    {
        Swap4Byte(&msg.msg_type);
        Swap4Byte(&msg.result);
        Swap4Byte(&msg.fed_id);
        Swap4Byte(&msg.array_index);
    }
}

// Wait for a message to become available. Returns true if one is
// received within timeout_secs. Set timeout_secs to zero to see if any
// messages are queued and immediately available. Return value is the
// number of messages ready for reading.
static inline uint AwaitMsg(bool use_timeout, uint timeout_usecs,
                            const fd_set& wait_set, fd_set& ready_set)
{
    ready_set = wait_set;

    timeval* tvp = NULL;
    timeval timeout;

    if (use_timeout)
    {
        timeout.tv_sec = long(timeout_usecs / 1e6);
        timeout.tv_usec = long(timeout_usecs - timeout.tv_sec*1e6);
        tvp = &timeout;
    }

    int result = select(FD_SETSIZE, &ready_set, 0, 0, tvp);
    assert(result != SOCKET_ERROR);

    return result;
}

void RTSP::Setup(const char* fed_name, uint fed_index, 
                 const char* interface_addr)
{
    // verify appropriate sizes for message data types
    assert(sizeof(char)   == 1);
    assert(sizeof(uchar)  == 1);
    assert(sizeof(short)  == 2);
    assert(sizeof(ushort) == 2);
    assert(sizeof(int)    == 4);
    assert(sizeof(uint)   == 4);
    assert(sizeof(float)  == 4);
    assert(sizeof(double) == 8);

#ifdef _WIN32
    // initialize winsock
    const WORD winsock_version = MAKEWORD(2, 0); // winsock v2.0 required
    WSADATA wsaData;    // winsock implementation details

    if (WSAStartup(winsock_version, &wsaData))
        RTSP::Error(true, "ERROR: Could not start Winsock");
#endif

    // determine machine endianness
    union
    {
        unsigned char c[4];
        unsigned long l;
    } endian_chk;

    endian_chk.l = 0;
    endian_chk.c[0] = 1;

    m_IsBigEndian = (endian_chk.l != 1) ? true : false;

    if (interface_addr)
        strcpy(m_InterfaceAddr, interface_addr);
    else
        m_InterfaceAddr[0] = '\0';

    m_State = Stopped;
    m_FederationState = Stopped;

    m_PingCallback          = 0;
    m_InitializeCallback    = 0;
    m_StartCallback         = 0;
    m_HaltCallback          = 0;

    m_LogMsgs = false;
    m_TimerStarted = false;
    m_TimerStartValue = 0;

    FD_ZERO(&m_SocketSet);

    if (!m_IsController)
    {
		uint i;
        // Find federate information by searching for a matching name
        for (i=0; i<_NumFederateTypes; i++)
            if (strcmp(fed_name, _FederateInfoList[i].name) == 0)
            {
                m_FederateNum = i;
                break;
            }

        if ( i == _NumFederateTypes)
        {
            char buf[200];
            sprintf(buf, "ERROR in RTSP::Setup: Invalid federate name: %s",
                fed_name);
            Error(true, buf);
        }

        if (fed_index >= _FederateInfoList[m_FederateNum].dimension)
            Error(true, "ERROR in RTSP::Setup: fed_index parameter "
                "is too large");

        m_FederateIndex = fed_index;

        // Set up all outgoing sockets
        for (uint i=0; i<_FederateInfoList[m_FederateNum].num_outgoing_msgs; i++)
        {
            // Create an ordinary UDP socket
            int fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
            if (fd == INVALID_SOCKET)
                Error(true, "ERROR: socket call failed");

            // set up destination address
            struct sockaddr_in addr;
            memset(&addr, 0, sizeof(addr));
            addr.sin_family = AF_INET;
            uint msg_index = _FederateInfoList[m_FederateNum].outgoing_msgs[i];
            addr.sin_addr.s_addr = inet_addr(_MsgContentsList[msg_index].ip_addr);
            addr.sin_port = htons(_MsgContentsList[msg_index].port);

            _MsgContentsList[msg_index].file_descr = fd;
            _MsgContentsList[msg_index].sock_addr = addr;

            // Allow multiple federates to use this same address
            int value = 1;
            int result = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR,
                (char*) &value, sizeof(int));
            assert(result == 0);

            // Call bind so TTL can be set
            if (strlen(m_InterfaceAddr) > 0)
                addr.sin_addr.s_addr = inet_addr(m_InterfaceAddr);
            else
                addr.sin_addr.s_addr = htonl(INADDR_ANY);

            result = bind(fd, (struct sockaddr *) &addr, sizeof(addr));
            if (result < 0)
                Error(true, "ERROR: bind call failed");

            // Set multicast time-to-live
#ifdef _WIN32
            uint ttl = _MulticastTtl;
#else
            uchar ttl = _MulticastTtl;
#endif

            result = setsockopt(fd, IPPROTO_IP, IP_MULTICAST_TTL,
                (char*) &ttl, sizeof(ttl));

            assert(result == 0);
        }
    }

    // Set up sockets for controller communication
    for (uint i=0; i<2; i++)
    {
        // Create an ordinary UDP socket
        int fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
        if (fd == INVALID_SOCKET)
            Error(true, "ERROR: socket call failed");

        // set up destination address
        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = inet_addr(_ControllerData[i].ip_addr);
        addr.sin_port = htons(_ControllerData[i].port);

        _ControllerData[i].file_descr = fd;
        _ControllerData[i].sock_addr = addr;

        uint send_index = m_IsController ? 0 : 1;
        if (i == send_index)
        {
            // Allow multiple federates to use this same address
            int value = 1;
            int result = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR,
                (char*) &value, sizeof(int));
            assert(result == 0);

            // Call bind so TTL can be set
            if (strlen(m_InterfaceAddr) > 0)
                addr.sin_addr.s_addr = inet_addr(m_InterfaceAddr);
            else
                addr.sin_addr.s_addr = htonl(INADDR_ANY);

            result = bind(fd, (struct sockaddr *) &addr, sizeof(addr));
            if (result < 0)
                Error(true, "ERROR: bind call failed");

            // Set multicast time-to-live
#ifdef _WIN32
            uint ttl = _MulticastTtl;
#else
            uchar ttl = _MulticastTtl;
#endif

            result = setsockopt(fd, IPPROTO_IP, IP_MULTICAST_TTL,
                (char*) &ttl, sizeof(ttl));

            assert(result == 0);
        }
    }

    // Set up to receive controller messages (either as controller or not)
    int rcv_index = m_IsController ? 1 : 0;
    int fd = _ControllerData[rcv_index].file_descr;

    // set up destination address
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;

    if (strlen(m_InterfaceAddr) > 0)
        addr.sin_addr.s_addr = inet_addr(m_InterfaceAddr);
    else
        addr.sin_addr.s_addr = htonl(INADDR_ANY);

    addr.sin_port = htons(_ControllerData[rcv_index].port);

    // Allow multiple federates to receive control messages
    int value = 1;
    int result = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char*) &value,
        sizeof(int));
    assert(result == 0);

    // bind to receive address
    if (bind(fd, (struct sockaddr *) &addr, sizeof(addr)) < 0)
        Error(true, "ERROR: bind call failed");

    // request to join a multicast group
    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = inet_addr(_ControllerData[rcv_index].ip_addr);

    if (strlen(m_InterfaceAddr) > 0)
        mreq.imr_interface.s_addr = inet_addr(m_InterfaceAddr);
    else
        mreq.imr_interface.s_addr = htonl(INADDR_ANY);

    if (setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&mreq,
        sizeof(mreq)) < 0)
    {
        char buf[200];
        sprintf(buf, "ERROR: unable to join multicast group \"%s\"\n",
            _ControllerData[rcv_index].ip_addr);
        Error(true, buf);
    }

    // Insert the controller receive socket into the set
    AddSocketToSet(fd);
}

RTSP::RTSP(const char* interface_addr)
{
    m_IsController = true;
    Setup(NULL, 0, interface_addr);
}

RTSP::RTSP(const char* fed_name, uint fed_index, const char* interface_addr)
{
    m_IsController = false;
    Setup(fed_name, fed_index, interface_addr);
}

RTSP::~RTSP()
{
    // Close all open incoming and outgoing sockets
	uint i;
    for (i=0; i<_NumMsgs; i++)
        if (_MsgContentsList[i].file_descr)
        {
#ifdef _WIN32
            int result = closesocket(_MsgContentsList[i].file_descr);
#else
            int result = close(_MsgContentsList[i].file_descr);
#endif

            assert(result == 0);
            _MsgContentsList[i].file_descr = 0;
        }

    // Close open sockets for controller communication
    for (i=0; i<2; i++)
        if (_ControllerData[i].file_descr)
        {
#ifdef _WIN32
            int result = closesocket(_MsgContentsList[i].file_descr);
#else
            int result = close(_MsgContentsList[i].file_descr);
#endif
            assert(result == 0);
            _ControllerData[i].file_descr = 0;
        }
}

void RTSP::EnableMessageLogging()
{
    m_MsgLogIov = fopen(GetLogFileName(), "wb");
    assert(m_MsgLogIov);

    m_LogMsgs = true;
}

void RTSP::DisableMessageLogging()
{
    m_LogMsgs = false;
}

char* RTSP::GetLogFileName()
{
    time_t timer;
    time(&timer);
    tm* t = localtime(&timer);

    static char buf[50];
    sprintf(buf, "%02d%02d%02d-%02d%02d%02d.log",
        (t->tm_year) % 100, t->tm_mon+1, t->tm_mday,
        t->tm_hour, t->tm_min, t->tm_sec);

    return buf;
}

void RTSP::SetEndianness(bool is_big_endian)
{
    m_IsBigEndian = is_big_endian;
}

bool RTSP::CmdFederation(MsgType cmd, double timeout_secs)
{
    if (!m_IsController)
    {
        Error(false, "ERROR in RTSP::StartRun: Non-controller "
        "cannot command the federation");
        return false;
    }

    // Set up a message to send out
    _ControlMsg msg;
    msg.fed_id = _AllFederates;
    msg.array_index = 0;
    msg.result = Success;
    _ControlMsgType resp_type;
    bool* status_array;

    switch(cmd)
    {
    case Ping:
        msg.msg_type = PingCmd;
        resp_type = PingResponse;
        status_array = &_FederatePinged[0];
        break;

    case Initialize:
        msg.msg_type = InitializeCmd;
        resp_type = InitializeResponse;
        status_array = &_FederateInitialized[0];
        break;

    case Start:
        msg.msg_type = StartCmd;
        resp_type = StartResponse;
        status_array = &_FederateRunning[0];
        break;

    case Halt:
        msg.msg_type = HaltCmd;
        resp_type = HaltResponse;
        status_array = &_FederateHalted[0];
        break;

    default:
        assert(0);
    };

    SendControlMsg(msg, m_IsController, m_IsBigEndian);

    uint start_time = ReadClock();
    uint timeout_time = AddTime(start_time, uint(timeout_secs*1e6));

    // Get responses from all federates
    for (uint i=0; i<_NumFederates; i++)
    {
        status_array[i] = false;
        _ResponseTime[i] = 0;
    }

    bool all_resps_rcvd = false;
    bool all_resps_success = false;
    while (!all_resps_rcvd)
    {
        uint wait_usecs = DeltaTime(ReadClock(), timeout_time);
        if (wait_usecs > usecPerHour/2) // Wait expired: wrapped to 1 hour
            break;

        fd_set msg_set;
        FD_ZERO(&msg_set);
        FD_SET(uint(_ControllerData[1].file_descr), &msg_set);

        uint num_msgs_ready = AwaitMsg(true, wait_usecs, msg_set, msg_set);

        if (num_msgs_ready == 0)
            break;

        ReadControlMsg(msg, m_IsController, m_IsBigEndian);
        uint completion_time = ReadClock();

        // If a valid response received, mark the federate as completed
        if (_ControlMsgType(msg.msg_type) == resp_type)
        {
            // Mark the federate as connected
            uint index = 0;
			uint i;
            for (i=0; i<msg.fed_id; i++)
                index += _FederateInfoList[i].dimension;

            index += msg.array_index;
            status_array[index] = true;
            _FederateMsgResult[index] = _ControlMsgResult(msg.result);
            _ResponseTime[index] = DeltaTime(start_time, completion_time);

            all_resps_rcvd = true;
            all_resps_success = true;
            for (i=0; i<_NumFederates; i++)
            {
                if (!status_array[i])
                {
                    all_resps_rcvd = false;
                    all_resps_success = false;
                    break;
                }

                if (_FederateMsgResult[i] != Success)
                    all_resps_success = false;
            }
        }
    }

    if (all_resps_success)
        switch(cmd)
        {
        case Initialize:
            m_FederationState = Initialized;
            break;

        case Start:
            m_FederationState = Running;
            break;

        case Halt:
            m_FederationState = Stopped;
            break;

        default:
            break;
        }


    return all_resps_rcvd;
}

void RTSP::WaitForInit(void)
{
    if (m_IsController)
        Error(false, "ERROR in RTSP::WaitForInit: Controller "
        "cannot wait for initialization");

    if (m_State != Stopped)
        Error(false, "ERROR in RTSP::WaitForInit: Must be in Stopped state");

    // Federate waits here until initialize message has been processed
    while (m_State != Initialized)
        PollRcv(false);
}

void RTSP::WaitForStart(void)
{
    if (m_IsController)
        Error(false, "ERROR in RTSP::WaitForStart: Controller "
        "can not wait for run to start");

    
    if (m_State != Initialized)
        Error(false, "ERROR in RTSP::WaitForStart: Must be in "
            "Initialized state");

    // Federate waits here until start run message has been processed
    while (m_State != Running)
        PollRcv(false);
}

void RTSP::WaitForHalt(void)
{
    if (m_IsController)
        Error(false, "ERROR in RTSP::WaitForHalt: Controller "
        "can not wait for run to end");

    if (m_State != Running)
        Error(false, "ERROR in RTSP::WaitForHalt: Must be in "
            "Running state");

    // Federate waits here until halt message has been processed
    while (m_State != Stopped)
        PollRcv(false);
}

void RTSP::RequestHalt(void)
{
    if (m_IsController)
        Error(false, "ERROR in RTSP::RequestHalt: Controller "
        "can not wait for run to end");

    if (m_State != Running)
        Error(false, "ERROR in RTSP::RequestHalt: Must be in "
            "Running state");

    _ControlMsg msg;
    msg.msg_type = HaltRequest;
    msg.fed_id = m_FederateNum;
    msg.array_index = m_FederateIndex;
    msg.result = Success;

    SendControlMsg(msg, m_IsController, m_IsBigEndian);
}

void _AddCallback(RTSP& rtsp, void (*f)(void), void* p, uchar index)
{
    assert(index < _NumMsgs);

    // Join the multicast group, if not already a member
    if (_MsgContentsList[index].file_descr == 0)
    {
        // create an ordinary UDP socket
        int fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
        if (fd == INVALID_SOCKET)
            RTSP::Error(true, "ERROR: socket call failed");

        // set up destination address
        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;

        if (strlen(rtsp.InterfaceAddr()) > 0)
            addr.sin_addr.s_addr = inet_addr(rtsp.InterfaceAddr());
        else
            addr.sin_addr.s_addr = htonl(INADDR_ANY);

        addr.sin_port = htons(_MsgContentsList[index].port);
    
        // Allow multiple federates to receive the same messages
        int value = 1;
        int result = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char*) &value,
            sizeof(int));
        assert(result == 0);

        // bind to receive address
        if (bind(fd, (struct sockaddr *) &addr, sizeof(addr)) < 0)
            RTSP::Error(true, "ERROR: bind call failed");
    
        // request to join a multicast group
        struct ip_mreq mreq;
        mreq.imr_multiaddr.s_addr = inet_addr(_MsgContentsList[index].ip_addr);

        if (strlen(rtsp.InterfaceAddr()) > 0)
            mreq.imr_interface.s_addr = inet_addr(rtsp.InterfaceAddr());
        else
            mreq.imr_interface.s_addr = htonl(INADDR_ANY);

        if (setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&mreq,
            sizeof(mreq)) < 0)
        {
            char buf[200];
            sprintf(buf, "ERROR: unable to join multicast group \"%s\"\n",
                _MsgContentsList[index].ip_addr);
            RTSP::Error(true, buf);
        }

        _MsgContentsList[index].file_descr = fd;
        _MsgContentsList[index].sock_addr = addr;

        rtsp.AddSocketToSet(fd);
    }

    // store callback function address and data pointer
    _MsgContentsList[index].callback = f;
    _MsgContentsList[index].rcv_addr = p;
}

bool _SendMessage(RTSP& rtsp, void* p, uchar index, uchar rcv_index)
{
    if (index >= _NumMsgs)
    {
        RTSP::Error(false, "ERROR: Message index too large");
        return false;
    }

    // Set sender index
    uchar* ucp = (uchar*) p + _MsgContentsList[index].sender_index_offset;
    *ucp = rtsp.FederateIndex();

    // Set receiver index
    ucp = (uchar*) p + _MsgContentsList[index].rcvr_index_offset;
    *ucp = rcv_index;

    // Set sequence number
    ushort* usp = (ushort*)((uchar*) p + _MsgContentsList[index].sequence_num_offset);
    *usp = _MsgContentsList[index].cur_sequence_num[rtsp.FederateIndex()]++;

    // Set time tag
    uint* uip = (uint*)((uchar*) p + _MsgContentsList[index].time_tag_offset);
    *uip = rtsp.ReadClock();

    // Swap bytes if needed
    if (!rtsp.IsBigEndian())
    {
        // Copy message to temporary buffer
        memcpy(_MsgSwapBuf, p, _MsgContentsList[index].size);
        p = _MsgSwapBuf;

        for (uint i=0; i<_MsgContentsList[index].num_elements; i++)
        {
            _ElementType et = _MsgContentsList[index].element_info[i].type;

            switch (et)
            {
            case Short:
            case Ushort:
                Swap2Byte((uchar *) p +
                    _MsgContentsList[index].element_info[i].offset);
                break;

            case Int:
            case Uint:
            case Float:
                Swap4Byte((uchar *) p +
                    _MsgContentsList[index].element_info[i].offset);
                break;

            case Double:
                Swap8Byte((uchar *) p +
                    _MsgContentsList[index].element_info[i].offset);
                break;

            default:
                break;
            }
        }
    }

    // transmit the message to its IP multicast group address
    int result = sendto(_MsgContentsList[index].file_descr, (char *) p,
        _MsgContentsList[index].size, 0,
        (struct sockaddr *) &_MsgContentsList[index].sock_addr,
        sizeof(struct sockaddr_in));

    if (result < 0)
    {
        RTSP::Error(false, "ERROR: sendto call failed");
        return false;
    }

    return true;
}

uint RTSP::AddTime(uint time1, uint time2)
{
    uint sum = time1 + time2;

    if (sum < time1 || sum < time2)
        sum += (uint(-1) - usecPerHour) + 1u;

    if (sum >= usecPerHour)
        sum -= usecPerHour;

    return sum;
}

uint RTSP::DeltaTime(uint earlier_time, uint later_time)
{
    uint delta_time;

    if (later_time >= earlier_time)
        delta_time = later_time - earlier_time;
    else
        delta_time = (usecPerHour - earlier_time) + later_time;

    return delta_time;
}

void RTSP::StartTimer(void)
{
    m_TimerStartValue = ReadClock();
    m_TimerStarted = true;
}

uint RTSP::ReadTimer(void)
{
    if (m_TimerStarted)
        return DeltaTime(m_TimerStartValue, ReadClock());
    else
        return m_TimerStartValue;
}

#ifdef _WIN32
uint RTSP::ReadClock(void)
{
#ifdef GPS_CLOCK
    // Put GPS read routine and conversion to microsecs past the hour in here.
#else
    static double timerScale = 0.0;

    LARGE_INTEGER i;
    if (timerScale == 0.0)
    {
        int result = QueryPerformanceFrequency(&i);
        assert(result); // returns false if no performance counter available
        timerScale = 1.0e6 / i.QuadPart;
    }

    int result = QueryPerformanceCounter(&i);
    assert(result);

    return uint(i.QuadPart * timerScale + 0.5);
#endif
}
#endif

#ifdef UNIX
uint RTSP::ReadClock(void)
{
#ifdef GPS_CLOCK
    // Put GPS read routine and conversion to microsecs past the hour in here.
#else
    struct timeval tv;
    struct timezone tz;

    gettimeofday(&tv, &tz);

    struct tm *t = localtime(&tv.tv_sec);

    const int usecPerSec = 1000000u;

    return (t->tm_min*60u + t->tm_sec) * usecPerSec + tv.tv_usec;
#endif
}
#endif

#ifdef VXWORKS
uint RTSP::ReadClock(void)
{
#ifdef GPS_CLOCK
    // Put GPS read routine and conversion to microsecs past the hour in here.
#else
    // The implementation of this function will depend on the BSP in use.
    // This is for an MV1604 BSP.

    static uint clock_start_sec = 0, clock_start_usec = 0;
    static bool save_start_time = false;

    // Since vxAbsTicks increments at system clock rate (60 hz nominal)
    // and timestamp is a real time counter, read both twice without
    // disabling interrupts (16.7 mS wraparound time for timestamp)

    TICK tick1  = vxAbsTicks;
    UINT32 dec1 = sysTimestamp();

    TICK tick2  = vxAbsTicks;
    UINT32 dec2 = sysTimestamp();

    // Don't worry about overflow to TICK.upper (happens every 2.26 years
    // at a 60 hz system clock rate)
    uint dec, tick;
    if (tick1.lower == tick2.lower) // dec1 is valid
    {
    dec = dec1;
    tick = tick1.lower;
    }
    else // Tick counter incremented between reads; dec2 is valid
    {
    dec = dec2;
    tick = tick2.lower;
    }

    uint timestamp_usec = (dec*6) / 100; // divide by 16 2/3
    uint tick_usec = ((tick % 60) * 1000000) / 60;
    uint cur_usec = timestamp_usec + tick_usec; // usec into current tick
    uint cur_sec = tick / 60;

    if (save_start_time)
    {
    clock_start_sec = cur_sec;
    clock_start_usec = cur_usec;
    }

    uint sec_diff = cur_sec - clock_start_sec;
    uint usec_diff;
    if (cur_usec > clock_start_usec)
    usec_diff = cur_usec - clock_start_usec;
    else
    {
    usec_diff = (1000000u + cur_usec) - clock_start_usec;
    sec_diff--;
    }

    return sec_diff*1000000u + usec_diff;
#endif
}
#endif

void RTSP::PollRcv(bool use_timeout, uint timeout_usec)
{
    // See if any incoming messages are available
    // If any are available, process them
    fd_set ready_set;
    FD_ZERO(&ready_set);

    uint num_msgs_ready = AwaitMsg(use_timeout, timeout_usec, m_SocketSet,
        ready_set);

    for (uint i=0; i<num_msgs_ready; i++)
    {
        // First check for a control message
        int rcv_index = m_IsController ? 1 : 0;
        int fd = _ControllerData[rcv_index].file_descr;

        // Handle a control message: Call the callback and send a response
        if (FD_ISSET(fd, &ready_set))
        {
            FD_CLR((uint)fd, &ready_set);
            _ControlMsg msg;

            ReadControlMsg(msg, m_IsController, m_IsBigEndian);

            // Log message if necessary
            if (m_LogMsgs)
            {
                MsgLogHeader hdr;
                hdr.rec_size = sizeof(hdr) + sizeof(msg);
                hdr.port = _ControllerData[rcv_index].port;
                hdr.log_time_tag = ReadClock();

                fwrite(&hdr, sizeof(hdr), 1, m_MsgLogIov);
                fwrite(&msg, sizeof(msg), 1, m_MsgLogIov);
            }

            msg.fed_id = m_FederateNum;
            msg.array_index = m_FederateIndex;
            msg.result = Success; // For now
            bool result;
            uint i, j;

            switch (msg.msg_type)
            {
            case PingCmd:
                if (m_IsController)
                    Error(true, "Ping command received by controller");

                msg.msg_type = PingResponse;
                msg.result = Success;

                // Send immediate response for timing purposes
                SendControlMsg(msg, m_IsController, m_IsBigEndian);

                if (m_PingCallback)
                {
                    result = m_PingCallback();
                    if (!result)
                        msg.result = UserRoutineFailure;
                }
                
                break;

            case InitializeCmd:
                if (m_IsController)
                    Error(true, "Initialize command received by controller");

                msg.msg_type = InitializeResponse;
                msg.result = Success;

                if (m_State == Running)
                    msg.result = InitFailed_Running;
                else
                {
                    if (m_InitializeCallback)
                    {
                        result = m_InitializeCallback();
                        if (result)
                            m_State = Initialized;
                        else
                            msg.result = UserRoutineFailure;
                    }
                }

                // Don't send response until callback has completed
                SendControlMsg(msg, m_IsController, m_IsBigEndian);
                break;

            case StartCmd:
                if (m_IsController)
                    Error(true, "Start command received by controller");

                // Start all sequence numbers at zero
                for (i=0; i<_NumMsgs; i++)
                    for (j=0; j<_NumFederates; j++)
                        _MsgContentsList[i].cur_sequence_num[j] = 0;

                // Clear list of lost messages
                m_NumLostMsgs = 0;

                msg.msg_type = StartResponse;
                msg.result = Success;

                if (m_State == Stopped)
                    msg.result = StartFailed_NoInit;
                else if (m_State == Running)
                    msg.result = StartFailed_Running;
                else
                {
                    // Time zero is right now; Start the timer
                    StartTimer();

                    if (m_StartCallback)
                    {
                        result = m_StartCallback();
                        if (result)
                            m_State = Running;
                        else
                            msg.result = UserRoutineFailure;
                    }
                }
            
                // Don't send response until callback has completed
                SendControlMsg(msg, m_IsController, m_IsBigEndian);
                break;

            case HaltCmd:
                if (m_IsController)
                    Error(true, "Halt command received by controller");

                msg.msg_type = HaltResponse;
                msg.result = Success;

                if (m_State != Running)
                    msg.result = HaltFailed_NotRunning;
                else
                {
                    if (m_HaltCallback)
                    {
                        result = m_HaltCallback();
                        if (result)
                            m_State = Stopped;
                        else
                            msg.result = UserRoutineFailure;
                    }
                }

                // Open a new log file if logging messages
                if (m_LogMsgs)
                {
                    fclose(m_MsgLogIov);
                    EnableMessageLogging();
                }

                // Don't send response until callback has completed
                SendControlMsg(msg, m_IsController, m_IsBigEndian);
                break;

            case HaltRequest:
                if (!m_IsController)
                    Error(true, "Halt request received by non-controller");

                CmdFederation(Halt);
                break;

            default: // Invalid command received; Ignore it
                break;
            }
        }
        else // Handle a user-defined message
        {
            for (uint j=0; j<_NumMsgs; j++)
            {
                int fd = _MsgContentsList[j].file_descr;
                if (FD_ISSET(fd, &ready_set))
                {
                    FD_CLR((uint)fd, &ready_set);

                    // Read the message
                    struct sockaddr_in addr = _MsgContentsList[j].sock_addr;
                    char* p = (char*) _MsgContentsList[j].rcv_addr;
                    int size = _MsgContentsList[j].size;

                    socklen_t addrlen = sizeof(struct sockaddr_in);

                    int result = recvfrom(fd, p, size, 0,
                        (struct sockaddr *) &addr, &addrlen);

                    assert(result == size);

                    // Swap the message bytes
                    if (!m_IsBigEndian)
                    {
                        for (uint k=0; k<_MsgContentsList[j].num_elements; k++)
                        {
                            _ElementType et =
                                _MsgContentsList[j].element_info[k].type;

                            switch (et)
                            {
                            case Short:
                            case Ushort:
                                Swap2Byte((uchar *) p + _MsgContentsList[j].
                                    element_info[k].offset);
                                break;

                            case Int:
                            case Uint:
                            case Float:
                                Swap4Byte((uchar *) p + _MsgContentsList[j].
                                    element_info[k].offset);
                                break;

                            case Double:
                                Swap8Byte((uchar *) p + _MsgContentsList[j].
                                    element_info[k].offset);
                                break;

                            default:
                                break;
                            }
                        }
                    }

                    // Log message if necessary
                    if (m_LogMsgs)
                    {
                        MsgLogHeader hdr;
                        hdr.rec_size = sizeof(hdr) + size;
                        hdr.port = _MsgContentsList[j].port;
                        hdr.log_time_tag = ReadClock();

                        fwrite(&hdr, sizeof(hdr), 1, m_MsgLogIov);
                        fwrite(p, size, 1, m_MsgLogIov);
                    }

                    // Get the sender index
                    uchar* ucp = (uchar*) p +
                        _MsgContentsList[j].sender_index_offset;
                    uchar send_index = *ucp;

                    // Get the receiver index
                    ucp = (uchar*) p +
                        _MsgContentsList[j].rcvr_index_offset;
                    uchar rcv_index = *ucp;

                    // Check the sequence number
                    ushort seq_num = *(ushort*)((uchar*) p +
                        _MsgContentsList[j].sequence_num_offset);

                    // See if one or more messages were lost
                    if (seq_num != _MsgContentsList[j].
                        cur_sequence_num[send_index]++)
                    {
                        // Get the time tag
                        uint* uip = (uint*)((uchar*) p +
                            _MsgContentsList[j].time_tag_offset);
                        uint time_tag = *uip;

                        // Save lost message info
                        if (m_NumLostMsgs < m_LostMsgLogSize)
                        {
                            m_LostMsg[m_NumLostMsgs].msg_index = j;
                            m_LostMsg[m_NumLostMsgs].send_index = send_index;
                            m_LostMsg[m_NumLostMsgs].rcv_index = rcv_index;
                            m_LostMsg[m_NumLostMsgs].lost_seq_start = 
                                _MsgContentsList[j].
                                cur_sequence_num[send_index] - 1;
                            m_LostMsg[m_NumLostMsgs].lost_seq_end = seq_num - 1;
                            m_LostMsg[m_NumLostMsgs].time_tag = time_tag;
                        }

                        m_NumLostMsgs++;

                        // Resynchronize sequence numbers
                        _MsgContentsList[j].cur_sequence_num[send_index] =
                            seq_num + 1;
                    }

                    // Call the callback if receiver index matches
                    if (rcv_index == _AllIndexes ||
                        rcv_index == m_FederateIndex)
                        (_MsgContentsList[j].callback)();
                }
            }
        }
    }
}

void RTSP::RcvMainLoop(void)
{
    for (;;)
        PollRcv(false);
}

void RTSP::Error(bool abort, const char* msg)
{
    fprintf(stderr, "%s\n", msg);

    if (abort)
        exit(1);
}

RTSP::StateType RTSP::FederateState()
{
    return m_State;
}

RTSP::StateType RTSP::FederationState()
{
    if (!m_IsController)
    {
        Error(false, "ERROR in RTSP::FederationState: Non-controller "
        "cannot return federation state");
    }

    return m_FederationState;
}
