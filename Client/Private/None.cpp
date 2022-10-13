#include "stdafx.h"
#include "..\Public\None.h"
#include "GameInstance.h"

CNone::CNone(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
}

CNone::CNone(const CNone & rhs)
	: CGameObj(rhs)
{
}

HRESULT CNone::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CNone::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;
	*(CGameObject**)pArg = this;

	m_ItemInfo.strName = TEXT("");
	m_ItemInfo.strInfo = TEXT("");
	m_ItemInfo.iItemNum = 99;
	m_ItemInfo.iNum = 0;
	m_ItemInfo.iPrice = 0;

	m_fSizeX = 40.f;
	m_fSizeY = 40.f;
	m_fX = 0.f;
	m_fY = 0.f;

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixTranspose(XMMatrixIdentity()));
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f)));

	_float3 vScale = { m_fSizeX,m_fSizeY,0.f };

	m_pTransformCom->Set_Scale(XMLoadFloat3(&vScale));

	return S_OK;
}

void CNone::Tick(_float fTimeDelta)
{

}

void CNone::Late_Tick(_float fTimeDelta)
{
	if (nullptr != m_pRendererCom && m_bInven)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CNone::Render()
{
	if (nullptr == m_pShaderCom || nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	return S_OK;
}

HRESULT CNone::Ready_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Item"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CNone::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_World4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_SRV(3))))
		return E_FAIL;

	m_pShaderCom->Begin();
	m_pVIBufferCom->Render();

	return S_OK;
}

CNone * CNone::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CNone*	pInstance = new CNone(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CNone"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CNone::Clone(void * pArg)
{
	CGameObject*	pInstance = new CNone(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CNone"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNone::Free()
{
	__super::Free();

}
