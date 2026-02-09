#pragma once
#include "CComponent.h"

class CTransform :
    public CComponent
{
private:
    Vec3    m_vRelativePos;
    Vec3    m_vRelativeScale;
    Vec3    m_vRelativeRot;
    Vec3    m_vRelativePosOffset;

    Quat m_qRotation;

    bool    m_bAbsolute;

    Vec3    m_vRelativeDir[3];
    Vec3    m_vWorldDir[3];

    Vec3    m_FollowOffset;

    Matrix  m_matWorldScale;
    Matrix  m_matWorld;
    Matrix  m_matWorldInv;
    Matrix  m_DrawRayWorld;

    float _mt;

    bool m_bBilboard;
public:
    void SetRelativePos(Vec3 _vPos);
    void SetRelativeScale(Vec3 _vScale) { m_vRelativeScale = _vScale; }
    void SetRelativeRot(Vec3 _vRot);
    void SetRelativePosOffset(Vec3 _Offset) { m_vRelativePosOffset = _Offset; }

    void SetRelativePos(float _x, float _y, float _z) { m_vRelativePos = Vec3(_x, _y, _z); }
    void SetRelativePosX(float _x) { m_vRelativePos.x = _x; }
    void SetRelativePosY(float _y) { m_vRelativePos.y = _y; }
    void SetRelativePosZ(float _z) { m_vRelativePos.z = _z; }
    void SetRelativeScale(float _x, float _y, float _z) { m_vRelativeScale = Vec3(_x, _y, _z); }
    void SetRelativeRot(float _x, float _y, float _z) { m_vRelativeRot = Vec3(_x, _y, _z);  }

    // ��� �̵�, ũ�⸦ ���밪���� ����  
    void SetAbsolute(bool _Set) { m_bAbsolute = _Set; }    

    Vec3 GetRelativePos() const { return m_vRelativePos; }
    Vec3 GetRelativePosOffset() const { return m_vRelativePosOffset; }
    Vec3 GetRelativeScale() const { return m_vRelativeScale; }
    Vec3 GetRelativeRot() const { return m_vRelativeRot; }

    Vec3 GetRelativeDir(DIR_TYPE _type) const { return m_vRelativeDir[(UINT)_type]; }
    Vec3 GetWorldDir(DIR_TYPE _type) const { { return m_vWorldDir[(UINT)_type]; } }
    Vec3 GetWorldPos() { return m_matWorld.Translation(); }

    void SetFollowOffset(Vec3 _Offset) { m_FollowOffset = _Offset; }
    Vec3 GetFollowOffset() { return m_FollowOffset; }

    void SetBilboard(bool _bTrue) { m_bBilboard = _bTrue; }

    const Matrix& GetWorldScaleMat() { return m_matWorldScale; }
    const Matrix& GetWorldMat() const { return m_matWorld; }
    const Matrix& GetWorldInvMat() const { return m_matWorldInv; }
    const Matrix& GetDrawRayMat() const { return m_DrawRayWorld; }

    void SetWorldMat(const Matrix& _mat) { m_matWorld = _mat; }

    void SetMt(float mt) { _mt = mt; }
public:
    virtual void finaltick() override;    
    void UpdateData();

    void operator = (const CTransform& _OtherTransform)
    {
		m_vRelativePos = _OtherTransform.m_vRelativePos;
		m_vRelativeScale = _OtherTransform.m_vRelativeScale;
		m_vRelativeRot = _OtherTransform.m_vRelativeRot;
		m_bAbsolute = _OtherTransform.m_bAbsolute;
		m_matWorldScale = _OtherTransform.m_matWorldScale;
		m_matWorld = _OtherTransform.m_matWorld;
		m_matWorldInv = _OtherTransform.m_matWorldInv;
        m_bBilboard = _OtherTransform.m_bBilboard;

        for (int i = 0; i < 3; ++i)
        {
            m_vRelativeDir[i] = _OtherTransform.m_vRelativeDir[i];
            m_vWorldDir[i] = _OtherTransform.m_vWorldDir[i];
        }
    }


public:
    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;

    CLONE(CTransform);
public:
    CTransform();
    virtual ~CTransform();
};

