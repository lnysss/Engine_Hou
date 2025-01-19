//
// Created by hyx on 2024/12/24.
//

#ifndef ENGINE_HOU_CLION_H_MATH_H
#define ENGINE_HOU_CLION_H_MATH_H

#define H_PI 3.141f
#define H_PI_DEGREE 180.0f

#include "h_matrix.h"
#include "h_vertex.h"

inline float Radians(float degrees) {
    return degrees * H_PI / 180.0f;
}

inline float Degrees(float radians) {
    return radians * 180.0f / H_PI;
}

struct Quaternion {
    float w, x, y, z;
    Quaternion(float w, float x, float y, float z) : w(w), x(x), y(y), z(z) {}
};

Quaternion eulerToQuaternion(float roll, float pitch, float yaw) {

    float cy = cos(yaw * 0.5f);
    float sy = sin(yaw * 0.5f);
    float cp = cos(pitch * 0.5f);
    float sp = sin(pitch * 0.5f);
    float cr = cos(roll * 0.5f);
    float sr = sin(roll * 0.5f);

    Quaternion q(
            cr * cp * cy + sr * sp * sy,
            sr * cp * cy - cr * sp * sy,
            cr * sp * cy + sr * cp * sy,
            cr * cp * sy - sr * sp * cy
    );

    return q;
}

Mat4x4 RotateQuaternion(const Quaternion& q) {

    float xx = q.x * q.x;
    float yy = q.y * q.y;
    float zz = q.z * q.z;
    float xy = q.x * q.y;
    float zw = q.z * q.w;
    float zx = q.z * q.x;
    float yw = q.y * q.w;
    float yz = q.y * q.z;
    float xw = q.x * q.w;

    return Mat4x4 {
            1 - 2 * (yy + zz), 2 * (xy + zw),      2 * (zx - yw),     0,
            2 * (xy - zw),     1 - 2 * (xx + zz),  2 * (yz + xw),     0,
            2 * (zx + yw),     2 * (yz - xw),      1 - 2 * (xx + yy), 0,
            0,                 0,                  0,                 1,

    };
}

Vec3 NormalizeEuler(Vec3 angle) {
    if (angle.x > H_PI_DEGREE) angle.x -= 2 * H_PI_DEGREE;
    if (angle.x < -H_PI_DEGREE) angle.x += 2 * H_PI_DEGREE;
    if (angle.y > H_PI_DEGREE) angle.y -= 2 * H_PI_DEGREE;
    if (angle.y < -H_PI_DEGREE) angle.y += 2 * H_PI_DEGREE;
    if (angle.z > H_PI_DEGREE) angle.z -= 2 * H_PI_DEGREE;
    if (angle.z < -H_PI_DEGREE) angle.z += 2 * H_PI_DEGREE;
    return angle;
}

struct Rect {
    Vec2 pos;
    Vec2 size;
};

template <typename T>
inline int Sign(T value) {
    return value > 0 ? 1 : (value == 0 ? 0 : -1);
}


inline Mat4x4 View(Vec3 eye, Vec3 center, Vec3 up){
    Mat4x4 Result;
    Vec3 const f(Normalize(center - eye));
    Vec3 const s(Normalize(Cross(f, up)));
    Vec3 const u(Cross(s, f));

    return Mat4x4{
            -s.x,       -s.y,      -s.z,    Dot(s, eye),
            -u.x,       -u.y,      -u.z,    Dot(u, eye),
            -f.x,       -f.y,      -f.z,    Dot(f, eye),
            0,         0,        0,         1,
    };
}

inline Mat4x4 Ortho(float l, float r, float b, float t, float n, float f) {
    return Mat4x4{
            2 / (r - l), 0,           0, -(l + r) / (r - l),
            0, 2 / (t - b),           0, -(t + b) / (t - b),
            0,           0, 2 / (n - f), -(n + f) / (n - f),
            0,           0,           0,            1,
    };
}

inline Mat4x4 Persp(float fov, float aspect, float near, float far) {
    float tanHalf = std::tan(fov * 0.5);
    char sign = Sign(near);
    return Mat4x4{
            sign / (aspect * tanHalf),            0,                            0,                             0,
            0, sign / tanHalf,                           0,                             0,
            0,              0,           (near + far) / (near - far),           2 * near * far / (far - near),
            0,              0,                           1,                             0,
    };
}

inline Mat4x4 Translate(float x, float y, float z) {
    return Mat4x4{
            1, 0, 0, x,
            0, 1, 0, y,
            0, 0, 1, z,
            0, 0, 0, 1
    };
}

inline Mat4x4 RotateEuler(float x, float y, float z) {
    float sinx = std::sin(x),
            cosx = std::cos(x),
            siny = std::sin(y),
            cosy = std::cos(y),
            sinz = std::sin(z),
            cosz = std::cos(z);
    return Mat4x4{
            cosz, -sinz, 0, 0,
            sinz,  cosz, 0, 0,
            0,     0, 1, 0,
            0,     0, 0, 1,
    } * Mat4x4{
            cosy, 0, siny, 0,
            0, 1,    0, 0,
            -siny, 0, cosy, 0,
            0, 0,    0, 1
    } * Mat4x4{
            1,    0,     0, 0,
            0, cosx, -sinx, 0,
            0, sinx,  cosx, 0,
            0,    0,     0, 1
    };
}

inline Mat4x4 Scale(float x, float y, float z) {
    return Mat4x4{
            x, 0, 0, 0,
            0, y, 0, 0,
            0, 0, z, 0,
            0, 0, 0, 1
    };
}

inline std::ostream& operator<<(std::ostream& o, const Rect& r) {
    printf("Rect(%f, %f, %f, %f)", r.pos.x, r.pos.y, r.size.w, r.size.h);
    return o;
}

template <typename T>
T Clamp(T value, T min, T max) {
    return std::min(std::max(value, min), max);
}

template <typename T>
T Mix(T a, T b, float t) {
    return a + t * (b - a);
}

template <typename T>
T SmoothStep(T edge0, T edge1, float t) {
    t = std::min(std::max((t - edge0) / (edge1 - edge0), 0.0f), 1.0f);
    return t * t * (3 - 2 * t);
}

inline bool isPointInFrustum(const Vec4& point,const Vec4 (&frustumPlanes)[6]) {

    for (int i = 0; i < 6; ++i) {
        const Vec4& plane = frustumPlanes[i];

        float distance = plane.x * point.x + plane.y * point.y + plane.z * point.z + plane.w;
        if (distance < -0.5f) {
            return false;
        }
    }
    return true;
}



inline bool IsPointInRect(const Vec2 &p, const Rect &r) {
    return p.x >= r.pos.x && p.x <= r.pos.x + r.size.w &&
           p.y >= r.pos.y && p.y <= r.pos.y + r.size.h;
}

inline bool insideTriangle(int x, int y, const Vertex* _v){
    Vec3 v[3];
    for(int i=0;i<3;i++)
        v[i] = {_v[i].pos2.x,_v[i].pos2.y, 1.0};
    Vec3 f0,f1,f2;
    f0 = Cross(v[1], v[0]);
    f1 = Cross(v[2], v[1]);
    f2 = Cross(v[0], v[2]);
    Vec3 p{float(x),float(y),1.0f};
    if((Dot(p, f0) * Dot(v[2], f0)>0) && (Dot(p, f1)*Dot(v[0], f1)>0) && (Dot(p, f2)*Dot(v[1], f2)>0))
        return true;
    return false;
}

inline bool IsPointInTriangle(const Vec2 &p, const Vec3 &bc) {

    if (bc.alpha < 0 || bc.beta < 0 || bc.gamma < 0) {
        return false;
    }
    return true;
}


inline bool RectsIntersect(const Rect &r1, const Rect &r2, Rect *result) {
    Rect rect;
    rect.pos.x = std::max(r1.pos.x, r2.pos.x);
    rect.pos.y = std::max(r1.pos.y, r2.pos.y);
    rect.pos.w =
            std::min(r1.pos.x + r1.size.w, r2.pos.x + r2.size.w) - rect.pos.x;
    rect.pos.h =
            std::min(r1.pos.y + r1.size.h, r2.pos.y + r2.size.h) - rect.pos.y;
    if (rect.size.w > 0 && rect.size.h > 0) {
        if (result) {
            *result = rect;
        }
        return true;
    }
    return false;
}

inline Vec3 Barycentric(const TriangleH& t1, const Vec2& p) {

    Vec2 v1 = t1.v1.pos2;
    Vec2 v2 = t1.v2.pos2;
    Vec2 v3 = t1.v3.pos2;

    Vec3 result;
    Vec3 c1{v1.x - v2.x, v1.x - v3.x, p.x - v1.x},
            c2{v1.y - v2.y, v1.y - v3.y, p.y - v1.y};
    result = Cross(c1, c2);
    if (result.z == 0) {
        return Vec3{-1, -1, -1};
    }
    return Vec3{1 - result.x / result.z - result.y / result.z,
                result.x / result.z,
                result.y / result.z};
}

inline Rect AABB(const TriangleH& tri) {

    Vec2 v1 = tri.v1.pos2;
    Vec2 v2 = tri.v2.pos2;
    Vec2 v3 = tri.v3.pos2;

    int x1 = std::min({v1.x, v2.x, v3.x}),
            y1 = std::min({v1.y, v2.y, v3.y}),
            x2 = std::max({v1.x, v2.x, v3.x}),
            y2 = std::max({v1.y, v2.y, v3.y});
    return Rect{Vec2{float(x1), float(y1)},
                Vec2{float(x2 - x1), float(y2 - y1)}};
}


template <typename T>
inline T Lerp(T a, T b, float percent) {
    return a + (b - a) * percent;
}

inline Vec3 Reflect(const Vec3& v, const Vec3& n) {
    return v - 2 * Dot(v,n) * n;
}

#endif //ENGINE_HOU_CLION_H_MATH_H
