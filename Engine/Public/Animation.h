#pragma once

#include "Base.h"

BEGIN(Engine)

class CAnimation final : public CBase
{
private:
	CAnimation();
	virtual ~CAnimation() = default;

public:
	HRESULT Initialize(class CModel* pModel, aiAnimation* pAIAnimation);
	HRESULT Bin_Initialize(DATA_BINANIM* pAIAnimation, class CModel* pModel); // 추가
	void Invalidate_TransformationMatrix(_float fTimeDelta);
	void Invalidate_TransformationMatrix2(_float fTimeDelta, vector<class CChannel*> _Channel);
	_bool	Get_AnimEnd() { return m_bAnimEnd; }
	_bool	Get_End() { return m_bEnd; }
	void	Set_AnimEnd() { m_bAnimEnd = false; }
	void	Set_End() { m_bEnd = false; }
	void	Set_Loop() { m_isLoop = false; }
	vector<class CChannel*> Get_Channel() { return m_Channels; }
	void	Reset2();
	void	Reset3();
private:
	char				m_szName[MAX_PATH] = "";

	/* 현재 애니메이션 하나를 재생하는데 걸리는 총 시간. */
	_float				m_fDuration = 0.f;

	/* 1초에 재생되어야할 애니메이션의 속도. */
	_float				m_fTickPerSecond = 0.f;
	_float				m_fCurrentTime = 0.f;
	_float				m_fCurrentTime2 = 0.f;
	/* Channel : 뼈. */

	/* 이 애니메이션을 구동하는데 필요한 뼈의 갯수. */
	_uint							m_iNumChannels = 0;

	vector<class CChannel*>			m_Channels;

	_bool							m_isFinished = false;
	_bool							m_bAnimEnd = false;

	_bool							m_isLoop = true;
	_bool							m_bEnd = false;
public: // For. Data 추가
	void Get_AnimData(DATA_BINANIM* pAnimData);
public:
	static CAnimation* Create(class CModel* pModel, aiAnimation* pAIAnimation);
	static CAnimation* Bin_Create(DATA_BINANIM* pAIAnimation, class CModel* pModel); // 추가
	virtual void Free() override;
};

END