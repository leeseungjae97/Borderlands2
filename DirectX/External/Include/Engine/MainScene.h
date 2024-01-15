#pragma once
#include "Scene.h"
class MainScene :
    public Scene
{
public:
    virtual void tick() override;
    virtual void finaltick() override;
    void enter() override;
    void exit() override;

public:
    MainScene();
    ~MainScene();
};

