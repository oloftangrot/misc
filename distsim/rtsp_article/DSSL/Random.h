#ifndef RANDOM_H
#define RANDOM_H

class Random
{
public:
    Random() : m_initialized(false), m_iset(false) { }

    // This routine uses Knuth's subtractive method for a portable random
    // number generator. It returns a value in the range 0..1. Set initialize
    // true to initialize or reinitialize the generator. The seed should be an
    // integer greater than zero. Note that initializing this routine also
    // initializes the sequence for gauss() below. If you don't call ran() with
    // initialize set true on the first call it will initialize with the
    // value of seed that is passed, or with a seed of 1 if no value is passed.
    double Uniform(bool initialize=false, int seed=1);

    // This routine uses the Box-Muller method to generate random deviates. It
    // internally saves the second value of each pair and returns it on
    // alternate calls. The value returned has a mean of zero and a variance
    // of unity. The initialize and seed parameters behave the same as in
    // ran() above, and since gauss() calls ran() initializing one also
    // initializes the other.
    double Normal(bool initialize=false, int seed=1);

private:
    enum { Len = 56 };
    bool m_initialized, m_iset;
    int m_ma[Len], m_inext, m_inextp;
    double m_fac, m_gset;
};

#endif // RANDOM_H
