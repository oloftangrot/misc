#ifndef LU_MATRIX_H
#define LU_MATRIX_H

#include <Vector.h>
#include <Matrix.h>

template <int rows, int cols, class T=double> class LU_Matrix
{
    Matrix<rows, cols, T> m_lu;
    Vector<rows, int> m_pivot;
    bool m_even_num_swaps;

public:
    inline LU_Matrix(const Matrix<rows, cols, T> &m, bool &is_singular);

    inline Matrix<rows, cols, T> Inverse();
    inline T Determinant();
    inline Vector<rows, T> Solve(const Vector<rows, T> &b);

    inline T max(T e1, T e2) { return (e1 > e2) ? e1 : e2; }
    inline void swap(T& e1, T& e2) { T tmp=e1; e1=e2; e2=tmp; }
    inline T abs(T e1) { return (e1 >= T(0)) ? e1 : -e1; }
};

// Implementation of inline functions:

template <int rows, int cols, class T>
LU_Matrix<rows, cols, T>::LU_Matrix(const Matrix<rows, cols, T> &m,
                                    bool &is_singular)
{
    assert(rows == cols);
    is_singular = false;

    m_even_num_swaps = true; // zero row swaps so far

    m_lu = m; // copy matrix for in-place transformation
    Vector<rows> v; // v stores implicit scaling for each row

    // loop over rows to get implicit scaling information
    for (int i=0; i<rows; i++)
    {
        T row_max = T(0);

        for (int j=0; j<cols; j++)
            row_max = max(row_max, fabs(m_lu[i][j]));

        if (row_max == T(0))
        {
            is_singular = true;
            return;
        }

        v[i] = T(1) / row_max; // save the scaling
    }

    // loop over columns (Crout's decomposition method)
    for ( int j = 0; j < cols; j++ )
    {
        for ( int i = 0; i < j; i++ )
        {
            T sum = m_lu[i][j];
            for (int k=0; k<i; k++)
                sum -= m_lu[i][k]*m_lu[k][j];

            m_lu[i][j] = sum;
        }

        T pivot_max = T(0); // start search for pivot element
        int pivot_index, i;
        for ( i = j; i < rows; i++ )
        {
            T sum = m_lu[i][j];
            for (int k=0; k<j; k++)
                sum -= m_lu[i][k]*m_lu[k][j];

            m_lu[i][j] = sum;

            T pivot_fom = v[i]*fabs(sum); // pivot figure of merit
            if (pivot_fom > pivot_max) // save best pivot so far
            {
                pivot_max = pivot_fom;
                pivot_index = i;
            }
        }

        if (j != pivot_index) // then need to interchange rows
        {
            for (int k=0; k<cols; k++)
                swap(m_lu[pivot_index][k], m_lu[j][k]);

            m_even_num_swaps = !m_even_num_swaps; // toggle swap parity
            v[pivot_index] = v[j];
        }

        m_pivot[j] = pivot_index;
        T pivot = m_lu[j][j];
        if (pivot == T(0))
        {
            is_singular = true;
            return;
        }

        if (j != rows-1)
        {
            T mult = T(1) / pivot;

            for (i=j+1; i<rows; i++)
                m_lu[i][j] *= mult;
        }
    }
}

template <int rows, int cols, class T>
T LU_Matrix<rows, cols, T>::Determinant()
{
    T result = m_even_num_swaps ? T(1) : T(-1);

    for (int j=0; j<cols; j++)
        result *= m_lu[j][j];

    return result;
}

template <int rows, int cols, class T>
Matrix<rows, cols, T> LU_Matrix<rows, cols, T>::Inverse()
{
    Matrix<rows, cols, T> result;

    for (int j=0; j<cols; j++)
    {
        Vector<cols> col; // zeros all elements
        col[j] = T(1);

        Vector <cols> solution = Solve(col);
        for (int i=0; i<cols; i++)
            result[i][j] = solution[i];
    }

    return result;
}

template <int rows, int cols, class T>
Vector<rows, T> LU_Matrix<rows, cols, T>::Solve(const Vector<rows, T> &b)
{
    Vector<rows, T> result=b;

    int ii=-1;   // ii gets set to the index of the 1st nonzero element of b
		int i;

    for ( i = 0; i < rows; i++)
    {
        int ip = m_pivot[i];
        T sum = result[ip];
        result[ip] = result[i];

        if (ii >= 0) // first nonzero element of b has been found
            for (int j=ii; j<=i-1; j++)
                sum -= m_lu[i][j]*result[j];
        else
            if (sum != T(0))
                ii = i;

        result[i] = sum;
    }

    for ( i = rows - 1; i >= 0; i-- )
    {
        T sum = result[i];
        for (int j=i+1; j<cols; j++)
            sum -= m_lu[i][j]*result[j];

        result[i] = sum / m_lu[i][i];
    }

    return result;
}

#endif // LU_MATRIX_H
