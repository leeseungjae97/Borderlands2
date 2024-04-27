#include "pch.h"
#include "TextMgr.h"

#include "CCamera.h"
#include "CConstBuffer.h"
#include "CDevice.h"
#include "CEngine.h"
#include "CRenderMgr.h"
#include "CResMgr.h"
#include "CTransform.h"

TextMgr::TextMgr()
{
}

TextMgr::~TextMgr()
{
	m_spriteBatch.reset();
	m_font.reset();
}

void TextMgr::init()
{
	wstring path = CPathMgr::GetInst()->GetContentPath();
	path += L"\\font\\compacta_bold_bt_32pt.spritefont";

	m_font = std::make_unique<SpriteFont>(DEVICE, path.c_str());
	m_spriteBatch = std::make_unique<SpriteBatch>(CONTEXT);
}

void TextMgr::DrawSpriteText(const wstring& str, Vec2 m_fontPos, float _fRotate, Vec2 _vScale, Vec4 vColor, bool Outline, float fScale)
{
	m_spriteBatch->SetRotation(DXGI_MODE_ROTATION_IDENTITY);
	m_spriteBatch->Begin(SpriteSortMode_Deferred
		, CDevice::GetInst()->GetBSState(BS_TYPE::ALPHA_BLEND).Get()
		, CDevice::GetInst()->GetSState(0).Get()
		, CDevice::GetInst()->GetDSState(DS_TYPE::LESS_EQUAL).Get()
		, CDevice::GetInst()->GetRSState(RS_TYPE::CULL_NONE).Get()
		, nullptr
	);
	float outlineSize = 1.5f;
	if(Outline)
	{
		m_font->DrawString(m_spriteBatch.get(), str.c_str(),
			m_fontPos + Vector2(outlineSize, outlineSize), Colors::Black, _fRotate, _vScale, fScale);
		m_font->DrawString(m_spriteBatch.get(), str.c_str(),
			m_fontPos + Vector2(-outlineSize, outlineSize), Colors::Black, _fRotate, _vScale, fScale);
		m_font->DrawString(m_spriteBatch.get(), str.c_str(),
			m_fontPos + Vector2(-outlineSize, -outlineSize), Colors::Black, _fRotate, _vScale, fScale);
		m_font->DrawString(m_spriteBatch.get(), str.c_str(),
			m_fontPos + Vector2(outlineSize, -outlineSize), Colors::Black, _fRotate, _vScale, fScale);
	}
	m_font->DrawString(m_spriteBatch.get(), str.c_str(),
		m_fontPos, vColor, _fRotate, _vScale, fScale);

	m_spriteBatch->End();
}

void TextMgr::DrawSpriteText(const wstring& str, Vec3 m_fontPos, Vec4 _vColor, float _fRotate, Vec2 _vScale, Matrix matWorld, float alpha, bool Outline, float fScale)
{
	CCamera* cam = CRenderMgr::GetInst()->GetMainCam();
	Matrix WVP = matWorld * cam->GetViewMat() * cam->GetProjMat();
	m_spriteBatch->SetRotation(DXGI_MODE_ROTATION_UNSPECIFIED);
	m_spriteBatch->Begin(SpriteSortMode_Deferred
		, CDevice::GetInst()->GetBSState(BS_TYPE::ALPHA_BLEND).Get()
		, CDevice::GetInst()->GetSState(0).Get()
		, CDevice::GetInst()->GetDSState(DS_TYPE::NO_WRITE).Get()
		, CDevice::GetInst()->GetRSState(RS_TYPE::CULL_NONE).Get()
		, [=]
		{
			tMtrlConst m_Const;
			m_Const.arrInt[1] = 1;
			m_Const.arrFloat[1] = alpha;
			CConstBuffer* pMtrlBuffer = CDevice::GetInst()->GetConstBuffer(CB_TYPE::MATERIAL);
			pMtrlBuffer->SetData(&m_Const);
			pMtrlBuffer->UpdateData();

			CONTEXT->PSSetShader(CResMgr::GetInst()->FindRes<CGraphicsShader>(L"SpriteFontUI2DShader")->GetPS().Get(), nullptr, 0);
		}
		, WVP
	);
	
	float outlineSize = 1.5f;
	if (Outline)
	{
		m_font->DrawString(m_spriteBatch.get(), str.c_str(),
			m_fontPos + Vector2(outlineSize, outlineSize), Colors::Black, _fRotate, _vScale, fScale, SpriteEffects_FlipVertically, m_fontPos.z);
		m_font->DrawString(m_spriteBatch.get(), str.c_str(),
			m_fontPos + Vector2(-outlineSize, outlineSize), Colors::Black, _fRotate, _vScale, fScale, SpriteEffects_FlipVertically, m_fontPos.z);
		m_font->DrawString(m_spriteBatch.get(), str.c_str(),
			m_fontPos + Vector2(-outlineSize, -outlineSize), Colors::Black, _fRotate, _vScale, fScale, SpriteEffects_FlipVertically, m_fontPos.z);
		m_font->DrawString(m_spriteBatch.get(), str.c_str(),
			m_fontPos + Vector2(outlineSize, -outlineSize), Colors::Black, _fRotate, _vScale, fScale, SpriteEffects_FlipVertically, m_fontPos.z);
	}

	m_font->DrawString(m_spriteBatch.get(), str.c_str(),
		m_fontPos, _vColor, _fRotate, _vScale, fScale, SpriteEffects_FlipVertically, m_fontPos.z);
	
	m_spriteBatch->End();
	m_spriteBatch->SetRotation(DXGI_MODE_ROTATION_UNSPECIFIED);
}

Vec2 TextMgr::GetTextSize(const wstring& str, float _TextScale)
{
	return m_font->MeasureString(str.c_str()) / _TextScale;
}
