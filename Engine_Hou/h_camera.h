//
// Created by hyx on 2024/12/24.
//

#ifndef ENGINE_HOU_CLION_H_cameraH
#define ENGINE_HOU_CLION_H_cameraH

#include "h_math.h"

class Camera{
public:

    Camera() = default;
    Camera(const float fov, const float w, const float h, const float nearP, const float farP)
    :fov(fov), weight(w), height(h), aspect(w / h), near(nearP), far(farP){};

    virtual ~Camera() = default;

    Vec3 lookfrom = {0.0f, 0.0f, -2.0f};
    Vec3 lookat   = {0.0f, 0.0f, 1.0f};
    Vec3   up     = {0.0f, 1.0f, 0.0f};

    Mat4x4 model = Mat4x4::Eye();
    Mat4x4 view = Mat4x4::Ones();
    Mat4x4 projection = Mat4x4::Ones();

    void calculateFrustumPlanes() {
        Mat4x4 vp = projection * view;

        frustumPlanes[0] = Vec4{
                vp.Get(3, 0) + vp.Get( 0, 0),
                vp.Get(3, 1) + vp.Get( 0, 1),
                vp.Get(3, 2) + vp.Get( 0, 2),
                vp.Get(3, 3) + vp.Get( 0, 3)
        };


        frustumPlanes[1] = Vec4{
                vp.Get(3, 0) - vp.Get(0, 0),
                vp.Get(3, 1) - vp.Get(0, 1),
                vp.Get(3, 2) - vp.Get(0, 2),
                vp.Get(3, 3) - vp.Get(0, 3)
        };


        frustumPlanes[2] = Vec4{
                vp.Get(3, 0) + vp.Get(1, 0),
                vp.Get(3, 1) + vp.Get(1, 1),
                vp.Get(3, 2) + vp.Get(1, 2),
                vp.Get(3, 3) + vp.Get(1, 3)
        };


        frustumPlanes[3] = Vec4{
                vp.Get(3, 0) - vp.Get(1, 0),
                vp.Get(3, 1) - vp.Get(1, 1),
                vp.Get(3, 2) - vp.Get(1, 2),
                vp.Get(3, 3) - vp.Get(1, 3)
        };

/*        frustumPlanes[4] = Vec4{
                vp.Get(3, 0) + vp.Get(2, 0),
                vp.Get(3, 1) + vp.Get(2, 1),
                vp.Get(3, 2) + vp.Get(2, 2),
                vp.Get(3, 3) + vp.Get(2, 3)
        };

        frustumPlanes[5] = Vec4{
                vp.Get(3, 0) - vp.Get(2, 0),
                vp.Get(3, 1) - vp.Get(2, 1),
                vp.Get(3, 2) - vp.Get(2, 2),
                vp.Get(3, 3) - vp.Get(2, 3)
        };*/

        for (int i = 0; i < 4; ++i) {
            float length = std::sqrt(
                    frustumPlanes[i].x * frustumPlanes[i].x +
                    frustumPlanes[i].y * frustumPlanes[i].y +
                    frustumPlanes[i].z * frustumPlanes[i].z
            );
            frustumPlanes[i].x /= length;
            frustumPlanes[i].y /= length;
            frustumPlanes[i].z /= length;
            frustumPlanes[i].w /= length;
        }

        frustumPlanes[4] = Vec4{0, 0, 1, -1.0f + near};
        frustumPlanes[5] = Vec4{0, 0, -1, -far};

    }

public:

    Vec4 frustumPlanes[6];
    float aspect;
    float weight;
    float height;
    float near;
    float far;
    float fov;
};

#endif //ENGINE_HOU_CLION_H_cameraH
