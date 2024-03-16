#include "pch.h"
#include "MenuUI.h"

#include "CLevelSaveLoad.h"
#include <Engine\CEventMgr.h>
#include <Engine\CGameObject.h>
#include <Engine\components.h>
#include <Engine\CScript.h>
#include <Engine\CLevelMgr.h>

#include <Script\CScriptMgr.h>

#include "ImGuiMgr.h"
#include "OutlinerUI.h"
#include "InspectorUI.h"
#include "CLevelSaveLoad.h"

static int currentShape = 0;
static int currentType = 0;
static string strStr = "";

MenuUI::MenuUI()
	: UI("##Menu")
{
	SetName("Menu");
}

MenuUI::~MenuUI()
{
}

void MenuUI::finaltick()
{
	if (!IsActive())
		return;

	render_update();
}

int MenuUI::render_update()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Save Level"))
			{
				CLevel* curLevel = CLevelMgr::GetInst()->GetCurLevel();
				assert(curLevel);
				CLevelSaveLoad::SaveLevel(L"Level\\" + curLevel->GetName() + L".lv", curLevel);
			}

			if (ImGui::BeginMenu("Load Level"))
			{
				char str[100] = {};
				memset(str, 0, sizeof(char) * 100);
				wstring levelName;
				ImGui::Text("Level Name : ");
				ImGui::SameLine();
				ImGui::InputText("##Load Level Name", str, 100);
				if (ImGui::IsKeyPressed(ImGuiKey_Enter))
				{
					string nStr = Strsubstr(str, ".lv");
					wstring wstr(nStr.begin(), nStr.end());
					CLevel* pLoadedLevel = CLevelSaveLoad::LoadLevel(L"Level\\" + wstr);
					pLoadedLevel->ChangeState(LEVEL_STATE::LOAD);

					tEvent evn = {};
					evn.Type = EVENT_TYPE::LEVEL_LOAD;
					evn.wParam = (DWORD_PTR)pLoadedLevel;

					CEventMgr::GetInst()->AddEvent(evn);
				}
				ImGui::EndMenu();
			}

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("GameObject"))
		{
			// 현재 레벨에 게임오브젝트 생성
			if (ImGui::BeginMenu("Create Empty Object"))
			{
				ImGui::BeginChild("##CreateEmptyObjectBtn", ImVec2(150, 60));
				CreateEmptyObject();
				ImGui::EndChild();
				ImGui::EndMenu();
			}
			ImGui::Separator();


			if (ImGui::BeginMenu("Add Component"))
			{
				if(ImGui::BeginMenu(ToString(COMPONENT_TYPE::COLLIDER3D)))
				{
					ImGui::BeginChild("##Collider3DMenu", ImVec2(150, 60));
					AddCollider();
					ImGui::EndChild();
					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu(ToString(COMPONENT_TYPE::RIGIDBODY)))
				{
					ImGui::BeginChild("##RigidBodyMenu", ImVec2(200, 80));
					AddRigidBody();
					ImGui::EndChild();
					ImGui::EndMenu();
				}
				for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; ++i)
				{
					if (ImGui::MenuItem(ToString((COMPONENT_TYPE)i)))
					{
						if(COMPONENT_TYPE::COLLIDER3D == (COMPONENT_TYPE)i
							|| COMPONENT_TYPE::RIGIDBODY == (COMPONENT_TYPE)i)
						{
							continue;
						}
						AddComponent(COMPONENT_TYPE(i));
					}
				}
				ImGui::EndMenu();
			}

			ImGui::Separator();

			if (ImGui::BeginMenu("Add Script"))
			{
				vector<wstring> vecScripts;
				CScriptMgr::GetScriptInfo(vecScripts);

				for (size_t i = 0; i < vecScripts.size(); ++i)
				{
					string strScriptName = string(vecScripts[i].begin(), vecScripts[i].end());
					if (ImGui::MenuItem(strScriptName.c_str()))
					{
						AddScript(vecScripts[i]);
					}
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Level"))
		{
			CLevel* CurLevel = CLevelMgr::GetInst()->GetCurLevel();
			bool PlayEnable = true;
			bool PauseEnable = true;
			bool StopEnable = true;

			if (CurLevel->GetState() == LEVEL_STATE::PLAY)
			{
				PlayEnable = false;
				PauseEnable = true;
				StopEnable = true;
			}
			else if (CurLevel->GetState() == LEVEL_STATE::PAUSE)
			{
				PlayEnable = true;
				PauseEnable = false;
				StopEnable = true;
			}
			else if (CurLevel->GetState() == LEVEL_STATE::STOP)
			{
				PlayEnable = true;
				PauseEnable = false;
				StopEnable = false;
			}



			if (ImGui::MenuItem("Play", nullptr, nullptr, PlayEnable))
			{
				CLevelSaveLoad::SaveLevel(L"Level\\Temp.lv", CurLevel);
				CurLevel->ChangeState(LEVEL_STATE::PLAY);
			}
			else if (ImGui::MenuItem("Pause", nullptr, nullptr, PauseEnable))
			{
				CurLevel->ChangeState(LEVEL_STATE::PAUSE);
			}
			else if (ImGui::MenuItem("Stop", nullptr, nullptr, StopEnable))
			{
				CurLevel->ChangeState(LEVEL_STATE::STOP);
				CLevel* pNewLevel = CLevelSaveLoad::LoadLevel(L"Level\\Temp.lv");

				tEvent evn = {};
				//evn.Type = EVENT_TYPE::LEVEL_CHANGE;
				evn.Type = EVENT_TYPE::LEVEL_RESET;
				evn.wParam = DWORD_PTR(pNewLevel);
				CEventMgr::GetInst()->AddEvent(evn);

				// InspectorUI
				InspectorUI* Inspector = (InspectorUI*)ImGuiMgr::GetInst()->FindUI("##Inspector");
				Inspector->SetTargetObject(nullptr);
			}

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Resource"))
		{
			if (ImGui::MenuItem("Create Empty Material"))
			{
				CreateEmptyMaterial();
			}

			ImGui::EndMenu();
		}



		ImGui::EndMainMenuBar();
	}

	return 0;
}

void MenuUI::CreateEmptyObject()
{
	ImGui::Text("Object Name : ");
	ImGui::InputText("##nameObject", &strStr);
	

	if (strStr.length() != 0 )
	{
		int a = 0;
	}
	CLevel* curLevel = CLevelMgr::GetInst()->GetCurLevel();
	
	for (int i = 0; i < MAX_LAYER; ++i)
	{
		CLayer* layer = curLevel->GetLayer(i);
		const wstring layerName = layer->GetName();

		string layerNameStr = string(layerName.begin(), layerName.end());

		if (layerName == L"") continue;

		if(ImGui::Button(layerNameStr.c_str()))
		{
			wstring strWStr(strStr.begin(), strStr.end());
			
			CGameObject* pNewObject = new CGameObject;
			pNewObject->AddComponent(new CTransform);
			
			pNewObject->SetName(strWStr);
			SpawnGameObject(pNewObject, Vec3(0.f, 0.f, 0.f), i);

			OutlinerUI* outliner = (OutlinerUI*)ImGuiMgr::GetInst()->FindUI("##Outliner");

			outliner->SetSelectedNodeData(DWORD_PTR(pNewObject));
		}
	}

	
}

void MenuUI::CreateEmptyMaterial()
{
	Ptr<CMaterial> pNewMtrl = new CMaterial;
	CResMgr::GetInst()->AddRes<CMaterial>(L"material\\EmptyMtrl.mtrl", pNewMtrl);
	pNewMtrl->Save(pNewMtrl->GetKey());
}

void MenuUI::AddComponent(COMPONENT_TYPE _type)
{
	OutlinerUI* outliner = (OutlinerUI*)ImGuiMgr::GetInst()->FindUI("##Outliner");
	InspectorUI* inspector = (InspectorUI*)ImGuiMgr::GetInst()->FindUI("##Inspector");

	CGameObject* pSelectedObject = outliner->GetSelectedObject();

	if (nullptr == pSelectedObject)
		return;

	switch (_type)
	{
	case COMPONENT_TYPE::TRANSFORM:
		pSelectedObject->AddComponent(new CTransform);
		break;
	case COMPONENT_TYPE::COLLIDER3D:
		AddCollider();
		break;
	case COMPONENT_TYPE::RIGIDBODY:
		AddRigidBody();
		break;
	case COMPONENT_TYPE::ANIMATOR3D:
		pSelectedObject->AddComponent(new CAnimator3D);
		break;
	case COMPONENT_TYPE::ANIMATOR2D:
		pSelectedObject->AddComponent(new CAnimator2D);
		break;
	case COMPONENT_TYPE::LIGHT3D:
		pSelectedObject->AddComponent(new CLight3D);
		break;
	case COMPONENT_TYPE::GIZMO:
		pSelectedObject->AddComponent(new CGizmo);
		break;
	case COMPONENT_TYPE::CAMERA:
		pSelectedObject->AddComponent(new CCamera);
		break;

	case COMPONENT_TYPE::MESHRENDER:
		pSelectedObject->AddComponent(new CMeshRender);
		break;
	case COMPONENT_TYPE::PARTICLESYSTEM:
		pSelectedObject->AddComponent(new CParticleSystem);
		break;
	case COMPONENT_TYPE::SKYBOX:
		pSelectedObject->AddComponent(new CSkyBox);
		break;
	case COMPONENT_TYPE::LANDSCAPE:
		pSelectedObject->AddComponent(new CLandScape);
		break;
	case COMPONENT_TYPE::DECAL:
		pSelectedObject->AddComponent(new CDecal);
		break;
	}

	inspector->SetTargetObject(pSelectedObject);
}

void MenuUI::AddScript(const wstring& _strScriptName)
{
	OutlinerUI* outliner = (OutlinerUI*)ImGuiMgr::GetInst()->FindUI("##Outliner");
	InspectorUI* inspector = (InspectorUI*)ImGuiMgr::GetInst()->FindUI("##Inspector");

	CGameObject* pSelectedObject = outliner->GetSelectedObject();

	if (nullptr == pSelectedObject)
		return;

	CScript* pScript = CScriptMgr::GetScript(_strScriptName);

	pSelectedObject->AddComponent(pScript);

	inspector->SetTargetObject(pSelectedObject);
}

void MenuUI::AddCollider()
{
	OutlinerUI* outliner = (OutlinerUI*)ImGuiMgr::GetInst()->FindUI("##Outliner");
	InspectorUI* inspector = (InspectorUI*)ImGuiMgr::GetInst()->FindUI("##Inspector");
	CGameObject* pSelectedObject = outliner->GetSelectedObject();

	ImGui::Text("Is Attach RigidBody?");
	if(ImGui::Button("Yes##yesattach"))
	{
		pSelectedObject->AddComponent(new CCollider3D(true));
		inspector->SetTargetObject(pSelectedObject);
	}
	ImGui::SameLine();
	if (ImGui::Button("No##noattach"))
	{
		pSelectedObject->AddComponent(new CCollider3D(false));
		inspector->SetTargetObject(pSelectedObject);
	}
}

void MenuUI::AddRigidBody()
{
	OutlinerUI* outliner = (OutlinerUI*)ImGuiMgr::GetInst()->FindUI("##Outliner");
	InspectorUI* inspector = (InspectorUI*)ImGuiMgr::GetInst()->FindUI("##Inspector");
	CGameObject* pSelectedObject = outliner->GetSelectedObject();

	RIGID_BODY_TYPE tType = (RIGID_BODY_TYPE)currentType;
	RIGID_BODY_SHAPE_TYPE tShape = (RIGID_BODY_SHAPE_TYPE)currentShape;

	ImGui::Text("Rigid Body Type : %s", RIGID_BODY_STR[currentType]);
	for (UINT i = 0; i < (UINT)RIGID_BODY_TYPE::END; ++i)
	{
		if(ImGui::Button(RIGID_BODY_STR[i]))
		{
			currentType = i;
			tType = (RIGID_BODY_TYPE)currentType;
		}
		if (i % 2 == 0)
		{
			ImGui::SameLine();
		}
	}
	ImGui::Text("Rigid Body Shape Type : %s", RIGID_BODY_SHAPE_STR[currentShape]);
	for (UINT i = 0; i < (UINT)RIGID_BODY_SHAPE_TYPE::END; ++i)
	{
		
		if (ImGui::Button(RIGID_BODY_SHAPE_STR[i]))
		{
			currentShape = i;
			tShape = (RIGID_BODY_SHAPE_TYPE)currentShape;
		}
		if (i % 2 == 0)
		{
			ImGui::SameLine();
		}
	}

	if(ImGui::Button("Make##MakeRigidBodyBtn"))
	{
		pSelectedObject->AddComponent(new CRigidBody(tShape, tType));
		inspector->SetTargetObject(pSelectedObject);
	}
}
