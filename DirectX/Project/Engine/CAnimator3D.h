#pragma once
#include "CAnimClip.h"
#include "CComponent.h"
#include "struct.h"

class CStructuredBuffer;
class CMesh;

class CAnimator3D :
    public CComponent
{
private:
    vector<tMTBone>				m_pVecBones;
    map<wstring, tMTAnimClip>	m_pMapClip;

    vector<float>				m_vecClipUpdateTime;
    vector<Matrix>				m_vecFinalBoneMat;
    int							m_iFrameCount;

    CStructuredBuffer*          m_pBoneFinalMatBuffer;
    bool						m_bFinalMatUpdate;

    CAnimClip*                  m_pCurClip;
    CAnimClip*                  m_pNextClip;

	bool						m_bLoop;
	bool						m_bMultipleClip;

	bool						m_bBlendMode;
	bool						m_bBlend;
	float						m_fBlendTime;
	float						m_fBlendAcc;

	float						m_fBRatio;

	bool						m_bStop;
	bool						m_bMaRatio;
	int							m_iManualIdx;
	float						m_iManualRatio;

	std::map<wstring, CAnimClip*> mClips;
	std::map<wstring, Events*> mEvents;
	std::map<UINT, wstring> m_mapPreDefineAnim;

	int							m_iHeadIdx;
	int							m_iCameraIdx;
	int							m_iWeaponHandIdx;
	int							m_iWeaponMuzzleIdx;

	Vec4						m_vHeadPos;
	Vec4						m_vMuzzlePos;

private:
    void check_mesh(Ptr<CMesh> _pMesh);
	void check_mesh(Ptr<CMesh> _pMesh, CStructuredBuffer* _buffer);
	void create_clip();

public:
    virtual void finaltick() override;
    void UpdateData();
    void UpdateData(CStructuredBuffer* structuredBuffer, bool IsRotate, bool IsTrans);

public:
	void SetBones(const vector<tMTBone>* _vecBones);
	void SetAnimClip(const map<wstring, tMTAnimClip>& _vecAnimClip);

	const std::map<wstring, CAnimClip*>& GetAnimClips() { return mClips; }
	const std::map<wstring, Events*>& GetEvents() { return mEvents; }
	const std::map<UINT, wstring>& GetPrefDefineAnimation() { return m_mapPreDefineAnim; }
	void SetCurAnimClip(CAnimClip* _Clip);
	CAnimClip* GetCurAnimClip() { return m_pCurClip; }

	// Animation Ȯ�ο� -----------------------------------------------------
	void ManualIdxUp();
	void ManualIdxDown();

	void StopPlay() { m_bStop = true; }
	void Proceed() { m_bStop = false; }
	bool IsPlayManual() { return m_bStop; }

	void ManualRatio() { m_bMaRatio = true; }
	void AutoRatio() { m_bMaRatio = false; }
	bool IsManualRatio() { return m_bMaRatio; }

	int GetManualIdx() { return m_iManualIdx; }

	void SetManualRatio(float _Ratio) { m_iManualRatio = _Ratio; }
	float GetManualRatio() { return m_iManualRatio; }
	// -----------------------------------------------------------------------

	// ������ -------------------------------------------------------------------
	void SetBlend(bool _bBlend, float _fBlendTime);
	void SetBlendRatio(float _Ratio) { m_fBRatio = _Ratio; }
	float GetBlendRatio() { return m_fBRatio; }
	float GetBlendAcc() { return m_fBlendAcc; }
	// -----------------------------------------------------------------------

	CStructuredBuffer* GetFinalBoneMat() { return m_pBoneFinalMatBuffer; }
	UINT GetBoneCount() { return (UINT)m_pVecBones.size(); }
	vector<tMTBone> GetBone() { return m_pVecBones; }

	void ClearData();

	void SetDefineAnimation(wstring animName, UINT _Type);
	const wstring& GetDefineAnimationName(UINT _Type);
	bool DeleteDefineAnimation(wstring animName);
	void DeleteDefine(UINT _Type);

	//void SetDefineAnimation(wstring animName, GUN_ANIMATION_TYPE _Type);
	//const wstring& GetDefineAnimationName(GUN_ANIMATION_TYPE _Type);
	//void DeleteDefine(GUN_ANIMATION_TYPE _Type);

	UINT FindDefineAnimation(wstring animName);
	void SetPreDefineAnimation(const std::map<UINT, wstring>& _mapAnim) { m_mapPreDefineAnim = _mapAnim; }

	int GetHeadIdx() { return m_iHeadIdx; }
	int GetCameraIdx() { return m_iCameraIdx; }
	int GetWeaponHandIdx() { return m_iWeaponHandIdx; }
	int GetWeaponMuzzleIdx() { return m_iWeaponMuzzleIdx; }

	Vec4 GetHeadPos();
	Vec4 GetMuzzlePos() { return m_vMuzzlePos; }

public:
	void Play(const wstring& _Name, bool _Loop);
	void Play(UINT _type, bool _Loop);
	Events* FindEvents(const wstring& name);
	CAnimClip* FindClip(const wstring& name);

    std::shared_ptr<std::function<void()>>& StartEvent(const wstring key);
	std::shared_ptr<std::function<void()>>& StartEvent(const UINT key);

	std::shared_ptr<std::function<void()>>& CompleteEvent(const wstring key);
	std::shared_ptr<std::function<void()>>& CompleteEvent(const UINT key);

	std::shared_ptr<std::function<void()>>& EndEvent(const wstring key);
	std::shared_ptr<std::function<void()>>& EndEvent(const UINT key);

	std::shared_ptr<std::function<void()>>& ProgressEvent(const wstring key);
	std::shared_ptr<std::function<void()>>& ProgressEvent(const UINT key);

public:
    virtual void SaveToLevelFile(FILE* _pFile) override;
    virtual void LoadFromLevelFile(FILE* _pFile) override;
    CLONE(CAnimator3D);

public:
    CAnimator3D();
    CAnimator3D(const CAnimator3D& _origin);
    ~CAnimator3D();
};

