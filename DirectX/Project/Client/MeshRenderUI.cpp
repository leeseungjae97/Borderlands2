#include "pch.h"
#include "MeshRenderUI.h"

#include <Engine\CMeshRender.h>
#include <Engine\CResMgr.h>

#include "ImGuiMgr.h"
#include "ListUI.h"

#include "TreeUI.h"

extern int iInst0;
extern int iInst1;
extern int iInst2;
extern int iInst3;

MeshRenderUI::MeshRenderUI()
	: ComponentUI("##MeshRender", COMPONENT_TYPE::MESHRENDER)	
{
	SetName("MeshRender");
}

MeshRenderUI::~MeshRenderUI()
{
}


int MeshRenderUI::render_update()
{
	if (FALSE == ComponentUI::render_update())
		return FALSE;

	char szBuff[50] = {};

	Ptr<CMesh> pMesh = GetTarget()->MeshRender()->GetMesh();
	Ptr<CMaterial> pMtrl = GetTarget()->MeshRender()->GetMaterial(0);
		
	ImGui::Text("Mesh    ");
	ImGui::SameLine();	
	GetResKey(pMesh.Get(), szBuff, 50);
	ImGui::InputText("##MeshName", szBuff, 50, ImGuiInputTextFlags_ReadOnly);

	// Mesh 드랍 체크
	if (ImGui::BeginDragDropTarget())
	{
		// 해당 노드에서 마우스 뗀 경우, 지정한 PayLoad 키값이 일치한 경우
		const ImGuiPayload* pPayLoad = ImGui::AcceptDragDropPayload("Resource");
		if (pPayLoad)
		{
			TreeNode* pNode = (TreeNode*)pPayLoad->Data;
			CRes* pRes = (CRes*)pNode->GetData();
			if (RES_TYPE::MESH == pRes->GetType())
			{
				GetTarget()->MeshRender()->SetMesh((CMesh*)pRes);
			}
		}

		ImGui::EndDragDropTarget();
	}


	ImGui::SameLine();
	if (ImGui::Button("##MeshSelectBtn", ImVec2(18, 18)))
	{
		const map<wstring, Ptr<CRes>>& mapMesh = CResMgr::GetInst()->GetResources(RES_TYPE::MESH);

		ListUI* pListUI = (ListUI*)ImGuiMgr::GetInst()->FindUI("##List");
		pListUI->Reset("Mesh List", ImVec2(300.f, 500.f));
		for (const auto& pair : mapMesh)
		{
			pListUI->AddItem(string(pair.first.begin(), pair.first.end()));
		}

		// 항목 선택시 호출받을 델리게이트 등록
		pListUI->AddDynamic_Select(this, (UI_DELEGATE_1)&MeshRenderUI::SelectMesh);
	}
		
	ImGui::Text("Material");
	ImGui::SameLine();
	GetResKey(pMtrl.Get(), szBuff, 50);
	ImGui::InputText("##MtrlName", szBuff, 50, ImGuiInputTextFlags_ReadOnly);

	if (ImGui::BeginDragDropTarget())
	{
		// 해당 노드에서 마우스 뗀 경우, 지정한 PayLoad 키값이 일치한 경우
		const ImGuiPayload* pPayLoad = ImGui::AcceptDragDropPayload("Resource");
		if (pPayLoad)
		{
			TreeNode* pNode = (TreeNode*)pPayLoad->Data;
			CRes* pRes = (CRes*)pNode->GetData();
			if (RES_TYPE::MATERIAL == pRes->GetType())
			{
				GetTarget()->MeshRender()->SetMaterial((CMaterial*)pRes, 0);
			}
		}

		ImGui::EndDragDropTarget();
	}


	ImGui::SameLine();

	if (ImGui::Button("##MtrlSelectBtn", ImVec2(18, 18)))
	{
		const map<wstring, Ptr<CRes>>& mapMtrl = CResMgr::GetInst()->GetResources(RES_TYPE::MATERIAL);

		ListUI* pListUI = (ListUI*)ImGuiMgr::GetInst()->FindUI("##List");
		pListUI->Reset("Material", ImVec2(300.f, 500.f));
		for (const auto& pair : mapMtrl)
		{
			pListUI->AddItem(string(pair.first.begin(), pair.first.end()));
		}

		// 항목 선택시 호출받을 델리게이트 등록
		pListUI->AddDynamic_Select(this, (UI_DELEGATE_1)&MeshRenderUI::SelectMaterial);
	}

	ImGui::Text("RenderComponent Material");
	for(int i = 0 ; i < GetTarget()->MeshRender()->GetMtrlCount(); ++i)
	{
		Ptr<CMaterial> mtrl = GetTarget()->MeshRender()->GetMaterial(i);
		if (nullptr == mtrl)
			continue;

		tMtrlConst mtrlConst =mtrl->GetConst();

		float fTexFlowSpeed = mtrlConst.fTexFlowSpeed;
		ImGui::Text("Tex Flow Speed : ");
		ImGui::SameLine();
		ImGui::InputFloat("##texflowspeed", &fTexFlowSpeed);
		mtrl->SetFlowSpeed(fTexFlowSpeed);

		Vec2 vTexFlowDir = mtrlConst.vTexDir;
		ImGui::Text("Tex Flow Dir : ");
		ImGui::SameLine();
		float dir[2];
		dir[0] = vTexFlowDir.x;
		dir[1] = vTexFlowDir.y;
		ImGui::InputFloat2("##texflowdir", dir);
		vTexFlowDir.x = dir[0];
		vTexFlowDir.y = dir[1];
		mtrl->SetFlowDir(vTexFlowDir);

		float fEmisCoef = mtrlConst.fEmisCoeff;
		ImGui::Text("Emissive Coeff : ");
		ImGui::SameLine();
		ImGui::InputFloat("##emiscoeff", &fEmisCoef);
		mtrl->SetEmissiveCoeff(fEmisCoef);

		for(int j = 0 ; j < TEX_PARAM::TEX_7; ++j)
		{
			Ptr<CTexture> tex = mtrl->GetTexParam((TEX_PARAM)j);
			if(nullptr != tex)
			{
				ImGui::Text("%d Material, %d Texture", i, j);
				string str = "SET##" + std::to_string(i) + "mtrl" + std::to_string(j) + "tex";
				if(ImGui::Button(str.c_str()))
				{
					const map<wstring, Ptr<CRes>>& pTexture = CResMgr::GetInst()->GetResources(RES_TYPE::TEXTURE);

					ListUI* pListUI = (ListUI*)ImGuiMgr::GetInst()->FindUI("##List");
					pListUI->Reset("Texture List", ImVec2(300.f, 500.f));
					for (const auto& pair : pTexture)
					{
						pListUI->AddItem(string(pair.first.begin(), pair.first.end()));
					}
					iInst1 = i;
					iInst0 = j;
					pListUI->AddDynamic_Select(this, (UI_DELEGATE_2)&MeshRenderUI::SelectTexture);
					//mtrl->SetTexParam((TEX_PARAM)j, );
				}
				ImVec2 uv_min = ImVec2(0.0f, 0.0f);						// Top-left
				ImVec2 uv_max = ImVec2(1.0f, 1.0f);						// Lower-right
				ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
				ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.5f); // 50% opaque white

				ImGui::Image((ImTextureID)tex->GetSRV().Get(), ImVec2(150, 150), uv_min, uv_max, tint_col, border_col);

				str = "DELETE##" + std::to_string(i) + "mtrl" + std::to_string(j) + "tex";
				if (ImGui::Button(str.c_str()))
				{
					mtrl->SetTexParam((TEX_PARAM)j, nullptr);
				}
				if(mtrl->GetShader()->GetDomain() == SHADER_DOMAIN::DOMAIN_DEFERRED)
				{
					str = "FORWARD##" + std::to_string(i) + "mtrl" + std::to_string(j) + "tex";
					if (ImGui::Button(str.c_str()))
					{
						mtrl->SetShader(CResMgr::GetInst()->FindRes<CGraphicsShader>(L"Std3DShader"));
					}
				}else
				{
					str = "DEFERRED##" + std::to_string(i) + "mtrl" + std::to_string(j) + "tex";
					if (ImGui::Button(str.c_str()))
					{
						mtrl->SetShader(CResMgr::GetInst()->FindRes<CGraphicsShader>(L"Std3D_DeferredShader"));
					}
				}
				bool bFlow = mtrlConst.arrTexFlow[j];
				if(bFlow)
				{
					str = "STOP##" + std::to_string(i) + "mtrl" + std::to_string(j) + "tex";
					if (ImGui::Button(str.c_str()))
					{
						bFlow = !bFlow;
					}
				}else
				{
					str = "FLOW##" + std::to_string(i) + "mtrl" + std::to_string(j) + "tex";
					if (ImGui::Button(str.c_str()))
					{
						bFlow = !bFlow;
					}
				}
				mtrl->SetFlow(j, bFlow);
				
			}else
			{
				ImGui::Text("%d Material, %d Texture", i, j);
				
				string str = "SET##" + std::to_string(i) + "mtrl" + std::to_string(j) + "tex";
				if (ImGui::Button(str.c_str()))
				{
					const map<wstring, Ptr<CRes>>& pTexture = CResMgr::GetInst()->GetResources(RES_TYPE::TEXTURE);

					ListUI* pListUI = (ListUI*)ImGuiMgr::GetInst()->FindUI("##List");
					pListUI->Reset("Texture List", ImVec2(300.f, 500.f));
					for (const auto& pair : pTexture)
					{
						pListUI->AddItem(string(pair.first.begin(), pair.first.end()));
					}
					iInst1 = i;
					iInst0 = j;
					pListUI->AddDynamic_Select(this, (UI_DELEGATE_2)&MeshRenderUI::SelectTexture);
					//mtrl->SetTexParam((TEX_PARAM)j, );
				}
			}
		}
	}

	return TRUE;
}
void MeshRenderUI::SelectMesh(DWORD_PTR _Key)
{
	string strKey = (char*)_Key;
	Ptr<CMesh> pMesh = CResMgr::GetInst()->FindRes<CMesh>(wstring(strKey.begin(), strKey.end()));
	GetTarget()->MeshRender()->SetMesh(pMesh);
}

void MeshRenderUI::SelectMaterial(DWORD_PTR _Key)
{
	string strKey = (char*)_Key;
	Ptr<CMaterial> pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(wstring(strKey.begin(), strKey.end()));
	GetTarget()->MeshRender()->SetMaterial(pMtrl, 0);
}

void MeshRenderUI::SelectTexture(DWORD_PTR _Key, DWORD_PTR _Key2)
{
	string strKey = (char*)_Key;
	int texParamIdx = (int)_Key2;
	Ptr<CTexture> pTex= CResMgr::GetInst()->FindRes<CTexture>(wstring(strKey.begin(), strKey.end()));
	GetTarget()->MeshRender()->GetMaterial(iInst1)->SetTexParam((TEX_PARAM)texParamIdx, pTex);
}
