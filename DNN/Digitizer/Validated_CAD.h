#ifndef R3BVALIDATED_H
#define R3BVALIDATED_H

//=============================================================================
//
// D A templatised class to represent data which can be validated, for instance
// D for data caching.
// D Original: https://bitbucket.org/davidcorne/mutable/
// D Modified by Jan Mayer

#include <assert.h>

//=============================================================================
template <typename T>
class Validated_CAD
{
  public:
    Validated_CAD();
    // invalid constructor

    Validated_CAD(const T& data);
    // valid constructor

    bool valid() const;
    // is this valid or not?

    void invalidate();
    // makes the class invalid again, so call in a function which will change the
    // cached value.

    void set(const T& data);
    // set the data, makes the instance valid.

    T get() const;
    // explicit conversion

    operator T();
    operator T() const;
    // implicit conversions to the template type.

  private:
    bool m_valid;
    T m_data;
};

//=============================================================================
//
// D Include the source in the header file as it is a templatised class.
//

//=============================================================================
template <typename T>
Validated_CAD<T>::Validated_CAD()
    //
    // D Invalid constructor
    //
    : m_valid(false)
{
}

//=============================================================================
template <typename T>
Validated_CAD<T>::Validated_CAD(const T& data)
    //
    // D Valid constructor
    //
    : m_valid(true)
    , m_data(data)
{
}

//=============================================================================
template <typename T>
bool Validated_CAD<T>::valid() const
//
// D is this valid or not?
//
{
    return m_valid;
}

//=============================================================================
template <typename T>
void Validated_CAD<T>::invalidate()
//
// D Makes the class invalid again, so call when the client calls a function
// D which will change the cached value.
//
{
    m_valid = false;
}

//=============================================================================
template <typename T>
void Validated_CAD<T>::set(const T& data)
//
// D set the data.
//
{
    m_data = data;
    m_valid = true;
}

//=============================================================================
template <typename T>
T Validated_CAD<T>::get() const
{
    assert(valid() && "Cannot return invalid data.");
    return m_data;
}

//=============================================================================
template <typename T>
Validated_CAD<T>::operator T()
//
// D implicit conversion to the template type.
//
{
    assert(valid() && "Cannot return invalid data.");
    return m_data;
}

//=============================================================================
template <typename T>
Validated_CAD<T>::operator T() const
//
// D implicit conversion to the template type.
//
{
    // a nice way of returning a message with a standard assert.
    assert(valid() && "Cannot return invalid data.");
    return m_data;
}

#endif // R3BVALIDATED_H
