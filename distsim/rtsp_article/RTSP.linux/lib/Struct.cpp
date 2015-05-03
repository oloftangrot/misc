// Struct.cpp: implementation of the Struct class.

#include "Struct.h"
#include "Message.h"
#include "Parser.h"

#include <assert.h>

uint Struct::s_SequenceNum = 0;

Struct::Struct(const ElementMapType* element_map) :
    m_ElementMap(element_map), m_SequenceNum(s_SequenceNum++)
{
    m_Size  = 0;
    m_Align = 0;
    m_Num1s = 0;
    m_Num2s = 0;
    m_Num4s = 0;
    m_Num8s = 0;

    int size_1s = 0, size_2s = 0, size_4s = 0, size_8s = 0;

    ElementMapType::const_iterator ei = m_ElementMap->begin();
    for (; ei != m_ElementMap->end(); ei++)
    {
        Element *e = (*ei).second;
        uint align = e->Align();

        if (align == 8)
        {
            m_Num8s++;
            size_8s += e->Size();

            if (m_Align < 8)
                m_Align = 8;
        }
        else
        {
            if (align == 4)
            {
                m_Num4s++;
                size_4s += e->Size();

                if (m_Align < 4)
                    m_Align = 4;
            }
            else
            {
                if (align == 2)
                {
                    m_Num2s++;
                    size_2s += e->Size();

                    if (m_Align < 2)
                        m_Align = 2;
                }
                else
                {
                    assert(align == 1);

                    m_Num1s++;
                    size_1s += e->Size();

                    if (m_Align < 1)
                        m_Align = 1;
                }
            }
        }

        m_Size += e->Size();
    }

    // Add padding so everything is aligned
    m_PadInts = 0;
    m_PadShorts = 0;
    m_PadChars = 0;

    if (m_Num8s > 0)
    {
        if (m_Num1s == 0)
        {
            if (m_Num2s == 0)    // struct can have 4s and 8s
                m_PadInts = (size_4s % 8) / 4;
            else                 // struct can have 2s, 4s and 8s
                m_PadShorts =  ((8 - (size_4s + size_2s) % 8) % 8) / 2;
        }
        else                     // struct can have 1s, 2s, 4s and 8s
            m_PadChars = ((8 - (size_4s + size_2s + size_1s) % 8) % 8);
    }
    else
        if (m_Num4s > 0)
        {
            if (m_Num1s == 0)
                if (m_Num2s > 0) // struct can have 2s and 4s
                    m_PadShorts =  ((8 - size_2s % 8) % 8) / 2;
        }
        else
            if (m_Num2s > 0)     // struct can have 1s and 2s
                m_PadChars = size_1s % 2;

    m_Size += 4*m_PadInts + 2*m_PadShorts + m_PadChars;
}

void Struct::FindSubStructs(StructSeqType& structs)
{
    ElementMapType::const_iterator ei = m_ElementMap->begin();
    for (; ei != m_ElementMap->end(); ei++)
    {
        const Element *e = (*ei).second;

        if (e->Type() == Element::Struct)
        {
            Struct* s = e->GetStruct();
            structs.insert(StructSeqType::value_type(s->SequenceNum(), s));

            s->FindSubStructs(structs);
        }
    }
}

void Struct::GenerateCppCode(FILE *iov, bool is_message, bool gen_send_code,
    int seq_num, std::string owner) const
{
    if (is_message)
    {
        if (gen_send_code)
            fprintf(iov, "    struct %s\n    {\n", m_Name.c_str());
        else
            fprintf(iov, "    struct %s%s\n    {\n", owner.c_str(),
                m_Name.c_str());
    }
    else
        fprintf(iov, "    struct %s\n    {\n", m_Name.c_str());

    uint num8s_left = m_Num8s, num4s_left = m_Num4s, num2s_left = m_Num2s,
        num1s_left = m_Num1s;

    // output 8 byte-aligned elements
    ElementMapType::const_iterator ei = m_ElementMap->begin();
    for (; num8s_left > 0 && ei != m_ElementMap->end(); ei++)
    {
        Element* e = (*ei).second;
        if (e->Type() == Element::Double)
        {
            e->GenerateCppCode(iov);
            num8s_left--;
        }

        if (e->Type() == Element::Struct && e->GetStruct()->Align() == 8)
            e->GenerateCppCode(iov);
    }

    // output 4 byte-aligned elements
    ei = m_ElementMap->begin();
    for (; num4s_left > 0 && ei != m_ElementMap->end(); ei++)
    {
        Element* e = (*ei).second;
        if (e->Type() == Element::Uint || e->Type() == Element::Int ||
            e->Type() == Element::Float)
        {
            e->GenerateCppCode(iov);
            num4s_left--;
        }

        if (e->Type() == Element::Struct && e->GetStruct()->Align() == 4)
            e->GenerateCppCode(iov);
    }

    // output 2 byte-aligned elements
    ei = m_ElementMap->begin();
    for (; num2s_left > 0 && ei != m_ElementMap->end(); ei++)
    {
        Element* e = (*ei).second;
        if (e->Type() == Element::Ushort || e->Type() == Element::Short)
        {
            e->GenerateCppCode(iov);
            num2s_left--;
        }

        if (e->Type() == Element::Struct && e->GetStruct()->Align() == 2)
            e->GenerateCppCode(iov);
    }

    // output unaligned elements
    ei = m_ElementMap->begin();
    for (; num1s_left > 0 && ei != m_ElementMap->end(); ei++)
    {
        Element* e = (*ei).second;
        if (e->Type() == Element::Uchar || e->Type() == Element::Char)
        {
            e->GenerateCppCode(iov);
            num1s_left--;
        }

        if (e->Type() == Element::Struct && e->GetStruct()->Align() == 1)
            e->GenerateCppCode(iov);
    }

    // output padding, if any is needed
    uint pad_num = 0;
    if (m_PadInts > 0)
        fprintf(iov, "        uint _pad%d;\n", pad_num++);
    else
        if (m_PadShorts > 0)
            for ( unsigned int i = 0; i < m_PadShorts; i++)
                fprintf(iov, "        ushort _pad%d;\n", pad_num++);
        else
            for ( unsigned int i = 0; i < m_PadChars; i++)
                fprintf(iov, "        uchar _pad%d;\n", pad_num++);

    // output methods and finish the class
    if (is_message)
    {
        if (gen_send_code)
            fprintf(iov, "\n"
			"        bool Send(RTSP& rtsp, uchar rcv_index = _AllIndexes)\n"
            "            { return _SendMessage(rtsp, this, %d, rcv_index); }\n",
			seq_num);
        else
            fprintf(iov, "\n        void SetCallback(RTSP& rtsp, "
                "void (*_f)())\n            { _AddCallback(rtsp, _f, "
                "this, %d); }\n", seq_num);
    }

    fprintf(iov, "    };\n");
}

uint Struct::OutputElementInfo(FILE* iov, uint& offset, Message* owner) const
{
    uint num_elements = 0;
    uint num8s_left = m_Num8s, num4s_left = m_Num4s, num2s_left = m_Num2s,
        num1s_left = m_Num1s;

    // output 8 byte-aligned elements
    ElementMapType::const_iterator ei = m_ElementMap->begin();
    for (; num8s_left > 0 && ei != m_ElementMap->end(); ei++)
    {
        Element* e = (*ei).second;
        if (e->Type() == Element::Double)
        {
            for ( unsigned int i = 0; i < e->NumElements(); i++)
            {
                fprintf(iov, "    { Double, %4d },\n", offset);
                offset += 8;
                num_elements++;
            }
            num8s_left--;
        }

        if (e->Type() == Element::Struct && e->GetStruct()->Align() == 8)
            for ( unsigned int i = 0; i < e->NumElements(); i++)
                num_elements += e->GetStruct()->OutputElementInfo(iov, offset,
                    owner);
    }

    // output 4 byte-aligned elements
    ei = m_ElementMap->begin();
    for (; num4s_left > 0 && ei != m_ElementMap->end(); ei++)
    {
        Element* e = (*ei).second;
        if (e->Type() == Element::Uint)
        {
            if (e->Name() == "_TimeTag")
                owner->SetTimeTagOffset(offset);

            for ( unsigned int i = 0; i < e->NumElements(); i++)
            {
                fprintf(iov, "    { Uint  , %4d },\n", offset);
                offset += 4;
                num_elements++;
            }
            num4s_left--;
        }

        if (e->Type() == Element::Int)
        {
            for ( unsigned int i = 0; i < e->NumElements(); i++)
            {
                fprintf(iov, "    { Int   , %4d },\n", offset);
                offset += 4;
                num_elements++;
            }
            num4s_left--;
        }

        if (e->Type() == Element::Float)
        {
            for ( unsigned int i = 0; i < e->NumElements(); i++)
            {
                fprintf(iov, "    { Float , %4d },\n", offset);
                offset += 4;
                num_elements++;
            }
            num4s_left--;
        }

        if (e->Type() == Element::Struct && e->GetStruct()->Align() == 4)
            for ( unsigned int i = 0; i < e->NumElements(); i++)
                num_elements += e->GetStruct()->OutputElementInfo(iov, offset,
                    owner);
    }

    // output 2 byte-aligned elements
    ei = m_ElementMap->begin();
    for (; num2s_left > 0 && ei != m_ElementMap->end(); ei++)
    {
        Element* e = (*ei).second;
        if (e->Type() == Element::Ushort)
        {
            if (e->Name() == "_SequenceNum")
                owner->SetSequenceNumOffset(offset);

            for ( unsigned int i = 0; i < e->NumElements(); i++)
            {
                fprintf(iov, "    { Ushort, %4d },\n", offset);
                offset += 2;
                num_elements++;
            }
            num2s_left--;
        }

        if (e->Type() == Element::Short)
        {
            for ( unsigned int i = 0; i < e->NumElements(); i++)
            {
                fprintf(iov, "    { Short , %4d },\n", offset);
                offset += 2;
                num_elements++;
            }
            num2s_left--;
        }

        if (e->Type() == Element::Struct && e->GetStruct()->Align() == 2)
            for ( unsigned int i = 0; i < e->NumElements(); i++)
                num_elements += e->GetStruct()->OutputElementInfo(iov, offset,
                    owner);
    }

    // output unaligned elements
    ei = m_ElementMap->begin();
    for (; num1s_left > 0 && ei != m_ElementMap->end(); ei++)
    {
        Element* e = (*ei).second;
        if (e->Type() == Element::Uchar)
        {
            if (e->Name() == "_SenderIndex")
                owner->SetSenderIndexOffset(offset);

            if (e->Name() == "_ReceiverIndex")
                owner->SetReceiverIndexOffset(offset);

            for ( unsigned int i = 0; i < e->NumElements(); i++)
            {
                fprintf(iov, "    { Uchar , %4d },\n", offset);
                offset += 1;
                num_elements++;
            }
            num1s_left--;
        }

        if (e->Type() == Element::Char)
        {
            for ( unsigned int i = 0; i < e->NumElements(); i++)
            {
                fprintf(iov, "    { Char  , %4d },\n", offset);
                offset += 1;
                num_elements++;
            }
            num1s_left--;
        }

        if (e->Type() == Element::Struct && e->GetStruct()->Align() == 1)
            for ( unsigned int i = 0; i < e->NumElements(); i++)
                num_elements += e->GetStruct()->OutputElementInfo(iov, offset,
                    owner);
    }

    assert(m_Align > 0);

    if (offset % m_Align != 0)
        offset += m_Align - (offset % m_Align);

    return num_elements;
}

std::vector<_ElementInfo*> Struct::GenerateElementData(uint& offset, 
	Message* owner) const
{
    uint num_elements = 0;
    uint num8s_left = m_Num8s, num4s_left = m_Num4s, num2s_left = m_Num2s,
        num1s_left = m_Num1s;

    std::vector<_ElementInfo*> eiv;

    // Generate 8 byte-aligned elements
    ElementMapType::const_iterator ei = m_ElementMap->begin();
    for (; num8s_left > 0 && ei != m_ElementMap->end(); ei++)
    {
        Element* e = (*ei).second;
        if (e->Type() == Element::Double)
        {
            for ( unsigned int i = 0; i < e->NumElements(); i++)
            {
		        _ElementInfo* elem = new _ElementInfo;
                elem->type = Double;
                elem->offset = offset;
                eiv.push_back(elem);
                offset += 8;
                num_elements++;
            }
            num8s_left--;
        }

        if (e->Type() == Element::Struct && e->GetStruct()->Align() == 8)
            for ( unsigned int i = 0; i < e->NumElements(); i++)
			{
                std::vector<_ElementInfo*> struct_eiv =
					e->GetStruct()->GenerateElementData(offset, owner);
				num_elements += struct_eiv.size();

				eiv.insert(eiv.end(), struct_eiv.begin(), struct_eiv.end());
			}
    }

    // output 4 byte-aligned elements
    ei = m_ElementMap->begin();
    for (; num4s_left > 0 && ei != m_ElementMap->end(); ei++)
    {
        Element* e = (*ei).second;
        if (e->Type() == Element::Uint)
        {
            if (e->Name() == "_TimeTag")
                owner->SetTimeTagOffset(offset);

            for ( unsigned int i = 0; i < e->NumElements(); i++)
            {
		        _ElementInfo* elem = new _ElementInfo;
                elem->type = Uint;
                elem->offset = offset;
                eiv.push_back(elem);
                offset += 4;
                num_elements++;
            }
            num4s_left--;
        }

        if (e->Type() == Element::Int)
        {
            for ( unsigned int i = 0; i < e->NumElements(); i++)
            {
		        _ElementInfo* elem = new _ElementInfo;
                elem->type = Int;
                elem->offset = offset;
                eiv.push_back(elem);
                offset += 4;
                num_elements++;
            }
            num4s_left--;
        }

        if (e->Type() == Element::Float)
        {
            for ( unsigned int i = 0; i < e->NumElements(); i++)
            {
		        _ElementInfo* elem = new _ElementInfo;
                elem->type = Float;
                elem->offset = offset;
                eiv.push_back(elem);
                offset += 4;
                num_elements++;
            }
            num4s_left--;
        }

        if (e->Type() == Element::Struct && e->GetStruct()->Align() == 4)
            for ( unsigned int i = 0; i<e->NumElements(); i++)
			{
                std::vector<_ElementInfo*> struct_eiv =
					e->GetStruct()->GenerateElementData(offset, owner);
				num_elements += struct_eiv.size();

				eiv.insert(eiv.end(), struct_eiv.begin(), struct_eiv.end());
			}
    }

    // output 2 byte-aligned elements
    ei = m_ElementMap->begin();
    for (; num2s_left > 0 && ei != m_ElementMap->end(); ei++)
    {
        Element* e = (*ei).second;
        if (e->Type() == Element::Ushort)
        {
            if (e->Name() == "_SequenceNum")
                owner->SetSequenceNumOffset(offset);

            for ( unsigned int i = 0; i < e->NumElements(); i++)
            {
		        _ElementInfo* elem = new _ElementInfo;
                elem->type = Ushort;
                elem->offset = offset;
                eiv.push_back(elem);
                offset += 2;
                num_elements++;
            }
            num2s_left--;
        }

        if (e->Type() == Element::Short)
        {
            for ( unsigned int i = 0; i < e->NumElements(); i++)
            {
		        _ElementInfo* elem = new _ElementInfo;
                elem->type = Short;
                elem->offset = offset;
                eiv.push_back(elem);
                offset += 2;
                num_elements++;
            }
            num2s_left--;
        }

        if (e->Type() == Element::Struct && e->GetStruct()->Align() == 2)
            for ( unsigned int i = 0; i<e->NumElements(); i++)
			{
                std::vector<_ElementInfo*> struct_eiv =
					e->GetStruct()->GenerateElementData(offset, owner);
				num_elements += struct_eiv.size();

				eiv.insert(eiv.end(), struct_eiv.begin(), struct_eiv.end());
			}
    }

    // output unaligned elements
    ei = m_ElementMap->begin();
    for (; num1s_left > 0 && ei != m_ElementMap->end(); ei++)
    {
        Element* e = (*ei).second;
        if (e->Type() == Element::Uchar)
        {
            if (e->Name() == "_SenderIndex")
                owner->SetSenderIndexOffset(offset);

            if (e->Name() == "_ReceiverIndex")
                owner->SetReceiverIndexOffset(offset);

            for ( unsigned int i = 0; i<e->NumElements(); i++)
            {
		        _ElementInfo* elem = new _ElementInfo;
                elem->type = Uchar;
                elem->offset = offset;
                eiv.push_back(elem);
                offset += 1;
                num_elements++;
            }
            num1s_left--;
        }

        if (e->Type() == Element::Char)
        {
            for ( unsigned int i = 0; i<e->NumElements(); i++)
            {
		        _ElementInfo* elem = new _ElementInfo;
                elem->type = Char;
                elem->offset = offset;
                eiv.push_back(elem);
                offset += 1;
                num_elements++;
            }
            num1s_left--;
        }

        if (e->Type() == Element::Struct && e->GetStruct()->Align() == 1)
            for ( unsigned int i = 0; i<e->NumElements(); i++)
			{
                std::vector<_ElementInfo*> struct_eiv =
					e->GetStruct()->GenerateElementData(offset, owner);
				num_elements += struct_eiv.size();

				eiv.insert(eiv.end(), struct_eiv.begin(), struct_eiv.end());
			}
    }

    assert(m_Align > 0);

    if (offset % m_Align != 0)
        offset += m_Align - (offset % m_Align);

    return eiv;
}

void Struct::Initialize()
{
    s_SequenceNum = 0;
    StructMap.clear();
}
