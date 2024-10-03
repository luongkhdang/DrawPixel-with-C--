#define SDL_MAIN_HANDLED
#include "screen.h"
#include <numeric>


struct vec3{
    float x,y,z;
};

struct connection{
    int a,b;
};

void rotate(vec3& point, float x = 1, float y=1, float z=1){
    float rad_x = x;
    float rad_y = y;
    float rad_z = z;

    // Copy the original values to avoid overwriting
    float tempY, tempZ, tempX;

    // Rotate around x-axis
    tempY = std::cos(rad_x) * point.y - std::sin(rad_x) * point.z;
    tempZ = std::sin(rad_x) * point.y + std::cos(rad_x) * point.z;

    point.y = tempY;
    point.z = tempZ;

    // Rotate around y-axis
    tempX = std::cos(rad_y) * point.x + std::sin(rad_y) * point.z;
    tempZ = -std::sin(rad_y) * point.x + std::cos(rad_y) * point.z;

    point.x = tempX;
    point.z = tempZ;

    // Rotate around z-axis
    tempX = std::cos(rad_z) * point.x - std::sin(rad_z) * point.y;
    tempY = std::sin(rad_z) * point.x + std::cos(rad_z) * point.y;

    point.x = tempX;
    point.y = tempY;

}


void line(Screen& screen, float x1, float y1, float x2, float y2){
    float dx = x2 - x1;
    float dy = y2 - y1;

    float length = std::sqrt(dx*dx + dy*dy);
    float angle = std::atan2(dy, dx);

    for (float i = 0; i < length; i ++){
        screen.pixel(
            x1+std::cos(angle)*i, 
            y1+std::sin(angle)*i);

    }
}

int main(){
    Screen screen;

    std::vector<vec3> points {
        {173, 173, 173},
        {400, 173, 173},
        {400, 400, 173},
        {173, 400, 173},

        {173, 173, 400},
        {400, 173, 400},
        {400, 400, 400},
        {173, 400, 400},
    };

    std::vector<connection> connections{
        {0,4},
        {1,5},
        {2,6},
        {3,7},

        {0,1},
        {1,2},
        {2,3},
        {3,0},

        {4,5},
        {5,6},
        {6,7},
        {7,4},


    };

    //Calculate centroid
    //

    vec3 c{0,0,0}; //centeroid
    for(auto& p : points) {
        c.x += p.x;
        c.y += p.y;
        c.z += p.z;
    }

    c.x /= points.size();
    c.y /= points.size();
    c.z /= points.size();





    while(true){
        for(auto& p: points) {
            p.x -= c.x;
            p.y -= c.y;
            p.z -= c.z;
            rotate(p, 0.002,0.005,0.002);
            p.x += c.x;
            p.y += c.y;
            p.z += c.z;
            screen.pixel(p.x, p.y);
        }
        for(auto& conn: connections){
            line(screen,
                points[conn.a].x,
                points[conn.a].y,
                points[conn.b].x,
                points[conn.b].y
            );
        }

   
        screen.show();
        screen.clear(); 
        screen.input();
        SDL_Delay(3);
    }
    return 0;
}