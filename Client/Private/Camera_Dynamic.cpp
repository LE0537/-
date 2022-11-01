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
	
	if (FAILED(__super::Initialize(&((CAMERADESC_DERIVED*)pArg)->CameraDesc)))
		return E_FAIL;
	
	m_FovAngle = XMConvertToRadians(60.f);
	return S_OK;
}

void CCamera_Dynamic::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	if (!g_bBag && !g_bPokeDeck && !g_PokeInfo)
	{
		Key_Input(fTimeDelta);
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
		else if ((XMConvertToRadians(45.f) > m_CameraDesc.fFovy))
		{
			m_CameraDesc.fFovy = XMConvertToRadians(45.f);
			m_FovAngle = XMConvertToRadians(45.f);
		}
	}
	m_CameraDesc.fFovy = m_FovAngle;

	if (GetKeyState(VK_LSHIFT) & 0x8000 && GetKeyState(VK_LEFT) & 0x8000)
		CameraRotationX(fTimeDelta, -30.f);
	if (GetKeyState(VK_LSHIFT) & 0x8000 && GetKeyState(VK_RIGHT) & 0x8000)
		CameraRotationX(fTimeDelta, 30.f);
	if (GetKeyState(VK_LSHIFT) & 0x8000 && GetKeyState(VK_UP) & 0x8000)
		CameraRotationY(fTimeDelta, 20.f);
	if (GetKeyState(VK_LSHIFT) & 0x8000 && GetKeyState(VK_DOWN) & 0x8000)
		CameraRotationY(fTimeDelta, -20.f);

	Check_Pos();

	RELEASE_INSTANCE(CGameInstance);
}

void CCamera_Dynamic::CameraRotationX(_float fTimeDelta, _float fIncrease)
{
	m_XfAngle += XMConvertToRadians(180.f) * fIncrease * fTimeDelta;
}

void CCamera_Dynamic::CameraRotationY(_float fTimeDelta, _float fIncrease)
{
	m_YfAngle += XMConvertToRadians(90.f) * fIncrease * fTimeDelta;
	if (m_YfAngle > XMConvertToRadians(1000.f))
		m_YfAngle = XMConvertToRadians(1000.f);
	if (m_YfAngle < XMConvertToRadians(-1000.f))
		m_YfAngle = XMConvertToRadians(-1000.f);
}

void CCamera_Dynamic::Check_Pos()
{
	_vector vTargetPos = dynamic_cast<CGameObj*>(m_CameraDesc.pTarget)->Get_Transfrom()->Get_State(CTransform::STATE_TRANSLATION);
	_vector vPos = dynamic_cast<CGameObj*>(m_CameraDesc.pTarget)->Get_Transfrom()->Get_State(CTransform::STATE_TRANSLATION);

	_vector	vRight = dynamic_cast<CGameObj*>(m_CameraDesc.pTarget)->Get_Transfrom()->Get_State(CTransform::STATE_RIGHT);
	_vector	vUp = dynamic_cast<CGameObj*>(m_CameraDesc.pTarget)->Get_Transfrom()->Get_State(CTransform::STATE_UP);
	_vector	vLook = dynamic_cast<CGameObj*>(m_CameraDesc.pTarget)->Get_Transfrom()->Get_State(CTransform::STATE_LOOK);

	_matrix	RotationMatrixX = XMMatrixRotationAxis(vRight, XMConvertToRadians(m_YfAngle));

	vRight = XMVector3TransformNormal(vRight, RotationMatrixX);
	vUp = XMVector3TransformNormal(vUp, RotationMatrixX);
	vLook = XMVector3TransformNormal(vLook, RotationMatrixX);

	_matrix	RotationMatrixY = XMMatrixRotationAxis(vUp, XMConvertToRadians(m_XfAngle));

	vRight = XMVector3TransformNormal(vRight, RotationMatrixY);
	vUp = XMVector3TransformNormal(vUp, RotationMatrixY);
	vLook = XMVector3TransformNormal(vLook, RotationMatrixY);

	
	vPos = vPos + (XMVector3Normalize(vLook) * -5.f);
	vPos = vPos + (XMVector3Normalize(vUp) * 2.f);
	m_pTransform->Set_State(CTransform::CTransform::STATE_TRANSLATION, vPos);
	vTargetPos.m128_f32[1] += 2.f;
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
