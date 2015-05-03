// Message.h: interface for the Message class.
//
//////////////////////////////////////////////////////////////////////

#ifndef MESSAGE_H
#define MESSAGE_H

#include <Types.h>
#include <Struct.h>

#include <RTSP.h>
#include <RTSP_Internals.h>

#include <vector>

class Message  
{
public:
    Message(ElementMapType* element_map=0);
    void SetOwner(Federate* owner) { m_Owner = owner; }
    void FindSubStructs(StructSeqType& structs);

    void SetName(std::string name) { m_Struct.SetName(name); }
    void SetTimeTagOffset(uint offset) { m_TimeTagOffset = offset; }
    void SetSequenceNumOffset(uint offset) { m_SequenceNumOffset = offset; }
    void SetSenderIndexOffset(uint offset) { m_SenderIndexOffset = offset; }
    void SetReceiverIndexOffset(uint offset)
        { m_ReceiverIndexOffset = offset; }

    std::string Name() const { return m_Struct.Name(); }
    Federate* Owner() { return m_Owner; }
    uint SequenceNum() { return m_SequenceNum; }

    void GenerateCppCode(FILE* iov, bool gen_send_code) const;
    void OutputElementInfo(FILE* iov, uint index);
    void OutputMessageInfo(FILE* iov, uint index) const;
    _MsgContents GenerateMessageData(uint index, int num_federates);

    static uint NumMsgs() { return s_SequenceNum; }
    static uint MaxMessageSize() { return s_MaxMessageSize; }
    static const MessageVecType MessageVec() { return s_MessageVec; }
    static void Initialize();

private:
    Struct m_Struct;
    uint m_SequenceNum, m_NumElements;
    uint m_TimeTagOffset, m_SequenceNumOffset, m_SenderIndexOffset,
        m_ReceiverIndexOffset;
    Federate* m_Owner;

    static uint s_SequenceNum, s_MaxMessageSize;
    static MessageVecType s_MessageVec;
};

#endif
