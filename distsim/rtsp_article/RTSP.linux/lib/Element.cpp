// Element.cpp: implementation of the Element class.

#include "Element.h"
#include "Struct.h"

#include <assert.h>

Element::Element(ElType el_type) : m_Struct(0)
{
    SetType(el_type);
}

void Element::SetType(ElType el_type)
{
    m_ElType = el_type;

    m_NumElements = 1;
    for (uint i=0; i<NumDimensions(); i++)
        m_NumElements *= Dimension(i);

    switch (m_ElType)
    {
    case Char:
    case Uchar:
        m_Align = 1;
        m_Size = m_Align * m_NumElements;
        break;

    case Short:
    case Ushort:
        m_Align = 2;
        m_Size = m_Align * m_NumElements;
        break;

    case Int:
    case Uint:
    case Float:
        m_Align = 4;
        m_Size = m_Align * m_NumElements;
        break;

    case Double:
        m_Align = 8;
        m_Size = m_Align * m_NumElements;
        break;

    case Struct:
        if (m_Struct)
        {
            m_Align = m_Struct->Align();
            m_Size = m_Struct->Size() * m_NumElements;
        }
        break;

    default:
        m_Align = 0;
        m_Size = 0;
        break;
    }
}

void Element::SetStruct(class Struct* s)
{
    assert(m_ElType == Element::Struct);

    m_Struct = s;

    SetType(m_ElType); // to recalculate m_Align, m_Size & m_NumElements
}

void Element::AddDimension(uint dim)
{
    m_Dimension.push_back(dim);

    SetType(m_ElType); // to recalculate m_Align, m_Size & m_NumElements
}

uint Element::Dimension(uint dim_num) const 
{ 
    assert(dim_num < NumDimensions());

    return m_Dimension[dim_num]; 
}

void Element::GenerateCppCode(FILE *iov) const
{
    fprintf(iov, "        ");

    switch (m_ElType)
    {
    case Char:
        fprintf(iov, "char ");
        break;
    case Uchar:
        fprintf(iov, "uchar ");
        break;
    case Short:
        fprintf(iov, "short ");
        break;
    case Ushort:
        fprintf(iov, "ushort ");
        break;
    case Int:
        fprintf(iov, "int ");
        break;
    case Uint:
        fprintf(iov, "uint ");
        break;
    case Float:
        fprintf(iov, "float ");
        break;
    case Double:
        fprintf(iov, "double ");
        break;
    case Struct:
        fprintf(iov, "%s ", m_Struct->Name().c_str());
        break;
    default:
        assert(0);
        break;
    }

    fprintf(iov, "%s", m_Name.c_str());

    std::vector<uint>::const_iterator vi = m_Dimension.begin();
    for (; vi != m_Dimension.end(); vi++)
        fprintf(iov, "[%d]", *vi);

    fprintf(iov, ";\n");
}
