//
//  CObject.h
//  Open2D
//
//  Created by 황준호 on 2015. 3. 26..
//  Copyright (c) 2015년 황준호. All rights reserved.
//

#ifndef __Open2D__CObject__
#define __Open2D__CObject__

#include <stdio.h>
#include "CSprite.h"
#include "CSDL_Setup.h"
#include "Vector2D.h"
#include "CPolygon.h"
#include <vector>

enum {
    OBJECT_BOX,
    OBJECT_GROUND,
    OBJECT_WALL
};

class CObject {
public:
    CObject(CSDL_Setup *csdl, int type);
    ~CObject();
    
    void move(Vector2D delta);
    void move(double dx, double dy);
    void rotate(double dAngle);
    void draw();
    void drawNormals();
    void applyGravity();
    void applyImpulse(Vector2D& impulse);
    void applyImpulse(Vector2D& impulse, Vector2D& contactVector);
    void update();
    void accelerate(Vector2D& delta);
    void accelerate(double dVx, double dVy);
    void angularAcc(double delta);
    void setAngle(double angle);
    void setAngularVel(double angularVel);
    bool isInPolygon(Vector2D& p);

    Vector2D transRotation(const Vector2D& dst);
    Vector2D GetNormal(int index);
    Vector2D GetVertexVec(int index);
    Vector2D GetVertexAbs(int index);
    Vector2D GetSupport(int *sIndex, const Vector2D& dir);
    Vector2D GetFromLT(int index);
    Vector2D GetLeftTop();
    Vector2D GetCOM();
    CPolygon* polygon();
    CSprite* sprite();
    Vector2D pos;
    Vector2D velocity;
    Vector2D COM;
    double angle;
    double angularVel;
    double mass;
    double inv_mass;
    double inertia;
    double inv_inertia;
    double restitution;
    double staticFriction;
    double dynamicFriction;
    bool fixed;
private:
    CPolygon *m_polygon;
    CSprite *m_sprite;
    CSDL_Setup *m_csdl;
};

double Penetration(int *faceIndex, CObject *A, CObject *B);
void ResolveCollision(CObject *A, CObject *B);

#endif /* defined(__Open2D__CObject__) */