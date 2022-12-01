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

	//Capture
	_float			m_fCaptureTime = 0.f;
	_bool			m_bCaptureCam = false;
public:
	void	Set_Target(CGameObject* _pTarget);
public:
	static CCamera_Dynamic* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END