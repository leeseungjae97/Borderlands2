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
	m_spriteBatch = std::make_unique<SpriteBatch>(CONTEXT);
	m_font = std::make_unique<SpriteFont>(DEVICE, L"..\\bin\\content\\font\\compacta_bold_bt_32pt.spritefont");
	//Vec2 m_fontPos;
	//Vec2 resol = CEngine::GetInst()->GetWindowResolution();
	m_spriteBatch->SetRotation(DXGI_MODE_ROTATION_IDENTITY);
}

void TextMgr::DrawSpriteText(const wstring& str, Vec2 m_fontPos, float _fRotate, Vec2 _vScale, bool Outline, float fScale)
{
	m_spriteBatch->SetRotation(DXGI_MODE_ROTATION_IDENTITY);
	m_spriteBatch->Begin();
	Vector2 origin = m_font->MeasureString(str.c_str()) / 2.f;
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
		m_fontPos, Colors::White, _fRotate, _vScale, fScale);

	m_spriteBatch->End();
}

void TextMgr::DrawSpriteText(const wstring& str, Vec3 m_fontPos, float _fRotate, Vec2 _vScale, Matrix matWorld, float alpha, bool Outline, float fScale)
{
	m_spriteBatch->SetRotation(DXGI_MODE_ROTATION_UNSPECIFIED);
	CCamera* cam = CRenderMgr::GetInst()->GetMainCam();
	Matrix WVP = matWorld * cam->GetViewMat() * cam->GetProjMat();

	m_spriteBatch->Begin(SpriteSortMode_Deferred
		, CDevice::GetInst()->GetBSState(BS_TYPE::ALPHA_BLEND).Get()
		, nullptr
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
	//CONTEXT->PSSetConstantBuffers(, 1, pMtrlBuffer-);
	//CONTEXT->PSSetShaderResources();
		}
	, WVP);
	
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
		m_fontPos, Colors::White, _fRotate, _vScale, fScale, SpriteEffects_FlipVertically, m_fontPos.z);

	m_spriteBatch->End();
}

Ptr<CTexture> TextMgr::GetTextAsTexture()
{
	ComPtr<ID3D11ShaderResourceView> p;
	ComPtr<ID3D11Texture2D> tex2d;

	m_font->GetSpriteSheet(p.GetAddressOf());
	DEVICE->CreateShaderResourceView(tex2d.Get(), nullptr, p.GetAddressOf());
	if (p.Get() == nullptr)
		return nullptr;

	Ptr<CTexture> tex = new CTexture;
	if(tex->Create(tex2d) == S_OK)
	{
		return tex;
	}
	return nullptr;
}
