// Struct.h: interface for the Struct class.

#ifndef STRUCT_H
#define STRUCT_H

#include <Element.h>

#include <RTSP.h>
#include <RTSP_Internals.h>

class Message;

class Struct
{
public:
    Struct(const ElementMapType* element_map=0);
    void SetName(std::string name) { m_Name = name; }

    void AddElement(Element e);
    std::string Name() const { return m_Name; }
    uint Align() const { return m_Align; }
    uint Size() const { return m_Size; }
    uint SequenceNum() const { return m_SequenceNum; }
    void FindSubStructs(StructSeqType& structs);

    void GenerateCppCode(FILE *iov, bool is_message=false,
        bool gen_send_code=true, int seq_num=0, std::string owner="") const;
    uint OutputElementInfo(FILE* iov, uint& offset, Message* owner) const;
    std::vector<_ElementInfo*> GenerateElementData(uint& offset, 
		Message* owner) const;

    static void Initialize();

private:
    std::string m_Name;
    const ElementMapType* m_ElementMap;
    uint m_Align, m_Num1s, m_Num2s, m_Num4s, m_Num8s, m_Size, m_SequenceNum;
    uint m_PadInts, m_PadShorts, m_PadChars;

    static uint s_SequenceNum;
};

#endif
