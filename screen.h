#include <SDL2/SDL.h>
#include <vector>
#include <iostream>


class Screen{
    SDL_Event e;
    SDL_Window* window;
    SDL_Renderer* renderer;
    std::vector<SDL_FPoint> points;
    
public:
    Screen()
    {
        if (SDL_Init(SDL_INIT_VIDEO) != 0) {
            std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
            exit(1);  // Exit if SDL fails to initialize
        }

        if (SDL_CreateWindowAndRenderer(640 * 2, 480 * 2, 0, &window, &renderer) != 0) {
            std::cerr << "Window/Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
            SDL_Quit();
            exit(1);  // Exit if window/renderer creation fails
        }

        SDL_RenderSetScale(renderer, 2, 2);
    }

    ~Screen() {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();  // Clean up SDL when the object is destroyed
    }

    void pixel(float x, float y) {
        SDL_FPoint point = {x, y};  // Explicitly create an SDL_FPoint
        points.emplace_back(point);  //pushback emplace_back
    }

    void show(){
        SDL_SetRenderDrawColor(renderer,0,0,0,255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer,255,255,255,255);
        for(auto& point:points){
            SDL_RenderDrawPointF(renderer,point.x, point.y);
        }
        SDL_RenderPresent(renderer);
    }

    void clear(){
        points.clear();
    }

    void input() {
        while(SDL_PollEvent(&e)){
            if(e.type == SDL_QUIT){
                SDL_Quit();
                exit(0);
            }
        }
    }
};

