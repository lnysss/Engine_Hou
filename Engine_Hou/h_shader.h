//
// Created by hyx on 2024/12/24.
//

#ifndef ENGINE_HOU_CLION_H_SHADER_H
#define ENGINE_HOU_CLION_H_SHADER_H

#include <unordered_map>
#include <functional>
#include "h_framebuffer.h"


float Clamp(float value, float min, float max) {
    return std::min(std::max(value, min), max);
}

struct ShaderContext {
    std::unordered_map<int, float> varyingFloat;
    std::unordered_map<int, Vec2> varyingVec2;
    std::unordered_map<int, Vec3> varyingVec3;
    std::unordered_map<int, Vec4> varyingVec4;

    void Clear() {
        varyingFloat.clear();
        varyingVec2.clear();
        varyingVec3.clear();
        varyingVec4.clear();
    }
};

using VertexShader = std::function<Vec4(int index, ShaderContext &output)>;
using FragmentShader = std::function<Vec4(ShaderContext &input)>;


inline Color4 TextureSample(const FrameBuffer* const surface, Vec2 varyingVec2) {
    varyingVec2.x = Clamp(varyingVec2.x, 0, 1);
    varyingVec2.y = Clamp(varyingVec2.y, 0, 1);

    return surface->GetPixel(varyingVec2.x * surface->Width(), varyingVec2.y * surface->Height());
}



#endif //ENGINE_HOU_CLION_H_SHADER_H
