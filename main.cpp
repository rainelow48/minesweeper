#define SDL_MAIN_HANDLED
#include "gamescene.cpp"
#include "menuscene.cpp"
#include "scene.cpp"
#include <SDL2\SDL.h>
#include <SDL2\SDL_image.h>
#include <iostream>

//To change: Each scene should have a settable height and width.
const int DEFAULT_WINDOW_WIDTH = 600;
const int DEFAULT_WINDOW_HEIGHT = 600;

// Function declarations
bool Init();
void Quit();
void Testing();

// SDL Variables
SDL_Window *window;     //Application Window
SDL_Renderer *renderer; //Window Renderer
TTF_Font *font;         //Font

// Scene variables
Scene *SceneManager::curr_scene = NULL;
Scene *SceneManager::all_scene[Scene::SCENE_TOTAL];
// Scene *current_scene;
// Scene *scenes[Scene::SCENE_TOTAL];
/**
 * Initialise SDL subsystems and global variables.
 * @return true on success, or false on error
 */
bool Init() {
    // Init SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL cannot be initialised!" << std::endl
                  << SDL_GetError();
        return false;
    }
    // Init SDL_Image
    if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
        std::cerr << "SDL_IMG cannot be initialised!" << std::endl
                  << IMG_GetError();
        return false;
    }
    // Init TTF Font
    if (TTF_Init() < 0) {
        std::cerr << "SDL_TTF cannot be initialised!" << std::endl
                  << TTF_GetError();
    }
    // Create Window
    window = SDL_CreateWindow("Minesweeper Classic", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        std::cerr << "Window creation failed!" << std::endl
                  << SDL_GetError();
        return false;
    }
    // Create renderer from window
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL) {
        std::cerr << "Renderer creation failed!" << std::endl
                  << SDL_GetError();
        return false;
    }
    // Set global font
    font = TTF_OpenFont("C:\\Windows\\Fonts\\segoeui.ttf", 20);
    if (font == NULL) {
        std::cerr << "Font loading failed!" << std::endl
                  << TTF_GetError();
        return false;
    }
    //Initialise all_scene pointers to NULL
    for (int i = 0; i < Scene::SCENE_TOTAL; i++) {
        SceneManager::all_scene[i] = NULL;
    }
    //Create menu and set as current scene
    MenuScene *temp = new MenuScene{window, font};
    SceneManager::SetAndTransition(Scene::SCENE_MENU, temp);

    return true;
}

/**
 * Frees resources and quits SDL subsystems.
 */
void Quit() {
    //Destroy
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SceneManager::Free();

    //Deallocate
    renderer = NULL;
    window = NULL;
    // for (auto scene : scenes) {
    //     scene = NULL;
    // }

    //Quit SDL subsystems
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

int main(int argc, char *args[]) {
    // Initialisation of SDL
    if (!Init()) {
        std::cerr << "Failed to initialise!" << std::endl;
    } else {
        bool running = true;
        SDL_Event e;
        while (running) {
            while (SDL_PollEvent(&e) != 0) {
                if (e.type == SDL_QUIT) {
                    running = false;
                    break;
                }
            }
            //Clear Screen
            SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
            SDL_RenderClear(renderer);
            //Things happen here
            SceneManager::curr_scene->HandleEvent(&e);
            SceneManager::curr_scene->Render();
            //Render to window
            SDL_RenderPresent(renderer);
        }

        // Testing();
    }

    // Free resources and terminate program
    Quit();
    return 0;
}

// void Testing() {
//     bool running = true;
//     SDL_Event e;
//     while (running) {
//         while (SDL_PollEvent(&e) != 0) {
//             if (e.type == SDL_QUIT) {
//                 running = false;
//                 break;
//             }
//         }
//         SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
//         SDL_RenderClear(renderer);

//         //Things happen here

//         SDL_RenderPresent(renderer);
//     }
// }