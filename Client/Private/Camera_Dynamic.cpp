#include "stdafx.h"
#include "..\Public\Camera_Dynamic.h"
#include "GameInstance.h"
#include "GameObj.h"

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
	return S_OK;
}

void CCamera_Dynamic::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	if (!m_bEvent && !g_bBag && !g_bPokeDeck && !g_PokeInfo)
	{
		Key_Input(fTimeDelta);
	}
	if (m_bEvent)
	{
		EventCam(fTimeDelta);
	}
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
	if (!dynamic_cast<CGameObj*>(m_pTarget)->Get_PalyerInfo().bEvent)
		m_bEvent = false;

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
