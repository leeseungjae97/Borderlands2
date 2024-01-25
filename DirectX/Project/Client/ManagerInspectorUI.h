#pragma once
#include "TreeUI.h"
#include "UI.h"
class ManagerInspectorUI :
    public UI
{
private:
    TreeUI* m_Tree;

public:
    void ChangeLevel(DWORD_PTR _ptr);
    void ResetLevels();

public:
    virtual void init() override;
    virtual void tick() override;
    virtual int render_update() override;

public:
    ManagerInspectorUI();
    ~ManagerInspectorUI();  
};

