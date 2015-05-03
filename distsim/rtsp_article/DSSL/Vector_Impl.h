// Implementation of inline Vector functions:

#include <assert.h>

template <int dim, class T>
Vector<dim, T>::Vector(T elem)
//Vector<dim, T>::Vector
{
	for (int i=0; i<dim; i++)
		m_element[i] = elem;
}

template <int dim, class T>
Vector<dim, T>::Vector(const T *elem_list)
{
	for (int i=0; i<dim; i++)
		m_element[i] = elem_list[i];
}

template <int dim, class T>
T &Vector<dim, T>::operator[](int index)
{
	assert(index >= 0);
	assert(index < dim);

	return m_element[index];
}

template <int dim, class T>
T Vector<dim, T>::operator[](int index) const
{
	assert(index >= 0);
	assert(index < dim);

	return m_element[index];
}

template <int dim, class T>
const Vector<dim, T> Vector<dim, T>::operator-() const
{
	Vector result;

	for (int i=0; i<dim; i++)
		result.m_element[i] = -m_element[i];

	return result;
}

template <int dim, class T>
Vector<dim, T> &Vector<dim, T>::operator=(const Vector& v)
{
	if (&v == this)
		return *this;

	for (int i=0; i<dim; i++)
		m_element[i] = v.m_element[i];

	return *this;
}

template <int dim, class T>
Vector<dim, T> &Vector<dim, T>::operator+=(const Vector& v)
{
	for (int i=0; i<dim; i++)
		m_element[i] += v.m_element[i];

	return *this;
}

template <int dim, class T>
Vector<dim, T> &Vector<dim, T>::operator-=(const Vector& v)
{
	for (int i=0; i<dim; i++)
		m_element[i] -= v.m_element[i];

	return *this;
}

template <int dim, class T>
Vector<dim, T> &Vector<dim, T>::operator+=(T r)
{
	for (int i=0; i<dim; i++)
		m_element[i] += r;

	return *this;
}

template <int dim, class T>
Vector<dim, T> &Vector<dim, T>::operator-=(T r)
{
	for (int i=0; i<dim; i++)
		m_element[i] -= r;

	return *this;
}

template <int dim, class T>
Vector<dim, T> &Vector<dim, T>::operator*=(T r)
{
	for (int i=0; i<dim; i++)
		m_element[i] *= r;

	return *this;
}

template <int dim, class T>
Vector<dim, T> &Vector<dim, T>::operator/=(T r)
{
	for (int i=0; i<dim; i++)
		m_element[i] /= r;

	return *this;
}

template <int dim, class T>
T Vector<dim, T>::Dot(const Vector& v) const
{
	T result = T(0);

	for (int i=0; i<dim; i++)
		result += m_element[i]*v.m_element[i];

	return result;
}

template <int dim, class T>
const Vector<dim, T> Vector<dim, T>::Cross(const Vector& v) const
{
	assert(dim == 3);
	Vector result;

	result.m_element[0] = m_element[1]*v.m_element[2] - 
        m_element[2]*v.m_element[1];
	result.m_element[1] = m_element[2]*v.m_element[0] - 
        m_element[0]*v.m_element[2];
	result.m_element[2] = m_element[0]*v.m_element[1] - 
        m_element[1]*v.m_element[0];

	return result;
}

template <int dim, class T>
T Vector<dim, T>::Magnitude() const
{
	T result = T(0);

	for (int i=0; i<dim; i++)
		result += m_element[i]*m_element[i];

	return sqrt(result);
}

template <int dim, class T>
T Vector<dim, T>::Sum() const
{
	T result = T(0);

	for (int i=0; i<dim; i++)
		result += m_element[i];

	return result;
}

template <int dim, class T>
Vector<dim, T> Vector<dim, T>::Unit() const
{
	Vector result;
    T magnitude = Magnitude();
    assert(magnitude > 0);

    T inv_mag = 1 / magnitude;

	for (int i=0; i<dim; i++)
		result.m_element[i] += m_element[i]*inv_mag;

	return result;
}

template <int dim, class T>
const Vector<dim, T> Vector<dim, T>::operator*(T r) const
{
	Vector<dim, T> result;

	for (int i=0; i<dim; i++)
		result.m_element[i] = m_element[i] * r;

	return result;
}

template <int dim, class T>
const Vector<dim, T> Vector<dim, T>::operator/(T r) const
{
	Vector<dim, T> result;

	for (int i=0; i<dim; i++)
		result.m_element[i] = m_element[i] / r;

	return result;
}

template <int dim, class T>
const Vector<dim, T> operator+(const Vector<dim, T>& v1, 
                               const Vector<dim, T>& v2)
{
	Vector<dim, T> result;

	for (int i=0; i<dim; i++)
		result.m_element[i] = v1.m_element[i] + v2.m_element[i];

	return result;
}

template <int dim, class T>
const Vector<dim, T> operator-(const Vector<dim, T>& v1, 
                               const Vector<dim, T>& v2)
{
	Vector<dim, T> result;

	for (int i=0; i<dim; i++)
		result.m_element[i] = v1.m_element[i] - v2.m_element[i];

	return result;
}
