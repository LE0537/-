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

void CAnimation::Free()
{
	for (auto& pChannel : m_Channels)
		Safe_Release(pChannel);

	m_Channels.clear();
}