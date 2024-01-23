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

private:
	// Animation3D 정보
	map<wstring ,tMTAnimClip>	m_mapAnimClip;
	//vector<tMTAnimClip>		m_mapAnimClip;
	vector<tMTBone>			m_vecBones;
	vector<Matrix>			m_vecBoneOffset;

	//vector<tFrameTrans>		m_vecKeyFrameTrans;

	//vector<int>					m_veciFrameCount;

	CStructuredBuffer*		m_pBoneFrameData;   //
	CStructuredBuffer*		m_pBlendFrameData;   //
	CStructuredBuffer*		m_pBoneOffset;	    //


	// 하나의 버텍스버퍼에 여러개의 인덱스버퍼가 연결
	vector<tIndexInfo>		m_vecIdxInfo;
	
	void*					m_pVtxSys;

public:
	Vtx* GetVtxSysMem() { return (Vtx*)m_pVtxSys; }
	const vector<tIndexInfo>& GetIndexInfo() { return m_vecIdxInfo; }
	UINT GetSubsetCount() { return (UINT)m_vecIdxInfo.size(); }

	const vector<tMTBone>* GetBones() { return &m_vecBones; }
	UINT GetBoneCount() { return (UINT)m_vecBones.size(); }
	//const vector<tMTAnimClip>* GetAnimClip() { return &m_mapAnimClip; }
	const map<wstring, tMTAnimClip>& GetAnimClip() { return m_mapAnimClip; }
	bool IsAnimMesh() { return !m_mapAnimClip.empty(); }

	UINT GetVtxCount() { return m_VtxCount; }
	void SetVtxCount(UINT m_vtx_count) { m_VtxCount = m_vtx_count; }

	CStructuredBuffer* GetBoneFrameDataBuffer(const wstring& _AnimName); // 전체 본 프레임 정보
	CStructuredBuffer* GetBlendFrameDataBuffer(const wstring& _AnimName); // 전체 본 프레임 정보
	CStructuredBuffer* GetBoneOffsetBuffer() { return  m_pBoneOffset; }	   // 각 뼈의 offset 행렬

public:
	static CMesh* CreateFromContainer(FBXLoader& _loader);
	void Create(void* _VtxSysMem, UINT _iVtxCount, void* _IdxSysMem, UINT _IdxCount);

	static void TransKeyFrame(CMesh* mesh);

public:
	virtual int Load(const wstring& _strFilePath) override;
	virtual int Save(const wstring& _strRelativePath) override;


	void render(UINT _iSubSet);
	void render_instancing(UINT _iSubSet);
	void render_particle(UINT _iParticleCount);



private:
	virtual void UpdateData(UINT _iSubSet);
	virtual void UpdateData_Instancing(UINT _iSubSet);
	

public:
	CMesh(bool _bEngine = false);
	~CMesh();
};