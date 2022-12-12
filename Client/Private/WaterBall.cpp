#include "stdafx.h"
#include "..\Public\WaterBall.h"

#include "GameInstance.h"
#include "Level_GamePlay.h"

CWaterBall::CWaterBall(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
}

CWaterBall::CWaterBall(const CWaterBall & rhs)
	: CGameObj(rhs)
{
}

HRESULT CWaterBall::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWaterBall::Initialize(void * pArg)
{
	m_vMyBattlePos = ((SKILLINFO*)pArg)->vMyPos;
	m_vTargetBattlePos = ((SKILLINFO*)pArg)->vTargetPos;

	_vector vMyPos = XMLoadFloat4(&m_vMyBattlePos);
	_vector vTargetPos = XMLoadFloat4(&m_vTargetBattlePos);
	_vector		vDist = vTargetPos - vMyPos;
	vMyPos += XMVector3Normalize(vDist) * 0.25f;

	vMyPos.m128_f32[1] += 1.f;
	vTargetPos.m128_f32[1] += 1.f;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	_float3 vScale = { 0.01f,0.01f,0.01f};

	m_pTransformCom->Set_Scale(XMLoadFloat3(&vScale));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vMyPos);
	m_pTransformCom->LookAt(vTargetPos);

	_float fUp = rand() % 31 - 15.f;
	_float fRight = rand() % 31 - 15.f;

	m_pTransformCom->Turn2(XMVectorSet(0.f,1.f,0.f,0.f),XMConvertToRadians(fUp));
	m_pTransformCom->Turn2(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), XMConvertToRadians(fRight));

	XMStoreFloat4(&m_vLook, XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)));
	
	return S_OK;
}

void CWaterBall::Tick(_float fTimeDelta)
{

	Set_Pos(fTimeDelta);
	m_fDeadTime += fTimeDelta;
	if (m_fDeadTime > 1.5f)
	{
		Set_Dead();
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

		CLevel_GamePlay::LOADFILE tInfo;
		XMStoreFloat4(&tInfo.vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
		tInfo.vScale = { 0.f,0.f,0.f };

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_WaterGun3"), LEVEL_GAMEPLAY, TEXT("Layer_Effect"), &tInfo)))
			return;
	
		RELEASE_INSTANCE(CGameInstance);
	}
}

void CWaterBall::Late_Tick(_float fTimeDelta)
{

	if (g_Battle && nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
	
}

HRESULT CWaterBall::Render()
{

	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshContainers();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 4)))
			return E_FAIL;
	}



	
	return S_OK;
}

HRESULT CWaterBall::Ready_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model*/

	if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_STATIC, TEXT("Prototype_Component_Model_WaterBall"), (CComponent**)&m_pModelCom)))
		return E_FAIL;


	return S_OK;
}

HRESULT CWaterBall::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_World4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CWaterBall * CWaterBall::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CWaterBall*	pInstance = new CWaterBall(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CWaterBall"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObj * CWaterBall::Clone(void * pArg)
{
	CWaterBall*	pInstance = new CWaterBall(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CWaterBall"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWaterBall::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);

}

void CWaterBall::Set_Pos(_float fTimeDelta)
{
	

	_vector vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);

	m_pTransformCom->Turn(vRight, fTimeDelta * 2.f);

	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

	m_pTransformCom->Turn(vLook, fTimeDelta * 2.f);

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	vPos += XMLoadFloat4(&m_vLook) * 3.5f * fTimeDelta;

	m_fScale += fTimeDelta / 6.f;

	if (m_fScale > 0.25f)
		m_fScale = 0.25f;

	_vector vScale = { m_fScale,m_fScale,m_fScale,0.f};
	m_pTransformCom->Set_Scale(vScale);
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vPos);
}