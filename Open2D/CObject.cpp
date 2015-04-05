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
    staticFriction = 0.5;
    dynamicFriction = 0.5;
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
void CObject::setPos(double x, double y) {
    pos.x = x, pos.y = y;
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

void ResolveContact() {
    
}

void ResolveCollision(CObject *A, CObject *B) {
    int nA, nB;
    int selected = 0;
    double depthA = Penetration(&nA, A, B);
    double depthB = Penetration(&nB, B, A);
    if(depthA>=0 || depthB>=0) return;
    Vector2D normal, pVertex;
    int pIndex;
    double depth;
    if(depthA > depthB) {
        normal = -A->GetNormal(nA);
        pVertex = B->GetSupport(&pIndex, normal);
        depth = depthA;
        selected = 0;
    } else {
        normal = B->GetNormal(nB);
        pVertex = A->GetSupport(&pIndex, -normal);
        depth = depthB;
        selected = 1;
    }
    Vector2D rv = B->velocity - A->velocity;
    printf("A->velocity  %f %f\n",A->velocity.x, A->velocity.y);
    printf("B->velocity  %f %f\n",B->velocity.x, B->velocity.y);
    printf("first rv  %f %f\n",rv.x, rv.y);
    double velAlongNormal = Dot(rv, normal);
    
    const float percent = 0.8; // usually 20% to 80%
    Vector2D correction = normal * (depth / (A->inv_mass + B->inv_mass)) * percent;
    A->move(correction * -A->inv_mass);
    B->move(correction * B->inv_mass);
    
    if(velAlongNormal<0) return;
    /*Vector2D cp1 = Vector2D(0), cp2 = Vector2D(0);
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
        contact = cp2;*/
    
    Vector2D cp[2];
    int cpNum = 0;
    
    for(int i=0;i<A->polygon()->m_vertexCount;i++) {
        Vector2D eA = A->GetVertexAbs(i);
        Vector2D vA = A->GetVertexVec(i);
        Vector2D sA = eA - vA;
        Vector2D normA = Cross(1.0f, vA);
        for(int j=0;j<B->polygon()->m_vertexCount;j++) {
            Vector2D eB = B->GetVertexAbs(j);
            Vector2D vB = B->GetVertexVec(j);
            Vector2D sB = eB - vB;
            Vector2D normB = Cross(1.0f, vB);
            double p1 = Dot(sB - sA, normA) * Dot(eB - eA, normA);
            double p2 = Dot(sA - sB, normB) * Dot(eA - eB, normB);
            if(p1<0 && p2<0) {
                double ix, iy;
                if(sA.x == eA.x) {
                    ix = sA.x;
                    iy = (eB.y - sB.y) * (ix - sB.x) / (eB.x - sB.x) + sB.y;
                }
                else if(sB.x == eB.x) {
                    ix = sB.x;
                    iy = (eA.y - sA.y) * (ix - sA.x) / (eA.x - sA.x) + sA.y;
                } else {
                    ix = sA.x * (eA.y - sA.y) / (eA.x - sA.x) - sB.x * (eB.y - sB.y) / (eB.x - sB.x) + sB.y - sA.y;
                    ix /=(eA.y - sA.y) / (eA.x - sA.x) - (eB.y - sB.y) / (eB.x - sB.x);
                    iy = (eA.y - sA.y) * (ix - sA.x) / (eA.x - sA.x) + sA.y;
                }
                cp[cpNum++] = Vector2D(ix, iy);
            }
        }
    }
    
    Vector2D contact;
    if(cpNum == 1)
        contact = cp[0];
    if(cpNum == 2)
        contact = (cp[0] + cp[1]) / 2.0f;
    
    Vector2D COMA = A->GetLeftTop() + A->GetCOM();
    Vector2D COMB = B->GetLeftTop() + B->GetCOM();
    Vector2D rA, rB;
    Vector2D rA2, rB2;
    
    rA = cp[0] - COMA;
    rB = cp[0] - COMB;
    rA2 = cp[1] - COMA;
    rB2 = cp[1] - COMB;
    
    //C_DrawVec(COMA, rA, 1);
    //C_DrawVec(COMB, rB, 1);
    //C_DrawVec(COMA, rA2, 3);
    //C_DrawVec(COMB, rB2, 3);
    
    printf("cpNum  %d\n", cpNum);
    printf("contact  %f %f\n",contact.x, contact.y);
    printf("COMA  %f %f\n",COMA.x, COMA.y);
    printf("COMB  %f %f\n",COMB.x, COMB.y);
    printf("A->isInPolygon(COMA) : %d\n", A->isInPolygon(COMA));
    
    double e = Min(A->restitution, B->restitution);
    double j = -(1 + e) * velAlongNormal;
    
    j /= ( A->inv_mass + B->inv_mass + A->inv_inertia * square(Cross((rA+rA2)/2, normal) + B->inv_inertia * square(Cross((rB+rB2)/2, normal))) );
    
    Vector2D draw = normal * 20;
    //C_DrawVec(contact, draw, 3);
    
    Vector2D impulse = normal * j;
    Vector2D AImpulse1 = -impulse * rA.size() / (rA.size() + rA2.size()), BImpulse1 = impulse * rB.size() / (rB.size() + rB2.size());
    Vector2D AImpulse2 = -impulse * rA2.size() / (rA.size() + rA2.size()), BImpulse2 = impulse * rB2.size() / (rB.size() + rB2.size());
    //Vector2D rA = AImpulse * A->inv_mass / A->angularVel;
    //Vector2D rB = BImpulse * B->inv_mass / B->angularVel;
    A->applyImpulse(AImpulse1, rA);
    B->applyImpulse(BImpulse1, rB);
    A->applyImpulse(AImpulse2, rA2);
    B->applyImpulse(BImpulse2, rB2);
    
    Vector2D crA = (rA + rA2) / 2;
    Vector2D crB = (rB + rB2) / 2;
    A->angularVel -= (Cross(crA, impulse) * A->inv_inertia) * 180.0f/3.14159f * 0.5f;
    B->angularVel += (Cross(crB, impulse) * B->inv_inertia) * 180.0f/3.14159f * 0.5f;
    
    
    
    printf("rv  %lf %lf\n", rv.x, rv.y);
    printf("normal  %lf %lf\n", normal.x, normal.y);
    
    Vector2D tangent = rv - normal * Dot(rv, normal);
    tangent = tangent.unitVector();
    
    printf("tangent  %lf %lf\n",tangent.x, tangent.y);
    
    if(tangent.size()>0) {
        Vector2D trv1 = (B->velocity + Cross(B->angularVel * 3.14159/180.0f, rB)) - (A->velocity + Cross(A->angularVel * 3.14159/180.0f, rA));
        Vector2D trv2 = (B->velocity + Cross(B->angularVel * 3.14159/180.0f, rB2)) - (A->velocity + Cross(A->angularVel * 3.14159/180.0f, rA2));
        Vector2D cross1 = Cross(A->angularVel * 3.14159/180.0f, rA);
        Vector2D cross2 = Cross(B->angularVel * 3.14159/180.0f, rB);
        //C_DrawVec(contact, cross1, 1);
        //C_DrawVec(contact, cross2, 1);
        float jt1 = -Dot(trv1, tangent);
        float jt2 = -Dot(trv2, tangent);
        double fj1 = -(1 + e) * (Dot(trv1, tangent));
        double fj2 = -(1 + e) * (Dot(trv2, tangent));
        fj1 /= ( A->inv_mass + B->inv_mass + A->inv_inertia * square(Cross(rA, tangent) + B->inv_inertia * square(Cross(rB, tangent))) );
        jt1 = jt1 / ( A->inv_mass + B->inv_mass + A->inv_inertia * square(Cross(rA, tangent) + B->inv_inertia * square(Cross(rB, tangent))) );
        jt2 = jt2 / ( A->inv_mass + B->inv_mass + A->inv_inertia * square(Cross(rA2, tangent) + B->inv_inertia * square(Cross(rB2, tangent))) );
    
        float mu = sqrt(square(A->staticFriction) + square(B->staticFriction));
    
        Vector2D frictionImpulse;
        printf("Absf(jt1)  %f\n", Absf(jt1));
        printf("fj1 * mu  %f\n", fj1 * mu);
        if(Absf(jt1) < Absf(fj1 * mu))
            frictionImpulse = tangent * -jt1;
        else {
            double dynamicFriction = sqrt(square(A->dynamicFriction) + square(B->dynamicFriction));
            frictionImpulse = tangent * -fj1 * dynamicFriction;
        }
        printf("frictionImpulse  %f %f\n", frictionImpulse.x, frictionImpulse.y);
        Vector2D AFriction = frictionImpulse * rA.size() / (rA.size() + rA2.size());
        Vector2D BFriction = -frictionImpulse * rB.size() / (rB.size() + rB2.size());
        printf("AFriction  %f %f\n",AFriction.x, AFriction.y);
        printf("BFriction  %f %f\n",BFriction.x, BFriction.y);
        A->applyImpulse(AFriction, rA);
        B->applyImpulse(BFriction, rB);
        
        
        if(Absf(jt2) < fj2 * mu)
            frictionImpulse = tangent * -jt2;
        else {
            double dynamicFriction = sqrt(square(A->dynamicFriction) + square(B->dynamicFriction));
            frictionImpulse = tangent * -fj2 * dynamicFriction;
        }
        printf("frictionImpulse  %f %f\n", frictionImpulse.x, frictionImpulse.y);
        AFriction = frictionImpulse * rA2.size() / (rA.size() + rA2.size());
        BFriction = -frictionImpulse * rB2.size() / (rB.size() + rB2.size());
        printf("AFriction  %f %f\n",AFriction.x, AFriction.y);
        printf("BFriction  %f %f\n",BFriction.x, BFriction.y);
        A->applyImpulse(AFriction, rA2);
        B->applyImpulse(BFriction, rB2);
        //A->velocity = A->velocity + frictionImpulse * A->inv_mass;
        //B->velocity = B->velocity - frictionImpulse * B->inv_mass;
    }
}