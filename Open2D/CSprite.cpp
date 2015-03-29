//
//  CSprite.cpp
//  SDL_OpenGL
//
//  Created by 황준호 on 2015. 3. 24..
//  Copyright (c) 2015년 황준호. All rights reserved.
//

#include "CSprite.h"

CSprite::CSprite(CSDL_Setup* csdl, char *fPath) {
    surface = IMG_Load(fPath);
    this->csdl = csdl;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
}
CSprite::~CSprite() {
    glDeleteTextures(1, &textureID);
}

void CSprite::Draw(double x, double y, double angle) {
    double halfw = surface->w / 2.0;
    double halfh = surface->h / 2.0;
    glPushMatrix();
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
    glTranslatef(x, y, 0.0f);
    glRotatef(angle, 0.0f, 0.0f, 1.0f);
    glColor3f(1.f, 1.f, 1.f);
    glBegin( GL_QUADS );
    //Bottom-left vertex (corner)
    glTexCoord2i( 0, 0 );
    glVertex2d(-halfw, -halfh);
    //glVertex3f( x, y, 0.f );
    
    //Bottom-right vertex (corner)
    glTexCoord2i( 1, 0 );
    glVertex2d(halfw, -halfh);
    //glVertex3f( x + surface->w, y, 0.f );
    
    //Top-right vertex (corner)
    glTexCoord2i( 1, 1 );
    glVertex2d(halfw, halfh);
    //glVertex3f( x + surface->w, y + surface->h, 0.f );
    
    //Top-left vertex (corner)
    glTexCoord2i( 0, 1 );
    glVertex2d(-halfw, halfh);
    //glVertex3f( x, y + surface->h, 0.f );
    
    glEnd();
    glPopMatrix();
}
Vector2D* CSprite::rectVertices(double x, double y, double angle) {
    Vector2D *point = new Vector2D[4];
    double rad = angle * 3.14159/180;
    double halfw = surface->w / 2.0;
    double halfh = surface->h / 2.0;
    point[0].x = (x - halfw) * cos(rad) - (y - halfh) * sin(rad);
    point[0].y = (x - halfw) * sin(rad) + (y - halfh) * cos(rad);
    point[1].x = (x + halfw) * cos(rad) - (y - halfh) * sin(rad);
    point[1].y = (x + halfw) * sin(rad) + (y - halfh) * cos(rad);
    point[2].x = (x + halfw) * cos(rad) - (y + halfh) * sin(rad);
    point[2].y = (x + halfw) * sin(rad) + (y + halfh) * cos(rad);
    point[3].x = (x - halfw) * cos(rad) - (y + halfh) * sin(rad);
    point[3].y = (x - halfw) * sin(rad) + (y + halfh) * cos(rad);
    return point;
}
double CSprite::width() {
    return surface->w;
}
double CSprite::height() {
    return surface->h;
}

void C_DrawLine(Vector2D& p1, Vector2D& p2) {
    C_DrawLine(p1.x, p1.y, p2.x, p2.y);
}
void C_DrawLine(Vector2D& p1, Vector2D& p2, int color) {
    if(color == 0)
        glColor3f(0.5f,1.0f,1.0f);
    if(color == 1)
        glColor3f(1.0f,0.5f,1.0f);
    if(color == 2)
        glColor3f(1.0f,1.0f,0.5f);
    if(color == 3)
        glColor3f(0.5f,1.0f,0.5f);
    C_DrawLine(p1, p2);
}
void C_DrawLine(double x1, double y1, double x2, double y2) {
    glBegin(GL_LINES);
    glVertex2d(x1, y1);
    glVertex2d(x2, y2);
    glEnd();
}

void C_DrawArrow(Vector2D& from, Vector2D& to) {
    C_DrawLine(from, to);
    Vector2D f = from;
    Vector2D t = to;
    Vector2D delta = t - f;
    Vector2D branch = delta.unitVector() * 5;
    glPushMatrix();
    glTranslatef(to.x, to.y, 0.f);
    glRotatef(135.0, 0.f, 0.f, 1.f);
    C_DrawLine(0, 0, branch.x, branch.y);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(to.x, to.y, 0.f);
    glRotatef(-135.0, 0.f, 0.f, 1.f);
    C_DrawLine(0, 0, branch.x, branch.y);
    glPopMatrix();
}

void C_DrawArrow(Vector2D& from, Vector2D& to, int color) {
    if(color == 0)
        glColor3f(0.5f,1.0f,1.0f);
    if(color == 1)
        glColor3f(1.0f,0.5f,1.0f);
    if(color == 2)
        glColor3f(1.0f,1.0f,0.5f);
    if(color == 3)
        glColor3f(0.5f,1.0f,0.5f);
    C_DrawArrow(from, to);
}

void C_DrawVec(Vector2D& from, Vector2D& vec) {
    Vector2D to = from + vec;
    C_DrawArrow(from, to);
}

void C_DrawVec(Vector2D& from, Vector2D& vec, int color) {
    if(color == 0)
        glColor3f(0.5f,1.0f,1.0f);
    if(color == 1)
        glColor3f(1.0f,0.5f,1.0f);
    if(color == 2)
        glColor3f(1.0f,1.0f,0.5f);
    if(color == 3)
        glColor3f(0.5f,1.0f,0.5f);
    C_DrawVec(from, vec);
}