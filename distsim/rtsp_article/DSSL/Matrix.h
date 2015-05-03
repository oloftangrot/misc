#ifndef MATRIX_H
#define MATRIX_H

#include <Vector.h>

template <int rows, int cols, class T=double> class Matrix
{
public:
  // Different ways of specifying initial values
  enum MatrixInitValType { Zero, Identity };
  enum MatrixFillType { Full, Diagonal, Symmetric };

  inline Matrix(MatrixInitValType init_val = Zero);
	inline Matrix(T elem);  // Fill matrix with elem
	inline Matrix(const T *elem_list, MatrixFillType fill_type = Full);

	inline Vector<cols, T>& operator[](int index);
	inline const Vector<cols, T>& operator[](int index) const;
	inline Matrix operator-() const;

	inline Matrix& operator=(const Matrix& m);
	inline Matrix& operator+=(const Matrix& m);
	inline Matrix& operator-=(const Matrix& m);
	inline Matrix& operator*=(const Matrix& m);

	inline Matrix& operator+=(T r);
	inline Matrix& operator-=(T r);
	inline Matrix& operator*=(T r);
	inline Matrix& operator/=(T r);

  inline const Matrix<cols, rows, T> Transpose() const;

  int Rows() const { return rows; }
  int Cols() const { return cols; }

  inline const Matrix operator*(const Matrix& m) const;
  inline const Matrix operator+(const Matrix& m) const;
  inline const Matrix operator-(const Matrix& m) const;

  inline friend const Matrix operator+(const Matrix& m1, const Matrix& m2);
  inline friend const Matrix operator-(const Matrix& m1, const Matrix& m2);
  inline friend const Matrix operator*(const Matrix& m1, const Matrix& m2);

  //inline friend const Matrix operator+(const Matrix& m, T r);
  //inline friend const Matrix operator-(const Matrix& m, T r);
  //inline friend const Matrix operator*(const Matrix& m, T r);
  //inline friend const Matrix operator/(const Matrix& m, T r);

  friend const Matrix operator+(T r, const Matrix& m) { return  m + r; }
  friend const Matrix operator-(T r, const Matrix& m) { return -m + r; }
  friend const Matrix operator*(T r, const Matrix& m) { return  m * r; }
  inline friend const Matrix operator/(T r, const Matrix& m);

	inline friend const Vector<rows, T> operator*(const Matrix& m,
        const Vector<cols, T>& v);

private:
	Vector<cols, T> m_row[rows];
};

#include <Matrix_Impl.h>

#endif // MATRIX_H
