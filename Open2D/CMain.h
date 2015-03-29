//
//  CMain.h
//  SDL_OpenGL
//
//  Created by 황준호 on 2015. 3. 23..
//  Copyright (c) 2015년 황준호. All rights reserved.
//

#ifndef __SDL_OpenGL__CMain__
#define __SDL_OpenGL__CMain__

#include <stdio.h>
#include "CSDL_Setup.h"
#include "CObject.h"
#include "CGroup.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

class CMain {
public:
    CMain();
    ~CMain();
    
    void Init();
    void Quit();
    void Start();
    void Update();
    void Render();
private:
    CSDL_Setup *csdl;
    CObject *obj1, *obj2, *ground, *wall1, *wall2;
    CGroup *group;
    
    bool quit;
    bool leftPushed, rightPushed, upPushed, downPushed;
    bool pushed[26];
};

#endif /* defined(__SDL_OpenGL__CMain__) */
