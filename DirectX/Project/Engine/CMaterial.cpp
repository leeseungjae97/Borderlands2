#include "pch.h"
#include "CMaterial.h"

#include "CDevice.h"
#include "CConstBuffer.h"

#include "CResMgr.h"
#include "CPathMgr.h"
#include "CTimeMgr.h"

CMaterial::CMaterial(bool _bEngine)
	: CRes(RES_TYPE::MATERIAL, _bEngine)
	, m_Const{}
	, m_arrTex{}	
{	
}

CMaterial::~CMaterial()
{
}

void CMaterial::UpdateData()
{
	if (nullptr == m_pShader)
		return;

	m_pShader->UpdateData();

	// Texture Update
	for (UINT i = 0; i < TEX_END; ++i)
	{
		if (nullptr == m_arrTex[i])
		{
			m_Const.arrTex[i] = 0;
			CTexture::Clear(i);
			continue;
		}
		else
		{
			m_Const.arrTex[i] = 1;
			m_arrTex[i]->UpdateData(i, PIPELINE_STAGE::PS_ALL);
		}
	}

	if (m_PaperBurnConst.paperBurn)
	{
		if (m_Const.arrTex[6] == 0)
			SetTexParam(TEX_6, CResMgr::GetInst()->FindRes<CTexture>(L"texture//noise//noise_03.jpg"));

		m_PaperBurnConst.paperAcc += DT / m_PaperBurnConst.paperTime;

		if (m_PaperBurnConst.paperAcc >= 1.f)
		{
			//m_PaperBurnConst.paperAcc = 0.0f;
			//m_PaperBurnConst.paperBurn = false;
			m_PaperBurnConst.paperBurnEnd = true;
		}
	}

	// Constant Update
	CConstBuffer* pMtrlBuffer = CDevice::GetInst()->GetConstBuffer(CB_TYPE::MATERIAL);
	pMtrlBuffer->SetData(&m_Const);
	pMtrlBuffer->UpdateData();

	pMtrlBuffer = CDevice::GetInst()->GetConstBuffer(CB_TYPE::PAPER_BURN);
	pMtrlBuffer->SetData(&m_PaperBurnConst);
	pMtrlBuffer->UpdateData();
}

void CMaterial::UpdateData_Instancing()
{
	if (nullptr == m_pShader)
		return;

	m_pShader->UpdateData_Instancing();

	// Texture Update
	for (UINT i = 0; i < TEX_END; ++i)
	{
		if (nullptr == m_arrTex[i])
		{
			m_Const.arrTex[i] = 0;
			CTexture::Clear(i);
			continue;
		}

		else
		{
			m_Const.arrTex[i] = 1;
			m_arrTex[i]->UpdateData(i, PIPELINE_STAGE::PS_ALL);
		}
	}

	if (m_PaperBurnConst.paperBurn)
	{
		if (m_Const.arrTex[6] == 0)
			SetTexParam(TEX_6, CResMgr::GetInst()->FindRes<CTexture>(L"texture//noise//noise_03.jpg"));

		m_PaperBurnConst.paperAcc += DT / m_PaperBurnConst.paperTime;

		if (m_PaperBurnConst.paperAcc >= 1.f)
		{
			//m_PaperBurnConst.paperAcc = 1.f;
			//m_PaperBurnConst.paperBurn = false;
			m_PaperBurnConst.paperBurnEnd = true;
		}
	}

	// Constant Update
	CConstBuffer* pMtrlBuffer = CDevice::GetInst()->GetConstBuffer(CB_TYPE::MATERIAL);
	pMtrlBuffer->SetData(&m_Const);
	pMtrlBuffer->UpdateData();

	pMtrlBuffer = CDevice::GetInst()->GetConstBuffer(CB_TYPE::PAPER_BURN);
	pMtrlBuffer->SetData(&m_PaperBurnConst);
	pMtrlBuffer->UpdateData();
}

void CMaterial::SetScalarParam(SCALAR_PARAM _Param, const void* _Src)
{
	switch (_Param)
	{
	case INT_0:
	case INT_1:
	case INT_2:
	case INT_3:
	case INT_4:
	case INT_5:
	case INT_6:
	case INT_7:
		m_Const.arrInt[_Param] = *((int*)_Src);
		break;
	case FLOAT_0:
	case FLOAT_1:
	case FLOAT_2:
	case FLOAT_3:
	case FLOAT_4:
	case FLOAT_5:
	case FLOAT_6:
	case FLOAT_7:
		m_Const.arrFloat[_Param - FLOAT_0] = *((float*)_Src);
		break;

	case VEC2_0:
	case VEC2_1:
	case VEC2_2:
	case VEC2_3:
		m_Const.arrV2[_Param - VEC2_0] = *((Vec2*)_Src);
		break;

	//case VEC3_0:
	//case VEC3_1:
	//case VEC3_2:
	//case VEC3_3:
	//	m_Const.arrV3[_Param - VEC3_0] = *((Vec3*)_Src);
	//	break;

	case VEC4_0:
	case VEC4_1:
	case VEC4_2:
	case VEC4_3:
		m_Const.arrV4[_Param - VEC4_0] = *((Vec4*)_Src);
		break;

	case MAT_0:
	case MAT_1:
	case MAT_2:
	case MAT_3:
		m_Const.arrMat[_Param - MAT_0] = *((Matrix*)_Src);
		break;	
	}
}

void CMaterial::SetTexParam(TEX_PARAM _Param, const Ptr<CTexture>& _Tex)
{
	m_arrTex[_Param] = _Tex;
}

void CMaterial::GetScalarParam(SCALAR_PARAM _param, void* _pData)
{
	switch (_param)
	{
	case INT_0:
	case INT_1:
	case INT_2:
	case INT_3:
	case INT_4:
	case INT_5:
	case INT_6:
	case INT_7:
	{
		int idx = (UINT)_param - (UINT)INT_0;
		*((int*)_pData) = m_Const.arrInt[idx];
	}
		break;
	case FLOAT_0:
	case FLOAT_1:
	case FLOAT_2:
	case FLOAT_3:
	case FLOAT_4:
	case FLOAT_5:
	case FLOAT_6:
	case FLOAT_7:
	{
		int idx = (UINT)_param - (UINT)FLOAT_0;
		*((float*)_pData) = m_Const.arrFloat[idx];
	}
		break;

	case VEC2_0:
	case VEC2_1:
	case VEC2_2:
	case VEC2_3:
	{
		int idx = (UINT)_param - (UINT)VEC2_0;
		*((Vec2*)_pData) = m_Const.arrV2[idx];
	}
		break;

	//case VEC3_0:
	//case VEC3_1:
	//case VEC3_2:
	//case VEC3_3:
	//{
	//	int idx = (UINT)_param - (UINT)VEC3_0;
	//	*((Vec3*)_pData) = m_Const.arrV3[idx];
	//}
	//break;

	case VEC4_0:
	case VEC4_1:
	case VEC4_2:
	case VEC4_3:
	{
		int idx = (UINT)_param - (UINT)VEC4_0;
		*((Vec4*)_pData) = m_Const.arrV4[idx];
	}
		break;

	case MAT_0:
	case MAT_1:
	case MAT_2:
	case MAT_3:
	{
		int idx = (UINT)_param - (UINT)MAT_0;
		*((Matrix*)_pData) = m_Const.arrMat[idx];
	}
		break;
	}
}

Ptr<CTexture> CMaterial::GetTexParam(TEX_PARAM _param)
{
	if (nullptr != m_arrTex[(UINT)_param])
		return m_arrTex[(UINT)_param];

	return nullptr;
}

// ================
// File Save / Load
// ================
int CMaterial::Save(const wstring& _strRelativePath)
{
	if (IsEngineRes())
		return E_FAIL;

	wstring strFilePath = CPathMgr::GetInst()->GetContentPath();
	strFilePath += _strRelativePath;
	
	FILE* pFile = nullptr;
	_wfopen_s(&pFile, strFilePath.c_str(), L"wb");

	// Entity
	SaveWString(GetName(), pFile);
	
	// Res
	SaveWString(GetKey(), pFile);	
	
	// Shader
	SaveResRef(m_pShader.Get(), pFile);

	// Constant
	fwrite(&m_Const, sizeof(tMtrlConst), 1, pFile);

	// Texture
	for (UINT i = 0; i < (UINT)TEX_PARAM::TEX_END; ++i)
	{
		SaveResRef(m_arrTex[i].Get(), pFile);
	}
	
	fclose(pFile);

	return S_OK;
}


int CMaterial::Load(const wstring& _strFilePath)
{
	FILE* pFile = nullptr;
	_wfopen_s(&pFile, _strFilePath.c_str(), L"rb");

	// Entity
	wstring strName;
	LoadWString(strName, pFile);
	SetName(strName);

	// Res
	wstring strKey;
	LoadWString(strKey, pFile);
	
	// Shader
	LoadResRef(m_pShader, pFile);

	// Constant
	fread(&m_Const, sizeof(tMtrlConst), 1, pFile);

	// Texture
	for (UINT i = 0; i < (UINT)TEX_PARAM::TEX_END; ++i)
	{
		LoadResRef(m_arrTex[i], pFile);
	}

	fclose(pFile);

	return S_OK;
}