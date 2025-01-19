//
// Created by hyx on 2024/12/24.
//

#include <array>
#include <algorithm>
#include "h_shader.h"

#ifndef ENGINE_HOU_CLION_H_VERTEX_H
#define ENGINE_HOU_CLION_H_VERTEX_H

struct Vertex {
    ShaderContext context;
    float rw;
    Vec4 pos4;
    Vec3 pos3;
    Vec2 pos2;
};

struct TriangleH{
    Vertex v1;
    Vertex v2;
    Vertex v3;
};

class Triangle{

public:
    Vec4 v[3]; /*the original coordinates of the triangle, v0, v1, v2 in counter clockwise order*/
    /*Per vertex values*/
    Vec3 color[3]; //color at each vertex;
    Vec2 tex_coords[3]; //texture u,v
    Vec3 normal[3]; //normal vector for each vertex

    Triangle(){
        v[0] = {0,0,0,1};
        v[1] = {0,0,0,1};
        v[2] = {0,0,0,1};

        color[0] ={0.0, 0.0, 0.0};
        color[1] ={0.0, 0.0, 0.0};
        color[2] ={0.0, 0.0, 0.0};

        tex_coords[0] ={0.0, 0.0};
        tex_coords[1] ={0.0, 0.0};
        tex_coords[2] ={0.0, 0.0};
    };

    Vec4 a() const { return v[0]; }
    Vec4 b() const { return v[1]; }
    Vec4 c() const { return v[2]; }

    void setVertex(int ind, Vec4 ver){
        v[ind].x = ver.x;
        v[ind].y = ver.y;
        v[ind].z = ver.z;
        v[ind].w = ver.w;
    }; /*set i-th vertex coordinates */
    void setNormal(int ind, Vec3 n){
        normal[ind].x = n.x;
        normal[ind].y = n.y;
        normal[ind].z = n.z;
    }; /*set i-th vertex normal vector*/
    void setColor(int ind, float r, float g, float b){
        if((r<0.0) || (r>255.) ||
           (g<0.0) || (g>255.) ||
           (b<0.0) || (b>255.)) {
            fprintf(stderr, "ERROR! Invalid color values");
            fflush(stderr);
            exit(-1);
        }

        color[ind] = Vec3{(float)r/255.0f,(float)g/255.0f,(float)b/255.0f};
   }; /*set i-th vertex color*/

    void setNormals(const std::array<Vec4, 3>& normals){
        normal[0] = {normals[0].x, normals[0].y, normals[0].z};
        normal[1] = {normals[1].x, normals[1].y, normals[1].z};
        normal[2] = {normals[2].x, normals[2].y, normals[2].z};
    };
    void setColors(const std::array<Vec4, 3>& colors){
        auto first_color = colors[0];
        setColor(0, colors[0][0], colors[0][1], colors[0][2]);
        setColor(1, colors[1][0], colors[1][1], colors[1][2]);
        setColor(2, colors[2][0], colors[2][1], colors[2][2]);
    };
    void setTexCoord(int ind,Vec2 uv ){
        tex_coords[ind].x = uv.x;
        tex_coords[ind].y = uv.y;
    }; /*set i-th vertex texture coordinate*/
/*    std::array<Vec4, 3> toVector4() const{
        std::array<Vec4, 3> res{};
        std::transform(std::begin(v), std::end(v), res.begin(), [](auto& vec) { return Vec4{vec.x(), vec.y(), vec.z(), 1.0f}; });
        return res;
    };*/
};

#endif //ENGINE_HOU_CLION_H_VERTEX_H
