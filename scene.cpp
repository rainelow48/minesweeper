#ifndef SCENE_CPP
#define SCENE_CPP

#include <SDL2\SDL.h>
#include <SDL2\SDL_ttf.h>

/**
 * An abstract class for storage of scenes
 */
class Scene {
public:
    Scene(SDL_Window *, TTF_Font *);

    /**
     * List of scenes throughout the application
     */
    enum States {
        SCENE_MENU,  //TODO: Implement and make the default
        SCENE_GAME,  //Current Default
        SCENE_SCORE, //TODO: Implement
        SCENE_TOTAL
    };

    virtual void Render() = 0;
    virtual void Free() = 0;
    virtual void HandleEvent(SDL_Event *) = 0;
    void Resize();

protected:
    void SetWindowSize(size_t, size_t);
    SDL_Window *g_window;
    TTF_Font *g_font;
    int win_width;
    int win_height;
};

/**
 * Initialise global window and font
 */
Scene::Scene(SDL_Window *window, TTF_Font *font) : g_window{window}, g_font{font} {}

/**
 * Sets the size of the window containing this scene.
 * @param width x length
 * @param height y length
 */
void Scene::SetWindowSize(size_t width, size_t height) {
    win_width = width;
    win_height = height;
    Resize();
}

/**
 * Resizes window according to set window size, used for scene transitions
 */
void Scene::Resize() {
    SDL_SetWindowSize(g_window, win_width, win_height);
}

/**
 * Manages transition between scenes
 */
class SceneManager {
public:
    static Scene *curr_scene;
    static Scene *all_scene[Scene::SCENE_TOTAL];

    static void Transition(Scene::States);
    static void SetAndTransition(Scene::States, Scene *);
    static void HandleEvent(SDL_Event *);
    static void Render();
    static void Free();
};

/**
 * Transition to scene at specified state. Target scene must be previously set, else use SetAndTransition.
 * @param state state to go to.
 */
void SceneManager::Transition(Scene::States state) {
    curr_scene = all_scene[state];
    curr_scene->Resize();
}

/**
 * Transition to scene at specified state.
 * @param state state to go to
 * @param scene scene to load
 */
void SceneManager::SetAndTransition(Scene::States state, Scene *scene) {
    all_scene[state] = scene;
    Transition(state);
}

/**
 * Gets current scene to handle event
 */
void SceneManager::HandleEvent(SDL_Event *e) {
    curr_scene->HandleEvent(e);
}

/**
 * Renders current scene
 */
void SceneManager::Render() {
    curr_scene->Render();
}

/**
 * Deallocates memory
 */
void SceneManager::Free() {
    // for (int i = 0; i < Scene::SCENE_TOTAL; i++) {
    //     if (all_scene[i] != NULL) {
    //         all_scene[i]->Free();
    //         all_scene[i] = NULL;
    //     }
    // }
}

#endif