// Federate.cpp: implementation of the Federate class.

#include <RTSP.h>
#define _RTSP_DATA_MODIFIABLE
#include <RTSP_Internals.h>

#include "Parser.h"
#include "Federate.h"
#include "Struct.h"

#include <time.h>
#include <assert.h>

Message* Federate::FindMessage(std::string name) const
{
    Message* result = 0;

    MessageMapType::const_iterator it = m_MessageMap.find(name);
    if (it != m_MessageMap.end())
        result = (*it).second;

    return result;
}

void Federate::GenerateCppCode(FILE *iov, bool gen_send_code) const
{
    // get date and time of code gen and remove trailing '\n'.
    time_t long_time;
    time(&long_time);
    char *cp = asctime(localtime(&long_time));
    cp[strlen(cp) - 1] = '\0';

    // print file header
    fprintf(iov, "// %s%s class\n", m_Name.c_str(),
        gen_send_code ? "Send" : "Rcv");
    fprintf(iov, "// Generated %s from %s\n", cp, MDF_File.c_str());
    fprintf(iov, "// *** This file was automatically generated. "
        "Do not edit it! ***\n\n");

    fprintf(iov, "#include <RTSP.h>\n#include <RTSP_Internals.h>\n\n");
    fprintf(iov, "class %s%s\n{\npublic:\n", m_Name.c_str(),
        gen_send_code ? "Send" : "Rcv");

    // print constant declarations as enum values
    extern ConstMapType ConstMap;
    ConstMapType::iterator ci = ConstMap.begin();
    for (; ci != ConstMap.end(); ci++)
        fprintf(iov, "    enum { %s = %d };\n", (*ci).first.c_str(),
            (*ci).second);

    // use the first_written flag to control insertion of blank lines
    bool first_written = false;
    if (ConstMap.size() > 0)
        first_written = true;

    // identify sub-structures that need to be defined
    const MessageMapType *mm = gen_send_code ?
        &m_MessageMap : &m_SubscriptionMap;

    StructSeqType ss;
    MessageMapType::const_iterator it = mm->begin();
    for (; it != mm->end(); it++)
        (*it).second->FindSubStructs(ss);

    // output sub-structure definitions
    StructSeqType::iterator si = ss.begin();
    for (; si != ss.end(); si++)
    {
        if (first_written)
            fprintf(iov, "\n");
        else
            first_written = true;

        (*si).second->GenerateCppCode(iov);
    }

    // output message definitions
    it = mm->begin();
    for (; it != mm->end(); it++)
    {
        if (first_written)
            fprintf(iov, "\n");
        else
            first_written = true;

        (*it).second->GenerateCppCode(iov, gen_send_code);
    }

    // close out the file
    fprintf(iov, "};\n\n");
}

void Federate::OutputOutgoingMsgInfo(FILE *iov, int index)
{
    if (m_MessageMap.size() == 0)
        return;

    fprintf(iov, "static const uint _msg_out%d[] = {", index);

    MessageMapType::iterator it = m_MessageMap.begin();
    for (; it != m_MessageMap.end(); it++)
    {
        if (it != m_MessageMap.begin())
            fprintf(iov, ", ");

        fprintf(iov, "%d", (*it).second->SequenceNum());
    }

    fprintf(iov, "};\n");
}

void Federate::OutputFederateInfo(FILE *iov, int index)
{
    char msg_info[50];
    if (m_MessageMap.size() > 0)
        sprintf(msg_info, "_msg_out%d", index);
    else
        sprintf(msg_info, "0");

    char fed_name[250];
    sprintf(fed_name, "\"%s\"", m_Name.c_str());

    fprintf(iov, "    { %-20s, %3d, %3lu, %-10s },\n",
        fed_name, m_Dimension, m_MessageMap.size(), msg_info);
}

void Federate::GenerateFederateData(int index)
{
	// Set up list of outgoing message numbers, if any defined
	uint* msg_num = 0;
	if (m_MessageMap.size() > 0)
	{
		msg_num = new uint[m_MessageMap.size()];

		MessageMapType::iterator it = m_MessageMap.begin();
		for (int i=0; it != m_MessageMap.end(); it++, i++)
			msg_num[i] = (*it).second->SequenceNum();
	}

	// Federate name
    char* cp = new char[strlen(m_Name.c_str())+1];
    strcpy(cp, m_Name.c_str());

    _FederateInfoList[index].name = cp;
	_FederateInfoList[index].dimension = m_Dimension;
	_FederateInfoList[index].num_outgoing_msgs = m_MessageMap.size();
	_FederateInfoList[index].outgoing_msgs = msg_num;
}

void Federate::Initialize()
{
    FederateMap.clear();
    IP_BaseAddr.s_addr = 0;
    IP_BasePort = 0;
    MulticastTtl = 1;
}
