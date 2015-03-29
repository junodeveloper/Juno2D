//
//  CSDL_Setup.cpp
//  SDL_OpenGL
//
//  Created by 황준호 on 2015. 3. 23..
//  Copyright (c) 2015년 황준호. All rights reserved.
//

#include "CSDL_Setup.h"

inline double absf(double n) { return (n<0?-n:n); }

CSDL_Setup::CSDL_Setup(char *title, int screenWidth, int screenHeight) {
    SDL_Init(SDL_INIT_EVERYTHING);
    window = SDL_CreateWindow(title,
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              screenWidth,
                              screenHeight,
                              SDL_WINDOW_SHOWN|SDL_WINDOW_OPENGL);
    event = new SDL_Event();
    GL_Init();
    srand( time(NULL) );
    this->screenWidth = screenWidth, this->screenHeight = screenHeight;
}
CSDL_Setup::~CSDL_Setup() {
    GL_Quit();
    delete event;
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void CSDL_Setup::GL_Init() {
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    glContext = SDL_GL_CreateContext(window);
    
    glEnable( GL_TEXTURE_2D );
    
    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
    
    //glViewport( 0, 0, 640, 480 );
    
    glShadeModel(GL_SMOOTH);
    
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    
    glOrtho(0.0f, 640, 480, 0.0f, -1.0f, 1.0f);
    
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    
    glDisable(GL_DEPTH_TEST);
}
void CSDL_Setup::GL_Quit() {
    SDL_GL_DeleteContext(glContext);
}

SDL_Renderer* CSDL_Setup::getRenderer() {
    return renderer;
}
SDL_Event* CSDL_Setup::getEvent() {
    return event;
}
SDL_Window* CSDL_Setup::getWindow() {
    return window;
}
int CSDL_Setup::GetScrWidth() {
    return screenWidth;
}
int CSDL_Setup::GetScrHeight() {
    return screenHeight;
}