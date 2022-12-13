#include "stdafx.h"
#include "..\Public\HydroPump2.h"

#include "GameInstance.h"
#include "Level_GamePlay.h"

CHydroPump2::CHydroPump2(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
}

CHydroPump2::CHydroPump2(const CHydroPump2 & rhs)
	: CGameObj(rhs)
{
}

HRESULT CHydroPump2::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHydroPump2::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_fSize = 0.2f;
	_vector vScale = { m_fSize , m_fSize,m_fSize ,0.f };


	m_vMyBattlePos = ((SKILLINFO*)pArg)->vMyPos;
	m_vTargetBattlePos = ((SKILLINFO*)pArg)->vTargetPos;

	m_vPos = m_vTargetBattlePos;
	_float fY = (_float)(rand() % 60 + 1) * 0.01f;

	m_vMyBattlePos.y += 0.8f + fY;
	m_vTargetBattlePos.y += 0.8f + fY;

	_vector vMyPos = XMLoadFloat4(&m_vMyBattlePos);
	_vector vTargetPos = XMLoadFloat4(&m_vTargetBattlePos);
	_vector		vDist = vTargetPos - vMyPos;
	XMVector3Normalize(vDist);
	vMyPos += vDist * 0.15f;

	_vector vRight = XMVector3Cross(XMVectorSet(0.f, 0.f, 1.f, 0.f), vDist);
	_vector vLook = XMVector3Cross(vRight, vDist);

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight * vScale.m128_f32[0]);
	m_pTransformCom->Set_State(CTransform::STATE_UP, vDist * vScale.m128_f32[1]);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook * vScale.m128_f32[2]);
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vMyPos);


	return S_OK;
}

void CHydroPump2::Tick(_float fTimeDelta)
{
	Set_Pos(fTimeDelta);

}

void CHydroPump2::Late_Tick(_float fTimeDelta)
{

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	//OnBillboard();

	if (pGameInstance->IsInFrustum(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION), 10.f))
	{
		if (!g_PokeInfo && !g_bPokeDeck && nullptr != m_pRendererCom)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
	}

	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CHydroPump2::Render()
{

	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;


	return S_OK;
}
HRESULT CHydroPump2::Ready_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_HydroPump1"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;


	return S_OK;
}

void CHydroPump2::OnBillboard()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_matrix		ViewMatrix = XMMatrixInverse(nullptr, pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));

	_vector vScale = XMLoadFloat3(&m_pTransformCom->Get_Scale());

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, ViewMatrix.r[0] * vScale.m128_f32[0]);

	m_pTransformCom->Set_State(CTransform::STATE_UP, ViewMatrix.r[1] * vScale.m128_f32[1]);

	m_pTransformCom->Set_State(CTransform::STATE_LOOK, ViewMatrix.r[2] * vScale.m128_f32[2]);

	RELEASE_INSTANCE(CGameInstance);
}

void CHydroPump2::Set_Pos(_float fTimeDelta)
{
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_vector vUp = m_pTransformCom->Get_State(CTransform::STATE_UP);
	
	vPos += XMVector3Normalize(vUp) * 6.2f * fTimeDelta;
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vPos);
	m_SkillTime += fTimeDelta;
	if (m_SkillTime > 0.15f)
	{
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

		CLevel_GamePlay::LOADFILE tInfo;
		XMStoreFloat4(&tInfo.vPos,m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_HydroPump4"), LEVEL_GAMEPLAY, TEXT("Effect"), &tInfo)))
			return;

		RELEASE_INSTANCE(CGameInstance);
		m_SkillTime = 0.f;
	}

	m_fDeadTime += fTimeDelta;

	if (m_fDeadTime > 0.7f)
		Set_Dead();

}

HRESULT CHydroPump2::SetUp_ShaderResources()
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

	if (FAILED(m_pShaderCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_SRV(1))))
		return E_FAIL;

	m_pShaderCom->Begin(16);
	m_pVIBufferCom->Render();

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CHydroPump2 * CHydroPump2::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CHydroPump2*	pInstance = new CHydroPump2(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CHydroPump2"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CHydroPump2::Clone(void * pArg)
{
	CGameObject*	pInstance = new CHydroPump2(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CHydroPump2"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHydroPump2::Free()
{
	__super::Free();


}
