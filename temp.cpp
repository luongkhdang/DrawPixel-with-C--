
Think of what 4-dimension could means to the cubes. Implement 4-dimension to quadrant 1 and 3 in different sense.
Add colors to the corners.
For quadrant 2, think of what would impress the user when they see it. In a sense of 3d rendering. Make sure it has a "WOW" factor. 
make sure all cubes are perfectly symmetrical. 

//screen.h
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

        if (SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer) != 0) {
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


//main.cpp
#define SDL_MAIN_HANDLED
#include "screen.h"
#include <cmath>
#include <algorithm>
#include <chrono>

// Define a 3D vector
struct Vec3 {
    float x, y, z;
};

// Define a quaternion for rotation
struct Quaternion {
    float w, x, y, z;

    Quaternion() : w(1), x(0), y(0), z(0) {}
    Quaternion(float w_, float x_, float y_, float z_) : w(w_), x(x_), y(y_), z(z_) {}

    Quaternion operator*(const Quaternion& q) const {
        return Quaternion(
            w*q.w - x*q.x - y*q.y - z*q.z,
            w*q.x + x*q.w + y*q.z - z*q.y,
            w*q.y - x*q.z + y*q.w + z*q.x,
            w*q.z + x*q.y - y*q.x + z*q.w
        );
    }

    Vec3 rotate(const Vec3& v) const {
        Quaternion p(0, v.x, v.y, v.z);
        Quaternion q = (*this) * p * conjugate();
        return Vec3{q.x, q.y, q.z};
    }

    Quaternion conjugate() const {
        return Quaternion(w, -x, -y, -z);
    }
};

// Create a quaternion from angle and axis
Quaternion angleAxis(float angle, const Vec3& axis) {
    float s = std::sin(angle / 2);
    float c = std::cos(angle / 2);
    return Quaternion(c, axis.x * s, axis.y * s, axis.z * s);
}

// Define projection parameters
constexpr float FOV = 60.0f; // Field of view in degrees
constexpr float NEAR_PLANE = 0.1f;
constexpr float FAR_PLANE = 100.0f;

// Define scaling factor to adjust cube size
constexpr float DEG2RAD = M_PI / 180.0f;

// Function to project 3D point to 2D
Vec3 project(const Vec3& v, float fov, float aspect, float near, float far, float scale) {
    float fov_rad = fov * DEG2RAD;
    float tan_half_fov = std::tan(fov_rad / 2);
    float z_range = far - near;

    // Prevent division by zero
    if (v.z == 0) return Vec3{0, 0, 0};

    float x = (v.x / (v.z * tan_half_fov)) * scale;
    float y = (v.y / (v.z * tan_half_fov * aspect)) * scale;
    float z = (v.z - near) / z_range * 2.0f - 1.0f;

    return Vec3{x, y, z};
}

// Function to draw a line with specified color
void drawLine(SDL_Renderer* renderer, const Vec3& start, const Vec3& end, int r, int g, int b) {
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    SDL_RenderDrawLineF(renderer, start.x, start.y, end.x, end.y);
}

int main() {
    Screen screen;
    SDL_Renderer* renderer = screen.getRenderer();

    // Define the cube's vertices
    std::vector<Vec3> points = {
        {-0.5f, -0.5f, -0.5f}, {0.5f, -0.5f, -0.5f},
        {0.5f, 0.5f, -0.5f}, {-0.5f, 0.5f, -0.5f},
        {-0.5f, -0.5f, 0.5f}, {0.5f, -0.5f, 0.5f},
        {0.5f, 0.5f, 0.5f}, {-0.5f, 0.5f, 0.5f}
    };

    // Define the cube's edges
    std::vector<std::pair<int, int>> connections = {
        {0, 1}, {1, 2}, {2, 3}, {3, 0},
        {4, 5}, {5, 6}, {6, 7}, {7, 4},
        {0, 4}, {1, 5}, {2, 6}, {3, 7}
    };

    // Calculate scale factor based on viewport size and FOV
    constexpr float TARGET_HEIGHT_RATIO = 0.6f; // 60% of viewport height
    float tan_half_fov = std::tan((FOV * DEG2RAD) / 2);
    float scale = (TARGET_HEIGHT_RATIO * VIEWPORT_HEIGHT / 2) / (0.5f / tan_half_fov);

    auto start_time = std::chrono::high_resolution_clock::now();

    while (!screen.shouldQuit()) {
        auto current_time = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float>(current_time - start_time).count();

        // Define rotation angles
        float rot_x = 0.5f * time;
        float rot_y = 0.3f * time;
        float rot_z = 0.2f * time;

        // Clear the renderer
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Iterate through each viewport (quadrant)
        for (int viewport = 0; viewport < 4; ++viewport) {
            // Calculate viewport position
            int vx = (viewport % VIEWPORT_COLUMNS) * VIEWPORT_WIDTH;
            int vy = (viewport / VIEWPORT_COLUMNS) * VIEWPORT_HEIGHT;

            SDL_Rect viewportRect = {vx, vy, VIEWPORT_WIDTH, VIEWPORT_HEIGHT};
            SDL_RenderSetViewport(renderer, &viewportRect);

            // Create rotation quaternion
            Quaternion rotation = angleAxis(rot_x, Vec3{1, 0, 0}) *
                                  angleAxis(rot_y, Vec3{0, 1, 0}) *
                                  angleAxis(rot_z, Vec3{0, 0, 1});

            // Adjust rotation based on viewport for different perspectives
            if (viewport == 1) rotation = angleAxis(-rot_x, Vec3{1, 0, 0}) * rotation;
            if (viewport == 2) rotation = angleAxis(-rot_y, Vec3{0, 1, 0}) * rotation;
            if (viewport == 3) rotation = angleAxis(-rot_z, Vec3{0, 0, 1}) * rotation;

            // Vector to store projected points
            std::vector<Vec3> projectedPoints;
            projectedPoints.reserve(points.size());

            for (const auto& point : points) {
                // Apply rotation
                Vec3 rotated = rotation.rotate(point);

                // Move the cube slightly back to ensure it's fully visible
                rotated.z += 2.0f;

                // Project the 3D point to 2D
                Vec3 projected = project(rotated, FOV, static_cast<float>(VIEWPORT_WIDTH) / VIEWPORT_HEIGHT, NEAR_PLANE, FAR_PLANE, scale);

                // Map projected coordinates to viewport center
                float screen_x = projected.x + VIEWPORT_WIDTH / 2.0f;
                float screen_y = -projected.y + VIEWPORT_HEIGHT / 2.0f; // Invert y-axis for correct orientation

                projectedPoints.emplace_back(Vec3{screen_x, screen_y, projected.z});
            }

            // Draw the cube edges
            for (const auto& conn : connections) {
                const Vec3& start = projectedPoints[conn.first];
                const Vec3& end = projectedPoints[conn.second];
                
                // Determine color based on depth for better visualization
                int r = 255, g = 255, b = 255;
                if (start.z < 0 && end.z < 0) { r = 255; g = 0; b = 0; }        // Back face
                else if (start.z >= 0 && end.z >= 0) { r = 0; g = 255; b = 0; }  // Front face
                else { r = 0; g = 0; b = 255; }                                // Side edge

                drawLine(renderer, start, end, r, g, b);
            }
        }

        // Present the rendered frame
        SDL_RenderPresent(renderer);

        // Delay to cap at ~60 FPS
        SDL_Delay(16);
    }

    return 0;
}