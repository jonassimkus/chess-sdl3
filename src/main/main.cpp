#include <SDL.h>
#include <SDL_main.h>
#include <iostream>

int main(){

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Chess - Jonas", 1024, 1024, SDL_WINDOW_VULKAN);

    if (window == nullptr){
        std::cout << "SDL3 - Window failed to initialize" << std::endl;
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);

    if (renderer == nullptr){
        std::cout << "SDL3 - renderer failed to initialize" << std::endl;
        return 1;
    }

    bool running = true;

    while (running){
        SDL_Event event;
        while (SDL_PollEvent(&event)){
            if (event.type == SDL_EVENT_QUIT){
                running = false;
            }
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        SDL_RenderClear(renderer);

        SDL_RenderPresent(renderer);
    }


    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
}