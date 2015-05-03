// Element.h: interface for the Element class.

#ifndef ELEMENT_H
#define ELEMENT_H

#include <Types.h>

#include <vector>

class Element  
{
public:
    enum ElType
    {
        Undefined,
        Char,
        Uchar,
        Short,
        Ushort,
        Int,
        Uint,
        Float,
        Double,
        Struct
    };

    Element(ElType el_type = Undefined);

    void SetName(std::string name) { m_Name = name; }
    void SetType(ElType el_type);
    void SetStruct(class Struct* s);
    void AddDimension(uint dim);

    std::string Name() const { return m_Name; }
    ElType Type() const { return m_ElType; }
    uint NumDimensions() const { return m_Dimension.size(); }
    uint Dimension(uint dim_num) const;
    uint Size() const { return m_Size; }
    uint Align() const { return m_Align; }
    uint NumElements() const { return m_NumElements; }
    class Struct* GetStruct() const { return m_Struct; }

    void GenerateCppCode(FILE *iov) const;

private:
    std::string m_Name;
    ElType m_ElType;
    uint m_Size, m_Align, m_NumElements;
    class Struct* m_Struct;
    std::vector<uint> m_Dimension;
};

#endif
