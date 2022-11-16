#include "..\Public\Animation.h"
#include "Channel.h"

CAnimation::CAnimation()
{
}

HRESULT CAnimation::Initialize(CModel* pModel, aiAnimation * pAIAnimation)
{
	strcpy_s(m_szName, pAIAnimation->mName.data);

	m_fDuration = (_float)pAIAnimation->mDuration;
	m_fTickPerSecond = (_float)pAIAnimation->mTicksPerSecond;

	m_iNumChannels = pAIAnimation->mNumChannels;

	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		aiNodeAnim*		pAIChannel = pAIAnimation->mChannels[i];

		CChannel*		pChannel = CChannel::Create(pModel, pAIAnimation->mChannels[i]);

		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.push_back(pChannel);
	}

	return S_OK;
}

void CAnimation::Invalidate_TransformationMatrix(_float fTimeDelta)
{
	/* 현재 재생중인 시간. */
	m_fCurrentTime += m_fTickPerSecond * fTimeDelta;

	if (m_fCurrentTime >= m_fDuration)
	{
		m_fCurrentTime = 0.f;

		m_isFinished = true;
		m_bEnd = true;
	}
	if (!m_isLoop && m_bEnd)
		return;

	for (auto& pChannel : m_Channels)
	{
		if (true == m_isFinished && true == m_isLoop)
			pChannel->Reset();

		pChannel->Invalidate_TransformationMatrix(m_fCurrentTime);
	}

	if (true == m_isFinished && true == m_isLoop)
		m_isFinished = false;

}

void CAnimation::Invalidate_TransformationMatrix2(_float fTimeDelta, vector<class CChannel*> _Channel)
{
	/* 현재 재생중인 시간. */
	m_fCurrentTime2 += fTimeDelta;
	
	if (m_fCurrentTime2 >= 0.2f)
	{
		m_fCurrentTime2 = 0.f;

		m_bAnimEnd = true;
		if (m_bAnimEnd)
			Reset2();
	}
	else
	{
		auto& Currentiter = m_Channels.begin();
		auto& Previter = _Channel.begin();
		for (_uint i = 0; i < m_Channels.size(); ++i)
			Currentiter[i]->Invalidate_TransformationMatrix2(m_fCurrentTime2, Previter[i]->Get_Key()[0]);
	}
	
}

void CAnimation::Reset2()
{
	for (auto& pChannel : m_Channels)
	{
		if (!m_isLoop)
			break;

		pChannel->Reset();
	}
	m_fCurrentTime = 0.f;
}
void CAnimation::Reset3()
{
	for (auto& pChannel : m_Channels)
	{
		pChannel->Reset();
	}
	m_fCurrentTime = 0.f;
}
CAnimation * CAnimation::Create(CModel* pModel, aiAnimation * pAIAnimation)
{
	CAnimation*	pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize(pModel, pAIAnimation)))
	{
		ERR_MSG(TEXT("Failed to Created : CAnimation"));
		Safe_Release(pInstance);
	}

	return pInstance;
}
HRESULT CAnimation::Bin_Initialize(DATA_BINANIM * pAIAnimation, CModel* pModel)
{
	strcpy_s(m_szName, pAIAnimation->szName);

	m_fDuration = pAIAnimation->fDuration;

	m_fTickPerSecond = pAIAnimation->fTickPerSecond;

	/* 현재 애니메이션에서 제어해야할 뼈들의 갯수를 저장한다. */
	m_iNumChannels = pAIAnimation->iNumChannels;

	m_isLoop = pAIAnimation->bLoop;

	/* 현재 애니메이션에서 제어해야할 뼈정보들을 생성하여 보관한다. */
	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		CChannel*		pChannel = CChannel::Bin_Create(&pAIAnimation->pBinChannel[i], pModel);
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.push_back(pChannel);
	}

	return S_OK;
}
CAnimation * CAnimation::Bin_Create(DATA_BINANIM * pAIAnimation, CModel* pModel)
{
	CAnimation*			pInstance = new CAnimation();

	if (FAILED(pInstance->Bin_Initialize(pAIAnimation, pModel)))
	{
		ERR_MSG(TEXT("Failed To Created : CAnimation_Bin"));
		Safe_Release(pInstance);
	}

	return pInstance;
}
void CAnimation::Get_AnimData(DATA_BINANIM * pAnimData)
{
	pAnimData->fDuration = m_fDuration;
	pAnimData->iNumChannels = m_iNumChannels;
	pAnimData->fTickPerSecond = m_fTickPerSecond;
	pAnimData->bLoop = m_isLoop;
	strcpy_s(pAnimData->szName, m_szName);

	pAnimData->pBinChannel = new DATA_BINCHANNEL[m_iNumChannels];

	for (_int i = 0; i < m_iNumChannels; ++i)
	{
		m_Channels[i]->Get_ChannelData(&pAnimData->pBinChannel[i]);
	}
}
void CAnimation::Free()
{
	for (auto& pChannel : m_Channels)
		Safe_Release(pChannel);

	m_Channels.clear();
}