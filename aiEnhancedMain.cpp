#define SDL_MAIN_HANDLED
#include "aiEnhancedScreen.h"
#include <cmath>
#include <algorithm>
#include <chrono>

// Define a 4D vector
struct Vec4 {
    float x, y, z, w;
};

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

// Function to project 4D point to 3D
Vec3 project4Dto3D(const Vec4& v, float wAngle) {
    // Rotate in the 4th dimension (w)
    float c = std::cos(wAngle);
    float s = std::sin(wAngle);

    float x = v.x * c - v.w * s;
    float w = v.x * s + v.w * c;

    return Vec3{x, v.y, v.z};
}

// Function to project 3D point to 2D
Vec3 project3Dto2D(const Vec3& v, float fov, float aspect, float near, float far, float scale) {
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

    // Define the cube's vertices in 4D space (tesseract)
    std::vector<Vec4> hypercubeVertices = {
        {-0.5f, -0.5f, -0.5f, -0.5f}, {0.5f, -0.5f, -0.5f, -0.5f},
        {0.5f,  0.5f, -0.5f, -0.5f}, {-0.5f,  0.5f, -0.5f, -0.5f},
        {-0.5f, -0.5f,  0.5f, -0.5f}, {0.5f, -0.5f,  0.5f, -0.5f},
        {0.5f,  0.5f,  0.5f, -0.5f}, {-0.5f,  0.5f,  0.5f, -0.5f},
        {-0.5f, -0.5f, -0.5f,  0.5f}, {0.5f, -0.5f, -0.5f,  0.5f},
        {0.5f,  0.5f, -0.5f,  0.5f}, {-0.5f,  0.5f, -0.5f,  0.5f},
        {-0.5f, -0.5f,  0.5f,  0.5f}, {0.5f, -0.5f,  0.5f,  0.5f},
        {0.5f,  0.5f,  0.5f,  0.5f}, {-0.5f,  0.5f,  0.5f,  0.5f}
    };

    // Scale the cube by a factor of 1.3
    for (auto& vertex : hypercubeVertices) {
        vertex.x *= 1.3f;
        vertex.y *= 1.3f;
        vertex.z *= 1.3f;
        vertex.w *= 1.3f;
    }

    // Define the hypercube's edges
    std::vector<std::pair<int, int>> hypercubeEdges = {
        {0,1},{1,2},{2,3},{3,0},
        {4,5},{5,6},{6,7},{7,4},
        {0,4},{1,5},{2,6},{3,7},
        {0,8},{1,9},{2,10},{3,11},
        {4,12},{5,13},{6,14},{7,15},
        {8,9},{9,10},{10,11},{11,8},
        {12,13},{13,14},{14,15},{15,12},
        {8,12},{9,13},{10,14},{11,15}
    };

    // Define colors for vertices
    std::vector<SDL_Color> vertexColors = {
        {255,0,0,255}, {0,255,0,255}, {0,0,255,255}, {255,255,0,255},
        {255,0,255,255}, {0,255,255,255}, {255,128,0,255}, {128,0,255,255},
        {255,255,255,255}, {128,128,128,255}, {64,64,64,255}, {192,192,192,255},
        {0,0,0,255}, {255,128,128,255}, {128,255,128,255}, {128,128,255,255}
    };

    // Calculate scale factor based on viewport size and FOV
    constexpr float TARGET_HEIGHT_RATIO = 0.6f; // 60% of viewport height
    float tan_half_fov = std::tan((FOV * DEG2RAD) / 2);
    float scale = (TARGET_HEIGHT_RATIO * VIEWPORT_HEIGHT / 2) / (0.5f / tan_half_fov);

    // Adjust the scale by 1.3 to make the cube appear larger
    scale *= 1.3f;

    auto start_time = std::chrono::high_resolution_clock::now();

    while (!screen.shouldQuit()) {
        auto current_time = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float>(current_time - start_time).count();

        // Define rotation angles
        float rot_x = 0.5f * time;
        float rot_y = 0.3f * time;
        float rot_z = 0.2f * time;
        float rot_w = 0.7f * time; // Rotation in the 4th dimension

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

            // Adjust rotation and 4D projection based on viewport
            bool use4D = false;
            if (viewport == 0) {
                use4D = true; // Quadrant 1: 4D rotation in x-w plane
            } else if (viewport == 2) {
                use4D = true; // Quadrant 3: 4D rotation in y-w plane
            }

            // For quadrant 2, create an impressive effect
            if (viewport == 1) {
                rotation = angleAxis(rot_x * 2, Vec3{1, 0, 0}) *
                           angleAxis(rot_y * 2, Vec3{0, 1, 0}) *
                           angleAxis(rot_z * 2, Vec3{0, 0, 1});
            }

            // Vector to store projected points
            std::vector<Vec3> projectedPoints;
            projectedPoints.reserve(hypercubeVertices.size());

            for (size_t i = 0; i < hypercubeVertices.size(); ++i) {
                Vec4 point = hypercubeVertices[i];
                Vec3 projected3D;

                if (use4D) {
                    // Rotate in 4D
                    if (viewport == 0) {
                        // Rotate in x-w plane
                        float c = std::cos(rot_w);
                        float s = std::sin(rot_w);
                        float x = point.x * c - point.w * s;
                        float w = point.x * s + point.w * c;
                        point.x = x;
                        point.w = w;
                    } else if (viewport == 2) {
                        // Rotate in y-w plane
                        float c = std::cos(rot_w);
                        float s = std::sin(rot_w);
                        float y = point.y * c - point.w * s;
                        float w = point.y * s + point.w * c;
                        point.y = y;
                        point.w = w;
                    }

                    // Project from 4D to 3D
                    projected3D = project4Dto3D(point, rot_w);
                } else {
                    // Discard w component
                    projected3D = Vec3{point.x, point.y, point.z};
                }

                // Apply rotation
                Vec3 rotated = rotation.rotate(projected3D);

                // Move the cube slightly back to ensure it's fully visible
                rotated.z += 2.0f; // Adjusted from 3.0f to 4.0f to account for increased size

                // Project the 3D point to 2D
                Vec3 projected = project3Dto2D(rotated, FOV, static_cast<float>(VIEWPORT_WIDTH) / VIEWPORT_HEIGHT, NEAR_PLANE, FAR_PLANE, scale);

                // Map projected coordinates to viewport center
                float screen_x = projected.x + VIEWPORT_WIDTH / 2.0f;
                float screen_y = -projected.y + VIEWPORT_HEIGHT / 2.0f; // Invert y-axis for correct orientation

                projectedPoints.emplace_back(Vec3{screen_x, screen_y, projected.z});
            }

            // Draw the cube edges
            for (const auto& edge : hypercubeEdges) {
                const Vec3& start = projectedPoints[edge.first];
                const Vec3& end = projectedPoints[edge.second];

                // Use colors from vertices
                SDL_Color colorStart = vertexColors[edge.first];
                SDL_Color colorEnd = vertexColors[edge.second];

                // Interpolate colors
                int r = (colorStart.r + colorEnd.r) / 2;
                int g = (colorStart.g + colorEnd.g) / 2;
                int b = (colorStart.b + colorEnd.b) / 2;

                drawLine(renderer, start, end, r, g, b);
            }

            // For quadrant 2, add a "WOW" factor with a pulsating sphere
            if (viewport == 1) {
                int numSegments = 100;
                float radius = 150 + 50 * std::sin(time * 2);

                SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255); // Gold color

                for (int i = 0; i < numSegments; ++i) {
                    float theta1 = (2.0f * M_PI * i) / numSegments;
                    float theta2 = (2.0f * M_PI * (i + 1)) / numSegments;

                    float x1 = VIEWPORT_WIDTH / 2 + radius * std::cos(theta1);
                    float y1 = VIEWPORT_HEIGHT / 2 + radius * std::sin(theta1);

                    float x2 = VIEWPORT_WIDTH / 2 + radius * std::cos(theta2);
                    float y2 = VIEWPORT_HEIGHT / 2 + radius * std::sin(theta2);

                    SDL_RenderDrawLineF(renderer, x1, y1, x2, y2);
                }
            }
        }

        // Present the rendered frame
        SDL_RenderPresent(renderer);

        // Delay to cap at ~60 FPS
        SDL_Delay(16);
    }

    return 0;
}
