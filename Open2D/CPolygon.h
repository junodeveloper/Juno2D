//
//  CPolygon.h
//  Open2D
//
//  Created by 황준호 on 2015. 3. 27..
//  Copyright (c) 2015년 황준호. All rights reserved.
//

#ifndef __Open2D__CPolygon__
#define __Open2D__CPolygon__

#include <stdio.h>
#include "Vector2D.h"

class CPolygon {
public:
    CPolygon(int vertexCount);
    CPolygon(double width, double height);
    ~CPolygon();
    
    void addVector(double x, double y);
    
    int m_vertexCount;
    Vector2D *m_vectors;
    Vector2D *m_normals;
    Vector2D *m_fromLT;
private:
    
};

#endif /* defined(__Open2D__CPolygon__) */
