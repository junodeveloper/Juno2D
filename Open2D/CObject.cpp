//
//  CObject.cpp
//  Open2D
//
//  Created by 황준호 on 2015. 3. 26..
//  Copyright (c) 2015년 황준호. All rights reserved.
//

#include "CObject.h"

double square(double n) { return n * n; }
double Min(double A, double B) { return A<B?A:B; }
CObject::CObject(CSDL_Setup *csdl, int type) {
    fixed = false;
    m_csdl = csdl;
    switch(type) {
        case OBJECT_BOX:
            m_sprite = new CSprite(csdl, "box.png");
            mass = m_sprite->width() * m_sprite->height();
            //mass = 100;
            inv_mass = 1.0f / mass;
            inertia = 1.0f/12.0f * mass * (square(m_sprite->width()) + square(m_sprite->height()));
            printf("mass %f  width %f  height %f\n",mass,m_sprite->width(),m_sprite->height());
            printf("inertia:%f\n",inertia);
            inv_inertia = 1.0f / inertia;
            COM = Vector2D(m_sprite->width()/2.0, m_sprite->height()/2.0);
            break;
        case OBJECT_GROUND:
            m_sprite = new CSprite(csdl, "ground.png");
            mass = 0;
            inv_mass = 0;
            inertia = 0;
            inv_inertia = 0;
            fixed = true;
            COM = Vector2D(0,0);
            break;
        case OBJECT_WALL:
            m_sprite = new CSprite(csdl, "wall.png");
            mass = 0;
            inv_mass = 0;
            inertia = 0;
            inv_inertia = 0;
            fixed = true;
            COM = Vector2D(0,0);
            break;
    }
    m_polygon = new CPolygon(m_sprite->width(), m_sprite->height());
    angle = 0.f;
    angularVel = 0.f;
    restitution = 0.5;
    pos = Vector2D(0,0);
    velocity = Vector2D(0,0);
}
CObject::~CObject() {
    
}

void CObject::move(Vector2D delta) {
    pos = pos + delta;
}
void CObject::move(double dx, double dy) {
    pos.x += dx, pos.y += dy;
}
void CObject::rotate(double dAngle) {
    angle += dAngle;
}
void CObject::draw() {
    m_sprite->Draw(pos.x, pos.y, angle);
}
void CObject::update() {
    applyGravity();
    move(velocity.x, velocity.y);
    rotate(angularVel);
}
void CObject::applyGravity() {
    accelerate(0, 0.1);
}
CPolygon* CObject::polygon() {
    return m_polygon;
}
Vector2D CObject::transRotation(const Vector2D& dst) {
    double rad = (angle * 3.14159/180);
    double newX = dst.x * cos(rad) - dst.y * sin(rad);
    double newY = dst.x * sin(rad) + dst.y * cos(rad);
    return Vector2D(newX, newY);
}
Vector2D CObject::GetNormal(int index) {
    return transRotation(m_polygon->m_normals[index]);
}
Vector2D CObject::GetVertexVec(int index) {
    return transRotation(m_polygon->m_vectors[index]);
}
Vector2D CObject::GetVertexAbs(int index) {
    return GetLeftTop() + transRotation(m_polygon->m_fromLT[index]);
}
Vector2D CObject::GetCOM() {
    return transRotation(COM);
}
Vector2D CObject::GetSupport(int *sIndex, const Vector2D& dir) {
    Vector2D bestVertex = GetVertexAbs(0);
    *sIndex = 0;
    for(int i=1;i<m_polygon->m_vertexCount;i++) {
        Vector2D v = GetVertexAbs(i);
        Vector2D m = v - bestVertex;
        double projection = Dot(m, dir);
        if(projection > 0) {
            bestVertex = v;
            *sIndex = i;
        }
    }
    return bestVertex;
}
Vector2D CObject::GetFromLT(int index) {
    if(index >= 0)
        return transRotation(m_polygon->m_fromLT[index]);
    else
        return Vector2D(0,0);
}
Vector2D CObject::GetLeftTop() {
    double halfw = m_sprite->width() / 2.0;
    double halfh = m_sprite->height() / 2.0;
    return pos + transRotation(Vector2D(-halfw, -halfh));
}
void CObject::drawNormals() {
    for(int i=0;i<m_polygon->m_vertexCount;i++) {
        Vector2D p1 = GetLeftTop() + GetFromLT(i) - GetVertexVec(i) * 0.5;
        Vector2D p2 = p1 + GetNormal(i) * 20;
        C_DrawArrow(p1, p2, i);
    }
}

void CObject::applyImpulse(Vector2D& impulse) {
    Vector2D dV = impulse * inv_mass;
    accelerate(dV);
}
void CObject::applyImpulse(Vector2D& impulse, Vector2D& contactVector) {
    applyImpulse(impulse);
    angularAcc((Cross(contactVector, impulse) * inv_inertia) * 180.0f/3.14159f);
}

void CObject::accelerate(Vector2D& delta) {
    accelerate(delta.x, delta.y);
}
void CObject::accelerate(double dVx, double dVy) {
    if(!fixed)
        velocity.x += dVx, velocity.y += dVy;
}
void CObject::angularAcc(double delta) {
    angularVel += delta;
}

double Penetration(int *faceIndex, CObject *A, CObject *B) {
    double best = -0x7fffffff;
    int bestIndex = 0;
    printf("A:\n");
    for(int i=0;i<A->polygon()->m_vertexCount;i++)
        printf("ABS %f %f  VEC %f %f  NOR %f %f\n", A->GetVertexAbs(i).x, A->GetVertexAbs(i).y, A->GetVertexVec(i).x, A->GetVertexVec(i).y, A->GetNormal(i).x, A->GetNormal(i).y);
    printf("B:\n");
    for(int i=0;i<B->polygon()->m_vertexCount;i++)
        printf("ABS %f %f  VEC %f %f  NOR %f %f\n", B->GetVertexAbs(i).x, B->GetVertexAbs(i).y, B->GetVertexVec(i).x, B->GetVertexVec(i).y, B->GetNormal(i).x, B->GetNormal(i).y);
    for(int i=0;i<A->polygon()->m_vertexCount;i++) {
        int sIndex;
        Vector2D n = A->GetNormal(i);
        B->GetSupport(&sIndex, n * -1);
        Vector2D s = B->GetVertexAbs(sIndex);
        Vector2D v = A->GetVertexAbs(i);
        //C_DrawArrow(v, s);
        printf("B angle : %.2f, sIndex : %d\n", B->angle, sIndex);
        double d = Dot(n, B->GetVertexAbs(sIndex) - A->GetVertexAbs(i));
        if(d > best) {
            best = d;
            bestIndex = i;
        }
    }
    *faceIndex = bestIndex;
    return best;
}

void ResolveCollision(CObject *A, CObject *B) {
    int normal1, normal2;
    double pene1 = Penetration(&normal1, A, B);
    double pene2 = Penetration(&normal2, B, A);
    if(pene1>=0 || pene2>=0) return;
    Vector2D normal, p;
    Vector2D pVertex, pVec;
    double pene;
    int sIndex;
    if(pene1 > pene2) {
        normal = -A->GetNormal(normal1);
        pVertex = A->GetVertexAbs(normal1);
        pVec = A->GetVertexVec(normal1);
        p = B->GetSupport(&sIndex, normal);
        pene = pene1;
    } else {
        normal = B->GetNormal(normal2);
        pVertex = B->GetVertexAbs(normal2);
        pVec = B->GetVertexVec(normal2);
        p = A->GetSupport(&sIndex, -normal);
        pene = pene2;
    }
    Vector2D rv = B->velocity - A->velocity;
    double velAlongNormal = Dot(rv, normal);
    Vector2D from = A->GetVertexAbs(normal1), vec = A->GetNormal(normal1) * 20;
    C_DrawVec(from, vec);
    
    const float percent = 0.8; // usually 20% to 80%
    Vector2D correction = normal * (pene / (A->inv_mass + B->inv_mass)) * percent;
    A->move(correction * -A->inv_mass);
    B->move(correction * B->inv_mass);
    
    if(velAlongNormal<0) return;
    
    Vector2D rA, rB;
    Vector2D v, dVec1, dVec2;
    std::vector<Vector2D> contacts;
    for(int i=0;i<A->polygon()->m_vertexCount;i++) {
        v = A->GetVertexAbs(i);
        dVec1 = v - pVertex;
        dVec2 = v - (pVertex - pVec);
        if(Dot(dVec1, normal)<0 && Dot(dVec1, pVec)<0 && Dot(dVec2, -pVec)<0)
            contacts.push_back(v);
    }
    for(int i=0;i<B->polygon()->m_vertexCount;i++) {
        v = B->GetVertexAbs(i);
        dVec1 = v - pVertex;
        dVec2 = v - (pVertex - pVec);
        if(Dot(dVec1, normal)<0 && Dot(dVec1, pVec)<0 && Dot(dVec2, -pVec)<0)
            contacts.push_back(v);
    }
    Vector2D centerContact = Vector2D(0,0);
    for(int i=0;i<contacts.size();i++) {
        centerContact = centerContact + contacts[i];
    }
    printf("Contact Size  %d\n",contacts.size());
    if(contacts.size()>0)
        centerContact = centerContact / contacts.size();
    rA = centerContact - (A->GetLeftTop() + A->GetCOM());
    rB = centerContact - (B->GetLeftTop() + B->GetCOM());
    /*Vector2D r1, r2;
    r1 = p - (A->GetLeftTop() + A->GetCOM());
    r2 = p - (B->GetLeftTop() + B->GetCOM());*/
    Vector2D COM1 = A->GetLeftTop() + A->GetCOM();
    Vector2D COM2 = B->GetLeftTop() + B->GetCOM();
    C_DrawVec(COM1, rA, 1);
    C_DrawVec(COM2, rB, 1);
    
    double e = Min(A->restitution, B->restitution);
    double j = -(1 + e) * velAlongNormal;
    
    j /= ( A->inv_mass + B->inv_mass + A->inv_inertia * square(Cross(rA, normal) + B->inv_inertia * square(Cross(rB, normal))) );
    
    Vector2D impulse = normal * j;
    Vector2D AImpulse = -impulse, BImpulse = impulse;
    //Vector2D rA = AImpulse * A->inv_mass / A->angularVel;
    //Vector2D rB = BImpulse * B->inv_mass / B->angularVel;
    A->applyImpulse(AImpulse, rA);
    B->applyImpulse(BImpulse, rB);
}