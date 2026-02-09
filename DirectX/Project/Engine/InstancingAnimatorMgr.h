#pragma once
#include "CSingleton.h"

#include "pch.h"
#include "ptr.h"

struct tAnimInstInfo
{
    int iBoneCount;
    int iCurIdx;
    int iNextIdx;
    int iIsBlend;
    int iNextClipIdx;
    float fRatio;
    float fNextClipRatio;
    int iOutBase;
    int iFrameBase;
    int iBlendFrameBase;
    int iOffsetBase;
    int iPad0;
};

class CAnimator3D;

struct tAnimInstEntry
{
    CAnimator3D* pAnim;
    int iOutBase;
};

class CStructuredBuffer;
class InstancingAnimation3DShader;

class InstancingAnimatorMgr :
    public CSingleton<InstancingAnimatorMgr>
{
    SINGLE(InstancingAnimatorMgr)
private:
    Ptr<InstancingAnimation3DShader>        m_pAnimationCopyShader;

    vector<tAnimInstEntry>              m_vecAnimators;

    CStructuredBuffer*                      m_pAnimInstBuffer;
    CStructuredBuffer*                      m_pGlobalFinalMatBuffer;
    
    CStructuredBuffer*                      m_pGlobalFrameBuffer;
    CStructuredBuffer*                      m_pGlobalBlendFrameBuffer;
    CStructuredBuffer*                      m_pGlobalOffsetBuffer;
    UINT                                    m_iMaxCount;
    UINT						            m_iAnimInstCount;

public:
    int                             m_iTotalOutBones;
    // Persistent caches for static animation data
    vector<tFrameTrans>                m_globalFrameCache;
    vector<tFrameTrans>                m_globalBlendFrameCache;
    vector<Matrix>                     m_globalOffsetCache;
    std::unordered_map<size_t, int>    m_frameBaseMap;
    std::unordered_map<size_t, int>    m_blendBaseMap;
    std::unordered_map<size_t, int>    m_offsetBaseMap;
public:
    int AddAnimator3D(CAnimator3D* anim3d);

    void BeginFrame();
    void SetData();
    CStructuredBuffer* GetFinalMatBuffer() { return m_pGlobalFinalMatBuffer; }

private:
    void Resize(UINT _iCount);
    size_t MakeMeshKey(CMesh* mesh) const;
    size_t MakeAnimKey(CMesh* mesh, const wstring& anim) const;
    bool BuildInstancingInfo(vector<tAnimInstInfo>& outInstInfo,
                             int& outTotalOutBones,
                             int& outMaxBoneCount,
                             bool& outHasBlend,
                             bool& outFrameDirty,
                             bool& outBlendDirty,
                             bool& outOffsetDirty);
    bool EnsureStaticBuffers(bool frameDirty, bool blendDirty, bool offsetDirty, bool hasBlend);
    void BindAndDispatch(UINT instCount, UINT maxBoneCount, bool hasBlend);

public:
    void init();
    void tick();
    void finaltick();
    void render();

};

