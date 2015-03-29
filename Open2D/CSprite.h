//
//  CSprite.h
//  SDL_OpenGL
//
//  Created by 황준호 on 2015. 3. 24..
//  Copyright (c) 2015년 황준호. All rights reserved.
//

#ifndef __SDL_OpenGL__CSprite__
#define __SDL_OpenGL__CSprite__

#include <stdio.h>
#include <SDL2_image/SDL_image.h>
#include "CSDL_Setup.h"
#include "Vector2D.h"

class CSprite {
public:
    CSprite(CSDL_Setup *csdl, char *fPath);
    ~CSprite();
    
    void Draw(double x, double y, double angle);
    void Move(int dx, int dy);
    double width();
    double height();
    
    Vector2D* rectVertices(double x, double y, double angle);
private:
    SDL_Surface *surface;
    CSDL_Setup *csdl;
    GLuint textureID;
};

void C_DrawLine(Vector2D& p1, Vector2D& p2, int color);
void C_DrawLine(Vector2D& p1, Vector2D& p2);
void C_DrawLine(double x1, double y1, double x2, double y2);
void C_DrawArrow(Vector2D& from, Vector2D& to);
void C_DrawArrow(Vector2D& from, Vector2D& to, int color);
void C_DrawVec(Vector2D& from, Vector2D& vec);
void C_DrawVec(Vector2D& from, Vector2D& vec, int color);

#endif /* defined(__SDL_OpenGL__CSprite__) */
