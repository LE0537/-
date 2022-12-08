#include "stdafx.h"
#include "..\Public\EvolLight3.h"

#include "GameInstance.h"

CEvolLight3::CEvolLight3(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
}

CEvolLight3::CEvolLight3(const CEvolLight3 & rhs)
	: CGameObj(rhs)
{
}

HRESULT CEvolLight3::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEvolLight3::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_fSizeX = 100.f;
	m_fSizeY = 100.f;

	m_fX = (g_iWinSizeX >> 1) + (rand() % 400 - 200.f);
	m_fY = (g_iWinSizeY >> 1) + (rand() % 200 - 100.f);

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixTranspose(XMMatrixIdentity()));
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, -200.f, 100.f)));

	_float3 vScale = { m_fSizeX,m_fSizeY,0.f };

	m_pTransformCom->Set_Scale(XMLoadFloat3(&vScale));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));

	return S_OK;
}

void CEvolLight3::Tick(_float fTimeDelta)
{
	Set_Pos(fTimeDelta);

	if (!g_bEvolution)
		Set_Dead();
}

void CEvolLight3::Late_Tick(_float fTimeDelta)
{
	if (g_bEvolution && nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CEvolLight3::Render()
{
	if (nullptr == m_pShaderCom || nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEvolLight3::Ready_Components()
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
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_EvolLight"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;


	return S_OK;
}

HRESULT CEvolLight3::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_World4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_SRV(m_iFrame))))
		return E_FAIL;

	m_pShaderCom->Begin(10);
	m_pVIBufferCom->Render();


	return S_OK;
}

void CEvolLight3::Set_Pos(_float fTimeDelta)
{
	m_fFrameTime += fTimeDelta;
	if (m_fFrameTime > 0.05f)
	{
		++m_iFrame;
		m_fFrameTime = 0.f;
	}
	if (m_iFrame >= 16)
		Set_Dead();
}




CEvolLight3 * CEvolLight3::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CEvolLight3*	pInstance = new CEvolLight3(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CEvolLight3"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CEvolLight3::Clone(void * pArg)
{
	CGameObject*	pInstance = new CEvolLight3(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CEvolLight3"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvolLight3::Free()
{
	__super::Free();

}
