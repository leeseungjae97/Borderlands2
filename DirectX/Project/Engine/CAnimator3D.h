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
			~Events(){}
			void SaveToLevelFile(FILE* file)
			{
				fwrite(&startEvent, sizeof(Event), 1, file);
				fwrite(&completeEvent, sizeof(Event), 1, file);
				fwrite(&progressEvent, sizeof(Event), 1, file);
				fwrite(&endEvent, sizeof(Event), 1, file);
			}

			void LoadFromLevelFile(FILE* file)
			{
				fread(&startEvent, sizeof(Event), 1, file);
				fread(&completeEvent, sizeof(Event), 1, file);
				fread(&progressEvent, sizeof(Event), 1, file);
				fread(&endEvent, sizeof(Event), 1, file);
			}
			Event startEvent;
			Event completeEvent;
			Event progressEvent;
			Event endEvent;
		};

private:
    const vector<tMTBone>*      m_pVecBones;
    const vector<tMTAnimClip>*  m_pVecClip;

    vector<float>				m_vecClipUpdateTime;
    vector<Matrix>				m_vecFinalBoneMat; // 텍스쳐에 전달할 최종 행렬정보
    int							m_iFrameCount; // 30
    //double						m_dCurTime;
    //int							m_iCurClip; // 클립 인덱스	

    //int							m_iFrameIdx; // 클립의 현재 프레임
    //int							m_iNextFrameIdx; // 클립의 다음 프레임
    //float						m_fRatio;	// 프레임 사이 비율

    CStructuredBuffer*          m_pBoneFinalMatBuffer;  // 특정 프레임의 최종 행렬
    bool						m_bFinalMatUpdate; // 최종행렬 연산 수행여부

    CAnimClip*                  m_pCurClip;

	bool						m_bLoop;

	bool						m_bMultipleClip;

	int							m_iClipIdx;

	std::map<std::wstring, CAnimClip*> mClips;
	std::map<std::wstring, Events*> mEvents;

private:
    void check_mesh(Ptr<CMesh> _pMesh);
	void create_clip();

public:
    virtual void finaltick() override;
    void UpdateData();

public:
	void SetBones(const vector<tMTBone>* _vecBones);
    void SetAnimClip(const vector<tMTAnimClip>* _vecAnimClip);

	const std::map<std::wstring, CAnimClip*>& GetAnimClips() { return mClips; }
	const std::map<std::wstring, Events*>& GetEvents() { return mEvents; }

	void SetCurAnimClip(CAnimClip* _Clip) { m_pCurClip = _Clip; }
	CAnimClip* GetCurAnimClip() { return m_pCurClip; }

	void SetClipIdx(int _idx) { m_iClipIdx = _idx; }
	int GetClipIdx() { return m_iClipIdx; }


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

