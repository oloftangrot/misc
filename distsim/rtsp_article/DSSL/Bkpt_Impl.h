// Implementations of the template functions defined in Bkpt.h

#include <cstdio>
#include <cassert>
#include <cmath>

template<int nBkpts>
void EqSpacedBkpt<nBkpts>::SetupData(double x_min, double dx)
{
    assert(dx > 0);

    m_Xmin      = x_min;
    m_Dx        = dx;
    m_DataValid = true;
}

template<int nBkpts>
void EqSpacedBkpt<nBkpts>::operator () (double x)
{
    assert(m_DataValid);
    assert(x >= m_Xmin);
    assert(x <= m_Xmin + (nBkpts-1) * m_Dx);

    m_BkptData.m_LowerBkpt = int((x - m_Xmin) / m_Dx);

    if (m_BkptData.m_LowerBkpt == nBkpts-1)
        m_BkptData.m_LowerBkpt--;

    m_BkptData.m_Fraction  = (x - (m_Xmin + m_BkptData.m_LowerBkpt*m_Dx)) /
        m_Dx;
}

template<int nBkpts>
double EqSpacedBkpt<nBkpts>::operator[](int index) const
{
    assert(m_DataValid);
    assert(index >= 0 && index < nBkpts);

    return m_Xmin + index*m_Dx;
}

template<int nBkpts>
UneqSpacedBkpt<nBkpts>::UneqSpacedBkpt(const char* data_file)
{
    SetupData(data_file);
}

template<int nBkpts>
UneqSpacedBkpt<nBkpts>::UneqSpacedBkpt(const double* data)
{
    for (int i=0; i<nBkpts; i++)
        m_BkptList[i] = data[i];

    m_PrevLowerBkpt = 0;
}

template<int nBkpts>
void UneqSpacedBkpt<nBkpts>::SetupData(const char* data_file)
{
    FILE* iov = fopen(data_file, "r");
    assert(iov);
		int i;

    for ( i = 0; i < nBkpts; i++)
    {
        if (fscanf(iov, "%lg", &m_BkptList[i]) == EOF)
            break;

        assert(i == 0 || m_BkptList[i] > m_BkptList[i-1]);
    }

    fclose(iov);

    if ( i == nBkpts )
        m_DataValid = true;

    m_PrevLowerBkpt = 0;
}

template<int nBkpts>
void UneqSpacedBkpt<nBkpts>::operator () (double x)
{
    assert(m_DataValid);
    assert(x >= m_BkptList[0] && x <= m_BkptList[nBkpts-1]);

    int lower;
    if (x >= m_BkptList[m_PrevLowerBkpt] && x <= m_BkptList[m_PrevLowerBkpt+1])
    {
        // Still in the same interval as last time; No need to search
        lower = m_PrevLowerBkpt;
    }
    else if (m_PrevLowerBkpt > 0 && x >= m_BkptList[m_PrevLowerBkpt-1] &&
        x <= m_BkptList[m_PrevLowerBkpt])
    {
        // The input moved to the next lower breakpoint interval
        lower = m_PrevLowerBkpt - 1;
    }
    else if (m_PrevLowerBkpt < nBkpts-2 && x >= m_BkptList[m_PrevLowerBkpt+1]
        && x <= m_BkptList[m_PrevLowerBkpt+2])
    {
        // The input moved to the next higher breakpoint interval
        lower = m_PrevLowerBkpt + 1;
    }
    else
    {
        // Perform bisection
        lower = 0;
        int upper = nBkpts-1;
        while (upper-lower > 1)
        {
            int mid = (upper+lower) / 2;

            if (m_BkptList[mid] > x)
                upper = mid;
            else
                lower = mid;
        }
    }

    m_PrevLowerBkpt = lower;
    m_BkptData.m_LowerBkpt = lower;

    m_BkptData.m_Fraction = (x - m_BkptList[lower]) /
        (m_BkptList[lower+1] - m_BkptList[lower]);
}

template<int nBkpts>
double UneqSpacedBkpt<nBkpts>::operator[](int index) const
{
    assert(m_DataValid);
    assert(index >= 0 && index < nBkpts);

    return m_BkptList[index];
}
