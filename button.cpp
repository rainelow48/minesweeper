#ifndef BUTTON_CPP
#define BUTTON_CPP

#include "texture.cpp"
class Button {
public:
    Button(TTF_Font *, SDL_Renderer *, const std::string &, int, int);

    /**
     * Different states to consider for rendering texture
     */
    enum ButtonState {
        MOUSE_OUT,
        MOUSE_IN,
        MOUSE_TOTAL
    };

    bool SetTexture(ButtonState, const SDL_Color *, const SDL_Color *bg = NULL);
    void Render(SDL_Renderer *);
    void Free();
    bool HandleEvent(SDL_Event *);

private:
    TTF_Font *g_font;
    SDL_Renderer *g_renderer;

    Texture button_texture[MOUSE_TOTAL];
    const std::string button_text;
    const int x_pos;
    const int y_pos;

    ButtonState curr_state = MOUSE_OUT;
    bool left_mouse_down = false;
};

/**
 * Creates new button and specifies its location on the screen
 * @param font font for text
 * @param renderer used for rendering text
 * @param text text to be displayed
 * @param x x position
 * @param y y position
 */
Button::Button(TTF_Font *font, SDL_Renderer *renderer, const std::string &text, int x, int y) : g_font{font}, g_renderer{renderer}, button_text{text}, x_pos{x}, y_pos{y} {}

/**
 * Set texture for a button state
 * @param state state to set texture
 * @param text_color color of text
 * @param bg_color color of background. If NULL or empty, text is rendered solid, else shaded.
 */
bool Button::SetTexture(ButtonState state, const SDL_Color *text_color, const SDL_Color *bg_color) {
    return button_texture[state].LoadFromRenderedText(g_font, g_renderer, button_text, text_color, bg_color);
}

/**
 * Renders to renderer
 * @param g_renderer location for rendering
 */
void Button::Render(SDL_Renderer *g_renderer) {
    int x, y;
    SDL_GetMouseState(&x, &y);
    //Check if outside button
    if (x < x_pos || y < y_pos || x > x_pos + button_texture[curr_state].GetWidth() || y > y_pos + button_texture[curr_state].GetHeight()) {
        curr_state = MOUSE_OUT;
    } else {
        curr_state = MOUSE_IN;
    }
    button_texture[curr_state].Render(g_renderer, x_pos, y_pos);
}

/**
 * @return true if left click has occurred, false otherwise.
 */
bool Button::HandleEvent(SDL_Event *e) {
    if ((e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP) && e->button.button == SDL_BUTTON_LEFT) {
        int x, y;
        SDL_GetMouseState(&x, &y);
        //Check if within button
        if (!(x < x_pos || y < y_pos || x > x_pos + button_texture[curr_state].GetWidth() || y > y_pos + button_texture[curr_state].GetHeight())) {
            if (e->type == SDL_MOUSEBUTTONDOWN)
                left_mouse_down = true;
            else if (e->type == SDL_MOUSEBUTTONUP && left_mouse_down) {
                left_mouse_down = false;
                return true;
            }
        }
    }
    return false;
}

/**
 * Frees memory
 */
void Button::Free() {
    for (auto x : button_texture) {
        x.Free();
    }
}

#endif