#include "pch.h"
#include "InstancingAnimatorMgr.h"
#include "InstancingAnimation3DShader.h"
#include "CDevice.h"
#include "CStructuredBuffer.h"
#include "CAnimator3D.h"
#include "CResMgr.h"
#include "CMeshRender.h"

InstancingAnimatorMgr::InstancingAnimatorMgr()
    : m_iMaxCount(10)
    , m_iAnimInstCount(10)
    , m_pAnimInstBuffer(nullptr)
    , m_pGlobalFinalMatBuffer(nullptr)
    , m_pGlobalFrameBuffer(nullptr)
    , m_pGlobalBlendFrameBuffer(nullptr)
    , m_pGlobalOffsetBuffer(nullptr)
    , m_iTotalOutBones(0)
{
}

InstancingAnimatorMgr::~InstancingAnimatorMgr()
{
    if (m_pAnimInstBuffer)
    {
        delete m_pAnimInstBuffer;
        m_pAnimInstBuffer = nullptr;
    }
    if(m_pGlobalFinalMatBuffer)
    {
        delete m_pGlobalFinalMatBuffer;
        m_pGlobalFinalMatBuffer = nullptr;
    }
    if(m_pGlobalFrameBuffer)
    {
        delete m_pGlobalFrameBuffer;
        m_pGlobalFrameBuffer = nullptr;
    }
    if(m_pGlobalBlendFrameBuffer)
    {
        delete m_pGlobalBlendFrameBuffer;
        m_pGlobalBlendFrameBuffer = nullptr;
    }
    if(m_pGlobalOffsetBuffer)
    {
        delete m_pGlobalOffsetBuffer;
        m_pGlobalOffsetBuffer = nullptr;
    }
    if (nullptr != m_pAnimationCopyShader)
    {
        m_pAnimationCopyShader->Clear();
    }
}
void InstancingAnimatorMgr::init()
{
    m_pAnimInstBuffer = new CStructuredBuffer;
    m_pGlobalFinalMatBuffer = new CStructuredBuffer;
    m_pGlobalFrameBuffer = new CStructuredBuffer;
    m_pGlobalBlendFrameBuffer = new CStructuredBuffer;
    m_pGlobalOffsetBuffer = new CStructuredBuffer;

    m_pAnimInstBuffer->Create(sizeof(tAnimInstInfo), m_iMaxCount, SB_TYPE::READ_ONLY, true);

    m_pAnimationCopyShader = (InstancingAnimation3DShader*)CResMgr::GetInst()->FindRes<CComputeShader>(L"InstacingAnimation3DCS").Get();
}


void InstancingAnimatorMgr::BeginFrame()
{
    m_vecAnimators.clear();
    m_iTotalOutBones = 0;
    m_pAnimationCopyShader->Clear();
}

void InstancingAnimatorMgr::ResetCache()
{
    m_frameBaseMap.clear();
    m_blendBaseMap.clear();
    m_offsetBaseMap.clear();

    m_globalFrameCache.clear();
    m_globalBlendFrameCache.clear();
    m_globalOffsetCache.clear();
}

void InstancingAnimatorMgr::tick()
{


}

void InstancingAnimatorMgr::finaltick()
{

}

void InstancingAnimatorMgr::render()
{

}


int InstancingAnimatorMgr::AddAnimator3D(CAnimator3D* anim3d)
{
    if (!anim3d) return -1;
    int base = m_iTotalOutBones;
    m_iTotalOutBones += (int)anim3d->GetBoneCount();
    m_vecAnimators.push_back({ anim3d, base });
    return base;
}

size_t InstancingAnimatorMgr::MakeMeshKey(CMesh* mesh) const
{
    if (nullptr == mesh)
        return 0;

    const wstring& key = mesh->GetKey();
    if (!key.empty())
        return std::hash<wstring>{}(key);

    return std::hash<CMesh*>{}(mesh);
}

size_t InstancingAnimatorMgr::MakeAnimKey(CMesh* mesh, const wstring& anim) const
{
    size_t h1 = MakeMeshKey(mesh);
    size_t h2 = std::hash<wstring>{}(anim);
    return h1 ^ (h2 + 0x9e3779b97f4a7c15ull + (h1 << 6) + (h1 >> 2));
}

bool InstancingAnimatorMgr::BuildInstancingInfo(vector<tAnimInstInfo>& outInstInfo,
                                                int& outTotalOutBones,
                                                int& outMaxBoneCount,
                                                bool& outHasBlend,
                                                bool& outFrameDirty,
                                                bool& outBlendDirty,
                                                bool& outOffsetDirty)
{
    outInstInfo.clear();
    outTotalOutBones = 0;
    outMaxBoneCount = 0;
    outHasBlend = false;
    outFrameDirty = false;
    outBlendDirty = false;
    outOffsetDirty = false;

    for (auto& entry : m_vecAnimators)
    {
        CAnimator3D* anim = entry.pAnim;
        if (!anim) continue;
        if (!anim->GetOwner()) continue;
        if (!anim->GetOwner()->MeshRender()) continue;

        CAnimClip* curClip = anim->GetCurAnimClip();
        if (!curClip) continue;

        Ptr<CMesh> mesh = anim->GetOwner()->MeshRender()->GetMesh();
        if (nullptr == mesh) continue;

        tAnimInstInfo info = {};
        info.iBoneCount = (int)anim->GetBoneCount();
        info.iCurIdx = curClip->GetClipIdx();
        info.iNextIdx = curClip->GetClipNextIdx();
        info.fRatio = curClip->GetRatio();

        info.iIsBlend = anim->IsBlend();
        if (info.iIsBlend && anim->GetNextAnimClip())
        {
            info.iNextClipIdx = anim->GetNextAnimClip()->GetClipIdx();
            info.fNextClipRatio = anim->GetBlendRatio();
            outHasBlend = true;
        }

        info.iOutBase = entry.iOutBase;
        outTotalOutBones = max(outTotalOutBones, entry.iOutBase + info.iBoneCount);
        if (info.iBoneCount > outMaxBoneCount)
            outMaxBoneCount = info.iBoneCount;

        size_t meshKey = MakeMeshKey(mesh.Get());
        auto itOff = m_offsetBaseMap.find(meshKey);
        if (itOff == m_offsetBaseMap.end())
        {
            int base = (int)m_globalOffsetCache.size();
            m_offsetBaseMap.insert({ meshKey, base });
            auto offsets = mesh->GetBoneOffset();
            m_globalOffsetCache.insert(m_globalOffsetCache.end(), offsets.begin(), offsets.end());
            outOffsetDirty = true;
        }
        info.iOffsetBase = m_offsetBaseMap[meshKey];

        size_t key = MakeAnimKey(mesh.Get(), curClip->GetAnimName());
        auto itFrame = m_frameBaseMap.find(key);
        if (itFrame == m_frameBaseMap.end())
        {
            const auto& animClipMap = mesh->GetAnimClip();
            auto clipIt = animClipMap.find(curClip->GetAnimName());
            if (clipIt == animClipMap.end())
            {
                continue;
            }

            int base = (int)m_globalFrameCache.size();
            m_frameBaseMap.insert({ key, base });
            const auto& clip = clipIt->second;
            m_globalFrameCache.insert(m_globalFrameCache.end(), clip.vecTransKeyFrame.begin(), clip.vecTransKeyFrame.end());
            outFrameDirty = true;
        }
        info.iFrameBase = m_frameBaseMap[key];

        if (info.iIsBlend && anim->GetNextAnimClip())
        {
            size_t keyB = MakeAnimKey(mesh.Get(), anim->GetNextAnimClip()->GetAnimName());
            auto itBlend = m_blendBaseMap.find(keyB);
            if (itBlend == m_blendBaseMap.end())
            {
                const auto& animClipMap = mesh->GetAnimClip();
                auto blendIt = animClipMap.find(anim->GetNextAnimClip()->GetAnimName());
                if (blendIt == animClipMap.end())
                {
                    info.iIsBlend = 0;
                    info.iBlendFrameBase = 0;
                }
                else
                {
                    int base = (int)m_globalBlendFrameCache.size();
                    m_blendBaseMap.insert({ keyB, base });
                    const auto& bclip = blendIt->second;
                    m_globalBlendFrameCache.insert(m_globalBlendFrameCache.end(), bclip.vecTransKeyFrame.begin(), bclip.vecTransKeyFrame.end());
                    outBlendDirty = true;
                }
            }
            info.iBlendFrameBase = info.iIsBlend ? m_blendBaseMap[keyB] : 0;
        }
        else
        {
            info.iBlendFrameBase = 0;
        }

        outInstInfo.push_back(info);
    }

    return !outInstInfo.empty();
}

bool InstancingAnimatorMgr::EnsureStaticBuffers(bool frameDirty, bool blendDirty, bool offsetDirty, bool hasBlend)
{
    if (m_globalFrameCache.empty() || m_globalOffsetCache.empty())
        return false;

    if (m_pGlobalFrameBuffer->GetElementCount() < (UINT)m_globalFrameCache.size())
    {
        m_pGlobalFrameBuffer->Create(sizeof(tFrameTrans), (UINT)m_globalFrameCache.size(), SB_TYPE::READ_ONLY, true);
        frameDirty = true;
    }
    if (frameDirty)
        m_pGlobalFrameBuffer->SetData(m_globalFrameCache.data(), (UINT)m_globalFrameCache.size() * sizeof(tFrameTrans));

    if (hasBlend && !m_globalBlendFrameCache.empty())
    {
        if (m_pGlobalBlendFrameBuffer->GetElementCount() < (UINT)m_globalBlendFrameCache.size())
        {
            m_pGlobalBlendFrameBuffer->Create(sizeof(tFrameTrans), (UINT)m_globalBlendFrameCache.size(), SB_TYPE::READ_ONLY, true);
            blendDirty = true;
        }
        if (blendDirty)
            m_pGlobalBlendFrameBuffer->SetData(m_globalBlendFrameCache.data(), (UINT)m_globalBlendFrameCache.size() * sizeof(tFrameTrans));
    }

    if (m_pGlobalOffsetBuffer->GetElementCount() < (UINT)m_globalOffsetCache.size())
    {
        m_pGlobalOffsetBuffer->Create(sizeof(Matrix), (UINT)m_globalOffsetCache.size(), SB_TYPE::READ_ONLY, true);
        offsetDirty = true;
    }
    if (offsetDirty)
        m_pGlobalOffsetBuffer->SetData(m_globalOffsetCache.data(), (UINT)m_globalOffsetCache.size() * sizeof(Matrix));

    return true;
}

void InstancingAnimatorMgr::BindAndDispatch(UINT instCount, UINT maxBoneCount, bool hasBlend)
{
    m_pAnimationCopyShader->SetAnimInstBuffer(m_pAnimInstBuffer);
    m_pAnimationCopyShader->SetBoneFrameDataBuffer(m_pGlobalFrameBuffer);
    if (hasBlend && m_pGlobalBlendFrameBuffer)
        m_pAnimationCopyShader->SetBlendFrameDataBuffer(m_pGlobalBlendFrameBuffer);
    else
        m_pAnimationCopyShader->SetBlendFrameDataBuffer(nullptr);
    m_pAnimationCopyShader->SetOffsetMatBuffer(m_pGlobalOffsetBuffer);
    m_pAnimationCopyShader->SetOutputBuffer(m_pGlobalFinalMatBuffer);

    m_pAnimationCopyShader->SetInstCount(instCount);
    m_pAnimationCopyShader->SetBoneCount(maxBoneCount);

    // arrInt[1]에 BoneCount 저장
    int iMaxBoneCount = (int)maxBoneCount;
    m_pAnimationCopyShader->SetScalarParam(INT_1, &iMaxBoneCount);

    // Unbind VS SRV (slot 30) before CS writes to the same buffer (UAV 0)
    {
        ID3D11ShaderResourceView* pNullSRV = nullptr;
        CONTEXT->VSSetShaderResources(30, 1, &pNullSRV);
    }

    m_pAnimationCopyShader->Execute();
    m_pAnimationCopyShader->Clear();
}

void InstancingAnimatorMgr::SetData()
{
    if (m_vecAnimators.empty()) return;

    vector<tAnimInstInfo> vecInstInfo;
    int totalOutBones = 0;
    int maxBoneCount = 0;
    bool hasBlend = false;
    bool frameDirty = false;
    bool blendDirty = false;
    bool offsetDirty = false;

    if (!BuildInstancingInfo(vecInstInfo, totalOutBones, maxBoneCount, hasBlend, frameDirty, blendDirty, offsetDirty))
        return;

    if ((UINT)vecInstInfo.size() > m_iMaxCount)
    {
        m_iMaxCount = (UINT)vecInstInfo.size();
        m_pAnimInstBuffer->Create(sizeof(tAnimInstInfo), m_iMaxCount, SB_TYPE::READ_ONLY, true);
    }
    m_pAnimInstBuffer->SetData(vecInstInfo.data(), (UINT)vecInstInfo.size() * sizeof(tAnimInstInfo));

    if (totalOutBones == 0 || maxBoneCount == 0)
        return;

    if (!EnsureStaticBuffers(frameDirty, blendDirty, offsetDirty, hasBlend))
        return;

    if (m_pGlobalFinalMatBuffer->GetElementCount() < (UINT)totalOutBones)
        m_pGlobalFinalMatBuffer->Create(sizeof(Matrix), (UINT)totalOutBones, SB_TYPE::READ_WRITE, true);

    BindAndDispatch((UINT)vecInstInfo.size(), (UINT)maxBoneCount, hasBlend);

    if (m_pGlobalFinalMatBuffer)
    {
        m_pGlobalFinalMatBuffer->UpdateData(30, PIPELINE_STAGE::PS_VERTEX);
    }
}

void InstancingAnimatorMgr::Resize(UINT _iCount)
{
    if (m_pAnimInstBuffer)
    {
        delete m_pAnimInstBuffer;
        m_pAnimInstBuffer = nullptr;
    }

    m_iMaxCount = _iCount;

    m_pAnimInstBuffer = new CStructuredBuffer;
    m_pAnimInstBuffer->Create(sizeof(tAnimInstInfo), m_iMaxCount, SB_TYPE::READ_ONLY, true);
}
