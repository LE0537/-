#include "stdafx.h"
#include "..\Public\BallRed.h"

#include "GameInstance.h"
#include "Level_GamePlay.h"

CBallRed::CBallRed(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
}

CBallRed::CBallRed(const CBallRed & rhs)
	: CGameObj(rhs)
{
}

HRESULT CBallRed::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBallRed::Initialize(void * pArg)
{

	if (FAILED(Ready_Components()))
		return E_FAIL;

	

	_vector vPos = XMLoadFloat4(&((CLevel_GamePlay::LOADFILE*)pArg)->vPos);
	_vector vTargetPos = XMLoadFloat4(&((CLevel_GamePlay::LOADFILE*)pArg)->vTargetPos);
	vTargetPos.m128_f32[1] += 1.f;

	_vector vLook = vTargetPos - vPos;

	XMVector3Normalize(vLook);

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vLook);

	vPos += vLook * 0.55f;

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vPos);
	m_pTransformCom->Set_Scale(XMVectorSet(1.5f,0.15f,1.f,0.f));

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CLevel_GamePlay::LOADFILE tInfo;

	XMStoreFloat4(&tInfo.vPos, vTargetPos);

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_BallRed2"), LEVEL_GAMEPLAY, TEXT("Layer_Effect"), &tInfo)))
		return E_FAIL;
	

	RELEASE_INSTANCE(CGameInstance);


	return S_OK;
}

void CBallRed::Tick(_float fTimeDelta)
{
	m_fDeadTime += fTimeDelta;
	if (m_fDeadTime > 0.25f)
		Set_Dead();
}

void CBallRed::Late_Tick(_float fTimeDelta)
{


	if (!g_PokeInfo && !g_bPokeDeck && nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

}

HRESULT CBallRed::Render()
{

	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;


	return S_OK;
}
HRESULT CBallRed::Render_ShadowDepth()
{
	return E_NOTIMPL;
}
HRESULT CBallRed::Ready_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_BallRed"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;


	return S_OK;
}




HRESULT CBallRed::SetUp_ShaderResources()
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

	m_pShaderCom->Begin(13);
	m_pVIBufferCom->Render();

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CBallRed * CBallRed::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBallRed*	pInstance = new CBallRed(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CBallRed"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CBallRed::Clone(void * pArg)
{
	CGameObject*	pInstance = new CBallRed(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CBallRed"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBallRed::Free()
{
	__super::Free();


}
