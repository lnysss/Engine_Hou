cmake_minimum_required(VERSION 3.27.2)
project(Engine_Hou_Clion)

set(CMAKE_CXX_STANDARD 17)

add_executable(Engine_Hou_Clion main.cpp
        renderer.h
        h_math.h
        h_vector.h
        h_matrix.h
        h_framebuffer.h
        h_drawline.h
        h_shader.h
        h_vertex.h
        engine.h
        h_camera.h
        h_light.h
        h_light.h
        h_obj.h
)
