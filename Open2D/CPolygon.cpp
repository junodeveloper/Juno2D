//
//  CPolygon.cpp
//  Open2D
//
//  Created by 황준호 on 2015. 3. 27..
//  Copyright (c) 2015년 황준호. All rights reserved.
//

#include "CPolygon.h"

CPolygon::CPolygon(int vertexCount) {
    m_vertexCount = 0;
    m_vectors = new Vector2D[vertexCount];
    m_normals = new Vector2D[vertexCount];
    m_fromLT = new Vector2D[vertexCount];
}

CPolygon::CPolygon(double width, double height) : CPolygon(4) {
    addVector(width,0);
    addVector(0,height);
    addVector(-width,0);
    addVector(0,-height);
}
CPolygon::~CPolygon() {
    if(m_vectors != NULL)
        delete m_vectors;
    if(m_normals != NULL)
        delete m_normals;
    if(m_fromLT != NULL)
        delete m_fromLT;
}

void CPolygon::addVector(double x, double y) {
    m_vectors[m_vertexCount] = Vector2D(x, y);
    m_normals[m_vertexCount] = Vector2D(y, -x).unitVector();
    Vector2D s = Vector2D(0,0);
    for(int i=0;i<m_vertexCount;i++)
        s = s + m_vectors[i];
    m_fromLT[m_vertexCount] = s + m_vectors[m_vertexCount];
    m_vertexCount++;
}