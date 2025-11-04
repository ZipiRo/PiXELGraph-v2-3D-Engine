#pragma once

class Vector4;
class Vector3
{
public:
    float x, y, z;

    Vector3();
    Vector3(float x, float y, float z);
    inline operator Vector4() const;

    Vector3 &operator=(const Vector3& vector);
    bool operator!() const;
    bool operator==(const Vector3& vector);
    bool operator!=(const Vector3& vector);
    
    Vector3 operator-() const;
    Vector3 operator+(const Vector3& vector) const;
    Vector3 operator-(const Vector3& vector) const;

    Vector3 operator*(float scalar) const;
    Vector3 operator/(float scalar) const;
    Vector3 &operator+=(const Vector3& vector);
    Vector3 &operator-=(const Vector3& vector);
    Vector3 &operator*=(float scalar);
    Vector3 &operator/=(float scalar);

    Vector3 normalize();

    static const Vector3 ZERO;
    static const Vector3 UP;
    static const Vector3 FORWARD;

    static float Distance(const Vector3 &a, const Vector3 &b);
    static float Length(const Vector3 &vector);
    static float DotProduct(const Vector3 &a, const Vector3 &b);
    static Vector3 CrossProduct(const Vector3 &a, const Vector3 &b);
    static Vector3 Normalize(const Vector3 &vector);
};

const Vector3 Vector3::ZERO = Vector3(0, 0, 0);
const Vector3 Vector3::UP = Vector3(0, 1, 0);
const Vector3 Vector3::FORWARD = Vector3(0, 0, 1); 

Vector3::Vector3() 
{
    x = 0;
    y = 0;
    z = 0;
}

Vector3::Vector3(float x, float y, float z) 
{
    this->x = x;
    this->y = y;
    this->z = z;
}

Vector3& Vector3::operator=(const Vector3& vector) 
{
    if (this != &vector) 
    {
        x = vector.x;
        y = vector.y;
        z = vector.z;
    }
    return *this;
}

bool Vector3::operator==(const Vector3& vector)
{
    return (x == vector.x && y == vector.y && z == vector.z);
}

bool Vector3::operator!=(const Vector3& vector)
{
    return (x != vector.x && y != vector.y  && z != vector.z);
}

bool Vector3::operator!() const
{
    return (!x && !y && !z);
}

Vector3 Vector3::operator-() const {
    return Vector3(-x, -y, -z);
}

Vector3 Vector3::operator+(const Vector3& vector) const 
{
    return Vector3(x + vector.x, y + vector.y, z + vector.z);
}

Vector3 Vector3::operator-(const Vector3& vector) const 
{
    return Vector3(x - vector.x, y - vector.y, z - vector.z);
}

Vector3 Vector3::operator*(float scalar) const 
{
    return Vector3(x * scalar, y * scalar, z * scalar);
}

Vector3 Vector3::operator/(float scalar) const 
{
    return Vector3(x / scalar, y / scalar, z / scalar);
}

Vector3& Vector3::operator+=(const Vector3& vector) 
{
    x += vector.x;
    y += vector.y;
    z += vector.z;
    return *this;
}

Vector3& Vector3::operator-=(const Vector3& vector) 
{
    x -= vector.x;
    y -= vector.y;
    z -= vector.z;
    return *this;
}

Vector3& Vector3::operator*=(float scalar) 
{
    x *= scalar;
    y *= scalar;
    z *= scalar;
    return *this;
}

Vector3& Vector3::operator/=(float scalar) 
{
    x /= scalar;
    y /= scalar;
    z /= scalar;
    return *this;
}

Vector3 multipyVector3(const Vector3 a, const Vector3 vector) 
{
    return Vector3(a.x * vector.x, a.y * vector.y, a.z * vector.z);
}

Vector3 divideVector3(const Vector3 a, const Vector3 vector) 
{
    return Vector3(a.x / vector.x, a.y / vector.y, a.z / vector.z);
}

float Vector3::Distance(const Vector3 &a, const Vector3 &b) 
{
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    float dz = a.z - b.z;

    return sqrt(dx * dx + dy * dy + dz * dz);
}

float Vector3::Length(const Vector3 &vector)
{
    return sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
}

Vector3 Vector3::Normalize(const Vector3 &vector) 
{
    float length = Vector3::Length(vector);
    if (length == 0) return Vector3();

    return Vector3(vector.x / length, vector.y / length, vector.z / length);
}

Vector3 Vector3::normalize()
{
    return Vector3::Normalize(*this);
}

float Vector3::DotProduct(const Vector3 &a, const Vector3 &b) 
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vector3 Vector3::CrossProduct(const Vector3 &a, const Vector3 &b) 
{
    return Vector3
    (
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    );
}