#include "stdafx.h"
#include "..\Public\BallEffect.h"

#include "GameInstance.h"
#include "Level_GamePlay.h"

CBallEffect::CBallEffect(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
}

CBallEffect::CBallEffect(const CBallEffect & rhs)
	: CGameObj(rhs)
{
}

HRESULT CBallEffect::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBallEffect::Initialize(void * pArg)
{

	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_fSize = 5.f;
	_vector vScale = { m_fSize ,m_fSize ,m_fSize ,0.f };
	m_pTransformCom->Set_Scale(vScale);

	_float4 vPos = ((CLevel_GamePlay::LOADFILE*)pArg)->vPos;
	vPos.y += 1.f;
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&vPos));

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	for (_int i = 0; i < 200; ++i)
	{
		CLevel_GamePlay::LOADFILE tInfo;

		tInfo.vPos = vPos;

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_BallEffect2"), LEVEL_GAMEPLAY, TEXT("Layer_Effect"), &tInfo)))
			return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CBallEffect::Tick(_float fTimeDelta)
{
	Set_Pos(fTimeDelta);

}

void CBallEffect::Late_Tick(_float fTimeDelta)
{

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	OnBillboard();

	if (pGameInstance->IsInFrustum(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION), 10.f))
	{
		if (!g_PokeInfo && !g_bPokeDeck && nullptr != m_pRendererCom)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
	}

	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CBallEffect::Render()
{

	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;


	return S_OK;
}
HRESULT CBallEffect::Render_ShadowDepth()
{
	return E_NOTIMPL;
}
HRESULT CBallEffect::Ready_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_BallEffect"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;


	return S_OK;
}

void CBallEffect::OnBillboard()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_matrix		ViewMatrix = XMMatrixInverse(nullptr, pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));

	_vector vScale = XMLoadFloat3(&m_pTransformCom->Get_Scale());

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, ViewMatrix.r[0] * vScale.m128_f32[0]);

	m_pTransformCom->Set_State(CTransform::STATE_UP, ViewMatrix.r[1] * vScale.m128_f32[1]);

	m_pTransformCom->Set_State(CTransform::STATE_LOOK, ViewMatrix.r[2] * vScale.m128_f32[2]);

	RELEASE_INSTANCE(CGameInstance);
}

void CBallEffect::Set_Pos(_float fTimeDelta)
{
	m_fSize += 10.f * fTimeDelta;

	_vector vScale = { m_fSize ,m_fSize ,m_fSize ,0.f };
	m_pTransformCom->Set_Scale(vScale);

	m_fDeadTime += fTimeDelta;
	if (m_fDeadTime > 0.3f)
	{
		Set_Dead();
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

		CLevel_GamePlay::LOADFILE tInfo;

		XMStoreFloat4(&tInfo.vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_BallEffect3"), LEVEL_GAMEPLAY, TEXT("Layer_Effect"), &tInfo)))
			return;
		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_BallEffect5"), LEVEL_GAMEPLAY, TEXT("Layer_Effect"), &tInfo)))
			return;
		for (_int i = 0; i < 100; ++i)
		{
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_BallEffect6"), LEVEL_GAMEPLAY, TEXT("Layer_Effect"), &tInfo)))
				return;
		}
		RELEASE_INSTANCE(CGameInstance);

	}
}

HRESULT CBallEffect::SetUp_ShaderResources()
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

	if (FAILED(m_pShaderCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_SRV(0))))
		return E_FAIL;

	m_pShaderCom->Begin();
	m_pVIBufferCom->Render();

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CBallEffect * CBallEffect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBallEffect*	pInstance = new CBallEffect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CBallEffect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CBallEffect::Clone(void * pArg)
{
	CGameObject*	pInstance = new CBallEffect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CBallEffect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBallEffect::Free()
{
	__super::Free();


}
