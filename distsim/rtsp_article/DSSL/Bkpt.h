#ifndef BKPT_H
#define BKPT_H

// Breakpoint classes

class Bkpt
{
public:
    struct BkptData
    {
        int m_LowerBkpt; // Index of lower bkpt containing the input value
        double m_Fraction; // Fraction of the way from lower bkpt to lower+1
    };

    Bkpt() : m_DataValid(false) {}
    virtual void operator () (double x) = 0;
    virtual double operator[](int index) const = 0;
    virtual int Size() const = 0;

    BkptData m_BkptData;

protected:
    bool m_DataValid;
};

template<int nBkpts>
class EqSpacedBkpt : public Bkpt
{
public:
    EqSpacedBkpt(double x_min, double dx) { SetupData(x_min, dx); }

    void SetupData(double x_min, double dx);
    virtual void operator () (double x);
    virtual double operator[](int index) const;
    virtual int Size() const { return nBkpts; }

private:
    double m_Xmin, m_Dx;
};

template<int nBkpts>
class UneqSpacedBkpt : public Bkpt
{
public:
    UneqSpacedBkpt(const char* data_file);
    UneqSpacedBkpt(const double* data);

    void SetupData(const char* data_file);
    virtual void operator () (double x);
    virtual double operator[](int index) const;
    virtual int Size() const { return nBkpts; }

private:
    double m_BkptList[nBkpts];
    int m_PrevLowerBkpt;
};

#include "Bkpt_Impl.h"

#endif // BKPT_H