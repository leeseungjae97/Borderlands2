#pragma once
#include <Engine\Scene.h>

class MainScene :
    public Scene
{
public:
    void init() override;
    void enter() override;
    void exit() override;

public:
    MainScene();
    ~MainScene();
};

