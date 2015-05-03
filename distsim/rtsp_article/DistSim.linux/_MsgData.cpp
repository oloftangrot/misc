// Message internal definitions
// Generated Mon Jun 09 22:47:15 2008 from DistSim.mdf
// *** This file was automatically generated. Do not edit it! ***

#include <RTSP.h>
#include <RTSP_Internals.h>

const uint _NumMsgs = 3;
const uint _NumFederateTypes = 3;
const uint _MulticastTtl = 1;
const uint _NumFederates = 3;
const uint _MaxMessageSize = 16;

static const _ElementInfo _msg0[] = // CommandGenerator.Update
{
    { Double,    0 },
    { Uint  ,    8 },
    { Ushort,   12 },
    { Uchar ,   14 },
    { Uchar ,   15 },
};

static const _ElementInfo _msg1[] = // Controller.Update
{
    { Double,    0 },
    { Uint  ,    8 },
    { Ushort,   12 },
    { Uchar ,   14 },
    { Uchar ,   15 },
};

static const _ElementInfo _msg2[] = // Plant.Update
{
    { Double,    0 },
    { Uint  ,    8 },
    { Ushort,   12 },
    { Uchar ,   14 },
    { Uchar ,   15 },
};

ushort _seq0[_NumFederates];
ushort _seq1[_NumFederates];
ushort _seq2[_NumFederates];

_MsgContents _MsgContentsList[_NumMsgs] =
{
    { "225.0.0.2", 20002,  16,   8,  12,  15,  14,   5, _msg0, _seq0, "CommandGenerator.Update"},
    { "225.0.0.3", 20003,  16,   8,  12,  15,  14,   5, _msg1, _seq1, "Controller.Update"},
    { "225.0.0.4", 20004,  16,   8,  12,  15,  14,   5, _msg2, _seq2, "Plant.Update"},
};

static const uint _msg_out0[] = {0};
static const uint _msg_out1[] = {1};
static const uint _msg_out2[] = {2};

_FederateInfo _FederateInfoList[_NumFederateTypes] =
{
    { "CommandGenerator"  ,   1,   1, _msg_out0  },
    { "Controller"        ,   1,   1, _msg_out1  },
    { "Plant"             ,   1,   1, _msg_out2  },
};

_ControllerInfo _ControllerData[2] =
{
    {"225.0.0.0", 20000}, // Controller output IP addr and port
    {"225.0.0.1", 20001}  // Controller input IP addr and port
};

char _MsgSwapBuf[_MaxMessageSize];
bool _FederatePinged[_NumFederates];
bool _FederateInitialized[_NumFederates];
uint _ResponseTime[_NumFederates];
bool _FederateRunning[_NumFederates];
bool _FederateHalted[_NumFederates];
_ControlMsgResult _FederateMsgResult[_NumFederates];
