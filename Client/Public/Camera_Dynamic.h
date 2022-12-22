#pragma once

#include "Client_Defines.h"
#include "Camera.h"


BEGIN(Engine)
END

BEGIN(Client)

class CCamera_Dynamic final : public CCamera
{
public:
	typedef struct tagCameraDesc_Derived
	{
		CCamera::CAMERADESC			CameraDesc;
	}CAMERADESC_DERIVED;
	struct RaceInfo {
		_float4  vPos;
		_int	 iType;
	};
private:
	CCamera_Dynamic(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Dynamic(const CCamera_Dynamic& rhs);
	virtual ~CCamera_Dynamic() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual void Tick(_float fTimeDelta)override;
	virtual void Late_Tick(_float fTimeDelta)override;
	virtual HRESULT Render() override;
private:
	void	Load();
	void	Key_Input(_float fTimeDelta);
	void	EventCam(_float fTimeDelta);
	void	BattleEventcam(_float fTimeDelta);
	void	Battlecam(_float fTimeDelta);
	void	CapturePos();
	void	EndingPos(_float fTimeDelta);
	void	EndingKeyInput(_float fTimeDelta);
	void	PlayerHitCam(_float fTimeDelta);
	void	TargetHitCam(_float fTimeDelta);
	void	RaceStartCamPos(_float fTimeDelta);
	void	RaceEndCamPos(_float fTimeDelta);
	void	ResetRace();
private:
	_float		 m_YfAngle = 0.f;
	_float		 m_XfAngle = 0.f;

	_float		 m_FovAngle = 0.f;

	_bool		 m_bEvent = false;

	CGameObject*	m_pTarget = nullptr;
	_float4			 m_TargetPos;
	_float			m_fOriginY = 0.f;

	_float4			camPos1, camPos2, camPos3, camPos4, camLookPos, camAt;
	_float4			m_vTargetBattlePos;
	_float			fSpeed = 0.f;
	_float			m_fBattletime = 0.f;

	//Ending
	_bool			m_bOrigin = false;
	_float4			OriginPos;
	_float4			EndingPos1, EndingPos2, EndingPos3, EndingAt1, EndingAt2;
	_float			fEndingSpeed = 0.f;
	_float			fEndingFov = 0.f;
	_bool			bNavi = false;
	//Capture
	_float			m_fCaptureTime = 0.f;
	_bool			m_bCaptureCam = false;

	//Battle
	_float			m_fPlayerHitTime = 0.f;
	_float			m_fTargetHitTime = 0.f;

	//Race
	vector<RaceInfo>	m_vecRace;
	_bool			m_bRaceStart = false;
	_float			m_fRaceStartTime = 0.f;
	_float4			RacePos1, RacePos2, RacePos3, RacePos4, RaceAt;
	_float4			vRaceStartLook;
	_float4			m_vRace1, m_vRace2, m_vRace3, m_vRace4;
	_float4			m_vRaceAt1, m_vRaceAt2, m_vRaceAt3, m_vRaceAt4;
	_float4			m_vCamPoint1, m_vCamPoint2, m_vCamPoint3;
	_float4         m_vCamLook1, m_vCamLook2, m_vCamLook3;
	_float4			m_vRaceEndCamPos, m_vRaceEndCamLook;
	_bool			m_bRaceAtUp = false;
	_bool			m_bCheckRacePoke = false;

	_float			m_fRaceTime = 0.f;
public:
	void	Set_RaceInfo(_float4  vPos, _int	 iType);
	void	Set_Target(CGameObject* _pTarget);
	void	Set_Ending(_float4 _EndingPos1, _float4 _EndingPos2, _float4 _EndingPos3, _float4 _EndingAt1, _float4 _EndingAt2) {
		EndingPos1 = _EndingPos1;
		EndingPos2 = _EndingPos2;
		EndingPos3 = _EndingPos3;
		EndingAt1 = _EndingAt1;
		EndingAt2 = _EndingAt2;
	}
public:
	static CCamera_Dynamic* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END