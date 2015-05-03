// PlantRcv class
// Generated Mon Jun 09 22:47:15 2008 from DistSim.mdf
// *** This file was automatically generated. Do not edit it! ***

#include <RTSP.h>
#include <RTSP_Internals.h>

class PlantRcv
{
public:
    struct ControllerUpdate
    {
        double drive;
        uint _TimeTag;
        ushort _SequenceNum;
        uchar _ReceiverIndex;
        uchar _SenderIndex;

        void SetCallback(RTSP& rtsp, void (*_f)())
            { _AddCallback(rtsp, _f, this, 1); }
    };
};

