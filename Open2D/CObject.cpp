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
double Absf(double n) { return n>0?n:-n; }
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
    staticFriction = 0.2;
    dynamicFriction = 0.05;
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
void CObject::setAngle(double angle) {
    this->angle = angle;
}
void CObject::setAngularVel(double angularVel) {
    this->angularVel = angularVel;
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

bool CObject::isInPolygon(Vector2D& p) {
    double product;
    for(int i=0;i<m_polygon->m_vertexCount;i++) {
        Vector2D v = GetVertexVec(i);
        Vector2D s = GetVertexAbs(i) - v;
        product = Cross(v, p - s);
        if(product <= 0) return false;
    }
    return true;
}

void ResolveCollision(CObject *A, CObject *B) {
    int nA, nB;
    int selected = 0;
    double depthA = Penetration(&nA, A, B);
    double depthB = Penetration(&nB, B, A);
    if(depthA>=0 || depthB>=0) return;
    Vector2D normal, pVertex;
    int pIndex;
    double minDepth;
    if(depthA > depthB) {
        normal = -A->GetNormal(nA);
        pVertex = B->GetSupport(&pIndex, normal);
        minDepth = depthA;
        selected = 0;
    } else {
        normal = B->GetNormal(nB);
        pVertex = A->GetSupport(&pIndex, -normal);
        minDepth = depthB;
        selected = 1;
    }
    Vector2D rv = B->velocity - A->velocity;
    printf("A->velocity  %f %f\n",A->velocity.x, A->velocity.y);
    printf("B->velocity  %f %f\n",B->velocity.x, B->velocity.y);
    printf("first rv  %f %f\n",rv.x, rv.y);
    double velAlongNormal = Dot(rv, normal);
    
    const float percent = 0.8; // usually 20% to 80%
    Vector2D correction = normal * (minDepth / (A->inv_mass + B->inv_mass)) * percent;
    A->move(correction * -A->inv_mass);
    B->move(correction * B->inv_mass);
    
    if(velAlongNormal<0) return;
    
    Vector2D cp1 = Vector2D(0), cp2 = Vector2D(0);
    Vector2D p1, p2;
    int sIndex;
    if(selected) {
        p1 = pVertex;
        p2 = A->GetVertexAbs((pIndex + 1) % A->polygon()->m_vertexCount);
    } else {
        p1 = A->GetSupport(&sIndex, -normal);
        p2 = A->GetVertexAbs((sIndex + 1) % A->polygon()->m_vertexCount);
    }
    if(B->isInPolygon(p1)) cp1 = p1;
    if(B->isInPolygon(p2)) cp2 = p2;
    if(cp1.none() && cp2.none()) {
        if(!selected) {
            p1 = pVertex;
            p2 = B->GetVertexAbs((pIndex + 1) % B->polygon()->m_vertexCount);
        } else {
            p1 = B->GetSupport(&sIndex, normal);
            p2 = B->GetVertexAbs((sIndex + 1) % B->polygon()->m_vertexCount);
        }
        if(A->isInPolygon(p1)) cp1 = p1;
        if(A->isInPolygon(p2)) cp2 = p2;
        printf("ONEONEONOEE\n");
    }
    Vector2D contact(0);
    if(!cp1.none() && !cp2.none())
        contact = (cp1 + cp2) / 2.0f;
    else if(!cp1.none())
        contact = cp1;
    else if(!cp2.none())
        contact = cp2;
    
    Vector2D COMA = A->GetLeftTop() + A->GetCOM();
    Vector2D COMB = B->GetLeftTop() + B->GetCOM();
    Vector2D rA, rB;
    
    rA = contact - COMA;
    rB = contact - COMB;
    
    C_DrawVec(COMA, rA, 1);
    C_DrawVec(COMB, rB, 1);
    
    printf("cp1  %f %f\n",cp1.x, cp1.y);
    printf("cp2  %f %f\n",cp2.x, cp2.y);
    printf("contact  %f %f\n",contact.x, contact.y);
    printf("COMA  %f %f\n",COMA.x, COMA.y);
    printf("COMB  %f %f\n",COMB.x, COMB.y);
    printf("A->isInPolygon(COMA) : %d\n", A->isInPolygon(COMA));
    
    double e = Min(A->restitution, B->restitution);
    double j = -(1 + e) * velAlongNormal;
    
    j /= ( A->inv_mass + B->inv_mass + A->inv_inertia * square(Cross(rA, normal) + B->inv_inertia * square(Cross(rB, normal))) );
    
    Vector2D impulse = normal * j;
    Vector2D AImpulse = -impulse, BImpulse = impulse;
    //Vector2D rA = AImpulse * A->inv_mass / A->angularVel;
    //Vector2D rB = BImpulse * B->inv_mass / B->angularVel;
    A->applyImpulse(AImpulse, rA);
    B->applyImpulse(BImpulse, rB);
    
    
    
    printf("rv  %lf %lf\n", rv.x, rv.y);
    printf("normal  %lf %lf\n", normal.x, normal.y);
    
    Vector2D tangent = rv - normal * Dot(rv, normal);
    tangent = tangent.unitVector();
    
    printf("tangent  %lf %lf\n",tangent.x, tangent.y);
    
    if(tangent.size()>0) {
        float jt = -Dot(rv, tangent);
        double fj = -(1 + e) * (Dot(rv, tangent));
        fj /= (A->inv_mass + B->inv_mass);
        printf("jt  %f\n", jt);
        jt = jt / (A->inv_mass + B->inv_mass);
    
        float mu = sqrt(square(A->staticFriction) + square(B->staticFriction));
    
        Vector2D frictionImpulse;
        if(Absf(jt) < fj * mu)
            frictionImpulse = tangent * -jt;
        else {
            double dynamicFriction = sqrt(square(A->dynamicFriction) + square(B->dynamicFriction));
            frictionImpulse = tangent * -fj * dynamicFriction;
        }
        printf("frictionImpulse  %f %f\n", frictionImpulse.x, frictionImpulse.y);
        Vector2D AFriction = frictionImpulse;
        Vector2D BFriction = -frictionImpulse;
        printf("AFriction  %f %f\n",AFriction.x, AFriction.y);
        printf("BFriction  %f %f\n",BFriction.x, BFriction.y);
        A->applyImpulse(AFriction, rA);
        B->applyImpulse(BFriction, rB);
        //A->velocity = A->velocity + frictionImpulse * A->inv_mass;
        //B->velocity = B->velocity - frictionImpulse * B->inv_mass;
    }
}