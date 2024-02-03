#include "pch.h"
#include "SamplerFilterUI.h"

#include <Engine\CDevice.h>
SamplerFilterUI::SamplerFilterUI()
	: UI("##SamplerFilter")
{
	SetName("SamplerFilter");
}

SamplerFilterUI::~SamplerFilterUI()
{
}

void SamplerFilterUI::init() { UI::init(); }
void SamplerFilterUI::tick() { UI::tick(); }

int SamplerFilterUI::render_update()
{
	int idx = CDevice::GetInst()->GetShadowFilterIdx();
	ImGui::InputInt("##maaaaaaa", &idx);
	CDevice::GetInst()->SetShadowSamplerFilter(idx);

	int aIdx = CDevice::GetInst()->GetAdsIdx();
	ImGui::InputInt("##maaaaaaa2", &aIdx);
	CDevice::GetInst()->SetSamplerAddress(aIdx);
	return TRUE;
}