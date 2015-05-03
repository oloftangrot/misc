// CommandGeneratorSend class
// Generated Mon Jun 09 22:47:15 2008 from DistSim.mdf
// *** This file was automatically generated. Do not edit it! ***

#include <RTSP.h>
#include <RTSP_Internals.h>

class CommandGeneratorSend
{
public:
    struct Update
    {
        double command;
        uint _TimeTag;
        ushort _SequenceNum;
        uchar _ReceiverIndex;
        uchar _SenderIndex;

        bool Send(RTSP& rtsp, uchar rcv_index = _AllIndexes)
            { return _SendMessage(rtsp, this, 0, rcv_index); }
    };
};

