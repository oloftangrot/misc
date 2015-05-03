#ifndef VECTOR_H
#define VECTOR_H

template <int dim, class T=double> class Vector
{
public:
	inline Vector(T elem = T(0));  // Fill vector with copies of elem
	inline Vector(const T *elem_list);

	inline T &operator[](int index);        // index must be in 0..dim-1
	inline T operator[](int index) const;   // index must be in 0..dim-1
	inline const Vector operator-() const;

	inline Vector& operator=(const Vector& v);
    inline Vector& operator+=(const Vector& v);
	inline Vector& operator-=(const Vector& v);

    inline Vector& operator+=(T r);
	inline Vector& operator-=(T r);
    inline Vector& operator*=(T r);
	inline Vector& operator/=(T r);

    int length() const { return dim; }

	inline T Dot(const Vector& v) const;
	inline const Vector Cross(const Vector& v) const; // dim must be 3
	inline T Magnitude() const;
    inline T Sum() const;           // Sum of elements
	inline Vector Unit() const;     // Must be nonzero vector

    inline const Vector operator*(T r) const;
    inline const Vector operator/(T r) const;

    inline friend const Vector operator+(const Vector& v1, const Vector& v2);
	inline friend const Vector operator-(const Vector& v1, const Vector& v2);

	friend const Vector operator*(T r, const Vector& v) { return  v * r; }

private:
	T m_element[dim];
};

#include <Vector_Impl.h>

#endif // VECTOR_H
