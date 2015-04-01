//
//  Vector2D.h
//  Juno2D
//
//  Created by 황준호 on 2015. 3. 10..
//  Copyright (c) 2015년 황준호. All rights reserved.
//

#ifndef __Juno2D__Vector2D__
#define __Juno2D__Vector2D__

#include <stdio.h>
#include <math.h>

class Vector2D {
public:
    Vector2D();
    Vector2D(int none);
    Vector2D(double x, double y);
    ~Vector2D();
    Vector2D unitVector();
    double size();
    Vector2D operator+(const Vector2D dst);
    Vector2D operator/(const double n);
    Vector2D operator*(const double n);
    Vector2D operator-(const Vector2D dst);
    Vector2D operator-();
    bool none();
    double x, y;
};
double Dot(const Vector2D& a, const Vector2D& b);
double Cross(const Vector2D& a, const Vector2D& b);


#endif /* defined(__Juno2D__Vector2D__) */
