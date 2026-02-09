#pragma once


void CreateLevels();

void CollisionSettings();

void CreateMainMenu(CLevel*& pCurLevel, float fDefaultScale);

void CreateLevel2(CLevel* pCurLevel, float fDefaultScale);

void CreateLevel1(float fDefaultScale);

void CreateFire2();

void CreateFire1();

void CreateRobot(float fDefaultScale, int i = 1, int j = 1);
