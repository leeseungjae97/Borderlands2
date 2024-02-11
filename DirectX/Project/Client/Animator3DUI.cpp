#include "pch.h"
#include "Animator3DUI.h"

#include <Engine/CGameObject.h>
#include <Engine/CAnimator3D.h>
#include <Engine/AnimationMgr.h>


Animator3DUI::Animator3DUI()
	: ComponentUI("##Animator3D", COMPONENT_TYPE::ANIMATOR3D)
{
	SetName("Animator3D");
}

Animator3DUI::~Animator3DUI()
{
}

int Animator3DUI::render_update()
{
	if (FALSE == ComponentUI::render_update())
		return FALSE;
	char str[50] = {};
	const map<wstring, CAnimClip*> clips = GetTarget()->Animator3D()->GetAnimClips();
	CAnimClip* curAnimClip = GetTarget()->Animator3D()->GetCurAnimClip();

	vector<tMTBone> bones = GetTarget()->Animator3D()->GetBone();

	if(ImGui::BeginMenu("Bone Names##ttt"))
	{
		for (int i = 0; i < bones.size(); ++i)
		{
			tMTBone bone = bones[i];
			string str = std::to_string(i) + " : " + string(bone.strBoneName.begin(), bone.strBoneName.end());
			ImGui::Text(str.c_str());
		}
		ImGui::EndMenu();
	}

	float ratio = GetTarget()->Animator3D()->GetBlendRatio();
	ImGui::Text("Blend Ratio");
	ImGui::SameLine();
	ImGui::InputFloat("##BlendRatio", &ratio, 0.1f);

	if(ImGui::Button("STOP##AnimstopBtn"))
	{
		GetTarget()->Animator3D()->StopPlay();
	}
	ImGui::SameLine();
	if (ImGui::Button("PLAY##AnimplayBtn"))
	{
		GetTarget()->Animator3D()->Proceed();
	}
	ImGui::SameLine();
	if (ImGui::Button("Manual Ratio##ManualRatioBtn"))
	{
		GetTarget()->Animator3D()->ManualRatio();
	}
	if (ImGui::Button("Auto Ratio##AutoRatioBtn"))
	{
		GetTarget()->Animator3D()->AutoRatio();
	}

	if(GetTarget()->Animator3D()->IsPlayManual())
	{
		ImGui::Text("Manual idx : ");
		ImGui::SameLine();
		if(ImGui::Button("+##mupbtn"))
		{
			GetTarget()->Animator3D()->ManualIdxUp();
		}
		ImGui::SameLine();
		if (ImGui::Button("-##mdownbtn"))
		{
			GetTarget()->Animator3D()->ManualIdxDown();
		}
		if(GetTarget()->Animator3D()->IsManualRatio())
		{
			ImGui::Text("Manual Ratio : ");
			ImGui::SameLine();
			float mRatio = GetTarget()->Animator3D()->GetManualRatio();
			ImGui::InputFloat("##manualRatio", &mRatio);
			GetTarget()->Animator3D()->SetManualRatio(mRatio);
		}
	}

	if(curAnimClip)
	{
		wstring wanimName = curAnimClip->GetCurClip().strAnimName;
		string animName = string(wanimName.begin(), wanimName.end());
		ImGui::Text("cur anim : %s", animName.c_str());

		if (!GetTarget()->Animator3D()->IsPlayManual())
			sprintf_s(str, "Anim Frame : %d", GetTarget()->Animator3D()->GetCurAnimClip()->GetClipIdx());
		else
			sprintf_s(str, "Anim Frame : %d", GetTarget()->Animator3D()->GetManualIdx());
		ImGui::Text(str);

		sprintf_s(str, "Blend Time : %d", GetTarget()->Animator3D()->GetBlendAcc());
		ImGui::Text(str);

		sprintf_s(str, "Anim Ratio : %f", GetTarget()->Animator3D()->GetCurAnimClip()->GetRatio());
		ImGui::Text(str);

		sprintf_s(str, "Anim Bone : %d", GetTarget()->Animator3D()->GetBoneCount());
		ImGui::Text(str);
	}

	wstring mChosen = L"";
	string comboLabel = "";
	int count = 0;

	const char** STRS = nullptr;
	int STRSSIZE = 0;
	if (GetTarget()->IsItem())
	{
		STRS = GUN_ANIMATION_TYPE_STR;
		STRSSIZE = (int)GUN_ANIMATION_TYPE::END;
	}
	else
	{
		STRS = ANIMATION_TYPE_STR;
		STRSSIZE = (int)ANIMATION_TYPE::END;
	}


	int strMaxLen = 0;
	ImVec2 strSize;
	ImGuiComboFlags flags = 0;
	for(int i = 0 ; i < STRSSIZE; ++i)
	{
		const string str = STRS[i];
		strSize = ImGui::CalcTextSize(str.c_str());
		strMaxLen = strMaxLen < strSize.x ? strSize.x : strMaxLen;
	}

	for (const auto& pair : clips)
	{
		wstring wanimName = pair.first;
		string animName = string(wanimName.begin(), wanimName.end());

		if(ImGui::Button(animName.c_str()))
		{
			mChosen = pair.first;
		}
		{
			UINT setType;
			setType = GetTarget()->Animator3D()->FindDefineAnimation(wanimName);
			ImGui::Text("^ Define anim : ");
			int currentItem = (int)setType;
			ImGui::SameLine();
			comboLabel = "##atc" + std::to_string(count);
			ImGui::SetNextItemWidth(strMaxLen * 2.f);

			if (ImGui::BeginCombo(comboLabel.c_str(), STRS[currentItem], flags))
			{
				for (int n = 0; n < STRSSIZE; n++)
				{
					const bool is_selected = (currentItem == n);

					if (ImGui::Selectable(STRS[n], is_selected))
					{
						currentItem = n;
						GetTarget()->Animator3D()->SetDefineAnimation(wanimName, currentItem);
					}
					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
	
		}
		++count;
	}
	if(L"" != mChosen)
	{
		GetTarget()->Animator3D()->Play(mChosen, true);
		//GetTarget()->Animator3D()->SetCurAnimClip(clips.at(mChosen));
		//GetTarget()->Animator3D()->SetClipIdx(idx);
	}
	ImGui::Text("Adapt Animation");
	if (ImGui::Button("Target Layer Only##AnimationAdaptBtn"))
	{
		AnimationMgr::GetInst()->AdaptAnimation(GetTarget());
	}
	if (ImGui::Button("All Layer##AnimationAdaptBtn"))
	{
		AnimationMgr::GetInst()->AdaptAnimation(GetTarget(), true);
	}
	

	return TRUE;
}