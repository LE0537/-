#pragma once
#include "Base.h"

BEGIN(Engine)

class CChannel final : public CBase
{
private:
	CChannel();
	virtual ~CChannel() = default;

public:
	HRESULT Initialize(class CModel* pModel, aiNodeAnim* pAIChannel);
	HRESULT Bin_Initialize(DATA_BINCHANNEL* pAIChannel, class CModel* pModel); // 추가
	void Invalidate_TransformationMatrix(_float fCurrentTime);
	void Invalidate_TransformationMatrix2(_float fCurrentTime, KEYFRAME _key);
	void Reset();
	vector<KEYFRAME> Get_Key() { return m_KeyFrames; }

private:
	char					m_szName[MAX_PATH] = "";
	class CHierarchyNode*	m_pBoneNode = nullptr;

private:
	_uint					m_iNumKeyframes = 0;
	vector<KEYFRAME>		m_KeyFrames;

	_uint					m_iCurrentKeyFrameIndex = 0;
public: // For. Data
	void Get_ChannelData(DATA_BINCHANNEL* pChannelData);
public:
	static CChannel* Create(class CModel* pModel, aiNodeAnim* pAIChannel);
	static CChannel* Bin_Create(DATA_BINCHANNEL* pAIChannel, class CModel* pModel);	// 추가
	virtual void Free() override;
};

END