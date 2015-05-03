#ifndef INTERP_H
#define INTERP_H

// Function Interpolation

#include "Bkpt.h"

class Interp
{
public:
    Interp() : m_BkptsValid(false), m_DataValid(false) {}
    virtual double operator () () = 0;

protected:
    bool m_BkptsValid, m_DataValid;
};

template<int nDimensions>
class LinearInterp : public Interp
{
public:
    LinearInterp();
    LinearInterp(const LinearInterp& linear_interp);
    ~LinearInterp();

    void SetDimension(int index, const Bkpt* bkpt);
    void SetupData(const char* data_file);
    void SetupData(const double* data);
    virtual double operator () ();

private:
    const Bkpt* m_Bkpt[nDimensions];
    int m_Offset[nDimensions];
    double* m_Data;
};

#include "Interp_Impl.h"

#endif // INTERP_H