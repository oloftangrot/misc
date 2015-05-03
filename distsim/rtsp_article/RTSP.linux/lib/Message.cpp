// Message.cpp: implementation of the Message class.

#include "Message.h"
#include "Element.h"
#include "Struct.h"
#include "Federate.h"
#include "Parser.h"

uint Message::s_SequenceNum = 0;
uint Message::s_MaxMessageSize = 0;
MessageVecType Message::s_MessageVec;

Message::Message(ElementMapType* element_map) :
    m_Struct(element_map), m_SequenceNum(s_SequenceNum++)
{
    if (m_Struct.Size() > s_MaxMessageSize)
        s_MaxMessageSize = m_Struct.Size();

    s_MessageVec.push_back(this);
}

void Message::FindSubStructs(StructSeqType& structs)
{
    m_Struct.FindSubStructs(structs);
}

void Message::GenerateCppCode(FILE* iov, bool gen_send_code) const
{
    m_Struct.GenerateCppCode(iov, true, gen_send_code, m_SequenceNum,
        m_Owner->Name());
}

void Message::OutputElementInfo(FILE* iov, uint index)
{
    fprintf(iov, "static const _ElementInfo _msg%d[] = // %s.%s\n{\n", index,
        m_Owner->Name().c_str(), MessageVec()[index]->Name().c_str());

    uint offset = 0;
    m_NumElements = m_Struct.OutputElementInfo(iov, offset, this);

    fprintf(iov, "};\n");
}

void Message::OutputMessageInfo(FILE* iov, uint index) const
{
    // set the IP address for this message
    // 2 offset is for controller send+rcv channels
    uint ip_addr = ntohl(IP_BaseAddr.s_addr) + index + 2;
    struct in_addr net_addr;
    net_addr.s_addr = htonl(ip_addr);

    if (ip_addr >= (240u << (3*8)))
    {
        char buf[100];
        sprintf(buf, "ERROR: IP address too high "
            "(highest multicast address: 239.255.255.255)\n");
        ErrorMsg(buf);
    }

    uint port = IP_BasePort + index + 2;

    fprintf(iov, "    { \"%s\", %3d, %3d, %3d, %3d, %3d, %3d, %3d, _msg%d, _seq%d, "
        "\"%s.%s\"},\n", inet_ntoa(net_addr), port, m_Struct.Size(),
        m_TimeTagOffset, m_SequenceNumOffset, m_SenderIndexOffset,
        m_ReceiverIndexOffset, m_NumElements, index, index, m_Owner->Name().c_str(), 
        MessageVec()[index]->Name().c_str());
}

_MsgContents Message::GenerateMessageData(uint index, int num_federates)
{
    // set the IP address for this message
    // 2 offset is for controller send+rcv channels
    uint ip_addr = ntohl(IP_BaseAddr.s_addr) + index + 2;
    struct in_addr net_addr;
    net_addr.s_addr = htonl(ip_addr);

    if (ip_addr >= (240u << (3*8)))
    {
        char buf[100];
        sprintf(buf, "ERROR: IP address too high "
            "(highest multicast address: 239.255.255.255)\n");
        ErrorMsg(buf);
    }

    uint offset = 0;
    std::vector<_ElementInfo*> struct_eiv =
		m_Struct.GenerateElementData(offset, this);
	m_NumElements = struct_eiv.size();

	_ElementInfo* msg = new _ElementInfo[m_NumElements];
	std::vector<_ElementInfo*>::iterator it = struct_eiv.begin();
	for (int i=0; i<m_NumElements; i++, it++)
		msg[i] = *(*it);

	_MsgContents result;

	char* cp = inet_ntoa(net_addr);
	char *addr_buf = new char[strlen(cp) + 1];
	strcpy(addr_buf, cp);
	result.ip_addr = addr_buf;

    result.port = IP_BasePort + index + 2;
	result.size = m_Struct.Size();
    result.time_tag_offset = m_TimeTagOffset;
	result.sequence_num_offset = m_SequenceNumOffset;
	result.sender_index_offset = m_SenderIndexOffset;
    result.rcvr_index_offset = m_ReceiverIndexOffset;
	result.num_elements = m_NumElements;
	result.element_info = msg;

	result.rcv_addr = 0;
	result.callback = 0;
	result.file_descr = 0;
	memset(&result.sock_addr, 0, sizeof(result.sock_addr));

	result.cur_sequence_num = new ushort[num_federates];

	return result;
}

void Message::Initialize()
{
    s_SequenceNum = 0;
    s_MaxMessageSize = 0;
    s_MessageVec.clear();
}
