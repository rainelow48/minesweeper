#ifndef TEXTURE_CPP
#define TEXTURE_CPP

#include <SDL2\SDL.h>
#include <SDL2\SDL_image.h>
#include <SDL2\SDL_ttf.h>
#include <iostream>

/**
 * Wrapper class for textures for images and rendered text.
 */
class Texture {
public:
    Texture();
    ~Texture();

    bool LoadFromFile(SDL_Renderer *, const std::string &);
    bool LoadFromRenderedText(TTF_Font *, SDL_Renderer *, const std::string &, const SDL_Color *, const SDL_Color *bg_color = NULL);

    void Render(SDL_Renderer *, const SDL_Rect *src = NULL, const SDL_Rect *dest = NULL);
    void Render(SDL_Renderer *, int, int, const SDL_Rect *src = NULL);
    void Free();

    int GetWidth();
    int GetHeight();

private:
    SDL_Texture *m_texture;
    int m_width;
    int m_height;
};

/**
 * Default Constructor. Initialisation via list.
 */
Texture::Texture() : m_texture{NULL}, m_width{0}, m_height{0} {};

/**
 * Destructor
 */
Texture::~Texture() {
    Free();
}

/**
 * Loads texture from rendered text
 * @param g_font font of text
 * @param g_renderer used for rendering
 * @param text text to be shown
 * @param text_color pointer to color of text
 * @param bg_color pointer to color of background. If empty or NULL, solid rendering is used, else shaded rendering is used.
 * @return true on success, false otherwise
 */
bool Texture::LoadFromRenderedText(TTF_Font *g_font, SDL_Renderer *g_renderer, const std::string &text, const SDL_Color *text_color, const SDL_Color *bg_color) {
    Free();
    SDL_Surface *temp_surface = NULL;
    if (bg_color == NULL) {
        temp_surface = TTF_RenderText_Solid(g_font, text.c_str(), *text_color);
    } else {
        temp_surface = TTF_RenderText_Shaded(g_font, text.c_str(), *text_color, *bg_color);
    }
    if (temp_surface == NULL) {
        printf("Unable to render text surface! TTF Error: %s\n", TTF_GetError());
        return false;
    }
    m_texture = SDL_CreateTextureFromSurface(g_renderer, temp_surface);
    if (m_texture == NULL) {
        printf("Unable to create texture from text! SDL Error: %s\n", SDL_GetError());
        return false;
    }
    m_width = temp_surface->w;
    m_height = temp_surface->h;
    SDL_FreeSurface(temp_surface);
    return true;
}

/**
 * Loads texture from image file
 * @param g_renderer used for rendering
 * @param path location of image file
 * @return true on success, false otherwise
 */
bool Texture::LoadFromFile(SDL_Renderer *g_renderer, const std::string &path) {
    Free();
    SDL_Surface *tempSurface = IMG_Load(path.c_str());
    if (tempSurface == NULL) {
        printf("Unable to load image %s! IMG Error: %s\n", path.c_str(), IMG_GetError());
        return false;
    }
    SDL_SetColorKey(tempSurface, SDL_TRUE, SDL_MapRGB(tempSurface->format, 0xff, 0xff, 0xff));
    m_texture = SDL_CreateTextureFromSurface(g_renderer, tempSurface);
    if (m_texture == NULL) {
        printf("Unable to create texture from image! SDL Error: %s\n", SDL_GetError());
        return false;
    }
    m_width = tempSurface->w;
    m_height = tempSurface->h;
    SDL_FreeSurface(tempSurface);
    return true;
}

/**
 * Renders this texture to the destination
 * @param g_renderer renderer to be loaded to
 * @param src clip of this texture. If empty or NULL, full texture is used
 * @param dest location to render texture to. If empty or NULL, full rendering target is used.
 */
void Texture::Render(SDL_Renderer *g_renderer, const SDL_Rect *src, const SDL_Rect *dest) {
    SDL_RenderCopy(g_renderer, m_texture, src, dest);
}

/**
 * Renders this texture unscaled to the destination
 * @param g_renderer renderer to be loaded to
 * @param x x coordinate of rendering destination
 * @param x y coordinate of rendering destination
 * @param src clip of this texture. If empty or NULL, full texture is used.
 */
void Texture::Render(SDL_Renderer *g_renderer, int x, int y, const SDL_Rect *src) {
    const SDL_Rect dest{x, y, m_width, m_height};
    SDL_RenderCopy(g_renderer, m_texture, src, &dest);
}

/**
 * Deallocates memory.
 */
void Texture::Free() {
    if (!m_texture) {
        SDL_DestroyTexture(m_texture);
        m_texture = NULL;
    }
}

/**
 * @return width of texture
 */
int Texture::GetWidth() {
    return m_width;
}

/**
 * @return height of texture
 */
int Texture::GetHeight() {
    return m_height;
}

#endif