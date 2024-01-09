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
class CMesh
	: public CRes
{
private:
	ComPtr<ID3D11Buffer>	m_VB;
	D3D11_BUFFER_DESC		m_tVBDesc;
	UINT					m_VtxCount;


	// 하나의 버텍스버퍼에 여러개의 인덱스버퍼가 연결
	vector<tIndexInfo>		m_vecIdxInfo;
	
	void*					m_pVtxSys;

public:
	Vtx* GetVtxSysMem() { return (Vtx*)m_pVtxSys; }
	UINT GetSubsetCount() { return (UINT)m_vecIdxInfo.size(); }

public:
	static CMesh* CreateFromContainer(FBXLoader& _loader);
	void Create(void* _VtxSysMem, UINT _iVtxCount, void* _IdxSysMem, UINT _IdxCount);

private:
	virtual int Load(const wstring& _strFilePath) { return S_OK; }
public:
	virtual int Save(const wstring& _strRelativePath) { return S_OK; }


	void render(UINT _iSubSet);
	void render_particle(UINT _iParticleCount);



private:
	virtual void UpdateData(UINT _iSubSet);
	

public:
	CMesh(bool _bEngine = false);
	~CMesh();
};