#include "stdafx.h"
#include "..\Public\Bag.h"

#include "GameInstance.h"

CBag::CBag(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
	for (_int i = 0; i < 18; ++i)
	{
		m_pVIBufferPoke[i] = nullptr;
		m_pTransformPoke[i] = nullptr;
		m_pShaderPoke[i] = nullptr;
	}
	for (_int i = 0; i < 7; ++i)
	{
		m_pVIBufferItem[i] = nullptr;
		m_pTransformItem[i] = nullptr;
		m_pShaderItem[i] = nullptr;
	}
}

CBag::CBag(const CBag & rhs)
	: CGameObj(rhs)
{
	for (_int i = 0; i < 18; ++i)
	{
		m_pVIBufferPoke[i] = rhs.m_pVIBufferPoke[i];
		m_pTransformPoke[i] = rhs.m_pTransformPoke[i];
		m_pShaderPoke[i] = rhs.m_pShaderPoke[i];
	}
	for (_int i = 0; i < 7; ++i)
	{
		m_pVIBufferItem[i] = rhs.m_pVIBufferItem[i];
		m_pTransformItem[i] = rhs.m_pTransformItem[i];
		m_pShaderItem[i] = rhs.m_pShaderItem[i];
	}
}

HRESULT CBag::Initialize_Prototype()
{
	



	return S_OK;
}

HRESULT CBag::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;
	m_vecPoke.reserve(6);
	m_vecItem.reserve(50);
	


	m_fSizeX = g_iWinSizeX;
	m_fSizeY = g_iWinSizeY;
	m_fX = g_iWinSizeX >> 1;
	m_fY = g_iWinSizeX >> 1;

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixTranspose(XMMatrixIdentity()));
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f)));

	_float3 vScale = { m_fSizeX,m_fSizeY,0.f };

	m_pTransformCom->Set_Scale(XMLoadFloat3(&vScale));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));

	m_fSizeX = 400;
	m_fSizeY = 50;
	m_fX = 900;
	m_fY = 100;
	vScale = { m_fSizeX,m_fSizeY,0.f };

	m_pTransformCom2->Set_Scale(XMLoadFloat3(&vScale));
	m_pTransformCom2->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));

	return S_OK;
}

void CBag::Tick(_float fTimeDelta)
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Key_Down(DIK_UP))
	{
		if (m_iSelect == 0)
		{
			if (m_iItemScoll != 0)
				--m_iItemScoll;
			else
				m_iItemScoll = m_iItemScoll;
		}
		else
			--m_iSelect;
		SetSelectButton(m_iSelect);
	}

	if (pGameInstance->Key_Down(DIK_DOWN))
	{
		 if (m_iSelect == 7)
			{
				if (m_iItemScoll != 50)
					++m_iItemScoll;
				else
					m_iItemScoll = m_iItemScoll;
			}
		 else
		 {
			 if(m_vecItem[m_iItemPos])

			 ++m_iSelect;
		 }

		SetSelectButton(m_iSelect);
	}
	

	if (m_bItemSelect && pGameInstance->Key_Down(DIK_LEFT))
		{
			m_bItemSelect = !m_bItemSelect;
			SetSelectButton(m_iSelect);
		}
	else if (!m_bItemSelect && pGameInstance->Key_Down(DIK_RIGHT))
		{
			m_bItemSelect = !m_bItemSelect;
			SetSelectButton(m_iSelect);
		}
	
	Safe_Release(pGameInstance);
}

void CBag::Late_Tick(_float fTimeDelta)
{
	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CBag::Render()
{
	if (nullptr == m_pShaderCom || nullptr == m_pVIBufferCom || nullptr == m_pShaderCom2 || nullptr == m_pVIBufferCom2 || nullptr == m_pShaderCom3 || nullptr == m_pVIBufferCom3)
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBag::Ready_Components()
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
	if (FAILED(__super::Add_Components(TEXT("Com_Texture2"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI"), (CComponent**)&m_pTextureCom2)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture3"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI"), (CComponent**)&m_pTextureCom3)))
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

HRESULT CBag::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom || nullptr == m_pShaderCom2 || nullptr == m_pShaderCom3)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_World4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_SRV(1))))
		return E_FAIL;

	m_pShaderCom->Begin();
	m_pVIBufferCom->Render();

	if (FAILED(m_pShaderCom2->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_World4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom2->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom2->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom2->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_SRV(4))))
		return E_FAIL;

	m_pShaderCom2->Begin();
	m_pVIBufferCom2->Render();

	if (FAILED(m_pShaderCom3->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_World4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom3->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom3->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom3->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_SRV(0))))
		return E_FAIL;

	m_pShaderCom3->Begin();
	m_pVIBufferCom3->Render();

	return S_OK;
}

HRESULT CBag::SetSelectButton(_int iIndex)
{
	m_fSizeX = 50;
	m_fSizeY = 50;
	if (m_bItemSelect)
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

CBag * CBag::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBag*	pInstance = new CBag(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CBag"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CBag::Clone(void * pArg)
{
	CGameObject*	pInstance = new CBag(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CBag"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBag::Free()
{
	__super::Free();
	Safe_Release(m_pTextureCom2);
	Safe_Release(m_pTextureCom3);
	Safe_Release(m_pTransformCom2);
	Safe_Release(m_pTransformCom3);
	Safe_Release(m_pShaderCom2);
	Safe_Release(m_pShaderCom3);
	Safe_Release(m_pVIBufferCom2);
	Safe_Release(m_pVIBufferCom3);
}
