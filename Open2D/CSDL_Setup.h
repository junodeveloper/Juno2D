//
//  CSDL_Setup.h
//  SDL_OpenGL
//
//  Created by 황준호 on 2015. 3. 23..
//  Copyright (c) 2015년 황준호. All rights reserved.
//

#ifndef __SDL_OpenGL__CSDL_Setup__
#define __SDL_OpenGL__CSDL_Setup__

#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <stdlib.h>
#include <time.h>

class CSDL_Setup {
public:
    CSDL_Setup(char *title, int screenWidth, int screenHeight);
    ~CSDL_Setup();
    
    void GL_Init();
    void GL_Quit();
    
    SDL_Renderer* getRenderer();
    SDL_Event* getEvent();
    SDL_Window* getWindow();
    int GetScrWidth(), GetScrHeight();
private:
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_GLContext glContext;
    SDL_Event *event;
    int screenWidth, screenHeight;
};

#endif /* defined(__SDL_OpenGL__CSDL_Setup__) */
