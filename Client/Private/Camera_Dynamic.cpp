#include "stdafx.h"
#include "..\Public\Camera_Dynamic.h"
#include "GameInstance.h"
#include "GameObj.h"
#include "Level_GamePlay.h"
#include "Player.h"

CCamera_Dynamic::CCamera_Dynamic(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera(pDevice, pContext)
{
}

CCamera_Dynamic::CCamera_Dynamic(const CCamera_Dynamic & rhs)
	: CCamera(rhs)
{
}

HRESULT CCamera_Dynamic::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCamera_Dynamic::Initialize(void* pArg)
{
	*((CGameObject**)&((CAMERADESC_DERIVED*)pArg)->CameraDesc.pCamera) = this;
	
	if (FAILED(__super::Initialize(&((CAMERADESC_DERIVED*)pArg)->CameraDesc)))
		return E_FAIL;
	
	m_FovAngle = XMConvertToRadians(60.f);
	Load();
	m_vTargetBattlePos = dynamic_cast<CGameObj*>(m_CameraDesc.pTarget)->Get_TargetBattlePos();
	camLookPos.y += 3.f;


	fEndingFov = 60.f;

	return S_OK;
}

void CCamera_Dynamic::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (g_bEnding)
	{
	//	if(!bNavi)
			EndingPos(fTimeDelta);
	//	else if(bNavi)
	//		EndingKeyInput(fTimeDelta);
	}
	else if (g_bRace)
	{
		if(!g_bRaceEnd)
			RaceStartCamPos(fTimeDelta);
		else if(g_bRaceEnd)
			RaceEndCamPos(fTimeDelta);
	}
	else
	{
		if (g_bCaptureRender)
		{
			if (!m_bCaptureCam)
			{
				m_fCaptureTime = 0.f;
				m_bCaptureCam = true;
			}
		}
		if (!g_Battle)
		{
			if (!m_bEvent && !g_bBag && !g_bEvolution && !g_bPokeDeck && !g_PokeInfo)
			{
				Key_Input(fTimeDelta);
			}
			if (m_bEvent)
			{
				EventCam(fTimeDelta);
			}
		}
		else if (!g_bCaptureRender && !m_bCaptureCam)
		{
			if (dynamic_cast<CPlayer*>(m_CameraDesc.pTarget)->Get_PlayerHit())
			{
				PlayerHitCam(fTimeDelta);
			}
			else if (dynamic_cast<CPlayer*>(m_CameraDesc.pTarget)->Get_TargetHit())
			{
				TargetHitCam(fTimeDelta);
			}
			else
			{
				if (fSpeed < 1.4f)
					BattleEventcam(fTimeDelta);
				else
					Battlecam(fTimeDelta);
			}
		}
		if (m_bCaptureCam)
		{
			m_fCaptureTime += fTimeDelta;
			CapturePos();
			if (m_fCaptureTime > 3.5f)
				m_bCaptureCam = false;
		}
	}
	if(!g_bRace)
		ResetRace();

	if (FAILED(Bind_OnPipeLine()))
		return;
}

void CCamera_Dynamic::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

}

HRESULT CCamera_Dynamic::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	
	return S_OK;
}
void CCamera_Dynamic::Key_Input(_float fTimeDelta)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_long MouseMove = 0;

	if (MouseMove = pGameInstance->Get_DIMMoveState(DIMM_WHEEL))
	{
		m_FovAngle += XMConvertToRadians(-MouseMove * fTimeDelta * 0.4f);
		if ((XMConvertToRadians(80.f) < m_CameraDesc.fFovy))
		{
			m_CameraDesc.fFovy = XMConvertToRadians(80.f);
			m_FovAngle = XMConvertToRadians(80.f);
		}
		else if ((XMConvertToRadians(35.f) > m_CameraDesc.fFovy))
		{
			m_CameraDesc.fFovy = XMConvertToRadians(35.f);
			m_FovAngle = XMConvertToRadians(35.f);
		}
	}
	m_CameraDesc.fFovy = m_FovAngle;

	static _float fAngleX = 0.f;
	static _float fAngleY = 0.f;

	_long         MouseMoveX = 0;
	_long         MouseMoveY = 0;

	CTransform* pTransform = dynamic_cast<CGameObj*>(m_CameraDesc.pTarget)->Get_Transfrom();


	if (MouseMoveY = pGameInstance->Get_DIMMoveState(DIMM_Y))
	{

		fAngleX += MouseMoveY * fTimeDelta * 8.f;

		if (40.f <= fAngleX)
			fAngleX = 40.f;
		else if (-40.f >= fAngleX)
			fAngleX = -40.f;

	}

	if (MouseMoveX = pGameInstance->Get_DIMMoveState(DIMM_X))
	{

		fAngleY += MouseMoveX * fTimeDelta * 8.f;

		if (360.f <= fAngleY)
			fAngleY = 0.f;
		else if (0.f >= fAngleY)
			fAngleY = 360.f;

	}
	 
	_int iIndex = dynamic_cast<CPlayer*>(m_CameraDesc.pTarget)->Get_Portal();

	if (iIndex != 0)
	{
		switch (iIndex)
		{
		case 10:
			fAngleX = -3.f;
			fAngleY = 80.f;
			break;
		case 20:
			fAngleX = 3.f;
			fAngleY = -80.f;
			break;
		default:
			break;
		}
		dynamic_cast<CPlayer*>(m_CameraDesc.pTarget)->Set_Portal(0);
	}
	_matrix matRotX = XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(fAngleX));
	_matrix matRotY = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(fAngleY));

	_vector vCamDir = XMVector3TransformNormal(XMVectorSet(0.f, 1.f, -1.f, 0.f), matRotX);
	vCamDir = XMVector3TransformNormal(vCamDir, matRotY);
	_vector vCamPos = vCamDir * 5.f;
	_vector vDestPos = pTransform->Get_State(CTransform::STATE_TRANSLATION) + vCamPos;

	m_pTransform->Set_State(CTransform::STATE_TRANSLATION, vDestPos);

	_vector vLookPos = XMVectorSetY(pTransform->Get_State(CTransform::STATE_TRANSLATION), XMVectorGetY(pTransform->Get_State(CTransform::STATE_TRANSLATION)) + 0.8f);
	vLookPos.m128_f32[1] += 1.f;
	m_pTransform->LookAt(vLookPos);

	RELEASE_INSTANCE(CGameInstance);
}

void CCamera_Dynamic::EventCam(_float fTimeDelta)
{
	if (m_pTarget->Get_Dead())
	{
	//	fSpeed = 0.f;
		m_bEvent = false;
		return;
	}
	if (!dynamic_cast<CGameObj*>(m_pTarget)->Get_PalyerInfo().bEvent)
	{
		m_bEvent = false;
		//fSpeed = 0.f;
	}
	fSpeed = 0.f;
}

void CCamera_Dynamic::BattleEventcam(_float fTimeDelta)
{
	_vector TargetPos;
	fSpeed += 0.2f * fTimeDelta;

	_vector vPos = XMVectorCatmullRom(XMLoadFloat4(&camPos1), XMLoadFloat4(&camPos4)
		, XMLoadFloat4(&camPos3), XMLoadFloat4(&camPos2), fSpeed);

	if (fSpeed < 0.8f)
	{
		TargetPos = XMLoadFloat4(&m_vTargetBattlePos);
		m_CameraDesc.fFovy = XMConvertToRadians(50.f);
	}
	else
	{
		TargetPos = dynamic_cast<CGameObj*>(m_CameraDesc.pTarget)->Get_Transfrom()->Get_State(CTransform::STATE_TRANSLATION);
		m_CameraDesc.fFovy = XMConvertToRadians(90.f);
	}

	TargetPos.m128_f32[1] += 2.f;
	vPos.m128_f32[1] += 3.f;
	m_pTransform->Set_State(CTransform::STATE_TRANSLATION, vPos);
	m_pTransform->LookAt(TargetPos);
}

void CCamera_Dynamic::Battlecam(_float fTimeDelta)
{
	m_CameraDesc.fFovy = XMConvertToRadians(60.f);
	m_pTransform->Set_State(CTransform::STATE_TRANSLATION,XMLoadFloat4(&camLookPos));
	m_pTransform->LookAt(XMLoadFloat4(&camAt));
}

void CCamera_Dynamic::CapturePos()
{
	_vector vTarget = XMLoadFloat4(&m_vTargetBattlePos);
	_vector vPos = dynamic_cast<CGameObj*>(m_CameraDesc.pTarget)->Get_Transfrom()->Get_State(CTransform::STATE_TRANSLATION);
	_vector vLook = vTarget - vPos;

	vPos += XMVector3Normalize(vLook) * 2.f;
	vPos.m128_f32[1] += 1.1f;
	vTarget.m128_f32[1] += 0.3f;
	m_CameraDesc.fFovy = XMConvertToRadians(20.f);
	m_pTransform->Set_State(CTransform::STATE_TRANSLATION, vPos);
	m_pTransform->LookAt(vTarget);
}

void CCamera_Dynamic::EndingPos(_float fTimeDelta)
{
	if (!m_bOrigin)
	{
		XMStoreFloat4(&OriginPos, m_pTransform->Get_State(CTransform::STATE_TRANSLATION));
		m_bOrigin = true;
	}
	_vector TargetPos;
	if (fEndingSpeed < 1.5f)
	{
		fEndingSpeed += 0.1f * fTimeDelta;
	}
	EndingPos1, EndingPos2, EndingPos3, EndingAt1, EndingAt2;

	_vector vPos = XMVectorCatmullRom(XMLoadFloat4(&OriginPos), XMLoadFloat4(&EndingPos1)
		, XMLoadFloat4(&EndingPos2), XMLoadFloat4(&EndingPos3), fEndingSpeed);

	if (fEndingSpeed < 1.f)
	{
		TargetPos = XMLoadFloat4(&EndingAt2);
		
	}
	else if (fEndingSpeed <= 1.5f)
	{
		TargetPos = dynamic_cast<CGameObj*>(m_CameraDesc.pTarget)->Get_Transfrom()->Get_State(CTransform::STATE_TRANSLATION);
		fEndingFov -= 0.3f;
		if (fEndingFov < 3.f)
			fEndingFov = 3.f;
	}
	else
	{
		TargetPos = dynamic_cast<CGameObj*>(m_CameraDesc.pTarget)->Get_Transfrom()->Get_State(CTransform::STATE_TRANSLATION);
		fEndingFov -= 0.3f;
		if (fEndingFov < 3.f)
		{
			bNavi = true;
			fEndingFov = 3.f;
		}
	}
	//TargetPos.m128_f32[1] +=.f;
	vPos.m128_f32[1] += 15.f;
	m_CameraDesc.fFovy = XMConvertToRadians(fEndingFov);
	m_pTransform->Set_State(CTransform::STATE_TRANSLATION, vPos);
	m_pTransform->LookAt(TargetPos);

}

void CCamera_Dynamic::EndingKeyInput(_float fTimeDelta)
{
	if (GetKeyState('T') < 0)
	{
		m_pTransform->Go_StraightNoNavi(fTimeDelta * 10.f);
	}

	if (GetKeyState('G') < 0)
	{
		m_pTransform->Go_Backward(fTimeDelta * 10.f);
	}

	if (GetKeyState('F') < 0)
	{

		m_pTransform->Go_Left(fTimeDelta * 10.f);
	}

	if (GetKeyState('H') < 0)
	{

		m_pTransform->Go_Right(fTimeDelta * 10.f);
	}

	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	_long			MouseMove = 0;

	if (GetKeyState(VK_UP) < 0)
	{
		m_pTransform->Turn(m_pTransform->Get_State(CTransform::STATE_RIGHT), fTimeDelta * 1 * 0.1f * -1.f);
	}

	if (GetKeyState(VK_DOWN) < 0)
	{
		m_pTransform->Turn(m_pTransform->Get_State(CTransform::STATE_RIGHT), fTimeDelta * 1 * 0.1f);
	}


	if (GetKeyState(VK_LEFT) < 0)
	{
		m_pTransform->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * 1 * 0.1f * -1.f);
	}

	if (GetKeyState(VK_RIGHT) < 0)
	{
		m_pTransform->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * 1 * 0.1f);
	}

	Safe_Release(pGameInstance);
}

void CCamera_Dynamic::PlayerHitCam(_float fTimeDelta)
{
	m_fPlayerHitTime += fTimeDelta;
	if (m_fPlayerHitTime <= 0.7f)
	{
		_vector vTarget = XMLoadFloat4(&m_vTargetBattlePos);
		_vector vPos = dynamic_cast<CGameObj*>(m_CameraDesc.pTarget)->Get_Transfrom()->Get_State(CTransform::STATE_TRANSLATION);
		_vector vLook = vTarget - vPos;

		vPos += XMVector3Normalize(vLook) * 2.5f;
		vPos.m128_f32[1] += 1.5f;
		vTarget.m128_f32[1] += 1.1f;
		m_CameraDesc.fFovy = XMConvertToRadians(70.f);
		m_pTransform->Set_State(CTransform::STATE_TRANSLATION, vPos);
		m_pTransform->LookAt(vTarget);
	}
	//else if (m_fPlayerHitTime <= 1.5f && m_fPlayerHitTime > 0.7f)
	//{
	//	_vector vPos = XMLoadFloat4(&m_vTargetBattlePos);
	//	_vector vTarget = dynamic_cast<CGameObj*>(m_CameraDesc.pTarget)->Get_Transfrom()->Get_State(CTransform::STATE_TRANSLATION);
	//	_vector vLook = vTarget - vPos;

	//	vPos += XMVector3Normalize(vLook) * 2.f;
	//	vPos.m128_f32[1] += 1.1f;
	//	vTarget.m128_f32[1] += 0.8f;
	//	//m_CameraDesc.fFovy = XMConvertToRadians(60.f);
	//	m_pTransform->Set_State(CTransform::STATE_TRANSLATION, vPos);
	//	m_pTransform->LookAt(vTarget);
	//}
	else if (m_fPlayerHitTime > 0.7f)
	{
		m_fPlayerHitTime = 0.f;
		dynamic_cast<CPlayer*>(m_CameraDesc.pTarget)->Set_PlayerHit(false);
	}
}

void CCamera_Dynamic::TargetHitCam(_float fTimeDelta)
{
	m_fTargetHitTime += fTimeDelta;
	if (m_fTargetHitTime <= 0.7f)
	{
		_vector vPos = XMLoadFloat4(&m_vTargetBattlePos);
		_vector vTarget = dynamic_cast<CGameObj*>(m_CameraDesc.pTarget)->Get_Transfrom()->Get_State(CTransform::STATE_TRANSLATION);
		_vector vLook = vTarget - vPos;

		vPos += XMVector3Normalize(vLook) * 2.5f;
		vPos.m128_f32[1] += 1.5f;
		vTarget.m128_f32[1] += 1.1f;
		m_CameraDesc.fFovy = XMConvertToRadians(70.f);
		m_pTransform->Set_State(CTransform::STATE_TRANSLATION, vPos);
		m_pTransform->LookAt(vTarget);
	}
	//else if(m_fTargetHitTime <= 1.5f && m_fTargetHitTime > 0.7f)
	//{
	//	_vector vTarget = XMLoadFloat4(&m_vTargetBattlePos);
	//	_vector vPos = dynamic_cast<CGameObj*>(m_CameraDesc.pTarget)->Get_Transfrom()->Get_State(CTransform::STATE_TRANSLATION);
	//	_vector vLook = vTarget - vPos;

	//	vPos += XMVector3Normalize(vLook) * 2.f;
	//	vPos.m128_f32[1] += 1.1f;
	//	vTarget.m128_f32[1] += 0.8f;
	//	//m_CameraDesc.fFovy = XMConvertToRadians(60.f);
	//	m_pTransform->Set_State(CTransform::STATE_TRANSLATION, vPos);
	//	m_pTransform->LookAt(vTarget);
	//}
	else if (m_fTargetHitTime > 0.7f)
	{
		m_fTargetHitTime = 0.f;
		dynamic_cast<CPlayer*>(m_CameraDesc.pTarget)->Set_TargetHit(false);
	}
}

void CCamera_Dynamic::RaceStartCamPos(_float fTimeDelta)
{
	if (g_iRaceNum == 99)
	{
		if (!m_bRaceStart)
		{
			for (auto& iter : m_vecRace)
			{
				if (iter.iType == 30)
				{
					RacePos1 = iter.vPos;
					RacePos1.y += 1.7f;
				}
				if (iter.iType == 31)
				{
					RacePos2 = iter.vPos;
					RacePos2.y += 1.7f;
				}
				if (iter.iType == 32)
				{
					RacePos3 = iter.vPos;
					RacePos3.y += 1.7f;
				}
				if (iter.iType == 33)
				{
					RacePos4 = iter.vPos;
					RacePos4.y += 1.7f;
				}
				if (iter.iType == 20)
				{
					RaceAt = iter.vPos;
					RaceAt.y += 1.5f;
				}
				if (iter.iType == 21)
				{
					vRaceStartLook = iter.vPos;
					vRaceStartLook.y += 1.f;
				}
				if (iter.iType == 12)
				{
					m_vRaceAt1 = iter.vPos;
					m_vRaceAt1.y += 1.5f;
				}
				if (iter.iType == 13)
				{
					m_vRaceAt2 = iter.vPos;
					m_vRaceAt2.y += 1.5f;
				}
				if (iter.iType == 14)
				{
					m_vRaceAt3 = iter.vPos;
					m_vRaceAt3.y += 1.5f;
				}
				if (iter.iType == 15)
				{
					m_vRaceAt4 = iter.vPos;
					m_vRaceAt4.y += 1.5f;
				}
				if (iter.iType == 16)
				{
					m_vRace1 = iter.vPos;
					m_vRace1.y += 1.8f;
				}
				if (iter.iType == 17)
				{
					m_vRace2 = iter.vPos;
					m_vRace2.y += 1.8f;
				}
				if (iter.iType == 18)
				{
					m_vRace3 = iter.vPos;
					m_vRace3.y += 1.8f;
				}
				if (iter.iType == 19)
				{
					m_vRace4 = iter.vPos;
					m_vRace4.y += 1.8f;
				}
				if (iter.iType == 22)
				{
					m_vCamPoint1 = iter.vPos;
					m_vCamPoint1.y += 1.8f;
				}
				if (iter.iType == 23)
				{
					m_vCamLook1 = iter.vPos;
					m_vCamLook1.y += 1.5f;
				}
				if (iter.iType == 24)
				{
					m_vCamPoint2 = iter.vPos;
					m_vCamPoint2.y += 2.8f;
				}
				if (iter.iType == 25)
				{
					m_vCamLook2 = iter.vPos;
					m_vCamLook2.y += 1.f;
				}
				if (iter.iType == 26)
				{
					m_vCamPoint3 = iter.vPos;
					m_vCamPoint3.y += 1.f;
				}
				if (iter.iType == 27)
				{
					m_vCamLook3 = iter.vPos;
					m_vCamLook3.y += 1.f;
				}
				if (iter.iType == 28)
				{
					m_vRaceEndCamLook = RaceAt;
					m_vRaceEndCamLook.y -= 0.5f;
				}
				if (iter.iType == 29)
				{
					m_vRaceEndCamPos = RaceAt;
					m_vRaceEndCamPos.x -= 10.f;
				//	m_vRaceEndCamPos.y += 1.f;
				}
			}
			m_bRaceStart = true;
		}
		if (m_fRaceStartTime <= 1.5f)
		{
			m_fRaceStartTime += 0.3f * fTimeDelta;

			_vector vPos = XMVectorCatmullRom(XMLoadFloat4(&RacePos1), XMLoadFloat4(&RacePos2)
				, XMLoadFloat4(&RacePos3), XMLoadFloat4(&RacePos4), m_fRaceStartTime);

			m_CameraDesc.fFovy = XMConvertToRadians(70.f);

			vPos.m128_f32[1] += 0.5f;
			m_pTransform->Set_State(CTransform::STATE_TRANSLATION, vPos);
			m_pTransform->LookAt(XMLoadFloat4(&RaceAt));
		}
		else if (m_fRaceStartTime < 4.5f && m_fRaceStartTime > 1.5f)
		{
			m_fRaceStartTime += fTimeDelta;
			if (!m_bRaceAtUp)
			{
				RaceAt.y += 3.f;
				m_bRaceAtUp = true;
			}
			m_CameraDesc.fFovy = XMConvertToRadians(40.f);
			m_pTransform->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&RaceAt));
			m_pTransform->LookAt(XMLoadFloat4(&vRaceStartLook));
		}
		else if (m_fRaceStartTime < 7.5f && m_fRaceStartTime >= 4.5f)
		{
			m_fRaceStartTime += fTimeDelta;
			m_pTransform->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&m_vRace1));
			m_pTransform->LookAt(XMLoadFloat4(&m_vRaceAt1));
		}
		else if (m_fRaceStartTime < 10.5f && m_fRaceStartTime >= 7.5f)
		{
			m_fRaceStartTime += fTimeDelta;
			m_pTransform->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&m_vRace2));
			m_pTransform->LookAt(XMLoadFloat4(&m_vRaceAt2));
		}
		else if (m_fRaceStartTime < 13.5f && m_fRaceStartTime >= 10.5f)
		{
			m_fRaceStartTime += fTimeDelta;
			m_pTransform->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&m_vRace3));
			m_pTransform->LookAt(XMLoadFloat4(&m_vRaceAt3));
		}
		else if (m_fRaceStartTime < 16.5f && m_fRaceStartTime >= 13.5f)
		{
			m_fRaceStartTime += fTimeDelta;
			m_pTransform->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&m_vRace4));
			m_pTransform->LookAt(XMLoadFloat4(&m_vRaceAt4));
		}
		else if (m_fRaceStartTime >= 16.5f)
		{
			m_pTransform->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&RaceAt));
			m_pTransform->LookAt(XMLoadFloat4(&vRaceStartLook));
		}

	}
	else
	{
		m_fRaceTime += fTimeDelta;
		if (m_fRaceTime < 7.5f && m_fRaceTime >= 3.5f)
		{
			m_CameraDesc.fFovy = XMConvertToRadians(25.f);
		}
		else if (m_fRaceTime < 12.5f && m_fRaceTime >= 7.5f)
		{
			m_CameraDesc.fFovy = XMConvertToRadians(50.f);
			m_pTransform->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&m_vCamPoint1));
			m_pTransform->LookAt(XMLoadFloat4(&m_vCamLook1));
		}
		else if (m_fRaceTime < 22.5f && m_fRaceTime >= 12.5f)
		{
			m_CameraDesc.fFovy = XMConvertToRadians(40.f);
			m_pTransform->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&m_vCamPoint2));
			m_pTransform->LookAt(XMLoadFloat4(&m_vCamLook2));
		}
		else if (m_fRaceTime >= 22.5f)
		{
			m_CameraDesc.fFovy = XMConvertToRadians(70.f);
			m_pTransform->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&m_vCamPoint3));
			m_pTransform->LookAt(XMLoadFloat4(&m_vCamLook3));
		}
	}
}

void CCamera_Dynamic::RaceEndCamPos(_float fTimeDelta)
{
	m_CameraDesc.fFovy = XMConvertToRadians(50.f);
	m_pTransform->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&m_vRaceEndCamPos));
	m_pTransform->LookAt(XMLoadFloat4(&m_vRaceEndCamLook));
}

void CCamera_Dynamic::ResetRace()
{
	m_bRaceStart = false;
	m_fRaceStartTime = 0.f;
	m_bRaceAtUp = false;
	m_bCheckRacePoke = false;
	m_fRaceTime = 0.f;
	RaceAt.y -= 3.f;
}

void CCamera_Dynamic::Set_RaceInfo(_float4 vPos, _int iType)
{
	RaceInfo tInfo;
	tInfo.iType = iType;
	tInfo.vPos = vPos;
	m_vecRace.push_back(tInfo);
}

void CCamera_Dynamic::Set_Target(CGameObject * _pTarget)
{
	m_pTarget = _pTarget;
	m_bEvent = true;
	_vector vPos = dynamic_cast<CGameObj*>(m_pTarget)->Get_Transfrom()->Get_State(CTransform::STATE_TRANSLATION);
	_vector vTargetPos = dynamic_cast<CGameObj*>(m_pTarget)->Get_Transfrom()->Get_State(CTransform::STATE_TRANSLATION);
	_vector vLook = dynamic_cast<CGameObj*>(m_pTarget)->Get_Transfrom()->Get_State(CTransform::STATE_LOOK);
	_vector vUp = dynamic_cast<CGameObj*>(m_pTarget)->Get_Transfrom()->Get_State(CTransform::STATE_UP);
	XMStoreFloat4(&m_TargetPos, vPos);
	XMVector3Normalize(vLook);
	XMVector3Normalize(vUp);
	vPos += vLook * 100.f;
	vPos += vUp * 60.f;
	m_fOriginY = vPos.m128_f32[1];
	m_pTransform->Set_State(CTransform::STATE_TRANSLATION, vPos);
	vTargetPos += vUp * 10.f;
	m_pTransform->LookAt(vTargetPos);
}

void CCamera_Dynamic::Load()
{
	HANDLE		hFile = CreateFile(L"../Data/BattlePos.dat",			// 파일 경로와 이름 명시
		GENERIC_READ,				// 파일 접근 모드 (GENERIC_WRITE 쓰기 전용, GENERIC_READ 읽기 전용)
		NULL,						// 공유방식, 파일이 열려있는 상태에서 다른 프로세스가 오픈할 때 허용할 것인가, NULL인 경우 공유하지 않는다
		NULL,						// 보안 속성, 기본값	
		OPEN_EXISTING,				// 생성 방식, CREATE_ALWAYS는 파일이 없다면 생성, 있다면 덮어 쓰기, OPEN_EXISTING 파일이 있을 경우에면 열기
		FILE_ATTRIBUTE_NORMAL,		// 파일 속성(읽기 전용, 숨기 등), FILE_ATTRIBUTE_NORMAL 아무런 속성이 없는 일반 파일 생성
		NULL);						// 생성도리 파일의 속성을 제공할 템플릿 파일, 우리는 사용하지 않아서 NULL

	if (INVALID_HANDLE_VALUE == hFile)
	{
		// 팝업 창을 출력해주는 기능의 함수
		// 1. 핸들 2. 팝업 창에 띄우고자하는 메시지 3. 팝업 창 이름 4. 버튼 속성
		MessageBox(g_hWnd, TEXT("Load File"), TEXT("Fail"), MB_OK);
		return;
	}

	// 2. 파일 쓰기

	DWORD		dwByte = 0;
	CLevel_GamePlay::SaveInfo		tInfo;
	
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	while (true)
	{
		ReadFile(hFile, &tInfo, sizeof(CLevel_GamePlay::SaveInfo), &dwByte, nullptr);

		if (0 == dwByte)	// 더이상 읽을 데이터가 없을 경우
			break;
		
		
		if (tInfo.iType == 2)
		{
			if (tInfo.vScale.y == 0.01f)
				camPos1 = tInfo.vPos;
			else if (tInfo.vScale.y == 0.02f)
				camPos2 = tInfo.vPos;
			else if (tInfo.vScale.y == 0.03f)
				camPos3 = tInfo.vPos;
			else if (tInfo.vScale.y == 0.04f)
				camPos4 = tInfo.vPos;
		}
		else if (tInfo.iType == 4)
		{
			if (tInfo.vScale.y == 0.005f)
				camLookPos = tInfo.vPos;
			else if (tInfo.vScale.y == 0.01f)
				camAt = tInfo.vPos;
		}
	}

	Safe_Release(pGameInstance);
	// 3. 파일 소멸
	CloseHandle(hFile);
}

CCamera_Dynamic * CCamera_Dynamic::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamera_Dynamic*	pInstance = new CCamera_Dynamic(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CCamera_Dynamic"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCamera_Dynamic::Clone(void* pArg)
{
	CCamera_Dynamic*	pInstance = new CCamera_Dynamic(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CCamera_Dynamic"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Dynamic::Free()
{
	__super::Free();

	
}
