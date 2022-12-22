#include "stdafx.h"
#include "..\Public\EvolLight.h"

#include "GameInstance.h"

CEvolLight::CEvolLight(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
}

CEvolLight::CEvolLight(const CEvolLight & rhs)
	: CGameObj(rhs)
{
}

HRESULT CEvolLight::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEvolLight::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_fSizeX = 50.f;
	m_fSizeY = 50.f;

	m_fX = (_float)(rand() % g_iWinSizeX);
	m_fY = (_float)(rand() % g_iWinSizeY);

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixTranspose(XMMatrixIdentity()));
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, -200.f, 100.f)));

	_float3 vScale = { m_fSizeX,m_fSizeY,0.f };

	m_pTransformCom->Set_Scale(XMLoadFloat3(&vScale));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));

	return S_OK;
}

void CEvolLight::Tick(_float fTimeDelta)
{
	Set_Pos(fTimeDelta);

	if (!g_bEvolution)
		Set_Dead();
}

void CEvolLight::Late_Tick(_float fTimeDelta)
{
	if (g_bEvolution && nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CEvolLight::Render()
{
	if (nullptr == m_pShaderCom || nullptr == m_pVIBufferCom )
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEvolLight::Render_ShadowDepth()
{
	return E_NOTIMPL;
}

HRESULT CEvolLight::Ready_Components()
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
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_EffectLight"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;


	return S_OK;
}

HRESULT CEvolLight::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_World4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_SRV(1))))
		return E_FAIL;

	m_pShaderCom->Begin(10);
	m_pVIBufferCom->Render();


	return S_OK;
}

void CEvolLight::Set_Pos(_float fTimeDelta)
{
	m_fY -= 10.f * fTimeDelta;

	if (m_fY < -5.f)
	{
		Set_Dead();
		return;
	}
	m_fSizeX -= 5.f * fTimeDelta;
	m_fSizeY -= 5.f * fTimeDelta;
	if (m_fSizeX < 0.f)
	{
		Set_Dead();
		return;
	}

	_float3 vScale = { m_fSizeX,m_fSizeY,0.f };

	m_pTransformCom->Set_Scale(XMLoadFloat3(&vScale));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));
}




CEvolLight * CEvolLight::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CEvolLight*	pInstance = new CEvolLight(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CEvolLight"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CEvolLight::Clone(void * pArg)
{
	CGameObject*	pInstance = new CEvolLight(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CEvolLight"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvolLight::Free()
{
	__super::Free();

}
