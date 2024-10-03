#pragma once
#include <SDL2/SDL.h>
#include <vector>
#include <iostream>

// Define window dimensions
constexpr int WINDOW_WIDTH = 1280;
constexpr int WINDOW_HEIGHT = 960;

// Define viewport dimensions (each quadrant)
constexpr int VIEWPORT_COLUMNS = 2;
constexpr int VIEWPORT_ROWS = 2;
constexpr int VIEWPORT_WIDTH = WINDOW_WIDTH / VIEWPORT_COLUMNS;   // 640
constexpr int VIEWPORT_HEIGHT = WINDOW_HEIGHT / VIEWPORT_ROWS;     // 480

class Screen {
    SDL_Event e;
    SDL_Window* window;
    SDL_Renderer* renderer;
    std::vector<SDL_FPoint> points;

public:
    Screen() {
        if (SDL_Init(SDL_INIT_VIDEO) != 0) {
            std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
            exit(1);
        }

        if (SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN, &window, &renderer) != 0) {
            std::cerr << "Window/Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
            SDL_Quit();
            exit(1);
        }

        // Set logical size to handle high-DPI displays if necessary
        SDL_RenderSetLogicalSize(renderer, WINDOW_WIDTH, WINDOW_HEIGHT);
    }

    ~Screen() {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    void pixel(float x, float y) {
        SDL_FPoint point = {x, y};
        points.emplace_back(point);
    }

    void show() {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        for (auto& point : points) {
            SDL_RenderDrawPointF(renderer, point.x, point.y);
        }
        SDL_RenderPresent(renderer);
    }

    void clear() {
        points.clear();
    }

    bool shouldQuit() {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                return true;
            }
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
                return true;
            }
        }
        return false;
    }

    SDL_Renderer* getRenderer() { return renderer; }
};
