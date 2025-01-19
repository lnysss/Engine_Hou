//
// Created by hyx on 2024/12/24.
//

#ifndef ENGINE_HOU_CLION_H_LIGHT_H
#define ENGINE_HOU_CLION_H_LIGHT_H

#include "h_vector.h"

struct PointLight
{
    Vec4 Position = { -2.0f, -2.0f, 0.0f, 1.0f };
    float Intensity = 0.0f;
    Vec4 Radiance = { 1.0f, 1.0f, 1.0f, 1.0f};
    float MinRadius = 0.001f;
    float Radius = 10.0f;
    float Falloff = 1.0f;
    char Padding[3]{ 0, 0 ,0};

    void SetPosition(const Vec4 &c) { Position = c; }
    void SetRadiance(const Vec4 &c) { Radiance = c; }
    void SetIntensity(const float &c) { Intensity = c; }
    void SetMinRadius(const float &c) { MinRadius = c; }
    void SetRadius(const float &c) { Radius = c; }
    void SetFalloff(const float &c) { Falloff = c; }
};
#endif //ENGINE_HOU_CLION_H_LIGHT_H
