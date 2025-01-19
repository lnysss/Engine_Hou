//
// Created by hyx on 2024/12/24.
//
#pragma once

#ifndef ENGINE_HOU_CLION_ENGINE_H
#define ENGINE_HOU_CLION_ENGINE_H

#include "SDL2/SDL.h"
#include <string>
#include <chrono>

class Engine {
public:
    Engine(const char* title, int w, int h): title(title) {
        SDL_Init(SDL_INIT_EVERYTHING);
        window = SDL_CreateWindow(title,
                                   SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                   w, h,
                                   SDL_WINDOW_SHOWN);
        if (!window) {
            SDL_Log("can't create window");
        }
        renderer = SDL_CreateRenderer(window, -1, 0);
        if (!renderer) {
            SDL_Log("can't create SDL renderer");
        }
        SDL_Log("SDL init OK");
    }

    virtual ~Engine() {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    void Run() {
        OnInit();
        auto t = std::chrono::high_resolution_clock::now();
        SDL_Log("start app");
        while (!ShouldExit()) {
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    Exit();
                }
                if (event.type == SDL_KEYDOWN) {
                    OnKeyDown(event.key);
                }
                if (event.type == SDL_WINDOWEVENT) {
                    if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                        OnWindowResize(event.window.data1, event.window.data2);
                    }
                }
            }
            auto elapse = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - t);
            t = std::chrono::high_resolution_clock::now();
            SDL_SetWindowTitle(window, (title + "fps: " + std::to_string(int(1000.0 / elapse.count()))).c_str());
            OnRender();
        }
        OnQuit();
    }

    void Exit() { isQuit = true; }
    bool ShouldExit() const { return isQuit; }

    SDL_Window* GetWindow() const { return window; }

    void SwapBuffer(SDL_Surface* surface) {
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (!texture) {
            SDL_Log("swap buffer failed");
        } else {
            SDL_RenderCopy(renderer, texture, nullptr, nullptr);
            SDL_DestroyTexture(texture);
        }
        SDL_RenderPresent(renderer);
    }

    virtual void OnInit() {}
    virtual void OnQuit() {}
    virtual void OnRender() {}

    virtual void OnKeyDown(const SDL_KeyboardEvent&) {}
    virtual void OnKeyUp(const SDL_KeyboardEvent&) {}
    virtual void OnMouseDown(const SDL_MouseButtonEvent&) {}
    virtual void OnMouseUp(const SDL_MouseButtonEvent&) {}
    virtual void OnMotion(const SDL_MouseMotionEvent&) {}
    virtual void OnWindowResize(int, int) {}

private:
    bool isQuit = false;
    SDL_Window* window;
    SDL_Renderer* renderer;
    std::string title;
};

#endif //ENGINE_HOU_CLION_ENGINE_H
