#ifndef MENUSCENE_CPP
#define MENUSCENE_CPP

#include "button.cpp"
#include "gamescene.cpp"
#include "scene.cpp"
#include "texture.cpp"
#include <vector>

/**
 * Display to select difficulty
 */
class MenuScene : public Scene {
public:
    /**
     * List of key and mouse events
     */
    enum TrackInput {
        MOUSE_LEFT,
        MOUSE_RIGHT,
        KEY_MOUSE_TOTAL
    };

    MenuScene(SDL_Window *, TTF_Font *);
    void Render();
    void Free();
    void HandleEvent(SDL_Event *);

private:
    static SDL_Color over_text_color;
    static SDL_Color default_text_color;

    Button *easy;
    Button *normal;
    Button *hard;
    Button *custom;

    std::vector<Button *> button_list;

    SDL_Renderer *g_renderer;
};

SDL_Color MenuScene::over_text_color = SDL_Color{255, 0, 0, 255};
SDL_Color MenuScene::default_text_color = SDL_Color{0, 0, 0, 255};

/**
 * Creates a menu
 * @param window SDL_Window pointer to load scene
 * @param font Font used to render text
 */
MenuScene::MenuScene(SDL_Window *window, TTF_Font *font) : Scene(window, font) {
    g_renderer = SDL_GetRenderer(window);
    SetWindowSize(400, 400);

    easy = new Button{g_font, g_renderer, "Easy", 150, 150};
    normal = new Button{g_font, g_renderer, "Normal", 150, 180};
    hard = new Button{g_font, g_renderer, "Hard", 150, 210};
    custom = new Button{g_font, g_renderer, "Custom", 150, 240};
    button_list.push_back(easy);
    button_list.push_back(normal);
    button_list.push_back(hard);
    button_list.push_back(custom);
    for (auto button : button_list) {
        button->SetTexture(Button::MOUSE_OUT, &default_text_color);
        button->SetTexture(Button::MOUSE_IN, &over_text_color);
    }
}

void MenuScene::Render() {
    easy->Render(g_renderer);
    normal->Render(g_renderer);
    hard->Render(g_renderer);
    custom->Render(g_renderer);
}

/**
 * Deallocates memory. Call this before going out of scope.
 */
void MenuScene::Free() {
    g_renderer = NULL;
    for (auto button : button_list) {
        button->Free();
    }
}

/**
 * Checks if buttons were pressed and transitions if necessary
 */
void MenuScene::HandleEvent(SDL_Event *e) {
    if (e->type == SDL_MOUSEBUTTONUP || e->type == SDL_MOUSEBUTTONDOWN) {
        GameScene *game = NULL;
        if (easy->HandleEvent(e)) {
            game = new GameScene{g_window, Level::EASY, g_font};
        } else if (normal->HandleEvent(e)) {
            game = new GameScene{g_window, Level::NORMAL, g_font};
        } else if (hard->HandleEvent(e)) {
            game = new GameScene{g_window, Level::HARD, g_font};
        }
        if (game != NULL) {
            cerr << "Transition to game" << endl;
            SceneManager::SetAndTransition(Scene::SCENE_GAME, game);
        }
    }
}

#endif