#include "pch.h"
#include "Animator3DUI.h"

#include <Engine/CGameObject.h>
#include <Engine/CAnimator3D.h>


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

	float ratio = GetTarget()->Animator3D()->GetBlendRatio();
	ImGui::Text("Blend Ratio");
	ImGui::SameLine();
	ImGui::InputFloat("##BlendRatio", &ratio, 0.1f);

	GetTarget()->Animator3D()->SetBlendRatio(ratio);

	wstring wanimName = curAnimClip->GetCurClip().strAnimName;
	string animName = string(wanimName.begin(), wanimName.end());
	ImGui::Text("cur anim : %s", animName.c_str());

	sprintf_s(str, "Anim Idx : %d", GetTarget()->Animator3D()->GetClipIdx());
	ImGui::Text(str);

	sprintf_s(str, "Anim Frame : %d", GetTarget()->Animator3D()->GetCurAnimClip()->GetClipNextFrame());
	ImGui::Text(str);

	sprintf_s(str, "Blend Time : %d", GetTarget()->Animator3D()->GetBlendAcc());
	ImGui::Text(str);

	sprintf_s(str, "Anim Ratio : %f", GetTarget()->Animator3D()->GetCurAnimClip()->GetRatio());
	ImGui::Text(str);

	sprintf_s(str, "Anim Bone : %d", GetTarget()->Animator3D()->GetBoneCount());
	ImGui::Text(str);

	wstring mChosen = L"";
	int idx = 0;
	int count = 0;
	for (const auto& pair : clips)
	{
		wstring wanimName = pair.first;
		string animName = string(wanimName.begin(), wanimName.end());
		if(ImGui::Button(animName.c_str()))
		{
			mChosen = pair.first;
			idx = count;
		}
		++count;
	}
	if(L"" != mChosen)
	{
		GetTarget()->Animator3D()->SetCurAnimClip(clips.at(mChosen));
		GetTarget()->Animator3D()->SetClipIdx(idx);
	}

	return TRUE;
}