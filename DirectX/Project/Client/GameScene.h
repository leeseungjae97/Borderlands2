#pragma once
#include <Engine\Scene.h>

class GameScene :
    public Scene
{
public:
    virtual void init() override;
    void enter() override;
    void exit() override;

public:
    GameScene();
    ~GameScene();
};

