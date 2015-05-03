// Implementations of the template functions defined in Bkpt.h

#include <cstdio>
#include <cassert>

template<int nDimensions>
LinearInterp<nDimensions>::LinearInterp() : m_Data(0)
{
    for (int i=0; i<nDimensions; i++)
        m_Bkpt[i] = 0;
}

template<int nDimensions>
LinearInterp<nDimensions>::LinearInterp(const LinearInterp& linear_interp)
{
    m_BkptsValid = linear_interp.m_BkptsValid;
    m_DataValid = linear_interp.m_DataValid;

    if (m_DataValid)
    {
        m_Data = new double[linear_interp.m_Offset[0]];
        memcpy(m_Data, linear_interp.m_Data,
            linear_interp.m_Offset[0]*sizeof(double));
    }
    else
        m_Data = 0;

    memcpy(m_Bkpt, linear_interp.m_Bkpt, nDimensions*sizeof(Bkpt*));
    memcpy(m_Offset, linear_interp.m_Offset, nDimensions*sizeof(double));
}

template<int nDimensions>
LinearInterp<nDimensions>::~LinearInterp()
{
    if (m_DataValid)
        delete m_Data;
}

template<int nDimensions>
void LinearInterp<nDimensions>::SetDimension(int index, const Bkpt* bkpt)
{
    assert(index >= 0 && index < nDimensions);

    m_Bkpt[nDimensions-index-1] = bkpt;

    // If all breakpoint pointers are nonzero set m_BkptsValid
    m_BkptsValid = true;
    for (int i=0; i<nDimensions; i++)
        if (!m_Bkpt[i])
            m_BkptsValid = false;

    // If all breakpoints are set, compute data offsets for each dimension
    if (m_BkptsValid)
        for (int i=0; i<nDimensions; i++)
        {
            m_Offset[i] = 1;

            for (int j=1; j<=i; j++)
                m_Offset[i] *= m_Bkpt[j]->Size();
        }
}

template<int nDimensions>
void LinearInterp<nDimensions>::SetupData(const char* data_file)
{
    assert(m_BkptsValid);
    assert(!m_DataValid);

    int size = 1;
    m_Offset[0] = 1;
    int i;

    for ( i = 1; i < nDimensions; i++)
    {
        size *= m_Bkpt[i]->Size();
        m_Offset[i] = size;
    }

    size = size * m_Bkpt[0]->Size();
    m_Data = new double[size];

    FILE* iov = fopen(data_file, "r");
    assert(iov);

    for ( i = 0; i < size; i++)
    {
        if (fscanf(iov, "%lg", &m_Data[i]) == EOF)
            break;
    }

    fclose(iov);

    if (i == size)
        m_DataValid = true;
    else
        delete m_Data;
}

template<int nDimensions>
void LinearInterp<nDimensions>::SetupData(const double* data)
{
    assert(m_BkptsValid);
    assert(!m_DataValid);

    int size = 1;
    m_Offset[0] = 1;
		int i;

    for ( i = 1; i < nDimensions; i++)
    {
        size *= m_Bkpt[i]->Size();
        m_Offset[i] = size;
    }

    size = size * m_Bkpt[0]->Size();
    m_Data = new double[size];

    for ( i = 0; i < size; i++)
        m_Data[i] = data[i];

    m_DataValid = true;
}

template<int nDimensions>
double LinearInterp<nDimensions>::operator () ()
{
    assert(m_BkptsValid);
    assert(m_DataValid);

    double result = 0;

    // Loop over each breakpoint in the hypercube surrounding the input point
    for (int i=0; i<(1<<nDimensions); i++)
    {
        // Compute the index & weight of this bkpt in m_Data
        int index = 0;
        double weight = 1;

        // Compute the index into m_Data and weight for the current bkpt
        for (int j=0; j<nDimensions; j++)
        {
            int bit = (i >> j) & 1; // Zero for lower bkpt, 1 for upper bkpt

            index += (m_Bkpt[j]->m_BkptData.m_LowerBkpt + bit) * m_Offset[j];

            if (bit)
                weight *= m_Bkpt[j]->m_BkptData.m_Fraction;   // Upper bkpt
            else
                weight *= 1-m_Bkpt[j]->m_BkptData.m_Fraction; // Lower bkpt
        }

        // Add the contribution from the current breakpoint to the sum
        result += m_Data[index] * weight;
    }

    return result;
}
