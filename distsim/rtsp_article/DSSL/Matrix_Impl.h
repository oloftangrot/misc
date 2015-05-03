// Implementation of inline Matrix functions:

template <int rows, int cols, class T>
Matrix<rows, cols, T>::Matrix(MatrixInitValType init_val) 
{
	for (int i=0; i<rows; i++)
        for (int j=0; j<cols; j++)
            if ((init_val == Identity) && (i == j))
                m_row[i][j] = T(1);
            else
        		m_row[i][j] = T(0);
}

template <int rows, int cols, class T>
Matrix<rows, cols, T>::Matrix(T elem) 
{
	for (int i=0; i<rows; i++)
		m_row[i] = Vector<cols, T>(elem);
}

template <int rows, int cols, class T>
Matrix<rows, cols, T>::Matrix(const T *elem_list,
    MatrixFillType fill_type) 
{
    switch (fill_type)
    {
    case Full:
        {
    	    for (int i=0; i<rows; i++)
	    	    m_row[i] = Vector<cols, T>(&elem_list[i*cols]);

            break;
        }
    case Diagonal:
        {
            assert(rows == cols);

    	    for (int i=0; i<rows; i++)
            {
	    	    m_row[i] = Vector<cols, T>();
                m_row[i][i] = elem_list[i];
            }
            break;
        }
    case Symmetric:
        {
            assert(rows == cols);

            for (int i=0; i<rows; i++)
	    	    m_row[i] = Vector<cols, T>();

            int k = 0;
            for ( int i = 0; i < rows; i++ )
                for ( int j = i; j < cols; j++ )
                {
                    m_row[i][j] = elem_list[k];
                    m_row[j][i] = elem_list[k++];
                }

            break;
        }
    };
}

template <int rows, int cols, class T>
Vector<cols, T>& Matrix<rows, cols, T>::operator[](int index) 
{
	assert(index >= 0);
	assert(index < rows);

	return m_row[index];
}

template <int rows, int cols, class T>
const Vector<cols, T>& Matrix<rows, cols, T>::operator[](int index) const
{
	assert(index >= 0);
	assert(index < rows);

	return m_row[index];
}

template <int rows, int cols, class T>
Matrix<rows, cols, T> Matrix<rows, cols, T>::operator-() const 
{
	Matrix<rows, cols, T> result;

	for (int i=0; i<rows; i++)
		result.m_row[i] = -m_row[i];

	return result;
}

template <int rows, int cols, class T>
Matrix<rows, cols, T>& Matrix<rows, cols, T>::operator=(const Matrix& m)
{
	if (&m == this)
		return *this;

	for (int i=0; i<rows; i++)
		m_row[i] = m.m_row[i];

	return *this;
}

template <int rows, int cols, class T>
Matrix<rows, cols, T>& Matrix<rows, cols, T>::operator+=(const Matrix& m) 
{
	for (int i=0; i<rows; i++)
		m_row[i] += m.m_row[i];

	return *this;
}

template <int rows, int cols, class T>
Matrix<rows, cols, T>& Matrix<rows, cols, T>::operator-=(const Matrix& m) 
{
	for (int i=0; i<rows; i++)
		m_row[i] -= m.m_row[i];

	return *this;
}

template <int rows, int cols, class T>
Matrix<rows, cols, T>& Matrix<rows, cols, T>::operator*=(const Matrix& m) 
{
    *this = *this * m;
	return *this;
}

template <int rows, int cols, class T>
Matrix<rows, cols, T>& Matrix<rows, cols, T>::operator+=(T r) 
{
	for (int i=0; i<rows; i++)
        for (int j=0; j<cols; j++)
    		m_row[i][j] += r;

	return *this;
}

template <int rows, int cols, class T>
Matrix<rows, cols, T>& Matrix<rows, cols, T>::operator-=(T r) 
{
	for (int i=0; i<rows; i++)
        for (int j=0; j<cols; j++)
    		m_row[i][j] -= r;

	return *this;
}

template <int rows, int cols, class T>
Matrix<rows, cols, T>& Matrix<rows, cols, T>::operator*=(T r) 
{
	for (int i=0; i<rows; i++)
        for (int j=0; j<cols; j++)
    		m_row[i][j] *= r;

	return *this;
}

template <int rows, int cols, class T>
Matrix<rows, cols, T>& Matrix<rows, cols, T>::operator/=(T r) 
{
	for (int i=0; i<rows; i++)
        for (int j=0; j<cols; j++)
    		m_row[i][j] /= r;

	return *this;
}

template <int rows, int cols, class T>
const Matrix<cols, rows, T> Matrix<rows, cols, T>::Transpose() const
{
    Matrix<cols, rows, T> result;

	for (int i=0; i<rows; i++)
		for (int j=0; j<cols; j++)
            result.m_row[j][i] = m_row[i][j];

    return result;
}

template <int rows, int cols, class T>
const Matrix<rows, cols, T> Matrix<rows, cols, T>::operator*(
    const Matrix<rows, cols, T>& m) const
{
	Matrix<rows, cols, T> result;

	for (int i=0; i<rows; i++)
		for (int j=0; j<cols; j++)
			for (int k=0; k<rows; k++)
				result.m_row[i][j] += m_row[i][k] * m.m_row[k][j];

	return result;
}

template <int rows, int cols, class T>
const Matrix<rows, cols, T> Matrix<rows, cols, T>::operator+(
    const Matrix<rows, cols, T>& m) const
{
	Matrix<rows, cols, T> result;

	for (int i=0; i<rows; i++)
		for (int j=0; j<cols; j++)
			result.m_row[i][j] = m_row[i][j] + m.m_row[i][j];

	return result;
}

template <int rows, int cols, class T>
const Matrix<rows, cols, T> Matrix<rows, cols, T>::operator-(
    const Matrix<rows, cols, T>& m) const
{
	Matrix<rows, cols, T> result;

	for (int i=0; i<rows; i++)
		for (int j=0; j<cols; j++)
			result.m_row[i][j] = m_row[i][j] - m.m_row[i][j];

	return result;
}

template <int rows, int cols, class T>
const Matrix<rows, cols, T> operator+(const Matrix<rows, cols, T>& m1, 
                                      const Matrix<rows, cols, T>& m2) 
{
	Matrix<rows, cols, T> result;

	for ( int i = 0; i < rows; i++ )
		result.m_row[i] = m1.m_row[i] + m2.m_row[i];

	return result;
}

template <int rows, int cols, class T>
const Matrix<rows, cols, T> operator-(const Matrix<rows, cols, T>& m1, 
                                      const Matrix<rows, cols, T>& m2) 
{
	Matrix<rows, cols, T> result;

	for (int i=0; i<rows; i++)
		result.m_row[i] = m1.m_row[i] - m2.m_row[i];

	return result;
}

template <int rows, int cols, class T>
const Matrix<rows, cols, T> operator*(const Matrix<rows, cols, T>& m1, 
                                      const Matrix<rows, cols, T>& m2) 
{
	Matrix<rows, cols, T> result;

	for (int i=0; i<rows; i++)
		for (int j=0; j<cols; j++)
			for (int k=0; k<rows; k++)
				result.m_row[i][j] += m1.m_row[i][k] * m2.m_row[k][j];

	return result;
}

#if 0
template <int rows, int cols, class T>
const Matrix<rows, cols, T> operator+(const Matrix<rows, cols, T>& m, T r) 
{
	Matrix<rows, cols, T> result;

	for (int i=0; i<rows; i++)
		result.m_row[i] = m.m_row[i] + r;

	return result;
}

template <int rows, int cols, class T>
const Matrix<rows, cols, T> operator-(const Matrix<rows, cols, T>& m, T r) 
{
	Matrix<rows, cols, T> result;

	for (int i=0; i<rows; i++)
		result.m_row[i] = m.m_row[i] - r;

	return result;
}

template <int rows, int cols, class T>
const Matrix<rows, cols, T> operator*(const Matrix<rows, cols, T>& m, T r) 
{
	Matrix<rows, cols, T> result;

	for (int i=0; i<rows; i++)
		result.m_row[i] = m.m_row[i] * r;

	return result;
}

template <int rows, int cols, class T>
const Matrix<rows, cols, T> operator/(const Matrix<rows, cols, T>& m, T r) 
{
	Matrix<rows, cols, T> result;

	for (int i=0; i<rows; i++)
		result.m_row[i] = m.m_row[i] / r;

	return result;
}
#endif

template <int rows, int cols, class T>
const Matrix<rows, cols, T> operator/(T r, const Matrix<rows, cols, T>& m)
{
	Matrix<rows, cols, T> result;

	for (int i=0; i<rows; i++)
		result.m_row[i] = r / m.m_row[i];

	return result;
}

template <int rows, int cols, class T>
const Vector<rows, T> operator*(const Matrix<rows, cols, T>& m,
                                const Vector<cols, T>& v) 
{
	Vector<rows, T> result;

	for (int i=0; i<rows; i++)
		for (int j=0; j<cols; j++)
			result[i] += m.m_row[i][j] * v[j];

	return result;
}

