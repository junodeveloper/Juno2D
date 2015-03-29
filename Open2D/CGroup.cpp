//
//  CGroup.cpp
//  Open2D
//
//  Created by 황준호 on 2015. 3. 29..
//  Copyright (c) 2015년 황준호. All rights reserved.
//

#include "CGroup.h"

CGroup::CGroup() {
    objects.clear();
}
CGroup::~CGroup() {
    for(int i=0;i<objects.size();i++)
        delete objects[i];
}

void CGroup::addObject(CObject *object) {
    objects.push_back(object);
}

void CGroup::update() {
    printf("size  %d\n",objects.size());
    for(int i=0;i<objects.size();i++)
        if(!objects[i]->fixed) objects[i]->update();
}

void CGroup::draw() {
    for(int i=0;i<objects.size();i++)
        objects[i]->draw();
    for(int i=0;i<objects.size();i++)
        for(int j=i+1;j<objects.size();j++) {
            ResolveCollision(objects[i], objects[j]);
        }
}