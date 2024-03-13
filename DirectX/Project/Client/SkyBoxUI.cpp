#include "pch.h"
#include "SkyBoxUI.h"

#include <Engine/CResMgr.h>
#include <Engine/CSkyBox.h>

#include "ListUI.h"
#include "TreeUI.h"

extern int iInst0;
extern int iInst1;
extern int iInst2;
extern int iInst3;

SkyBoxUI::SkyBoxUI()
	: ComponentUI("##SkyBoxUI", COMPONENT_TYPE::SKYBOX)
{
	SetName("SkyBoxUI");
}

SkyBoxUI::~SkyBoxUI()
{
}

int SkyBoxUI::render_update()
{
	if (FALSE == ComponentUI::render_update())
		return FALSE;

	char szBuff[500] = {};

	Ptr<CMaterial> mtrl = GetTarget()->SkyBox()->GetMaterial(0);
	Ptr<CMesh> mesh= GetTarget()->SkyBox()->GetMesh();
	Ptr<CTexture> tex = GetTarget()->SkyBox()->GetSkyBoxTex();

	ImGui::Text("Mesh	");
	ImGui::SameLine();
	GetResKey(mesh.Get(), szBuff, 50);
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
				GetTarget()->SkyBox()->SetMesh((CMesh*)pRes);
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
		pListUI->AddDynamic_Select(this, (UI_DELEGATE_1)&SkyBoxUI::SelectMesh);
	}
	if(nullptr != mtrl)
	{
		ImGui::Text("Material	");
		ImGui::SameLine();
		GetResKey(mtrl.Get(), szBuff, 50);
		ImGui::InputText("##MaterialName", szBuff, 50, ImGuiInputTextFlags_ReadOnly);

		tMtrlConst mtrlConst = mtrl->GetConst();
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

		ImGui::Text("Set Material Shader");
		ImGui::SameLine();

		Ptr<CGraphicsShader> pShader = mtrl->GetShader();
		if (nullptr != pShader)
		{
			string strKey = string(pShader->GetKey().begin(), pShader->GetKey().end());
			ImGui::InputText("##ShaderUIName", (char*)strKey.c_str(), strKey.length(), ImGuiInputTextFlags_::ImGuiInputTextFlags_ReadOnly);
		}
		if (ImGui::Button("##ShaderSelectBtn", ImVec2(18, 18)))
		{
			const map<wstring, Ptr<CRes>>& mapShader = CResMgr::GetInst()->GetResources(RES_TYPE::GRAPHICS_SHADER);

			ListUI* pListUI = (ListUI*)ImGuiMgr::GetInst()->FindUI("##List");
			pListUI->Reset("GraphicsShader List", ImVec2(300.f, 500.f));
			for (const auto& pair : mapShader)
			{
				pListUI->AddItem(string(pair.first.begin(), pair.first.end()));
			}
			iInst1 = 0;
			// 항목 선택시 호출받을 델리게이트 등록
			pListUI->AddDynamic_Select(this, (UI_DELEGATE_1)&SkyBoxUI::SelectShader);
		}
		bool bTexFlow = false;
		for (int j = 0; j < TEX_PARAM::TEX_7 + 1; ++j)
		{

			Ptr<CTexture> tex = mtrl->GetTexParam((TEX_PARAM)j);
			string str = "";
			if (nullptr != tex)
			{
				ImGui::Text("%d Texture", j);
				str = "SET##mtrl" + std::to_string(j) + "texSet";
				if (ImGui::Button(str.c_str()))
				{
					const map<wstring, Ptr<CRes>>& pTexture = CResMgr::GetInst()->GetResources(RES_TYPE::TEXTURE);

					ListUI* pListUI = (ListUI*)ImGuiMgr::GetInst()->FindUI("##List");
					pListUI->Reset("Texture List", ImVec2(300.f, 500.f));
					for (const auto& pair : pTexture)
					{
						pListUI->AddItem(string(pair.first.begin(), pair.first.end()));
					}
					iInst1 = 0;
					iInst0 = j;
					pListUI->AddDynamic_Select(this, (UI_DELEGATE_1)&SkyBoxUI::SelectTexture);
					//mtrl->SetTexParam((TEX_PARAM)j, );
				}
				ImVec2 uv_min = ImVec2(0.0f, 0.0f);						// Top-left
				ImVec2 uv_max = ImVec2(1.0f, 1.0f);						// Lower-right
				ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
				ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.5f); // 50% opaque white

				ImGui::Image((ImTextureID)tex->GetSRV().Get(), ImVec2(150, 150), uv_min, uv_max, tint_col, border_col);

				str = "DELETE##mtrl" + std::to_string(j) + "texDelete";
				if (ImGui::Button(str.c_str()))
				{
					mtrl->SetTexParam((TEX_PARAM)j, nullptr);
				}
				bool bFlow = mtrlConst.arrTexFlow[j];
				if (bFlow)
				{
					bTexFlow = true;
					str = "STOP##mtrl" + std::to_string(j) + "texStop";
					if (ImGui::Button(str.c_str()))
					{
						bFlow = false;
					}
				}
				else
				{
					str = "FLOW##mtrl" + std::to_string(j) + "texFlow";
					if (ImGui::Button(str.c_str()))
					{
						bFlow = true;
					}
				}
				mtrl->SetFlow(j, bFlow);

			}
			else
			{
				ImGui::Text("%d Texture", j);

				string str = "SET##mtrl" + std::to_string(j) + "texSet_nullptr";
				if (ImGui::Button(str.c_str()))
				{
					const map<wstring, Ptr<CRes>>& pTexture = CResMgr::GetInst()->GetResources(RES_TYPE::TEXTURE);

					ListUI* pListUI = (ListUI*)ImGuiMgr::GetInst()->FindUI("##List");
					pListUI->Reset("Texture List", ImVec2(300.f, 500.f));
					for (const auto& pair : pTexture)
					{
						pListUI->AddItem(string(pair.first.begin(), pair.first.end()));
					}
					iInst1 = 0;
					iInst0 = j;
					pListUI->AddDynamic_Select(this, (UI_DELEGATE_1)&SkyBoxUI::SelectTexture);
				}
			}
		}
		mtrl->SetTexFlow(bTexFlow);
	}

	return TRUE;
}

void SkyBoxUI::SelectMesh(DWORD_PTR _Key)
{
	string strKey = (char*)_Key;
	Ptr<CMesh> pMesh = CResMgr::GetInst()->FindRes<CMesh>(wstring(strKey.begin(), strKey.end()));
	GetTarget()->SkyBox()->SetMesh(pMesh);
}

void SkyBoxUI::SelectMaterial(DWORD_PTR _Key)
{
	string strKey = (char*)_Key;
	int materialIdx = iInst0;
	Ptr<CMaterial> pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(wstring(strKey.begin(), strKey.end()));
	GetTarget()->SkyBox()->SetMaterial(pMtrl, materialIdx);
}

void SkyBoxUI::SelectTexture(DWORD_PTR _Key)
{
	string strKey = (char*)_Key;
	int texParamIdx = iInst0;
	Ptr<CTexture> pTex = CResMgr::GetInst()->FindRes<CTexture>(wstring(strKey.begin(), strKey.end()));
	GetTarget()->SkyBox()->GetMaterial(iInst1)->SetTexParam((TEX_PARAM)texParamIdx, pTex);
}

void SkyBoxUI::SelectShader(DWORD_PTR _Key)
{
	string strKey = (char*)_Key;
	Ptr<CGraphicsShader> pShader = CResMgr::GetInst()->FindRes<CGraphicsShader>(wstring(strKey.begin(), strKey.end()));
	GetTarget()->SkyBox()->GetMaterial(iInst1)->SetShader(pShader);
}
