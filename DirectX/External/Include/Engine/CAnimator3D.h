#pragma once
#include "CAnimClip.h"
#include "CComponent.h"

class CStructuredBuffer;
class CMesh;

class CAnimator3D :
    public CComponent
{
public:
    	struct Event
		{
			Event(){};
			~Event(){};

			void operator=(std::shared_ptr<std::function<void()>> func)
			{
				mEvent = std::move(func);
			}
			void operator()()
			{
				if (mEvent)
					(*mEvent)();
			}

			std::shared_ptr<std::function<void()>> mEvent;
		};

		struct Events
		{
			Events()
				: startEvent{}
				, completeEvent{}
				, progressEvent{}
				, endEvent{}
		    {};
			~Events(){};
			Event startEvent;
			Event completeEvent;
			Event progressEvent;
			Event endEvent;
		};

private:
    const vector<tMTBone>*      m_pVecBones;
    const vector<tMTAnimClip>*  m_pVecClip;

    vector<float>				m_vecClipUpdateTime;
    vector<Matrix>				m_vecFinalBoneMat; // �ؽ��Ŀ� ������ ���� �������
    int							m_iFrameCount; // 30
    double						m_dCurTime;
    int							m_iCurClip; // Ŭ�� �ε���	

    int							m_iFrameIdx; // Ŭ���� ���� ������
    int							m_iNextFrameIdx; // Ŭ���� ���� ������
    float						m_fRatio;	// ������ ���� ����

    CStructuredBuffer*          m_pBoneFinalMatBuffer;  // Ư�� �������� ���� ���
    bool						m_bFinalMatUpdate; // ������� ���� ���࿩��

    CAnimClip*                  m_pCurClip;

	bool						m_bLoop;

	std::map<std::wstring, CAnimClip*> mClips;
	std::map<std::wstring, Events*> mEvents;

private:
    void check_mesh(Ptr<CMesh> _pMesh);

public:
    virtual void finaltick() override;
    void UpdateData();

public:
    void SetBones(const vector<tMTBone>* _vecBones) { m_pVecBones = _vecBones; m_vecFinalBoneMat.resize(m_pVecBones->size()); }
    void SetAnimClip(const vector<tMTAnimClip>* _vecAnimClip);
    void SetClipTime(int _iClipIdx, float _fTime) { m_vecClipUpdateTime[_iClipIdx] = _fTime; }

    CStructuredBuffer* GetFinalBoneMat() { return m_pBoneFinalMatBuffer; }
    UINT GetBoneCount() { return (UINT)m_pVecBones->size(); }
    void ClearData();

public:
	void Play(const std::wstring& _Name, bool repeat);
	CAnimator3D::Events* FindEvents(const std::wstring& name);
	CAnimClip* FindClip(const std::wstring& name);

    std::shared_ptr<std::function<void()>>& StartEvent(const std::wstring key);
	std::shared_ptr<std::function<void()>>& CompleteEvent(const std::wstring key);
	std::shared_ptr<std::function<void()>>& EndEvent(const std::wstring key);
	std::shared_ptr<std::function<void()>>& ProgressEvent(const std::wstring key);

public:
    virtual void SaveToLevelFile(FILE* _pFile) override;
    virtual void LoadFromLevelFile(FILE* _pFile) override;
    CLONE(CAnimator3D);

public:
    CAnimator3D();
    CAnimator3D(const CAnimator3D& _origin);
    ~CAnimator3D();
};
