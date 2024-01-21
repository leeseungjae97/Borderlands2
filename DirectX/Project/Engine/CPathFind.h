#pragma once

#ifndef PATH_FINDER_H
#define PATH_FINDER_H

#include "CComponent.h"
class CPathFind :
    public CComponent
{
    vector<tNode>   m_vecPath;

    Vec3            m_vecStart;
    Vec3            m_vecEnd;

    float           m_fSpeed;

public:
    vector<tNode> m_vec_path() const { return m_vecPath; }
    void set_m_vec_path(const vector<tNode>& m_vec_path) { m_vecPath = m_vec_path; }
    Vec3 m_vec_start() const { return m_vecStart; }
    void set_m_vec_start(const Vec3& m_vec_start) { m_vecStart = m_vec_start; }
    Vec3 m_vec_end() const { return m_vecEnd; }
    void set_m_vec_end(const Vec3& m_vec_end) { m_vecEnd = m_vec_end; }
    float m_f_speed() const { return m_fSpeed; }
    void set_m_f_speed(float m_f_speed) { m_fSpeed = m_f_speed; }


    void SetPath(const vector<tNode>& _Path) { m_vecPath = _Path; }
    

    void finaltick() override;

    void LoadFromLevelFile(FILE* _FILE) override;
    void SaveToLevelFile(FILE* _File) override;

    CLONE(CPathFind);
public:
    CPathFind();
    ~CPathFind();
};

#endif
