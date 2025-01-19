#include "renderer.h"
#include "engine.h"
#include "h_camera.h"
#include "h_light.h"
#include <string>
#include "h_obj.h"

constexpr int WindowWidth = 720;
constexpr int WindowHeight = 480;

FrameBuffer* texture = nullptr;
inline unsigned int BeginIndex = 0;

class H_Engine: public Engine {
public:
    H_Engine(): Engine("Position - WASDQE, Rotation - 1234, Light - j, Texture - k, Line - l", WindowWidth, WindowHeight) {}

    void OnInit() override {


        loader.reset(new Loader());

        std::cout<< "start load" << std::endl;
        loader->LoadFile("D:/GAMES/spot.obj");


        std::cout<< "load success" << std::endl;

        texture = new FrameBuffer("D:/GAMES/spot.jpg");

        for(auto mesh: loader->LoadedMeshes)
        {
            for(int i=0;i<mesh.Vertices.size();i+=3)
            {
                auto* t = new Triangle();
                for(int j=0;j<3;j++)
                {
                    t->setVertex(j,Vec4{mesh.Vertices[i+j].Position.X,mesh.Vertices[i+j].Position.Y,mesh.Vertices[i+j].Position.Z,1.0});
                    t->setNormal(j,Vec3{mesh.Vertices[i+j].Normal.X,mesh.Vertices[i+j].Normal.Y,mesh.Vertices[i+j].Normal.Z});
                    t->setTexCoord(j,Vec2{mesh.Vertices[i+j].TextureCoordinate.X, mesh.Vertices[i+j].TextureCoordinate.Y});
                }
                TriangleList.push_back(t);
            }
        }

        pos.x = 0;
        pos.y = 0;

        renderer.reset(new Renderer(WindowWidth, WindowHeight));
        renderer->SetFaceCull(CW);

        renderer->SetBG(Color4{0.678, 0.847, 0.902, 1.0});
        renderer->SetambiColor(Vec4{0.55f, 0.55f, 0.55f, 1.0f});
        renderer->SetdiffColor(Vec4{0.20f, 0.20f, 0.20f, 1.0f});
        renderer->SetspecColor(Vec4{0.05f, 0.05f, 0.05f, 1.0f});

        renderer->Clear();
        renderer->SetViewport(0, 0, WindowWidth, WindowHeight);

        camera.reset(new Camera(90, WindowWidth / 2.0f, WindowHeight / 2.0f, -0.1f, -5.0f));

        camera->projection = Persp(Radians(camera->fov), float(camera->weight) / camera->height, camera->near, camera->far);
        camera->view = View(camera->lookfrom, camera->lookat, camera->up);
        camera->calculateFrustumPlanes();

        for(int i = 0; i < 6; i++){
            renderer->planes[i] = camera->frustumPlanes[i];
        }


        light.reset(new PointLight());
        light->SetPosition(Vec4{2.0f, 2.0f, -2.0f, 1.0f});
        light->SetRadiance(Vec4{5.0f, 5.0f, 5.0f, 1.0f});
        light->SetRadius(5.0f);
        light->SetIntensity(10.0f);
        light->SetFalloff(0.85);

        renderer->SetVertexShader([&](int index, ShaderContext& output) {

            output.varyingVec2[Texcoord] = Vec2 {TriangleList[BeginIndex]->tex_coords[index].x, TriangleList[BeginIndex]->tex_coords[index].y};
            output.varyingVec4[Normal] = Inverse(camera->model) * Vec4{TriangleList[BeginIndex]->normal[index].x,TriangleList[BeginIndex]->normal[index].y, TriangleList[BeginIndex]->normal[index].z, 0.0f };
            output.varyingVec4[WorldPosition] = camera->model * Vec4{TriangleList[BeginIndex]->v[index].x ,TriangleList[BeginIndex]->v[index].y , TriangleList[BeginIndex]->v[index].z, 1.0f };
            output.varyingVec3[Color] = Vec3{TriangleList[BeginIndex]->color[index].x, TriangleList[BeginIndex]->color[index].y, TriangleList[BeginIndex]->color[index].z};
            output.varyingVec4[ViewPosition] = camera->view * output.varyingVec4[WorldPosition];
            return camera->projection * output.varyingVec4[ViewPosition];
        });

        renderer->SetFragmentShader([&](ShaderContext& input) {

            Vec4 final(renderer->GetambiColor());

            if(renderer->EnableLight()){

                Vec4 worldPos = input.varyingVec4[WorldPosition];
                Vec3 c = input.varyingVec3[Color];
                Vec4 lightPosition = light->Position;
                Vec4 ks = Vec4{0.7937, 0.7937, 0.7937, 1.0f};
                Vec4 kd = Vec4{c.x , c.y, c.z, 1.0f};

                Vec3 N = Normalize(Vec3{input.varyingVec4[Normal].x, input.varyingVec4[Normal].y, input.varyingVec4[Normal].z});
                Vec3 lightPos = Vec3{lightPosition.x, lightPosition.y, lightPosition.z};
                Vec3 Pos = Vec3{worldPos.x, worldPos.y, worldPos.z} / worldPos.w;
                Vec3 eye = camera->lookfrom;

                Vec3 L = Normalize(Pos - lightPos);
                Vec3 V = Normalize(eye - Pos);
                Vec3 H = Normalize(L + V);

                float p = 750.0f;
                float specular = std::pow(std::abs(Dot(H, N)),p);


                float ambient = 0.5f;
                float lambertian = std::abs(Dot(L, N));
                float diatance2 = Len2(lightPos - Pos);
                float radius2 = std::pow((light->Radius), 2.0f);

                float falloff = Clamp(1.0f - diatance2 / radius2, 0.0f, 1.0f) * light->Falloff;
                float intensity = light->Intensity / diatance2;


                Vec4 spec = ks * specular * intensity * renderer->GetspecColor();

                Vec4 diff = lambertian * intensity * renderer->GetdiffColor();


                final += falloff * light->Radiance * (spec + diff);
                if(final.x >1.0f) final.x = 1.0f;
                if(final.y >1.0f) final.y = 1.0f;
                if(final.z >1.0f) final.z = 1.0f;
                final.w = 1.0f;

            }

            if(renderer->EnableTexture()){
                final *= TextureSample(texture, Vec2{input.varyingVec2[Texcoord].x, 1.0f - input.varyingVec2[Texcoord].y});
            }

            float gamma = 0.454f;
            final.x = std::pow(final.x,gamma);
            final.y = std::pow(final.y,gamma);
            final.z = std::pow(final.z,gamma);
            return final;
        });
    }

    void OnKeyDown(const SDL_KeyboardEvent& e) override {

        if (e.keysym.sym == SDLK_KP_1) {
            euler.y -= 10;
        }
        if (e.keysym.sym == SDLK_KP_4) {
            euler.y += 10;
        }
        if (e.keysym.sym == SDLK_KP_2) {
            euler.x += 10;
        }
        if (e.keysym.sym == SDLK_KP_5) {
            euler.x -= 10;
        }
        if (e.keysym.sym == SDLK_KP_3) {
            euler.z += 10;
        }
        if (e.keysym.sym == SDLK_KP_6) {
            euler.z -= 10;
        }

        Vec3 normalEuler = NormalizeEuler(euler);

        if (e.keysym.sym == SDLK_w) {
            camera->lookfrom.y -= 0.1;
            camera->lookat.y -= 0.1;
        }
        if (e.keysym.sym == SDLK_s) {
            camera->lookfrom.y += 0.1;
            camera->lookat.y += 0.1;
        }
        if (e.keysym.sym == SDLK_a) {
            camera->lookfrom.x -= 0.1;
            camera->lookat.x -= 0.1;
        }
        if (e.keysym.sym == SDLK_d) {
            camera->lookfrom.x += 0.1;
            camera->lookat.x += 0.1;
        }
        if (e.keysym.sym == SDLK_q) {
            camera->lookfrom.z += 0.1;
            camera->lookat.z += 0.1;
        }
        if (e.keysym.sym == SDLK_e) {
            camera->lookfrom.z -= 0.1;
            camera->lookat.z -= 0.1;
        }
        if (e.keysym.sym == SDLK_1) {
            camera->lookat.x -= 0.2;
        }
        if (e.keysym.sym == SDLK_2) {
            camera->lookat.x += 0.2;
        }
        if (e.keysym.sym == SDLK_3) {
            camera->lookat.y -= 0.2;
        }
        if (e.keysym.sym == SDLK_4) {
            camera->lookat.y += 0.2;
        }

        Quaternion quaternion = eulerToQuaternion(Radians(normalEuler.x), Radians(normalEuler.y), Radians(normalEuler.z));
        Mat4x4 r = RotateQuaternion(quaternion);
        //Mat4x4 r = RotateEuler(Radians(normalEuler.x), Radians(normalEuler.y), Radians(normalEuler.z));


        camera->model = r;
        camera->view = View(camera->lookfrom, camera->lookat, camera->up);

        if (e.keysym.sym == SDLK_j) {
            renderer->ChangeLight();
        }
        if (e.keysym.sym == SDLK_k) {
            renderer->ChangeTexture();
        }
        if (e.keysym.sym == SDLK_l) {
            renderer->ChangeDrawLine();
        }
    }

    void OnRender() override {
        renderer->SetDrawColor(Color4{1, 1, 1, 1});
        renderer->Clear();

        BeginIndex = 0;

        for (int i = 0; i < TriangleList.size(); i++) {
            if(renderer->OnlyDrawLine()){
                renderer->DrawLine();
            }else{
                renderer->DrawPrimitive();
            };
            BeginIndex += 1;
        }


        SwapBuffer(renderer->GetFramebuffer()->GetRaw());
    }

    void OnQuit() override {
        loader.reset();
        renderer.reset();
        delete[] TriangleList.data();
        delete texture;
    }


private:
    std::vector<Triangle*> TriangleList;
    std::unique_ptr<Loader> loader;
    std::unique_ptr<PointLight> light;
    std::unique_ptr<Camera> camera;
    std::unique_ptr<Renderer> renderer;
    Vec3 pos;
    Vec3 euler;
};

int main(int, char**) {

    Renderer::Init();
    H_Engine engine;
    engine.Run();
    Renderer::Quit();
    return 0;
}

