#include "stdafx.h"
#include "..\Public\CountDown.h"

#include "GameInstance.h"
#include "Level_GamePlay.h"

CCountDown::CCountDown(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
}

CCountDown::CCountDown(const CCountDown & rhs)
	: CGameObj(rhs)
{
}

HRESULT CCountDown::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCountDown::Initialize(void * pArg)
{
	m_pTarget = (CGameObject*)pArg;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_fSizeX = 142;
	m_fSizeY = 78;
	m_fX = g_iWinSizeX >> 1;
	m_fY = g_iWinSizeY >> 1;

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixTranspose(XMMatrixIdentity()));
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f)));

	_float3 vScale = { m_fSizeX,m_fSizeY,0.f };

	m_pTransformCom->Set_Scale(XMLoadFloat3(&vScale));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));

	return S_OK;
}

void CCountDown::Tick(_float fTimeDelta)
{
	m_fFrameTime += fTimeDelta;
	if (m_fFrameTime > 1.f)
	{
		++m_iFrame;
		if (m_iFrame > 3)
			Set_Dead();

		m_fFrameTime = 0.f;
	}

}

void CCountDown::Late_Tick(_float fTimeDelta)
{

	if (g_bRace && nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

}

HRESULT CCountDown::Render()
{

	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;



	return S_OK;
}
HRESULT CCountDown::Render_ShadowDepth()
{
	return E_NOTIMPL;
}
HRESULT CCountDown::Ready_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Count"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;


	return S_OK;
}



HRESULT CCountDown::SetUp_ShaderResources()
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

	m_pShaderCom->Begin(6);
	m_pVIBufferCom->Render();



	return S_OK;
}

CCountDown * CCountDown::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCountDown*	pInstance = new CCountDown(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CCountDown"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CCountDown::Clone(void * pArg)
{
	CGameObject*	pInstance = new CCountDown(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CCountDown"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCountDown::Free()
{
	__super::Free();


}
