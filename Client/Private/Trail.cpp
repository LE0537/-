#include "stdafx.h"
#include "..\Public\Trail.h"

#include "GameInstance.h"
#include "Level_GamePlay.h"


CTrail::CTrail(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
}

CTrail::CTrail(const CTrail & rhs)
	: CGameObj(rhs)
{
}

HRESULT CTrail::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTrail::Initialize(void * pArg)
{
	*(CGameObject**)pArg = this;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	return S_OK;
}

void CTrail::Tick(_float fTimeDelta)
{

}

void CTrail::Late_Tick(_float fTimeDelta)
{
}

HRESULT CTrail::Render()
{

	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;


	return S_OK;
}
void CTrail::Set_Trail(CVIBuffer_Trail::BUFFERDESC _BUFFERDESC)
{
	m_pVIBufferTrailCom->Set_VBTrail(_BUFFERDESC);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}
void CTrail::Reset_Trail()
{
	m_pVIBufferTrailCom->Reset_VBTrail();
}
HRESULT CTrail::Ready_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Trail"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBufferTrailCom"), LEVEL_STATIC, TEXT("Prototype_Component_VIBufferTrailCom"), (CComponent**)&m_pVIBufferTrailCom)))
		return E_FAIL;


	return S_OK;
}



HRESULT CTrail::SetUp_ShaderResources()
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


	m_pShaderCom->Begin(18);
	m_pVIBufferTrailCom->Render();

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CTrail * CTrail::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTrail*	pInstance = new CTrail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CTrail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CTrail::Clone(void * pArg)
{
	CGameObject*	pInstance = new CTrail(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CTrail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTrail::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferTrailCom);
}
