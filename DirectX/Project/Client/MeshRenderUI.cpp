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

	char szBuff[500] = {};

	Ptr<CMesh> pMesh = GetTarget()->MeshRender()->GetMesh();
	CMeshRender* pMeshRender =GetTarget()->MeshRender();

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
				pMeshRender->SetMesh((CMesh*)pRes);
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

	if(nullptr != pMeshRender->GetMesh())
	{
		for (int i = 0; i < pMeshRender->GetMtrlCount(); ++i)
		{
			Ptr<CMaterial> pMtrl = pMeshRender->GetMaterial(i);

			ImGui::Text("Material");
			ImGui::SameLine();
			GetResKey(pMtrl.Get(), szBuff, 50);
			ImGui::InputText("##MtrlName", szBuff, 50, ImGuiInputTextFlags_ReadOnly);

			if (ImGui::BeginDragDropTarget())
			{
				const ImGuiPayload* pPayLoad = ImGui::AcceptDragDropPayload("Resource");
				if (pPayLoad)
				{
					TreeNode* pNode = (TreeNode*)pPayLoad->Data;
					CRes* pRes = (CRes*)pNode->GetData();
					if (RES_TYPE::MATERIAL == pRes->GetType())
					{
						pMeshRender->SetMaterial((CMaterial*)pRes, i);
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

				iInst0 = i;
				pListUI->AddDynamic_Select(this, (UI_DELEGATE_1)&MeshRenderUI::SelectMaterial);
			}
		}

	}

	if(ImGui::Button("Paper Burn##paperbutn"))
	{
		for (int i = 0; i < pMeshRender->GetMtrlCount(); ++i)
		{
			Ptr<CMaterial> mtrl = pMeshRender->GetDynamicMaterial(i);
			mtrl->SetPaperBurn(true);
			mtrl->SetPaperBurnTime(3.0f);
		}
	}

	ImGui::Text("RenderComponent Material");
	for(int i = 0 ; i < pMeshRender->GetMtrlCount(); ++i)
	{
		Ptr<CMaterial> mtrl = pMeshRender->GetMaterial(i);
		if (nullptr == mtrl)
			continue;
		string str = "";
		tMtrlConst mtrlConst = mtrl->GetConst();

		bool bTexAnim= mtrlConst.arrAnimData[2];
		ImGui::Text("Texture flow ");
		ImGui::SameLine();
		str = bTexAnim ? "TRUE" : "FALSE";
		str += "##TexAnimFlowBtn";
		if(ImGui::Button(str.c_str()))
		{
			bTexAnim = !bTexAnim;
		}
		mtrl->SetTexFlow(bTexAnim);

		float fTexFlowSpeed = mtrlConst.fTexFlowSpeed;
		ImGui::Text("Tex Flow Speed : ");
		ImGui::SameLine();
		str = "##texflowspeed" + std::to_string(i);
		ImGui::InputFloat(str.c_str(), &fTexFlowSpeed);
		mtrl->SetFlowSpeed(fTexFlowSpeed);

		int perlinNoise = mtrlConst.arrInt[1];
		ImGui::Text("Tex perlin : ");
		ImGui::SameLine();
		str = "##perlin" + std::to_string(i);
		ImGui::InputInt(str.c_str(), &perlinNoise);
		mtrl->SetPerlinNoise(perlinNoise);


		int bilboard = mtrlConst.arrInt[2];
		ImGui::Text("Tex bilboard : ");
		ImGui::SameLine();
		str = "##bilboard" + std::to_string(i);
		ImGui::InputInt(str.c_str(), &bilboard);
		mtrl->SetBilboard(bilboard);
		
		Vec2 vTexFlowDir = mtrlConst.vTexDir;
		ImGui::Text("Tex Flow Dir : ");
		ImGui::SameLine();
		float dir[2];
		dir[0] = vTexFlowDir.x;
		dir[1] = vTexFlowDir.y;
		str = "##texflowdir" + std::to_string(i);
		ImGui::InputFloat2(str.c_str(), dir);
		vTexFlowDir.x = dir[0];
		vTexFlowDir.y = dir[1];
		mtrl->SetFlowDir(vTexFlowDir);

		float fEmisCoef = mtrlConst.fEmisCoeff;
		ImGui::Text("Emissive Coeff : ");
		ImGui::SameLine();
		str = "##emiscoeff" + std::to_string(i);
		ImGui::InputFloat(str.c_str(), &fEmisCoef);
		mtrl->SetEmissiveCoeff(fEmisCoef);

		ImGui::Text("Set Material Shader");
		ImGui::SameLine();

		Ptr<CGraphicsShader> pShader = mtrl->GetShader();
		str = "##ShaderUIName" + std::to_string(i);
		if (nullptr != pShader)
		{
			string strKey = string(pShader->GetKey().begin(), pShader->GetKey().end());
			ImGui::InputText(str.c_str(), (char*)strKey.c_str(), strKey.length(), ImGuiInputTextFlags_::ImGuiInputTextFlags_ReadOnly);
		}
		else
		{
			char szEmtpy[10] = {};
			ImGui::InputText(str.c_str(), szEmtpy, 10, ImGuiInputTextFlags_::ImGuiInputTextFlags_ReadOnly);
		}
		str = "##ShaderSelectBtn" + std::to_string(i);
		if (ImGui::Button(str.c_str(), ImVec2(18, 18)))
		{
			const map<wstring, Ptr<CRes>>& mapShader = CResMgr::GetInst()->GetResources(RES_TYPE::GRAPHICS_SHADER);

			ListUI* pListUI = (ListUI*)ImGuiMgr::GetInst()->FindUI("##List");
			pListUI->Reset("GraphicsShader List", ImVec2(300.f, 500.f));
			for (const auto& pair : mapShader)
			{
				pListUI->AddItem(string(pair.first.begin(), pair.first.end()));
			}
			iInst1 = i;
			// 항목 선택시 호출받을 델리게이트 등록
			pListUI->AddDynamic_Select(this, (UI_DELEGATE_1)&MeshRenderUI::SelectShader);
		}

		//bool bTexFlow = false;
		for(int j = 0 ; j < TEX_PARAM::TEX_7 + 1; ++j)
		{
			
			Ptr<CTexture> tex = mtrl->GetTexParam((TEX_PARAM)j);
			if(nullptr != tex)
			{
				ImGui::Text("%d Material, %d Texture", i, j);
				str = "SET##" + std::to_string(i) + "mtrl" + std::to_string(j) + "texSet";
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
					pListUI->AddDynamic_Select(this, (UI_DELEGATE_1)&MeshRenderUI::SelectTexture);
					//mtrl->SetTexParam((TEX_PARAM)j, );
				}
				ImVec2 uv_min = ImVec2(0.0f, 0.0f);						// Top-left
				ImVec2 uv_max = ImVec2(1.0f, 1.0f);						// Lower-right
				ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
				ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.5f); // 50% opaque white

				ImGui::Image((ImTextureID)tex->GetSRV().Get(), ImVec2(150, 150), uv_min, uv_max, tint_col, border_col);
				
				str = "DELETE##" + std::to_string(i) + "mtrl" + std::to_string(j) + "texDelete";
				if (ImGui::Button(str.c_str()))
				{
					mtrl->SetTexParam((TEX_PARAM)j, nullptr);
				}
				bool bFlow = mtrlConst.arrTexFlow[j];
				if(bFlow)
				{
					//bTexFlow = true;
					str = "STOP##" + std::to_string(i) + "mtrl" + std::to_string(j) + "texStop";
					if (ImGui::Button(str.c_str()))
					{
						bFlow = false;
					}
				}else
				{
					str = "FLOW##" + std::to_string(i) + "mtrl" + std::to_string(j) + "texFlow";
					if (ImGui::Button(str.c_str()))
					{
						bFlow = true;
					}
				}
				mtrl->SetFlow(j, bFlow);
				bool bEmis = mtrlConst.arrTexEmis[j];
				if (bEmis)
				{
					str = "ADDCOLOR##" + std::to_string(i) + "mtrl" + std::to_string(j) + "texEmis";
					if (ImGui::Button(str.c_str()))
					{
						bEmis = false;
					}
				}
				else
				{
					str = "ADDEMIS##" + std::to_string(i) + "mtrl" + std::to_string(j) + "texEmis";
					if (ImGui::Button(str.c_str()))
					{
						bEmis = true;
					}
				}
				mtrl->SetEmis(j, bEmis);
			}else
			{
				ImGui::Text("%d Material, %d Texture", i, j);
				
				string str = "SET##" + std::to_string(i) + "mtrl" + std::to_string(j) + "texSet_nullptr";
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
					pListUI->AddDynamic_Select(this, (UI_DELEGATE_1)&MeshRenderUI::SelectTexture);
					//mtrl->SetTexParam((TEX_PARAM)j, );
				}
			}
		}
		//mtrl->SetTexFlow(bTexFlow);
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
	int materialIdx = iInst0;
	Ptr<CMaterial> pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(wstring(strKey.begin(), strKey.end()));
	GetTarget()->MeshRender()->SetMaterial(pMtrl, materialIdx);
}

void MeshRenderUI::SelectTexture(DWORD_PTR _Key)
{
	string strKey = (char*)_Key;
	int texParamIdx = iInst0;
	Ptr<CTexture> pTex= CResMgr::GetInst()->FindRes<CTexture>(wstring(strKey.begin(), strKey.end()));
	GetTarget()->MeshRender()->GetMaterial(iInst1)->SetTexParam((TEX_PARAM)texParamIdx, pTex);
}

void MeshRenderUI::SelectShader(DWORD_PTR _Key)
{
	string strKey = (char*)_Key;
	Ptr<CGraphicsShader> pShader = CResMgr::GetInst()->FindRes<CGraphicsShader>(wstring(strKey.begin(), strKey.end()));
	GetTarget()->MeshRender()->GetMaterial(iInst1)->SetShader(pShader);
}
