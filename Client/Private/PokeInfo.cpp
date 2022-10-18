#include "stdafx.h"
#include "..\Public\PokeInfo.h"

#include "GameInstance.h"

CPokeInfo::CPokeInfo(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
}

CPokeInfo::CPokeInfo(const CPokeInfo & rhs)
	: CGameObj(rhs)
{
}

HRESULT CPokeInfo::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPokeInfo::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_fSizeX = g_iWinSizeX;
	m_fSizeY = g_iWinSizeY;
	m_fX = g_iWinSizeX >> 1;
	m_fY = g_iWinSizeY >> 1;

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixTranspose(XMMatrixIdentity()));
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f)));

	_float3 vScale = { m_fSizeX,m_fSizeY,0.f };

	m_pTransformCom->Set_Scale(XMLoadFloat3(&vScale));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));

	m_fSizeX = 60.f;
	m_fSizeY = 60.f;
	m_fX = 45.f;
	m_fY = 112.f;
	vScale = { m_fSizeX,m_fSizeY,0.f };
	m_pTransformCom2->Set_Scale(XMLoadFloat3(&vScale));
	m_pTransformCom2->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));

	return S_OK;
}

void CPokeInfo::Tick(_float fTimeDelta)
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (g_PokeInfo)
	{
		if (m_bSelect && pGameInstance->Key_Down(DIK_LEFT))
		{
			m_bSelect = !m_bSelect;
		}
		else if (!m_bSelect && pGameInstance->Key_Down(DIK_RIGHT))
		{
			m_bSelect = !m_bSelect;
		}
		if (pGameInstance->Key_Down(DIK_RETURN))
		{
		

		}
		if (pGameInstance->Key_Down(DIK_BACKSPACE))
		{
			g_PokeInfo = false;
			g_bBag = true;
		}

	}
	Safe_Release(pGameInstance);
}

void CPokeInfo::Late_Tick(_float fTimeDelta)
{
	if (g_PokeInfo)
	{
		if (nullptr != m_pRendererCom)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
	}
}

HRESULT CPokeInfo::Render()
{
	if (g_PokeInfo)
	{
		if (nullptr == m_pShaderCom || nullptr == m_pVIBufferCom || nullptr == m_pShaderCom2 || nullptr == m_pVIBufferCom2 || nullptr == m_pShaderCom3 || nullptr == m_pVIBufferCom3)
			return E_FAIL;

		if (FAILED(SetUp_ShaderResources()))
			return E_FAIL;









	}
	return S_OK;
}

HRESULT CPokeInfo::Ready_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Transform2"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom2)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Transform3"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom3)))
		return E_FAIL;
	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Shader2"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), (CComponent**)&m_pShaderCom2)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Shader3"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), (CComponent**)&m_pShaderCom3)))
		return E_FAIL;
	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer2"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom2)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer3"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom3)))
		return E_FAIL;
	return S_OK;
}

HRESULT CPokeInfo::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom || nullptr == m_pShaderCom2 || nullptr == m_pShaderCom3)
		return E_FAIL;
	if (!m_bSelect)
	{
		if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_World4x4_TP(), sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_SRV(10))))
			return E_FAIL;

		m_pShaderCom->Begin();
		m_pVIBufferCom->Render();
	}
	else if (m_bSelect)
	{
		if (FAILED(m_pShaderCom3->Set_RawValue("g_WorldMatrix", &m_pTransformCom2->Get_World4x4_TP(), sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderCom3->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderCom3->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderCom3->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_SRV(0))))
			return E_FAIL;

		m_pShaderCom3->Begin();
		m_pVIBufferCom3->Render();

		if (FAILED(m_pShaderCom2->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_World4x4_TP(), sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderCom2->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderCom2->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderCom2->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_SRV(11))))
			return E_FAIL;

		m_pShaderCom2->Begin();
		m_pVIBufferCom2->Render();
	}
	return S_OK;
}

HRESULT CPokeInfo::SetSelectButton()
{
	m_fSizeX = 50;
	m_fSizeY = 50;
	if (m_bSelect)
	{
		m_fX = 1080;
		m_fY = 600;
	}
	else
	{
		m_fX = 930;
		m_fY = 600;
	}
	_float3 vScale = { m_fSizeX,m_fSizeY,0.f };
	m_pTransformCom3->Set_Scale(XMLoadFloat3(&vScale));
	m_pTransformCom3->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));


	return S_OK;
}

CPokeInfo * CPokeInfo::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPokeInfo*	pInstance = new CPokeInfo(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CPokeInfo"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CPokeInfo::Clone(void * pArg)
{
	CGameObject*	pInstance = new CPokeInfo(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CPokeInfo"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPokeInfo::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom2);
	Safe_Release(m_pTransformCom3);
	Safe_Release(m_pShaderCom2);
	Safe_Release(m_pShaderCom3);
	Safe_Release(m_pVIBufferCom2);
	Safe_Release(m_pVIBufferCom3);
}
