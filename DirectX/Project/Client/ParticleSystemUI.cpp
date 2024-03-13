#include "pch.h"
#include "ParticleSystemUI.h"

#include <Engine/CGameObject.h>
#include <Engine/CParticleSystem.h>
#include <Engine/CResMgr.h>

#include "ListUI.h"

class ListUI;

extern int iInst0;
extern int iInst1;
extern int iInst2;
extern int iInst3;
ParticleSystemUI::ParticleSystemUI()
	: ComponentUI("##ParticleSystem", COMPONENT_TYPE::PARTICLESYSTEM)
{
	SetName("ParticleSystem");
}

ParticleSystemUI::~ParticleSystemUI()
{
}

int ParticleSystemUI::render_update()
{
	if (FALSE == ComponentUI::render_update())
		return FALSE;

	tParticleModule data = GetTarget()->ParticleSystem()->GetModuleData();
	float fInput[3];
	int inputInt = 0;
	Vec3 vT; string str;

	bool fire = GetTarget()->ParticleSystem()->GetFire();
	ImGui::Text("Fire");
	ImGui::SameLine();
	str = fire ? "true" : "false";
	str += "##fire ";
	if (ImGui::Button(str.c_str()))
	{
		fire = !fire;
	};
	GetTarget()->ParticleSystem()->SetFire(fire);

	ImGui::Text("Velocity Type");
	inputInt = data.AddVelocityType;
	ImGui::SameLine();
	ImGui::InputInt("##velocityType", &inputInt);
	data.AddVelocityType = inputInt;

	ImGui::Text("Space Type");
	inputInt = data.Space;
	ImGui::SameLine();
	ImGui::InputInt("##spaceType", &inputInt);
	data.Space = inputInt;

	ImGui::Text("Velocity Scale Type");
	inputInt = data.VelocityScale;
	ImGui::SameLine();
	ImGui::InputInt("##velocityScaleType", &inputInt);
	data.VelocityScale = inputInt;

	ImGui::Text("Spawn Shape Type");
	inputInt = data.SpawnShapeType;
	ImGui::SameLine();
	ImGui::InputInt("##SpawnShapeType", &inputInt);
	data.SpawnShapeType = inputInt;

	ImGui::Text("AddVelocityType");
	inputInt = data.AddVelocityType;
	ImGui::SameLine();
	ImGui::InputInt("##AddVelocityType", &inputInt);
	data.AddVelocityType = inputInt;

	ImGui::Text("vVelocityDir");
	fInput[0] = data.vVelocityDir.x;
	fInput[1] = data.vVelocityDir.y;
	fInput[2] = data.vVelocityDir.z;
	ImGui::SameLine();
	ImGui::InputFloat3("##vVelocityDir", fInput);
	data.vVelocityDir.x = fInput[0];
	data.vVelocityDir.y = fInput[1];
	data.vVelocityDir.z = fInput[2];

	ImGui::Text("Box Shape Scale");
	vT = data.vBoxShapeScale;
	fInput[0] = vT.x;
	fInput[1] = vT.y;
	fInput[2] = vT.z;

	ImGui::SameLine();

	ImGui::InputFloat3("##vBoxShapeScale", fInput);
	vT.x = fInput[0];
	vT.y = fInput[1];
	vT.z = fInput[2];

	data.vBoxShapeScale = vT;

	ImGui::Text("Spawn Scale Min");

	vT = data.vSpawnScaleMin;
	fInput[0] = vT.x;
	fInput[1] = vT.y;
	fInput[2] = vT.z;

	ImGui::SameLine();

	ImGui::InputFloat3("##vSpawnScaleMin", fInput);
	vT.x = fInput[0];
	vT.y = fInput[1];
	vT.z = fInput[2];

	data.vSpawnScaleMin = vT;

	ImGui::Text("Spawn Scale Max");

	vT = data.vSpawnScaleMax;
	fInput[0] = vT.x;
	fInput[1] = vT.y;
	fInput[2] = vT.z;

	ImGui::SameLine();

	ImGui::InputFloat3("##vSpawnScaleMax", fInput);
	vT.x = fInput[0];
	vT.y = fInput[1];
	vT.z = fInput[2];

	data.vSpawnScaleMax = vT;

	ImGui::Text("Max Velocity Scale");

	vT = data.vMaxVelocityScale;
	fInput[0] = vT.x;
	fInput[1] = vT.y;
	fInput[2] = vT.z;

	ImGui::SameLine();

	ImGui::InputFloat3("##vMaxVelocityScale", fInput);
	vT.x = fInput[0];
	vT.y = fInput[1];
	vT.z = fInput[2];

	data.vMaxVelocityScale = vT;

	ImGui::Text("Speed");
	inputInt = data.Speed;
	ImGui::SameLine();
	ImGui::InputInt("##Speed", &inputInt);
	data.Speed = inputInt;

	ImGui::Text("vMaxSpeed");
	inputInt = data.vMaxSpeed;
	ImGui::SameLine();
	ImGui::InputInt("##vMaxSpeed", &inputInt);
	data.vMaxSpeed = inputInt;

	ImGui::Text("StartScale");
	inputInt = data.StartScale;
	ImGui::SameLine();
	ImGui::InputInt("##StartScale", &inputInt);
	data.StartScale = inputInt;

	ImGui::Text("EndScale");
	inputInt = data.EndScale;
	ImGui::SameLine();
	ImGui::InputInt("##EndScale", &inputInt);
	data.EndScale = inputInt;

	ImGui::Text("SpawnRate");
	inputInt = data.SpawnRate;
	ImGui::SameLine();
	ImGui::InputInt("##SpawnRate", &inputInt);
	data.SpawnRate = inputInt;


	ImGui::Text("VelocityScale");
	ImGui::SameLine();
	str = data.VelocityScale ? "true" : "false";
	str += "##VelocityScale";
	if (ImGui::Button(str.c_str()))
	{
		data.VelocityScale = !data.VelocityScale;
	};

	ImGui::Text("VelocityAlignment");
	ImGui::SameLine();
	str = data.VelocityAlignment ? "true" : "false";
	str += "##VelocityAlignment";
	if (ImGui::Button(str.c_str()))
	{
		data.VelocityAlignment = !data.VelocityAlignment;
	};

	ImGui::Text("StartDrag");
	inputInt = data.StartDrag;
	ImGui::SameLine();
	ImGui::InputInt("##StartDrag", &inputInt);
	data.StartDrag = inputInt;

	ImGui::Text("EndDrag");
	inputInt = data.EndDrag;
	ImGui::SameLine();
	ImGui::InputInt("##EndDrag", &inputInt);
	data.EndDrag = inputInt;

	GetTarget()->ParticleSystem()->SetModuleData(data);

	for(int i = 0; i < GetTarget()->ParticleSystem()->GetMtrlCount(); ++i)
	{
		Ptr<CMaterial> mtrl = GetTarget()->ParticleSystem()->GetMaterial(i);

		if (nullptr == mtrl)
			continue;

		tMtrlConst mtrlConst = mtrl->GetConst();

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
			pListUI->AddDynamic_Select(this, (UI_DELEGATE_1)&ParticleSystemUI::SelectShader);
		}

		bool bTexFlow = false;
		for (int j = 0; j < TEX_PARAM::TEX_7 + 1; ++j)
		{

			Ptr<CTexture> tex = mtrl->GetTexParam((TEX_PARAM)j);
			if (nullptr != tex)
			{
				ImGui::Text("%d Material, %d Texture", i, j);
				str = "SET##" + std::to_string(i) + "mtrl" + std::to_string(j) + "texSet";
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
					pListUI->AddDynamic_Select(this, (UI_DELEGATE_1)&ParticleSystemUI::SelectTexture);
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
				if (bFlow)
				{
					bTexFlow = true;
					str = "STOP##" + std::to_string(i) + "mtrl" + std::to_string(j) + "texStop";
					if (ImGui::Button(str.c_str()))
					{
						bFlow = false;
					}
				}
				else
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
			}
			else
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
					pListUI->AddDynamic_Select(this, (UI_DELEGATE_1)&ParticleSystemUI::SelectTexture);
					//mtrl->SetTexParam((TEX_PARAM)j, );
				}
			}
		}
		mtrl->SetTexFlow(bTexFlow);
	}
	return TRUE;
}
void ParticleSystemUI::SelectMesh(DWORD_PTR _Key)
{
	string strKey = (char*)_Key;
	Ptr<CMesh> pMesh = CResMgr::GetInst()->FindRes<CMesh>(wstring(strKey.begin(), strKey.end()));
	GetTarget()->ParticleSystem()->SetMesh(pMesh);
}

void ParticleSystemUI::SelectMaterial(DWORD_PTR _Key)
{
	string strKey = (char*)_Key;
	int materialIdx = iInst0;
	Ptr<CMaterial> pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(wstring(strKey.begin(), strKey.end()));
	GetTarget()->ParticleSystem()->SetMaterial(pMtrl, materialIdx);
}

void ParticleSystemUI::SelectTexture(DWORD_PTR _Key)
{
	string strKey = (char*)_Key;
	int texParamIdx = iInst0;
	Ptr<CTexture> pTex = CResMgr::GetInst()->FindRes<CTexture>(wstring(strKey.begin(), strKey.end()));
	GetTarget()->ParticleSystem()->GetMaterial(iInst1)->SetTexParam((TEX_PARAM)texParamIdx, pTex);
}

void ParticleSystemUI::SelectShader(DWORD_PTR _Key)
{
	string strKey = (char*)_Key;
	Ptr<CGraphicsShader> pShader = CResMgr::GetInst()->FindRes<CGraphicsShader>(wstring(strKey.begin(), strKey.end()));
	GetTarget()->ParticleSystem()->GetMaterial(iInst1)->SetShader(pShader);
}
