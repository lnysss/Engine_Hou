//
// Created by hyx on 2024/12/24.
//

#ifndef ENGINE_HOU_CLION_H_framebufferH
#define ENGINE_HOU_CLION_H_framebufferH

#define Log(fmt, ...) printf("%s[%s: %d]: " fmt "\n", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "h_math.h"

class FrameBuffer final {
public:
    FrameBuffer(const char *filename) {
        m_frameBuffer = SDL_ConvertSurfaceFormat(IMG_Load(filename), SDL_PIXELFORMAT_RGBA32, 0);
        if (!m_frameBuffer) {
            Log("load %s failed", filename);
        }
    }

    FrameBuffer(int w, int h) {
        m_frameBuffer =
                SDL_CreateRGBSurfaceWithFormat(0, w, h, 32, SDL_PIXELFORMAT_RGBA32);
        if (!m_frameBuffer) {
            Log("Create Surface failed: %s", SDL_GetError());
        }
    }

    FrameBuffer(const FrameBuffer &) = delete;

    ~FrameBuffer() { SDL_FreeSurface(m_frameBuffer); }

    FrameBuffer &operator=(const FrameBuffer &) = delete;

    inline int Width() const { return m_frameBuffer->w; }
    inline int Height() const { return m_frameBuffer->h; }
    inline Vec2 Size() const { return {float(m_frameBuffer->w), float(m_frameBuffer->h)}; }
    void PutPixel(int x, int y, const Color4 &color) {
        *getPixel(x, y) = SDL_MapRGBA(m_frameBuffer->format, color.r * 255,
                                      color.g * 255, color.b * 255, color.a * 255);
    }

    Color4 GetPixel(int x, int y) const {
        const Uint32 *color = getPixel(x, y);
        Uint8 r, g, b, a;
        SDL_GetRGBA(*color, m_frameBuffer->format, &r, &g, &b, &a);
        return Color4{r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f};
    }

    inline void Clear(const Color4 &color) {
        SDL_FillRect(m_frameBuffer, nullptr,
                     SDL_MapRGBA(m_frameBuffer->format, color.r * 255, color.g * 255,
                                 color.b * 255, color.a * 255));
    }



    SDL_Surface* GetRaw() const { return m_frameBuffer; }

private:
    SDL_Surface *m_frameBuffer;

    Uint32 *getPixel(int x, int y) const {
        Uint8 *ptr = (Uint8 *)m_frameBuffer->pixels;
        return (Uint32 *)(ptr + y * m_frameBuffer->pitch +
                          x * m_frameBuffer->format->BytesPerPixel);
    }
};

class Buffer2D {
public:
    Buffer2D(int w, int h): w_(w), h_(h) {
        data_ = new float[w * h];
        Fill(0);
    }

    void Fill(float value) {
        for (int i = 0; i < w_ * h_; i++) {
            data_[i] = value;
        }
    }

    float& Get(int x, int y) {
        return data_[x * h_ + y];
    }

    float Get(int x, int y) const {
        return data_[x * h_ + y];
    }

    void Set(int x, int y, float value) {
        data_[x * h_ + y] = value;
    }

    int Width() const { return w_; }
    int Height() const { return h_; }

    ~Buffer2D() {
        delete[] data_;
    }

private:
    float* data_;
    int w_;
    int h_;
};

#endif //ENGINE_HOU_CLION_H_framebufferH
