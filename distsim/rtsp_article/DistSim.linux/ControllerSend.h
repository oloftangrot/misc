// ControllerSend class
// Generated Mon Jun 09 22:47:15 2008 from DistSim.mdf
// *** This file was automatically generated. Do not edit it! ***

#include <RTSP.h>
#include <RTSP_Internals.h>

class ControllerSend
{
public:
    struct Update
    {
        double drive;
        uint _TimeTag;
        ushort _SequenceNum;
        uchar _ReceiverIndex;
        uchar _SenderIndex;

        bool Send(RTSP& rtsp, uchar rcv_index = _AllIndexes)
            { return _SendMessage(rtsp, this, 1, rcv_index); }
    };
};

