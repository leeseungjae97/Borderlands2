#include "pch.h"
#include "CLevelSaveLoad.h"

#include <Engine\CPathMgr.h>
#include <Engine\CLevelMgr.h>
#include <Engine\CLevel.h>
#include <Engine\CLayer.h>
#include <Engine\CGameObject.h>
#include <Engine\components.h>
#include <Engine\CScript.h>

#include <Script\CScriptMgr.h>



int CLevelSaveLoad::SaveLevel(const wstring& _LevelPath, CLevel* _Level)
{
	if (_Level->GetState() != LEVEL_STATE::STOP)
		return E_FAIL;

	wstring strPath = CPathMgr::GetInst()->GetContentPath();
	strPath += _LevelPath;

	FILE* pFile = nullptr;

	_wfopen_s(&pFile, strPath.c_str(), L"wb");

	if (nullptr == pFile)	
		return E_FAIL;

	// 레벨 이름 저장
	SaveWString(_Level->GetName(), pFile);


	// 레벨의 레이어들을 저장
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		CLayer* pLayer = _Level->GetLayer(i);

		// 레이어 이름 저장
		SaveWString(pLayer->GetName(), pFile);

		// 레이어의 게임오브젝트들 저장
		const vector<CGameObject*>& vecParent = pLayer->GetParentObject();

		// 오브젝트 개수 저장
		size_t objCount = vecParent.size();
		fwrite(&objCount, sizeof(size_t), 1, pFile);
		
		// 각 게임오브젝트
		for (size_t i = 0; i < objCount; ++i)
		{
			SaveGameObject(vecParent[i], pFile, true);
		}
	}

	fclose(pFile);


	return S_OK;
}

int CLevelSaveLoad::SaveGameObject(CGameObject* _Object, FILE* _File, bool _Root)
{
	bool bOwned = _Object->IsOwned();
	fwrite(&bOwned, sizeof(bool), 1, _File);

	if (bOwned && _Root)
		return FALSE;

	// 이름
	SaveWString(_Object->GetName(), _File);
	bool bItem = _Object->IsItem();
	bool bEqui = _Object->IsEqui();
	bool bWarrior = _Object->IsWarrior();
	
	CGameObject::OBJECT_STATE state = _Object->GetObjectState();

	fwrite(&bItem, sizeof(bool), 1, _File);
	fwrite(&bEqui, sizeof(bool), 1, _File);
	fwrite(&bWarrior, sizeof(bool), 1, _File);
	fwrite(&state, sizeof(UINT), 1, _File);

	// 컴포넌트
	for (UINT i = 0; i <= (UINT)COMPONENT_TYPE::END; ++i)
	{		
		if (i == (UINT)COMPONENT_TYPE::END)
		{
			// 컴포넌트 타입 저장
			fwrite(&i, sizeof(UINT), 1, _File);
			break;
		}

		CComponent* Com = _Object->GetComponent((COMPONENT_TYPE)i);
		if (nullptr == Com)
			continue;

		// 컴포넌트 타입 저장
		fwrite(&i, sizeof(UINT), 1, _File);

		// 컴포넌트 정보 저장
		Com->SaveToLevelFile(_File);
	}

	// 스크립트	
	const vector<CScript*>& vecScript = _Object->GetScripts();
	size_t ScriptCount = vecScript.size();
	fwrite(&ScriptCount, sizeof(size_t), 1, _File);

	for (size_t i = 0; i < vecScript.size(); ++i)
	{
		wstring ScriptName = CScriptMgr::GetScriptName(vecScript[i]);
		SaveWString(ScriptName, _File);
		vecScript[i]->SaveToLevelFile(_File);
	}


	// 자식 오브젝트
	const vector<CGameObject*>& vecChild = _Object->GetChild();
	size_t ChildCount = vecChild.size();
	fwrite(&ChildCount, sizeof(size_t), 1, _File);

	for (size_t i = 0; i < ChildCount; ++i)
	{
		SaveGameObject(vecChild[i], _File, false);		
	}

	const vector<CGameObject*>& vecGuns = _Object->GetGuns();
	size_t GunCount = vecGuns.size();
	fwrite(&GunCount, sizeof(size_t), 1, _File);

	for (size_t i = 0; i < GunCount; ++i)
	{
		SaveGameObject(vecGuns[i], _File, false);
	}


	return TRUE;
}

CLevel* CLevelSaveLoad::LoadLevel(const wstring& _LevelPath)
{
	wstring strPath = CPathMgr::GetInst()->GetContentPath();
	strPath += _LevelPath;

	FILE* pFile = nullptr;

	_wfopen_s(&pFile, strPath.c_str(), L"rb");

	if (nullptr == pFile)
		return nullptr;

	CLevel* NewLevel = new CLevel;

	// 레벨 이름
	wstring strLevelName;
	LoadWString(strLevelName, pFile);
	NewLevel->SetName(strLevelName);


	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		CLayer* pLayer = NewLevel->GetLayer(i);

		// 레이어 이름
		wstring LayerName;
		LoadWString(LayerName, pFile);
		pLayer->SetName(LayerName);

		// 게임 오브젝트 개수
		size_t objCount = 0;
		fread(&objCount, sizeof(size_t), 1, pFile);

		// 각 게임오브젝트
		for (size_t j = 0; j < objCount; ++j)
		{
			CGameObject* pNewObj = LoadGameObject(pFile, NewLevel, i, true);
			if (nullptr == pNewObj) continue;

			NewLevel->AddGameObject(pNewObj, i, false);
		}
	}

	fclose(pFile);

	NewLevel->ChangeState(LEVEL_STATE::STOP);

	return NewLevel;
}

CGameObject* CLevelSaveLoad::LoadGameObject(FILE* _File, CLevel* _NewLevel, int _LayerIdx, bool _Root)
{
	bool bOwned = false;
	fread(&bOwned, sizeof(bool), 1, _File);

	if (bOwned && _Root)
		return FALSE;

	CGameObject* pObject = new CGameObject;

	// 이름
	wstring Name;
	LoadWString(Name, _File);
	bool bItem = false;
	bool bEqui = false;
	bool bWarrior = false;

	CGameObject::OBJECT_STATE state = CGameObject::OBJECT_STATE::VISIBLE;

	fread(&bItem, sizeof(bool), 1, _File);
	fread(&bEqui, sizeof(bool), 1, _File);
	fread(&bWarrior, sizeof(bool), 1, _File);
	fread(&state, sizeof(UINT), 1, _File);
	
	pObject->SetIsItem(bItem);
	pObject->SetIsEqui(bEqui);
	pObject->SetIsOwned(bOwned);
	pObject->SetIsWarrior(bWarrior);

	pObject->SetObjectState(state);

	pObject->SetName(Name);

	// 컴포넌트
	while (true)
	{
		UINT ComponentType = 0;
		fread(&ComponentType, sizeof(UINT), 1, _File);

		// 컴포넌트 정보의 끝을 확인
		if ((UINT)COMPONENT_TYPE::END == ComponentType)
			break;

		CComponent* Component = nullptr;

		switch ((COMPONENT_TYPE)ComponentType)
		{
		case COMPONENT_TYPE::TRANSFORM:
			Component = new CTransform;
			break;
		case COMPONENT_TYPE::ANIMATOR3D:
			Component = new CAnimator3D;
			break;
		case COMPONENT_TYPE::ANIMATOR2D:
			Component = new CAnimator2D;
			break;
		case COMPONENT_TYPE::LIGHT3D:
			Component = new CLight3D;
			break;
		case COMPONENT_TYPE::RIGIDBODY:
			Component = new CRigidBody;
			break;
		case COMPONENT_TYPE::COLLIDER3D:
			Component = new CCollider3D;
			break;
		case COMPONENT_TYPE::GIZMO:
			Component = new CGizmo;
			break;
		case COMPONENT_TYPE::CAMERA:
			Component = new CCamera;
			break;
		case COMPONENT_TYPE::MESHRENDER:
			Component = new CMeshRender;
			break;
		case COMPONENT_TYPE::PATHFIND:
			Component = new CPathFind;
			break;
		case COMPONENT_TYPE::PARTICLESYSTEM:
			Component = new CParticleSystem;
			break;
		case COMPONENT_TYPE::LANDSCAPE:
			Component = new CLandScape;
			break;
		case COMPONENT_TYPE::DECAL:
			Component = new CDecal;
			break;
		case COMPONENT_TYPE::SKYBOX:
			Component = new CSkyBox;
			break;
		}

		Component->LoadFromLevelFile(_File);
		pObject->AddComponent(Component);
	}


	// 스크립트	
	size_t ScriptCount = 0;
	fread(&ScriptCount, sizeof(size_t), 1, _File);

	for (size_t i = 0; i < ScriptCount; ++i)
	{
		wstring ScriptName;
		LoadWString(ScriptName, _File);
		CScript* pScript = CScriptMgr::GetScript(ScriptName);
		pObject->AddComponent(pScript);
		pScript->LoadFromLevelFile(_File);
	}

	// 자식 오브젝트		
	size_t ChildCount = 0;
	fread(&ChildCount, sizeof(size_t), 1, _File);

	for (size_t i = 0; i < ChildCount; ++i)
	{
		CGameObject* ChildObject = LoadGameObject(_File, nullptr, 0, false);
		if (nullptr == ChildObject)
			continue;
		pObject->AddChild(ChildObject);
	}
	size_t GunCount = 0;
	fread(&GunCount, sizeof(size_t), 1, _File);

	for (size_t i = 0; i < GunCount; ++i)
	{
		CGameObject* GunObject = LoadGameObject(_File, nullptr, 0, false);
		if (nullptr == GunObject)
			continue;
		pObject->AddGun(GunObject);
		_NewLevel->AddGameObject(GunObject, _LayerIdx, false);
	}

	return pObject;
}
