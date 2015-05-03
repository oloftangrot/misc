#include "Vector.h"
#include "Matrix.h"

template <int rows, int cols, int cols2, class T>
Matrix<rows, cols2, T> operator*(Matrix<rows, cols, T>& m1,
                                 Matrix<cols, cols2, T>& m2) {
	Matrix<rows, cols, T> result;

	for (int i=0; i<rows; i++)
		for (int j=0; j<cols2; j++)
			for (int k=0; k<cols; k++)
				result[i][j] += m1[i][k]*m2[k][j];

	return result;
}

template <int dim, class T>
Matrix<1, dim, T> VTranspose(Vector<dim, T>& v)
{
    Matrix<1, dim, T> result;
    for (int i=0; i<dim; i++)
        result[0][i] = v[i];

    return result;
}

template <int rows, int cols, class T>
Matrix<cols, rows, T> MTranspose(Matrix<rows, cols, T>& m) {
	Matrix<cols, rows, T> result;

	for (int i=0; i<rows; i++)
		for (int j=0; j<cols; j++)
			result[j][i] = m[i][j];

	return result;
}

template <int dim, class T>
Matrix<dim, 1, T> ToMatrix(Vector<dim, T>& v)
{
    Matrix<dim, 1, T> result;

    for (int i=0; i<dim; i++)
        result[i][0] = v[i];

    return result;
}

template <int dim, class T>
Vector<dim, T> ToVector(Matrix<dim, 1, T>& m)
{
    Vector<dim, T> result;

    for ( int i = 0; i < dim; i++)
        result[i] = m[i][0];

    return result;
}
