#include "stdafx.h"
#include "..\Public\Weed.h"

#include "GameInstance.h"
#include "Level_GamePlay.h"

CWeed::CWeed(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
}

CWeed::CWeed(const CWeed & rhs)
	: CGameObj(rhs)
{
}

HRESULT CWeed::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWeed::Initialize(void * pArg)
{

	*(CGameObject**)pArg = this;

	m_iWeed = g_iWeed;
	++g_iWeed;
	if (FAILED(Ready_Components()))
		return E_FAIL;


	return S_OK;
}

void CWeed::Tick(_float fTimeDelta)
{
	if (!m_bTrue)
	{
		m_pVIBufferInstanceCom->Set_Pos(m_listPos, m_listScale);
		m_bTrue = true;
	}
}

void CWeed::Late_Tick(_float fTimeDelta)
{

	if (!g_Battle)
	{
		if (!g_PokeInfo && !g_bPokeDeck && nullptr != m_pRendererCom)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
	}
}

HRESULT CWeed::Render()
{

	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;


	return S_OK;
}
HRESULT CWeed::Ready_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Weed"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPointInstance"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	switch (m_iWeed)
	{
	case 0:
		if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_PointInstance"), (CComponent**)&m_pVIBufferInstanceCom)))
			return E_FAIL;
		break;
	case 1:
		if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_PointInstance2"), (CComponent**)&m_pVIBufferInstanceCom)))
			return E_FAIL;
		break;
	case 2:
		if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_PointInstance3"), (CComponent**)&m_pVIBufferInstanceCom)))
			return E_FAIL;
		break;
	default:
		break;
	}
	


	return S_OK;
}


HRESULT CWeed::SetUp_ShaderResources()
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

	if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_SRV(m_iWeed))))
		return E_FAIL;

	m_pShaderCom->Begin(0);
	m_pVIBufferInstanceCom->Render();

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CWeed * CWeed::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CWeed*	pInstance = new CWeed(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CWeed"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CWeed::Clone(void * pArg)
{
	CGameObject*	pInstance = new CWeed(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CWeed"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWeed::Free()
{
	__super::Free();


	m_listPos.clear();
	m_listScale.clear();

	Safe_Release(m_pVIBufferInstanceCom);
}
