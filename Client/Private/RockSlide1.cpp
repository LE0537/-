#include "stdafx.h"
#include "..\Public\RockSlide1.h"

#include "GameInstance.h"
#include "Level_GamePlay.h"

CRockSlide1::CRockSlide1(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
}

CRockSlide1::CRockSlide1(const CRockSlide1 & rhs)
	: CGameObj(rhs)
{
}

HRESULT CRockSlide1::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CRockSlide1::Initialize(void * pArg)
{
	m_vTargetBattlePos = ((SKILLINFO*)pArg)->vTargetPos;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_fSize = (_float(rand() % 40 + 1) * 0.02f);
	_vector vScale = { m_fSize ,m_fSize ,m_fSize ,0.f };
	m_pTransformCom->Set_Scale(vScale);

	m_vTargetBattlePos.x += _float(rand() % 150 - 75.f) * 0.01f;
	m_vTargetBattlePos.y += 5.f;
	m_vTargetBattlePos.z += _float(rand() % 150 - 75.f) * 0.01f;

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4((&m_vTargetBattlePos)));

	m_iFrame = rand() % 9;

	return S_OK;
}

void CRockSlide1::Tick(_float fTimeDelta)
{
	Set_Pos(fTimeDelta);

}

void CRockSlide1::Late_Tick(_float fTimeDelta)
{

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);



	if (pGameInstance->IsInFrustum(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION), 10.f))
	{
		if (!g_PokeInfo && !g_bPokeDeck && nullptr != m_pRendererCom)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
	}

	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CRockSlide1::Render()
{

	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;


	return S_OK;
}
HRESULT CRockSlide1::Render_ShadowDepth()
{
	return E_NOTIMPL;
}
HRESULT CRockSlide1::Ready_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_RockSlide1"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;


	return S_OK;
}

void CRockSlide1::OnBillboard()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_matrix		ViewMatrix = XMMatrixInverse(nullptr, pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));

	_vector vScale = XMLoadFloat3(&m_pTransformCom->Get_Scale());

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, ViewMatrix.r[0] * vScale.m128_f32[0]);

	m_pTransformCom->Set_State(CTransform::STATE_UP, ViewMatrix.r[1] * vScale.m128_f32[1]);

	m_pTransformCom->Set_State(CTransform::STATE_LOOK, ViewMatrix.r[2] * vScale.m128_f32[2]);

	RELEASE_INSTANCE(CGameInstance);
}

void CRockSlide1::Set_Pos(_float fTimeDelta)
{
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	vPos.m128_f32[1] -= 7.f *  fTimeDelta;

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vPos);

	m_fDeadTime += fTimeDelta;
	if (m_fDeadTime > 0.6f)
	{
		Set_Dead();
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

		CLevel_GamePlay::LOADFILE tInfo;

		XMStoreFloat4(&tInfo.vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_RockSlide2"), LEVEL_GAMEPLAY, TEXT("Effect"), &tInfo)))
			return;
		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_RockSlide3"), LEVEL_GAMEPLAY, TEXT("Effect"), &tInfo)))
			return;
	
		RELEASE_INSTANCE(CGameInstance);
	}
}

HRESULT CRockSlide1::SetUp_ShaderResources()
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

	if (FAILED(m_pShaderCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_SRV(m_iFrame))))
		return E_FAIL;

	m_pShaderCom->Begin(15);
	m_pVIBufferCom->Render();

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CRockSlide1 * CRockSlide1::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CRockSlide1*	pInstance = new CRockSlide1(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CRockSlide1"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CRockSlide1::Clone(void * pArg)
{
	CGameObject*	pInstance = new CRockSlide1(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CRockSlide1"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRockSlide1::Free()
{
	__super::Free();


}
