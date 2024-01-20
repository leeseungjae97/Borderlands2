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

	// Animation3D ����
	map<wstring ,tMTAnimClip>	m_vecAnimClip;
	//vector<tMTAnimClip>		m_vecAnimClip;
	vector<tMTBone>			m_vecBones;
	vector<Matrix>			m_vecBoneOffset;

	//vector<tFrameTrans>		m_vecKeyFrameTrans;

	//vector<int>					m_veciFrameCount;

	CStructuredBuffer*		m_pBoneFrameData;   //
	CStructuredBuffer*		m_pBlendFrameData;   //
	CStructuredBuffer*		m_pBoneOffset;	    //


	// �ϳ��� ���ؽ����ۿ� �������� �ε������۰� ����
	vector<tIndexInfo>		m_vecIdxInfo;
	
	void*					m_pVtxSys;

public:
	Vtx* GetVtxSysMem() { return (Vtx*)m_pVtxSys; }
	UINT GetSubsetCount() { return (UINT)m_vecIdxInfo.size(); }

	const vector<tMTBone>* GetBones() { return &m_vecBones; }
	UINT GetBoneCount() { return (UINT)m_vecBones.size(); }
	//const vector<tMTAnimClip>* GetAnimClip() { return &m_vecAnimClip; }
	const map<wstring, tMTAnimClip>& GetAnimClip() { return m_vecAnimClip; }
	bool IsAnimMesh() { return !m_vecAnimClip.empty(); }

	

	CStructuredBuffer* GetBoneFrameDataBuffer(const wstring& _AnimName); // ��ü �� ������ ����
	CStructuredBuffer* GetBlendFrameDataBuffer(const wstring& _AnimName); // ��ü �� ������ ����
	CStructuredBuffer* GetBoneOffsetBuffer() { return  m_pBoneOffset; }	   // �� ���� offset ���

public:
	static CMesh* CreateFromContainer(FBXLoader& _loader);
	void Create(void* _VtxSysMem, UINT _iVtxCount, void* _IdxSysMem, UINT _IdxCount);

	static void TransKeyFrame(CMesh* mesh);

public:
	virtual int Load(const wstring& _strFilePath) override;
	virtual int Save(const wstring& _strRelativePath) override;


	void render(UINT _iSubSet);
	void render_particle(UINT _iParticleCount);



private:
	virtual void UpdateData(UINT _iSubSet);
	

public:
	CMesh(bool _bEngine = false);
	~CMesh();
};