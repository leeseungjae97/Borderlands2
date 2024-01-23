#pragma once

#ifndef PATH_FINDER_H
#define PATH_FINDER_H

#include "CComponent.h"
class CPathFind :
    public CComponent
{
    vector<tNode>   m_vecPath;

    Vec3            m_vStart;
    Vec3            m_vEnd;
    Vec3            m_vPrevPos;

    Vec2            m_vCurCoord;
    float           m_fSpeed;

public:
    vector<tNode>& GetMVecPath() { return m_vecPath; }
    void SetMVecPath(vector<tNode> m_vec_path) { m_vecPath = m_vec_path; }
    Vec3 GetMVecStart() const { return m_vStart; }
    void SetMVecStart(Vec3 m_vec_start) { m_vStart = m_vec_start; }
    Vec3 GetMVecEnd() const { return m_vEnd; }
    void SetMVecEnd(Vec3 m_vec_end);
    float GetMFSpeed() const { return m_fSpeed; }
    void SetMFSpeed(float m_f_speed) { m_fSpeed = m_f_speed; }
    void SetPath(const vector<tNode>& _Path) { m_vecPath = _Path; }
    
    void finaltick() override;

    void Move();

    void LoadFromLevelFile(FILE* _FILE) override;
    void SaveToLevelFile(FILE* _File) override;

    CLONE(CPathFind);
public:
    CPathFind();
    ~CPathFind();
};

#endif
