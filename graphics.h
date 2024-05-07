#ifndef _STRUCTS__H
#define _STRUCTS__H

#include <SDL.h>
#include <SDL_image.h>
#include "defs.h"
#include<SDL_mixer.h>
#include <SDL_ttf.h>
#include "texting.h"

struct Graphics {
    SDL_Renderer *renderer;
    SDL_Window *window;

    void logErrorAndExit(const char* msg, const char* error)
    {
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR,
                       "%s: %s", msg, error);
        SDL_Quit();
    }

    void init() {
        if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
            logErrorAndExit("SDL_Init", SDL_GetError());

        window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED,
                   SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT,
                   SDL_WINDOW_SHOWN);
        if (window == nullptr)
                   logErrorAndExit("CreateWindow", SDL_GetError());

        if (!IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG))
            logErrorAndExit( "SDL_image error:", IMG_GetError());

        renderer = SDL_CreateRenderer(window, -1,SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

        //renderer =
                  SDL_CreateSoftwareRenderer(SDL_GetWindowSurface(window));

        if (renderer == nullptr)
             logErrorAndExit("CreateRenderer", SDL_GetError());

        //Initialize SDL_mixer
        if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 ) {
           logErrorAndExit( "SDL_mixer could not initialize! SDL_mixer Error: %s\n",
                            Mix_GetError() );
        }

        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
        SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);

        TTF_Init();
    }

    void prepareScene(SDL_Texture * background)
    {
        SDL_RenderClear(renderer);
        SDL_RenderCopy( renderer, background, NULL, NULL);
    }

    void presentScene()
    {
        SDL_RenderPresent(renderer);
    }


    SDL_Texture *loadTexture(const char *filename)
    {
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO,
                       "Loading %s", filename);
        SDL_Texture *texture = IMG_LoadTexture(renderer, filename);
        if (texture == NULL)
            SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION,
                  SDL_LOG_PRIORITY_ERROR, "Load texture %s", IMG_GetError());

        return texture;
    }
    TTF_Font* loadFont(const char* filename, int fontSize) {
        TTF_Font* gFont = TTF_OpenFont(filename, fontSize);
        if (!gFont) {
            std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        }
        return gFont;
    }
    SDL_Texture* loadText(const char *text, TTF_Font* font, SDL_Color color) {
        SDL_Surface* textSurface = TTF_RenderText_Solid(font, text, color);
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_FreeSurface(textSurface);
        return textTexture;
    }
    void updateText(SDL_Texture*& textTexture, const char* newText, TTF_Font* font, SDL_Color textColor) {
        if (textTexture) {
            SDL_DestroyTexture(textTexture);
            textTexture = nullptr;
        }
        textTexture = loadText(newText, font, textColor);
    }
    void renderText(SDL_Texture* texture, int x, int y, int w, int h) {
        SDL_Rect dstRect = {x, y, w, h};
        SDL_QueryTexture(texture, NULL, NULL, &dstRect.w, &dstRect.h);
        SDL_RenderCopy(renderer, texture, NULL, &dstRect);
    }


    void renderTexture(SDL_Texture *texture, int x, int y)
    {
        SDL_Rect dest;

        dest.x = x;
        dest.y = y;
        SDL_QueryTexture(texture, NULL, NULL, &dest.w, &dest.h);

        SDL_RenderCopy(renderer, texture, NULL, &dest);
    }

    Mix_Chunk* loadSound(const char* path) {
        Mix_Chunk* gChunk = Mix_LoadWAV(path);
        if (gChunk == nullptr) {
            SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION,
                           SDL_LOG_PRIORITY_ERROR,
               "Could not load sound! SDL_mixer Error: %s", Mix_GetError());
        }
    }

    void play(Mix_Chunk* gChunk) {
        if (gChunk != nullptr) {
            Mix_PlayChannel( -1, gChunk, 0 );
        }
    }


    void quit()
    {
        IMG_Quit();

        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }
};


#endif // _GRAPHICS__H
