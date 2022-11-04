#include "stdafx.h"
#include "..\Public\Screen.h"

#include "GameInstance.h"

CScreen::CScreen(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
}

CScreen::CScreen(const CScreen & rhs)
	: CGameObj(rhs)
{
}

HRESULT CScreen::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CScreen::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTarget = (CGameObject*)pArg;

	m_fSizeX = 640.f;
	m_fSizeY = 720.f;
	m_fX = 0.f;
	m_fY = 360.f;

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixTranspose(XMMatrixIdentity()));
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, -200.f, 100.f)));

	_float3 vScale = { m_fSizeX,m_fSizeY,0.f };

	m_pTransformCom->Set_Scale(XMLoadFloat3(&vScale));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));

	m_fX2 = 1280.f;
	
	m_pTransformCom2->Set_Scale(XMLoadFloat3(&vScale));
	m_pTransformCom2->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fX2 - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));

	return S_OK;
}

void CScreen::Tick(_float fTimeDelta)
{
	if (!dynamic_cast<CGameObj*>(m_pTarget)->Get_Event())
	{
		Set_Pos2();
		return;
	}
	Set_Pos();
}

void CScreen::Late_Tick(_float fTimeDelta)
{
	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup_Front(CRenderer::RENDER_UI, this);
}

HRESULT CScreen::Render()
{
	if (nullptr == m_pShaderCom || nullptr == m_pVIBufferCom || nullptr == m_pShaderCom2 || nullptr == m_pVIBufferCom2)
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	return S_OK;
}

HRESULT CScreen::Ready_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Transform2"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom2)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Shader2"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), (CComponent**)&m_pShaderCom2)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Battle"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer2"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom2)))
		return E_FAIL;

	return S_OK;
}

HRESULT CScreen::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom || nullptr == m_pShaderCom2)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_World4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_SRV(14))))
		return E_FAIL;

	m_pShaderCom->Begin(3);
	m_pVIBufferCom->Render();

	if (FAILED(m_pShaderCom2->Set_RawValue("g_WorldMatrix", &m_pTransformCom2->Get_World4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom2->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom2->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom2->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_SRV(13))))
		return E_FAIL;

	m_pShaderCom2->Begin(3);
	m_pVIBufferCom2->Render();

	return S_OK;
}

void CScreen::Set_Pos()
{
	m_fX += 6.f;
	m_fX2 -= 6.f;

	if (m_fX > 320.f)
		m_fX = 320.f;

	if (m_fX2 < 960.f)
		m_fX2 = 960.f;


	m_pTransformCom2->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fX2 - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));
}

void CScreen::Set_Pos2()
{
	m_fX -= 6.f;
	m_fX2 += 6.f;

	if (m_fX < -320.f)
		m_bDead = true;

	m_pTransformCom2->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fX2 - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));
}


CScreen * CScreen::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CScreen*	pInstance = new CScreen(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CScreen"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CScreen::Clone(void * pArg)
{
	CGameObject*	pInstance = new CScreen(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CScreen"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CScreen::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom2);
	Safe_Release(m_pShaderCom2);
	Safe_Release(m_pVIBufferCom2);
}
