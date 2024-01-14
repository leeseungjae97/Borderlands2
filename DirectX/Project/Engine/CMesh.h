#pragma once
#include "CRes.h"
#include "mFBXLoader.h"


struct tIndexInfo
{
	ComPtr<ID3D11Buffer>	pIB;
	D3D11_BUFFER_DESC		tIBDesc;
	UINT					iIdxCount;
	void*					pIdxSysMem;
};

class CStructuredBuffer;

class CMesh
	: public CRes
{
private:
	ComPtr<ID3D11Buffer>	m_VB;
	D3D11_BUFFER_DESC		m_tVBDesc;
	UINT					m_VtxCount;

	// Animation3D 정보
	vector<tMTAnimClip>		m_vecAnimClip;
	vector<tMTBone>			m_vecBones;
	vector<Matrix>			m_vecBoneOffset;

	//vector<tFrameTrans>		m_vecKeyFrameTrans;

	//vector<int>					m_veciFrameCount;

	CStructuredBuffer*		m_pBoneFrameData;   // 전체 본 프레임 정보(크기, 이동, 회전) (프레임 개수만큼)
	CStructuredBuffer*		m_pBoneOffset;	    // 각 뼈의 offset 행렬(각 뼈의 위치를 되돌리는 행렬) (1행 짜리)


	// 하나의 버텍스버퍼에 여러개의 인덱스버퍼가 연결
	vector<tIndexInfo>		m_vecIdxInfo;
	
	void*					m_pVtxSys;

public:
	Vtx* GetVtxSysMem() { return (Vtx*)m_pVtxSys; }
	UINT GetSubsetCount() { return (UINT)m_vecIdxInfo.size(); }

	const vector<tMTBone>* GetBones() { return &m_vecBones; }
	UINT GetBoneCount() { return (UINT)m_vecBones.size(); }
	const vector<tMTAnimClip>* GetAnimClip() { return &m_vecAnimClip; }
	bool IsAnimMesh() { return !m_vecAnimClip.empty(); }

	CStructuredBuffer* GetBoneFrameDataBuffer(int _Idx); // 전체 본 프레임 정보
	CStructuredBuffer* GetBoneOffsetBuffer() { return  m_pBoneOffset; }	   // 각 뼈의 offset 행렬

public:
	static CMesh* CreateFromContainer(FBXLoader& _loader);
	void Create(void* _VtxSysMem, UINT _iVtxCount, void* _IdxSysMem, UINT _IdxCount);

private:
	virtual int Load(const wstring& _strFilePath) override;
public:
	virtual int Save(const wstring& _strRelativePath) override;


	void render(UINT _iSubSet);
	void render_particle(UINT _iParticleCount);



private:
	virtual void UpdateData(UINT _iSubSet);
	

public:
	CMesh(bool _bEngine = false);
	~CMesh();
};