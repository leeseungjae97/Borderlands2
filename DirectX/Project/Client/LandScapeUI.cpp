#include "pch.h"
#include "LandScapeUI.h"

#include <Engine/CGameObject.h>
#include <Engine/CLandScape.h>

#include "TreeUI.h"
#include <Engine/CResMgr.h>

#include "ListUI.h"

LandScapeUI::LandScapeUI()
	: ComponentUI("##LandScape", COMPONENT_TYPE::LANDSCAPE)
{
	SetName("LandScape");
}

LandScapeUI::~LandScapeUI()
{
}

void LandScapeUI::SelectBrushTexture(DWORD_PTR _Key)
{
	string strKey = (char*)_Key;
	wstring strTowstring = wstring(strKey.begin(), strKey.end());
	Ptr<CTexture> pTexture = CResMgr::GetInst()->FindRes<CTexture>(strTowstring);
	GetTarget()->LandScape()->SetBrush(pTexture);
}

void LandScapeUI::SelectHeightMap(DWORD_PTR _Key)
{
	string strKey = (char*)_Key;
	wstring strTowstring= wstring(strKey.begin(), strKey.end());
	Ptr<CTexture> pTexture = CResMgr::GetInst()->FindRes<CTexture>(strTowstring);
	GetTarget()->LandScape()->SetHeightMapName(wstring(strKey.begin(), strKey.end()));
	GetTarget()->LandScape()->SetHeightMap(pTexture);
}

void LandScapeUI::SelectColorMap(DWORD_PTR _Key)
{
	string strKey = (char*)_Key;
	wstring strTowstring = wstring(strKey.begin(), strKey.end());
	Ptr<CTexture> pTexture = CResMgr::GetInst()->FindRes<CTexture>(strTowstring);
	GetTarget()->LandScape()->SetColorMapName(wstring(strKey.begin(), strKey.end()));
	GetTarget()->LandScape()->SetColorMap(pTexture);
}

int LandScapeUI::render_update()
{
	if (FALSE == ComponentUI::render_update())
		return FALSE;

	int iFaceX = (int)GetTarget()->LandScape()->GetFaceX();
	int iFaceZ = (int)GetTarget()->LandScape()->GetFaceZ();

	ImGui::Text("X");
	ImGui::SameLine();
	ImGui::InputInt("##FaceX", &iFaceX);

	ImGui::Text("Z");
	ImGui::SameLine();
	ImGui::InputInt("##FaceZ", &iFaceZ);

	GetTarget()->LandScape()->SetFaceX(iFaceX);
	GetTarget()->LandScape()->SetFaceZ(iFaceZ);

	int iPopUp= (int)GetTarget()->LandScape()->GetBrushPopUp();

	ImGui::Text("Brush Pop Up : ");
	ImGui::SameLine();
	char str[50] = {};
	sprintf_s(str, "%s", iPopUp ? "YES" : "NO");
	if(ImGui::Button(str))
	{
		GetTarget()->LandScape()->SetBrushPopUp(!iPopUp);
	}

	Vec2 scale = GetTarget()->LandScape()->GetBrushScale();
	float fScale[2] = {scale.x, scale.y};
	ImGui::Text("Brush Scale");
	ImGui::SameLine();
	ImGui::InputFloat2("##BrushScale", fScale);
	scale.x = fScale[0];
	scale.y = fScale[1];

	GetTarget()->LandScape()->SetBrushScale(scale);
	//float fEdge = (int)GetTarget()->LandScape()->GetTessEdge();
	//float fInside = (int)GetTarget()->LandScape()->GetTessInside();

	//ImGui::Text("Edge");
	//ImGui::SameLine();
	//ImGui::InputFloat("##TessEdge", &fEdge);

	//ImGui::Text("Inside");
	//ImGui::SameLine();
	//ImGui::InputFloat("##TessInside", &fInside);

	//GetTarget()->LandScape()->SetTessEdge(fEdge);
	//GetTarget()->LandScape()->SetTessInside(fInside);


	Ptr<CTexture> pHeightMap= GetTarget()->LandScape()->GetHeightMap();
	Ptr<CTexture> pColorMap= GetTarget()->LandScape()->GetColorMap();
	Ptr<CTexture> pBrush= GetTarget()->LandScape()->GetBrush();
	Ptr<CTexture> pArrTex = GetTarget()->LandScape()->GetWeightTexture();
	int weightIdx = GetTarget()->LandScape()->GetWeightIdx();

	ImVec2 uv_min = ImVec2(0.0f, 0.0f);                 // Top-left
	ImVec2 uv_max = ImVec2(1.0f, 1.0f);                 // Lower-right
	ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
	ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.5f); // 50% opaque white

	//ImGui::Text("Weight Map %d Texture" , weightIdx);
	//ImGui::Image((ImTextureID)pArrTex->GetSRV().Get(), ImVec2(150, 150), uv_min, uv_max, tint_col, border_col);

	ImGui::Text("Height Map Texture");
	ImGui::Image((ImTextureID)pHeightMap->GetSRV().Get(), ImVec2(150, 150), uv_min, uv_max, tint_col, border_col);

	if (ImGui::Button("Select Texture##HeightMapSelectBtn"))
	{
		const map<wstring, Ptr<CRes>>& pTexture = CResMgr::GetInst()->GetResources(RES_TYPE::TEXTURE);

		ListUI* pListUI = (ListUI*)ImGuiMgr::GetInst()->FindUI("##List");
		pListUI->Reset("Texture List", ImVec2(300.f, 500.f));
		for (const auto& pair : pTexture)
		{
			pListUI->AddItem(string(pair.first.begin(), pair.first.end()));
		}

		// 항목 선택시 호출받을 델리게이트 등록
		pListUI->AddDynamic_Select(this, (UI_DELEGATE_1)&LandScapeUI::SelectHeightMap);
	}

	ImGui::Text("Color Map Texture");
	ImGui::Image((ImTextureID)pColorMap->GetSRV().Get(), ImVec2(150, 150), uv_min, uv_max, tint_col, border_col);
	if (ImGui::Button("Select Texture##ColorMapSelectBtn"))
	{
		const map<wstring, Ptr<CRes>>& pTexture = CResMgr::GetInst()->GetResources(RES_TYPE::TEXTURE);

		ListUI* pListUI = (ListUI*)ImGuiMgr::GetInst()->FindUI("##List");
		pListUI->Reset("Texture List", ImVec2(300.f, 500.f));
		for (const auto& pair : pTexture)
		{
			pListUI->AddItem(string(pair.first.begin(), pair.first.end()));
		}

		// 항목 선택시 호출받을 델리게이트 등록
		pListUI->AddDynamic_Select(this, (UI_DELEGATE_1)&LandScapeUI::SelectColorMap);
	}

	ImGui::Text("Brush Texture");
	ImGui::Image((ImTextureID)pBrush->GetSRV().Get(), ImVec2(150, 150), uv_min, uv_max, tint_col, border_col);
	if (ImGui::Button("Select Texture##BrushSelectBtn"))
	{
		const map<wstring, Ptr<CRes>>& pTexture = CResMgr::GetInst()->GetResources(RES_TYPE::TEXTURE);

		ListUI* pListUI = (ListUI*)ImGuiMgr::GetInst()->FindUI("##List");
		pListUI->Reset("Texture List", ImVec2(300.f, 500.f));
		for (const auto& pair : pTexture)
		{
			pListUI->AddItem(string(pair.first.begin(), pair.first.end()));
		}

		// 항목 선택시 호출받을 델리게이트 등록
		pListUI->AddDynamic_Select(this, (UI_DELEGATE_1)&LandScapeUI::SelectBrushTexture);
	}

	return TRUE;
}
