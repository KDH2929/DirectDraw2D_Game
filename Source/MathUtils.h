#pragma once
#include <cmath>
#include <stdexcept>

constexpr float PI = 3.14159265f;

inline float degToRad(float degrees) {
    return degrees * PI / 180.0f;
}

template <typename T>
class Vector2 {
public:
    T x, y;

    // �⺻ ������
    Vector2() : x(T(0)), y(T(0)) {}

    // �Ű����� ������
    Vector2(T x, T y) : x(x), y(y) {}

    // �� ��ȯ ������: �ٸ� Ÿ���� Vector2�κ��� ��ȯ
    template <typename U>
    explicit Vector2(const Vector2<U>& other)
        : x(static_cast<T>(other.x)), y(static_cast<T>(other.y)) {}

    // ���� ���� ���
    T Length() const {
        return static_cast<T>(std::sqrt(x * x + y * y));
    }

    // ����ȭ�� ���� ��ȯ (���̰� 0�̸� ���� �߻�)
    Vector2 Normalized() const {
        T len = Length();
        if (len == T(0))
            throw std::runtime_error("Cannot normalize zero-length vector.");
        return *this / len;
    }

    // ������ �����ε�
    Vector2 operator+(const Vector2& other) const {
        return Vector2(x + other.x, y + other.y);
    }

    Vector2 operator-(const Vector2& other) const {
        return Vector2(x - other.x, y - other.y);
    }

    Vector2 operator*(T scalar) const {
        return Vector2(x * scalar, y * scalar);
    }

    Vector2 operator/(T scalar) const {
        if (scalar == T(0))
            throw std::runtime_error("Division by zero in Vector2 operator/");
        return Vector2(x / scalar, y / scalar);
    }

    Vector2& operator+=(const Vector2& other) {
        x += other.x;
        y += other.y;
        return *this;
    }

    Vector2 operator-() const {
        return Vector2(-x, -y);
    }
};

// �۷ι� ���� ���� �Լ���
template <typename T>
T Dot(const Vector2<T>& a, const Vector2<T>& b) {
    return a.x * b.x + a.y * b.y;
}

template <typename T>
Vector2<T> Normalize(const Vector2<T>& a) {
    T len = a.Length();
    if (len == T(0))
        return Vector2<T>(T(0), T(0));
    return a / len;
}

template <typename T>
Vector2<T> operator*(T scalar, const Vector2<T>& vec) {
    return Vector2<T>(scalar * vec.x, scalar * vec.y);
}


class Transform {
public:
    Vector2<float> position;    // ��ġ
    float rotation;             // ȸ�� (�� ����)
    Vector2<float> scale;       // ������

    Transform() : position(0, 0), rotation(0.0f), scale(1.0f, 1.0f) {}
};
