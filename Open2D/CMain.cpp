//
//  CMain.cpp
//  SDL_OpenGL
//
//  Created by 황준호 on 2015. 3. 23..
//  Copyright (c) 2015년 황준호. All rights reserved.
//

#include "CMain.h"

CMain::CMain() {
}
CMain::~CMain() {
}

void CMain::Update() {
    if(leftPushed) obj1->accelerate(-0.2,0);
    if(rightPushed) obj1->accelerate(0.2,0);
    if(upPushed) obj1->accelerate(0,-0.2);
    if(downPushed) obj1->accelerate(0,0.2);
    if(pushed['a' - 'a']) obj1->angularAcc(0.05);
    if(pushed['s' - 'a']) obj1->angularAcc(-0.05);
    if(pushed['d' - 'a']) {
        obj1->setAngle(0);
        obj1->setAngularVel(0);
    }
    //printf("penetration : %f\n", Penetration(obj1, obj2));
    group->update();
}
void CMain::Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    group->draw();
    obj1->drawNormals();
    obj2->drawNormals();
    SDL_GL_SwapWindow(csdl->getWindow());
}

void CMain::Start() {
    Init();
    while(!quit) {
        SDL_PollEvent(csdl->getEvent());
        SDL_Keycode c;
        switch(csdl->getEvent()->type) {
            case SDL_QUIT:
                quit = true;
                break;
            case SDL_KEYDOWN:
                c = csdl->getEvent()->key.keysym.sym;
                switch(c) {
                    case SDLK_LEFT: leftPushed=true; break;
                    case SDLK_RIGHT: rightPushed=true; break;
                    case SDLK_UP: upPushed=true; break;
                    case SDLK_DOWN: downPushed=true; break;
                }
                if(SDLK_a <= c && c <= SDLK_z)
                    pushed[c - SDLK_a] = true;
                break;
            case SDL_KEYUP:
                c = csdl->getEvent()->key.keysym.sym;
                switch(csdl->getEvent()->key.keysym.sym) {
                    case SDLK_LEFT: leftPushed=false; break;
                    case SDLK_RIGHT: rightPushed=false; break;
                    case SDLK_UP: upPushed=false; break;
                    case SDLK_DOWN: downPushed=false; break;
                }
                if(SDLK_a <= c && c <= SDLK_z)
                    pushed[c - SDLK_a] = false;
                break;
        }
        Update();
        Render();
        SDL_Delay(10);
    }
    Quit();
}

void CMain::Init() {
    quit = false;
    csdl = new CSDL_Setup("Open2D Test", SCREEN_WIDTH, SCREEN_HEIGHT);
    
    group = new CGroup();
    
    obj1 = new CObject(csdl, OBJECT_BOX);
    obj1->move(100,100);
    obj2 = new CObject(csdl, OBJECT_BOX);
    obj2->move(300,200);
    ground = new CObject(csdl, OBJECT_GROUND);
    ground->move(200,500);
    wall1 = new CObject(csdl, OBJECT_WALL);
    wall1->move(0,50);
    wall2 = new CObject(csdl, OBJECT_WALL);
    wall2->move(600,50);
    
    group->addObject(obj1);
    group->addObject(obj2);
    group->addObject(ground);
    group->addObject(wall1);
    group->addObject(wall2);
    
    leftPushed = rightPushed = upPushed = downPushed = false;
    for(int i=0;i<26;i++)
        pushed[i] = false;
}
void CMain::Quit() {
    delete group;
    delete csdl;
}