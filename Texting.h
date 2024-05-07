#ifndef _Texting__
#define _Texting__
#include<SDL.h>
#include<string>

enum Coloring
{
    RED_TEXT = 0,
    WHITE_TEXT = 1,
    GREEN_TEXT = 2,
    BLACK_TEXT = 3,
};
SDL_Color setColor (int col)
{
    SDL_Color color;
    switch (col)
    {
    case RED_TEXT:
        color = {255,0,0};
        break;
    case WHITE_TEXT:
        color = {255,255,255};
        break;
    case GREEN_TEXT:
        color = {0,255,0};
    default:
        color = {0,0,0};
        break;
    }
    return color;
};

#endif // _Texting__
