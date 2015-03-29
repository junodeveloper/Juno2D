//
//  CGroup.h
//  Open2D
//
//  Created by 황준호 on 2015. 3. 29..
//  Copyright (c) 2015년 황준호. All rights reserved.
//

#ifndef __Open2D__CGroup__
#define __Open2D__CGroup__

#include <stdio.h>
#include "CObject.h"
#include <vector>
using namespace std;
class CGroup {
public:
    CGroup();
    ~CGroup();
    
    void addObject(CObject *object);
    void update();
    void draw();
private:
    vector<CObject*> objects;
};

#endif /* defined(__Open2D__CGroup__) */
