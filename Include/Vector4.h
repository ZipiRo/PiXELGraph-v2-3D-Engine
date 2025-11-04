#pragma once

class Vector3;

class Vector4
{
public:
    float x, y, z, w;

    Vector4();
    Vector4(float x, float y, float z, float w);
    inline operator Vector3() const;

    Vector4 &operator=(const Vector4& vector);
    bool operator!() const;
    bool operator==(const Vector4& vector);
    bool operator!=(const Vector4& vector);
    
    Vector4 operator-() const;
    Vector4 operator+(const Vector4& vector) const;
    Vector4 operator-(const Vector4& vector) const;

    Vector4 operator*(float scalar) const;
    Vector4 operator/(float scalar) const;
    Vector4 &operator+=(const Vector4& vector);
    Vector4 &operator-=(const Vector4& vector);
    Vector4 &operator*=(float scalar);
    Vector4 &operator/=(float scalar);

    Vector4 normalize();

    static float Distance(const Vector4 &a, const Vector4 &b);
    static float Length(const Vector4 &vector);
    static Vector4 Normalize(const Vector4 &vector);
};

Vector4::Vector4() 
{
    x = 0;
    y = 0;
    z = 0;
    w = 0;
}

Vector4::Vector4(float x, float y, float z, float w) 
{
    this->x = x;
    this->y = y;
    this->z = z;
    this->w = w;
}

Vector4& Vector4::operator=(const Vector4& vector)
{
    if (this != &vector)
    {
        x = vector.x;
        y = vector.y;
        z = vector.z;
        w = vector.w;
    }
    return *this;
}

bool Vector4::operator==(const Vector4& vector)
{
    return (x == vector.x && y == vector.y && z == vector.z && w == vector.w);
}

bool Vector4::operator!=(const Vector4& vector)
{
    return (x != vector.x || y != vector.y || z != vector.z || w != vector.w);
}

bool Vector4::operator!() const
{
    return (x == 0 && y == 0 && z == 0 && w == 0);
}

Vector4 Vector4::operator-() const
{
    return Vector4(-x, -y, -z, -w);
}

Vector4 Vector4::operator+(const Vector4& vector) const
{
    return Vector4(x + vector.x, y + vector.y, z + vector.z, w + vector.w);
}

Vector4 Vector4::operator-(const Vector4& vector) const
{
    return Vector4(x - vector.x, y - vector.y, z - vector.z, w - vector.w);
}

Vector4 Vector4::operator*(float scalar) const
{
    return Vector4(x * scalar, y * scalar, z * scalar, w * scalar);
}

Vector4 Vector4::operator/(float scalar) const
{
    return Vector4(x / scalar, y / scalar, z / scalar, w / scalar);
}

Vector4& Vector4::operator+=(const Vector4& vector)
{
    x += vector.x;
    y += vector.y;
    z += vector.z;
    w += vector.w;
    return *this;
}

Vector4& Vector4::operator-=(const Vector4& vector)
{
    x -= vector.x;
    y -= vector.y;
    z -= vector.z;
    w -= vector.w;
    return *this;
}

Vector4& Vector4::operator*=(float scalar)
{
    x *= scalar;
    y *= scalar;
    z *= scalar;
    w *= scalar;
    return *this;
}

Vector4& Vector4::operator/=(float scalar)
{
    x /= scalar;
    y /= scalar;
    z /= scalar;
    w /= scalar;
    return *this;
}

Vector4 multipyVector3(const Vector4& a, const Vector4& b)
{
    return Vector4(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
}

Vector4 divideVector3(const Vector4& a, const Vector4& b)
{
    return Vector4(a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w);
}

float Vector4::Distance(const Vector4& a, const Vector4& b)
{
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    float dz = a.z - b.z;
    float dw = a.w - b.w;
    return std::sqrt(dx * dx + dy * dy + dz * dz + dw * dw);
}

float Vector4::Length(const Vector4& v)
{
    return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
}

Vector4 Vector4::Normalize(const Vector4& v)
{
    float len = Length(v);
    if (len == 0) return Vector4();

    return Vector4(v.x / len, v.y / len, v.z / len, v.w / len);
}

Vector4 Vector4::normalize()
{
    return Vector4::Normalize(*this);
}

#include "Vector3.h"

inline Vector4::operator Vector3() const
{
    return Vector3(x, y, z);
}

inline Vector3::operator Vector4() const
{
    return Vector4(x, y, z, 1);
}