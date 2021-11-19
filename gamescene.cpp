#ifndef GAMESCENE_CPP
#define GAMESCENE_CPP

#include "logic.cpp"
#include "scene.cpp"
#include "texture.cpp"
#include <SDL2\SDL.h>
#include <SDL2\SDL_image.h>
#include <SDL2\SDL_ttf.h>

/**
 * Displays the game
 */
class GameScene : public Scene {
public:
    GameScene(SDL_Window *, Level, TTF_Font *);

    /**
     * List of possible cell states
     */
    enum CellState {
        CELL_0,
        CELL_1,
        CELL_2,
        CELL_3,
        CELL_4,
        CELL_5,
        CELL_6,
        CELL_7,
        CELL_8,
        CELL_MINE, //Opened a mine (and thus lost the game)
        CELL_UNFLAGGED,
        CELL_FLAGGED,
        CELL_TOTAL,
    };

    /**
     * List of key and mouse events
     */
    enum TrackInput {
        MOUSE_LEFT,
        MOUSE_RIGHT,
        KEY_MOUSE_TOTAL
    };

    void Render();
    void Free();
    void HandleEvent(SDL_Event *);

private:
    const static std::string sprite_path;
    const static int c_length = 20;
    const static int board_x_pos = 0;
    const static int board_y_pos = c_length * 3;

    SDL_Renderer *g_renderer;
    Board *current_board;

    bool key_mouse_pressed[KEY_MOUSE_TOTAL];

    Texture tile_sheet_texture;
    SDL_Rect tile_sheet_clips[CELL_TOTAL];

    Texture unflagged_mines_texture;

    Texture timer_texture;
    Uint32 start_time{0};
    GameState curr_state;

    bool LoadMedia();
};

const string GameScene::sprite_path = "sprite.png";

/**
 * Creates a new game based on indicated difficulty
 * @param window SDL_Window pointer to load scene
 * @param difficulty @c Level::EASY or @c Level::NORMAL or @c Level::HARD each with predefined rows, columns and mines.
 * @param font Font used to render text
 */
GameScene::GameScene(SDL_Window *window, Level difficulty, TTF_Font *font) : Scene(window, font) {
    g_renderer = SDL_GetRenderer(window);
    current_board = new Board{difficulty};
    SetWindowSize(c_length * current_board->GetWidth(), c_length * current_board->GetHeight() + board_y_pos);

    // Load Textures
    if (!LoadMedia()) {
        std::cerr << "Failed to load media!" << std::endl;
    }
    unflagged_mines_texture.LoadFromRenderedText(g_font, g_renderer, std::to_string(current_board->GetFlagsLeft()), new SDL_Color{0, 0, 0, 255});
    timer_texture.LoadFromRenderedText(g_font, g_renderer, "0", new SDL_Color{0, 0, 0, 255});

    // Set clips
    tile_sheet_clips[CELL_0] = SDL_Rect{0, 0, 200, 200};
    tile_sheet_clips[CELL_1] = SDL_Rect{200, 0, 200, 200};
    tile_sheet_clips[CELL_2] = SDL_Rect{400, 0, 200, 200};
    tile_sheet_clips[CELL_3] = SDL_Rect{600, 0, 200, 200};
    tile_sheet_clips[CELL_4] = SDL_Rect{800, 0, 200, 200};
    tile_sheet_clips[CELL_5] = SDL_Rect{1000, 0, 200, 200};
    tile_sheet_clips[CELL_6] = SDL_Rect{1200, 0, 200, 200};
    tile_sheet_clips[CELL_7] = SDL_Rect{1400, 0, 200, 200};
    tile_sheet_clips[CELL_8] = SDL_Rect{1600, 0, 200, 200};
    tile_sheet_clips[CELL_MINE] = SDL_Rect{2200, 0, 200, 200};
    tile_sheet_clips[CELL_UNFLAGGED] = SDL_Rect{1800, 0, 200, 200};
    tile_sheet_clips[CELL_FLAGGED] = SDL_Rect{2000, 0, 200, 200};

    // Default key pressed to false
    for (int i = 0; i < KEY_MOUSE_TOTAL; i++) {
        key_mouse_pressed[i] = false;
    }

    start_time = 0;
}

/**
 * Transforms the current grid to a rendered image.
 */
void GameScene::Render() {
    // Draw unflagged mines
    unflagged_mines_texture.Render(g_renderer, 10, 10);

    // Load timer if started and still playing
    if (start_time && curr_state == GameState::PLAYING) {
        const std::string curr_time = std::to_string((SDL_GetTicks() - start_time) / 1000);
        timer_texture.LoadFromRenderedText(g_font, g_renderer, curr_time, new SDL_Color{0, 0, 0, 255});
    }
    // Draw timer
    int width;
    SDL_GetWindowSize(g_window, &width, NULL);
    timer_texture.Render(g_renderer, width - timer_texture.GetWidth() - 10, 10);

    // Draw game board
    auto grid = current_board->GetPlayerGrid(); //Current Player State
    SDL_Rect dest{0, 0, c_length, c_length};    //Specifies where each cell is displayed on renderer
    CellState c_state;
    for (int row = 0; row < grid.size(); row++) {
        dest.y = c_length * row + board_y_pos; //Updates y value whenever a new row.
        for (int col = 0; col < grid[0].size(); col++) {
            dest.x = c_length * col;             //Updates x value whenever a new col
            c_state = (CellState)grid[row][col]; //Cell state of current cell
            // Draw background under mines
            if (c_state == CELL_MINE) {
                tile_sheet_texture.Render(g_renderer, &tile_sheet_clips[CELL_0], &dest);
            }
            // Draw corresponding sprite
            tile_sheet_texture.Render(g_renderer, &tile_sheet_clips[c_state], &dest);
        }
    }
}

/**
 * Deallocates memory. Call this before going out of scope.
 */
void GameScene::Free() {
    g_renderer = NULL;
    current_board = NULL;
    tile_sheet_texture.Free();
    unflagged_mines_texture.Free();
    timer_texture.Free();
}

/**
 * Handle event and updates game logic
 */
void GameScene::HandleEvent(SDL_Event *e) {
    //Switch to menu
    if (e->type == SDL_KEYUP && e->key.keysym.sym == SDLK_ESCAPE) {
        SceneManager::Transition(Scene::SCENE_MENU);
    } else if (curr_state == PLAYING) { //If still playing then handle events
        switch (e->type) {
        case SDL_MOUSEBUTTONDOWN:
            switch (e->button.button) {
            case SDL_BUTTON_LEFT:
                key_mouse_pressed[MOUSE_LEFT] = true;
                break;
            case SDL_BUTTON_RIGHT:
                if (!key_mouse_pressed[MOUSE_RIGHT]) {
                    key_mouse_pressed[MOUSE_RIGHT] = true;
                    //if within board...
                    if (e->button.x >= board_x_pos && e->button.y >= board_y_pos) {
                        int col = (e->button.x - board_x_pos) / c_length;
                        int row = (e->button.y - board_y_pos) / c_length;
                        current_board->Flag(row, col);
                        //Update unflagged mines
                        unflagged_mines_texture.LoadFromRenderedText(g_font, g_renderer, std::to_string(current_board->GetFlagsLeft()), new SDL_Color{0, 0, 0, 255});
                        //Start timer if not started
                        if (start_time == 0) {
                            start_time = SDL_GetTicks();
                        }
                    }
                }
                break;
            }
            break;
        case SDL_MOUSEBUTTONUP:
            switch (e->button.button) {
            case SDL_BUTTON_LEFT:
                if (key_mouse_pressed[MOUSE_LEFT]) {
                    key_mouse_pressed[MOUSE_LEFT] = false;
                    // If within board
                    if (e->button.x >= board_x_pos && e->button.y >= board_y_pos) {
                        int col = (e->button.x - board_x_pos) / c_length;
                        int row = (e->button.y - board_y_pos) / c_length;
                        curr_state = current_board->Open(row, col);
                        //Start timer if not started
                        if (start_time == 0) {
                            start_time = SDL_GetTicks();
                        }
                    }
                }
                break;
            case SDL_BUTTON_RIGHT:
                key_mouse_pressed[MOUSE_RIGHT] = false;
                break;
            }
            break;
        }
    }
}

/**
 * Loads the sprite to texture and sets the clips corresponding to cell state.
 */
bool GameScene::LoadMedia() {
    // Load tile sheet texture
    if (!tile_sheet_texture.LoadFromFile(g_renderer, sprite_path)) {
        std::cerr << "Sprite loading failed!" << std::endl;
        return false;
    }
    return true;
}

#endif