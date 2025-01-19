//
// Created by hyx on 2024/12/24.
//

#ifndef ENGINE_HOU_CLION_rendererH
#define ENGINE_HOU_CLION_rendererH

#pragma once


#include <cstddef>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <map>

#include "h_drawline.h"
#include "h_framebuffer.h"

constexpr float floatInf = FLT_MAX;

enum UniformVec2 {
    Texcoord = 0,
    ScreenPosition = 1,
};

enum UniformVec3 {
    Color = 0,
};

enum UniformVec4 {
    Normal = 0,
    WorldPosition = 1,
    ViewPosition = 2,
};

enum FaceCull {
    CW = 1,
    CCW,
};

class Renderer final {
public:

    static void Init() {
        IMG_Init(IMG_INIT_PNG|IMG_INIT_JPG);
    }

    static void Quit() {
        IMG_Quit();
    }

    Renderer(int w, int h)
            : drawColor{0, 0, 0, 0} {
        framebuffer.reset(new FrameBuffer(w, h));
        depthBuffer = new Buffer2D(w, h);
    }

    ~Renderer() {
        delete depthBuffer;
    }

public:

    Vec4 planes[6];
    struct pos_buf_id
    {
        int pos_id = 0;
    };

    struct ind_buf_id
    {
        int ind_id = 0;
    };

    struct col_buf_id
    {
        int col_id = 0;
    };

    int normal_id = -1;
    int next_id = 0;
    int get_next_id() { return next_id++; }

    std::map<int, std::vector<Vec3>> pos_buf;
    std::map<int, std::vector<Vec3>> ind_buf;
    std::map<int, std::vector<Vec3>> col_buf;
    std::map<int, std::vector<Vec3>> nor_buf;

    pos_buf_id load_positions(const std::vector<Vec3> &positions)
    {
        auto id = get_next_id();
        pos_buf.emplace(id, positions);

        return {id};
    }

    ind_buf_id load_indices(const std::vector<Vec3> &indices)
    {
        auto id = get_next_id();
        ind_buf.emplace(id, indices);

        return {id};
    }

    col_buf_id load_normals(const std::vector<Vec3>& normals)
    {
        auto id = get_next_id();
        nor_buf.emplace(id, normals);

        normal_id = id;

        return {id};
    }

    void SetDrawColor(const Color4 &c) { drawColor = c; }
    void SetambiColor(const Color4 &c) { ambiColor = c; }
    void SetdiffColor(const Color4 &c) { diffColor = c; }
    void SetspecColor(const Color4 &c) { specColor = c; }
    void SetBG(const Color4 &c) { BG = c; }
    Vec4 GetambiColor() { return ambiColor;}
    Vec4 GetdiffColor() { return diffColor;}
    Vec4 GetspecColor() { return specColor;}

    std::shared_ptr<FrameBuffer> GetFramebuffer() { return framebuffer; }

    void SetFaceCull(FaceCull fc) { faceCull = fc; }

    void Clear() {
        framebuffer->Clear(BG);
        depthBuffer->Fill(0);
    }

    void DrawPixel(int x, int y, Color4 drawcolor) {
        if (IsPointInRect(Vec2{float(x), float(y)},
                          Rect{Vec2{0, 0}, framebuffer->Size()})) {
            framebuffer->PutPixel(x, y, drawcolor);
        }
    }

    void SetViewport(int x, int y, int w, int h) {
        viewport = Mat4x4::Zeros();
        viewport.Set(0, 0, w / 2.0f);
        viewport.Set(1, 1, h / 2.0f);
        viewport.Set(3, 0, w / 2.0f + x);
        viewport.Set(3, 1, h / 2.0f + y);
        viewport.Set(2, 2, 0.5);
        viewport.Set(3, 2, 1);
        viewport.Set(3, 3, 1);
    }

    const Mat4x4& GetViewport() const {
        return viewport;
    }



    void SetVertexShader(VertexShader shader) { vertexShader = shader; }
    void SetFragmentShader(FragmentShader shader) { fragmentShader = shader; }


    void EnableFaceCull(bool e) { enableFaceCull = e; }
    void EnableDepthTest(bool e) { enableDepthTest = e; }
    bool OnlyDrawLine() { return onlyDrawLine; }
    bool EnableLight() { return enableLight; }
    bool EnableTexture() { return enableTexture; }
    void ChangeDrawLine() { onlyDrawLine = !onlyDrawLine; }
    void ChangeLight() { enableLight = !enableLight; }
    void ChangeTexture() { enableTexture = !enableTexture; }
    bool DrawLine() {
        if (!vertexShader) {
            return false;
        }
        for (int i = 0; i < 3; i++) {
            Vertex& vertex = vertices[i];
            vertex.context.Clear();
            vertex.pos4 = vertexShader(i, vertices[i].context);
            vertex.rw = 1.0 / (vertex.pos4.w == 0 ? 1e-5 : vertex.pos4.w);
        }

        for (int i = 0; i < 3; i++) {
            float absw = std::abs(vertices[i].pos4.w);
            if (vertices[i].pos4.x < -absw || vertices[i].pos4.x > absw ||
                vertices[i].pos4.y < -absw || vertices[i].pos4.y > absw) {
                return false;
            }
        }
        for (auto& vertex : vertices) {

            vertex.pos4 *= vertex.rw;
            vertex.pos3 = Vec<3>(viewport * vertex.pos4);
            vertex.pos2.x = int(vertex.pos3.x + 0.5f);
            vertex.pos2.y = int(vertex.pos3.y + 0.5f);
            vertex.context.varyingVec2[ScreenPosition] = Vec2{vertex.pos2.x, vertex.pos2.y};
        }

        for(int i = 0; i < 3; ++i){
            Line2D::Bresenham bresenham(Vec2{vertices[i].pos2.x, vertices[i].pos2.y}, Vec2{vertices[(i + 1) % 3].pos2.x, vertices[(i + 1) % 3].pos2.y});
            while (!bresenham.IsFinished()) {
                DrawPixel(bresenham.CurPoint().x, bresenham.CurPoint().y, {1.0, 1.0, 1.0, 1.0});
                bresenham.Step();
            }
            DrawPixel(bresenham.CurPoint().x, bresenham.CurPoint().y, {1.0, 1.0, 1.0, 1.0});
        }


        return true;
    }

    bool DrawPrimitive() {
        if (!vertexShader) {
            return false;
        }

        for (int i = 0; i < 3; i++) {
            Vertex& vertex = vertices[i];

            vertex.context.Clear();
            vertex.pos4 = vertexShader(i, vertices[i].context);

            if(!isPointInFrustum(vertex.pos4, planes)){
                return false;
            }

            vertex.rw = 1.0 / (vertex.pos4.w == 0 ? 1e-5 : vertex.pos4.w);
        }

        if (enableFaceCull) {
            float result = Cross(Vec<2>(vertices[1].pos4 - vertices[0].pos4),
                                Vec<2>(vertices[2].pos4 - vertices[1].pos4));

            if (faceCull == CCW && result >= 0) {
                return false;
            } else if (faceCull == CW && result <= 0) {
                return false;
            }
        }

        for (auto& vertex : vertices) {

            vertex.pos4 *= vertex.rw;
            vertex.pos3 = Vec<3>(viewport * vertex.pos4);
            vertex.pos2.x = int(vertex.pos3.x + 0.5f);
            vertex.pos2.y = int(vertex.pos3.y + 0.5f);

        }


        if (Cross(vertices[0].pos2 - vertices[1].pos2,
                  vertices[0].pos2 - vertices[2].pos2) == 0) {
            return false;
        }


        TriangleH triangle {vertices[0], vertices[1], vertices[2]};

        Rect boundingBox = AABB(triangle);
        int minX = std::max<int>(boundingBox.pos.x, 0),
                minY = std::max<int>(boundingBox.pos.y, 0),
                maxX = std::min<int>(boundingBox.pos.x + boundingBox.size.w, framebuffer->Width()),
                maxY = std::min<int>(boundingBox.pos.y + boundingBox.size.h, framebuffer->Height());


        for (int i = minX; i < maxX; i++) {
            for (int j = minY; j < maxY; j++) {
                Vec2 p{i + 0.5f, j+ 0.5f};

                Vec3 barycentric = Barycentric(triangle, p);

                if (!IsPointInTriangle(p, barycentric)) {
                    continue;
                }

/*                if (!insideTriangle(i, j, vertices)) {
                    continue;
                }*/

                float rw = vertices[0].rw * barycentric.alpha + vertices[1].rw * barycentric.beta + vertices[2].rw * barycentric.gamma;
                float w = 1.0f / ((rw != 0.0f)? rw : 1.0f);

                barycentric.alpha *= vertices[0].rw * w;
                barycentric.beta *= vertices[1].rw * w;
                barycentric.gamma *= vertices[2].rw * w;

                if (barycentric.alpha < 0 && barycentric.beta < 0 && barycentric.gamma < 0) {
                    return false;
                }

                float z = 1.0 / (barycentric.alpha / vertices[0].pos3.z + barycentric.beta / vertices[1].pos3.z + barycentric.gamma / vertices[2].pos3.z);

                if (enableDepthTest) {
                    if (z <= depthBuffer->Get(i, j)) {
                        continue;
                    }
                    depthBuffer->Set(i, j, z);
                }

                ShaderContext input;
                ShaderContext& c0 = vertices[0].context,
                        c1 = vertices[1].context,
                        c2 = vertices[2].context;
                for (auto& [key, value] : c0.varyingFloat) {
                    input.varyingFloat[key] = c0.varyingFloat[key] * barycentric.alpha +
                                              c1.varyingFloat[key] * barycentric.beta +
                                              c2.varyingFloat[key] * barycentric.gamma;
                }
                for (auto& [key, value] : c0.varyingVec2) {
                    input.varyingVec2[key] = c0.varyingVec2[key] * barycentric.alpha +
                                             c1.varyingVec2[key] * barycentric.beta +
                                             c2.varyingVec2[key] * barycentric.gamma;
                }
                for (auto& [key, value] : c0.varyingVec3) {
                    input.varyingVec3[key] = c0.varyingVec3[key] * barycentric.alpha +
                                             c1.varyingVec3[key] * barycentric.beta +
                                             c2.varyingVec3[key] * barycentric.gamma;
                }
                for (auto& [key, value] : c0.varyingVec4) {
                    input.varyingVec4[key] = c0.varyingVec4[key] * barycentric.alpha +
                                             c1.varyingVec4[key] * barycentric.beta +
                                             c2.varyingVec4[key] * barycentric.gamma;
                }



                Vec4 color{0, 0, 0, 0};
                if (fragmentShader) {
                    color = fragmentShader(input);
                    framebuffer->PutPixel(i, j, color);
                }
            }
        }
        return true;
    }

private:

    Vertex vertices[3];
    std::shared_ptr<FrameBuffer> framebuffer;
    Color4 drawColor;
    Color4 ambiColor;
    Color4 diffColor;
    Color4 specColor;
    Color4 BG;

    VertexShader vertexShader = nullptr;
    FragmentShader fragmentShader = nullptr;
    Buffer2D* depthBuffer = nullptr;
    Mat4x4 viewport;
    FaceCull faceCull = CCW;
    bool enableFaceCull = true;
    bool enableDepthTest = true;
    bool enableLight = false;
    bool enableTexture = false;
    bool onlyDrawLine = false;
};


#endif //ENGINE_HOU_CLION_rendererH
