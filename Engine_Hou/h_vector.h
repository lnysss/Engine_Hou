//
// Created by hyx on 2024/12/24.
//

#ifndef ENGINE_HOU_CLION_H_VECTOR_H
#define ENGINE_HOU_CLION_H_VECTOR_H

#include <ostream>

template <size_t Dim> class Vector {
public:
    static const Vector<Dim> One;
    static const Vector<Dim> Zero;

    float data[Dim];

    Vector() = default;

    Vector(float value) {
        for (size_t i = 0; i < Dim; i++) {
            data[i] = value;
        }
    }

    Vector(const std::initializer_list<float> &l) { Assign(l); }

    float &operator[](size_t idx) { return data[idx]; }
    float operator[](size_t idx) const { return data[idx]; }

    void Assign(const std::initializer_list<float> &l) {
        size_t max = std::min(Dim, l.size());
        size_t i = 0;
        for (i = 0; i < max; i++) {
            data[i] = *(l.begin() + i);
        }
        for (; i < Dim; i++) {
            data[i] = 0;
        }
    }
};

template <size_t Dim> const Vector<Dim> Vector<Dim>::One(1);

template <size_t Dim> const Vector<Dim> Vector<Dim>::Zero(0);

template <> class Vector<2> {
public:
    static const Vector One;
    static const Vector Zero;

    union {
        struct { float x, y; };
        struct { float w, h; };
        float data[2];
    };

    Vector() = default;

    Vector(const std::initializer_list<float> &l) { Assign(l); }

    float &operator[](size_t idx) { return data[idx]; }
    float operator[](size_t idx) const { return data[idx]; }

    void Assign(const std::initializer_list<float> &l) {
        size_t max = std::min<size_t>(2, l.size());
        size_t i = 0;
        for (i = 0; i < max; i++) {
            data[i] = *(l.begin() + i);
        }
        for (; i < 2; i++) {
            data[i] = 0;
        }
    }
};

const Vector<2> Vector<2>::One{1, 1};
const Vector<2> Vector<2>::Zero{0, 0};

inline float Cross(const Vector<2> &v1, const Vector<2> &v2) {
    return v1.x * v2.y - v1.y * v2.x;
}

template <> class Vector<3> {
public:
    static const Vector One;
    static const Vector Zero;

    union {
        struct { float x, y, z; };
        struct { float r, g, b; };
        struct { float u, v, w; };
        struct { float alpha, beta, gamma; };
        float data[3];
    };

    Vector() = default;

    Vector(const std::initializer_list<float> &l) { Assign(l); }

    float &operator[](size_t idx) { return data[idx]; }
    float operator[](size_t idx) const { return data[idx]; }

    void Assign(const std::initializer_list<float> &l) {
        size_t max = std::min<size_t>(3, l.size());
        size_t i = 0;
        for (; i < max; i++) {
            data[i] = *(l.begin() + i);
        }
        for (; i < 3; i++) {
            data[i] = 0;
        }
    }
};

const Vector<3> Vector<3>::One{1, 1, 1};
const Vector<3> Vector<3>::Zero{0, 0, 0};

inline Vector<3> Cross(const Vector<3> &v1, const Vector<3> &v2) {
    return Vector<3>{v1.y * v2.z - v1.z * v2.y,
                     v1.z * v2.x - v1.x * v2.z,
                     v1.x * v2.y - v1.y * v2.x};
}

template <> class Vector<4> {
public:
    static const Vector One;
    static const Vector Zero;

    union {
        struct {
            float x, y, z, w;
        };
        struct {
            float r, g, b, a;
        };
        float data[4];
    };

    Vector() = default;

    Vector(const std::initializer_list<float> &l) { Assign(l); }

    float &operator[](size_t idx) { return data[idx]; }
    float operator[](size_t idx) const { return data[idx]; }

    void Assign(const std::initializer_list<float> &l) {
        size_t max = std::min<size_t>(4, l.size());
        size_t i = 0;
        for (; i < max; i++) {
            data[i] = *(l.begin() + i);
        }
        for (; i < 4; i++) {
            data[i] = 0;
        }
    }
};

const Vector<4> Vector<4>::One{1, 1, 1, 1};
const Vector<4> Vector<4>::Zero{0, 0, 0, 0};

template <size_t Dim>
Vector<Dim> operator*(const Vector<Dim> &self, float value) {
    Vector<Dim> v;
    for (size_t i = 0; i < Dim; i++) {
        v.data[i] = self.data[i] * value;
    }
    return v;
}

template <size_t Dim>
Vector<Dim> operator/(const Vector<Dim> &self, float value) {
    Vector<Dim> v;
    for (size_t i = 0; i < Dim; i++) {
        v.data[i] = self.data[i] / value;
    }
    return v;
}

template <size_t Dim>
Vector<Dim> operator*(const Vector<Dim> &self, const Vector<Dim> &o) {
    Vector<Dim> v;
    for (size_t i = 0; i < Dim; i++) {
        v.data[i] = self.data[i] * o.data[i];
    }
    return v;
}

template <size_t Dim>
Vector<Dim> operator/(const Vector<Dim> &self, const Vector<Dim> &o) {
    Vector<Dim> v;
    for (size_t i = 0; i < Dim; i++) {
        v.data[i] = self.data[i] / o.data[i];
    }
    return v;
}

template <size_t Dim>
Vector<Dim> operator+(const Vector<Dim> &self, const Vector<Dim> &o) {
    Vector<Dim> v;
    for (size_t i = 0; i < Dim; i++) {
        v.data[i] = self.data[i] + o.data[i];
    }
    return v;
}

template <size_t Dim>
Vector<Dim> operator-(const Vector<Dim> &self, const Vector<Dim> &o) {
    Vector<Dim> v;
    for (size_t i = 0; i < Dim; i++) {
        v.data[i] = self.data[i] - o.data[i];
    }
    return v;
}

template <size_t Dim>
Vector<Dim> &operator*=(Vector<Dim> &self, float value) {
    for (size_t i = 0; i < Dim; i++) {
        self.data[i] *= value;
    }
    return self;
}

template <size_t Dim>
Vector<Dim> &operator/=(const Vector<Dim> &self, float value) {
    for (size_t i = 0; i < Dim; i++) {
        self.data[i] /= value;
    }
    return self;
}

template <size_t Dim>
Vector<Dim> &operator*=(Vector<4ULL> &self, const Vector<Dim> &o) {
    for (size_t i = 0; i < Dim; i++) {
        self.data[i] *= o.data[i];
    }
    return self;
}

template <size_t Dim>
Vector<Dim> &operator/=(const Vector<Dim> &self, const Vector<Dim> &o) {
    for (size_t i = 0; i < Dim; i++) {
        self.data[i] /= o.data[i];
    }
    return self;
}

template <size_t Dim>
Vector<Dim> &operator+=(Vector<4ULL> &self, const Vector<Dim> &o) {
    for (size_t i = 0; i < Dim; i++) {
        self.data[i] += o.data[i];
    }
    return self;
}

template <size_t Dim>
Vector<Dim> &operator-=(const Vector<Dim> &self, const Vector<Dim> &o) {
    for (size_t i = 0; i < Dim; i++) {
        self.data[i] -= o.data[i];
    }
    return self;
}

template <size_t Dim> float Dot(const Vector<Dim> &v1, const Vector<Dim> &v2) {
    float sum = 0;
    for (size_t i = 0; i < Dim; i++) {
        sum += v1.data[i] * v2.data[i];
    }
    return sum;
}

template <size_t Dim>
bool operator==(const Vector<Dim> &v1, const Vector<Dim> &v2) {
    for (size_t i = 0; i < Dim; i++) {
        if (v1[i] != v2[i]) {
            return false;
        }
    }
    return true;
}

template <size_t Dim>
bool operator!=(const Vector<Dim> &v1, const Vector<Dim> &v2) {
    return !(v1 == v2);
}

template <size_t Dim1, size_t Dim2> Vector<Dim1> Vec(const Vector<Dim2> &v) {
    Vector<Dim1> result;
    size_t min = std::min(Dim1, Dim2);
    size_t i = 0;
    for (i = 0; i < min; i++) {
        result.data[i] = v.data[i];
    }
    for (; i < Dim1; i++) {
        result.data[i] = 0;
    }
    return result;
}

template <size_t Dim> float Len2(const Vector<Dim> &v) {
    float result = 0;
    for (auto &elem : v.data) {
        result += elem * elem;
    }
    return result;
}

template <size_t Dim> float Len(const Vector<Dim> &v) {
    return std::sqrt(Len2(v));
}

template <size_t Dim> Vector<Dim> operator*(float value, const Vector<Dim> &v) {
    return v * value;
}

template <size_t Dim> Vector<Dim> operator/(float value, const Vector<Dim> &v) {
    return v / value;
}

template <size_t Dim> Vector<Dim> Normalize(const Vector<Dim> &v) {
    Vector<Dim> result;
    float len = Len(v);
    for (size_t i = 0; i < Dim; i++) {
        result.data[i] = v.data[i] / len;
    }
    return result;
}

template <size_t Dim>
std::ostream &operator<<(std::ostream &o, const Vector<Dim> &v) {
    o << "Vector<" << Dim << ">(";
    for (size_t i = 0; i < Dim; i++) {
        o << v.data[i];
        if (i != Dim - 1) {
            o << ", ";
        }
    }
    o << ")";
    return o;
}

using Vec2 = Vector<2>;
using Vec3 = Vector<3>;
using Vec4 = Vector<4>;
using Color4 = Vec4;
using Color3 = Vec3;

#endif //ENGINE_HOU_CLION_H_VECTOR_H
