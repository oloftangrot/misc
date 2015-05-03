// Message internal definitions

#ifndef _RTSP_DATA_MODIFIABLE
#define _RTSP_DATA_MODIFIABLE
#endif

#include <RTSP.h>
#include <RTSP_Internals.h>

uint _NumMsgs;
uint _NumFederateTypes;
uint _MulticastTtl;
uint _NumFederates;
uint _MaxMessageSize;

_MsgContents _MsgContentsList[_MaxMessages];
_FederateInfo _FederateInfoList[_MaxFederateTypes];
_ControllerInfo _ControllerData[2];

char _MsgSwapBuf[_MessageBufSize];
bool _FederatePinged[_MaxFederates];
bool _FederateInitialized[_MaxFederates];
uint _ResponseTime[_MaxFederates];
bool _FederateRunning[_MaxFederates];
bool _FederateHalted[_MaxFederates];
_ControlMsgResult _FederateMsgResult[_MaxFederates];
