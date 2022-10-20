#include "stdafx.h"
#include "..\Public\PokeDeck.h"

#include "GameInstance.h"

CPokeDeck::CPokeDeck(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
}

CPokeDeck::CPokeDeck(const CPokeDeck & rhs)
	: CGameObj(rhs)
{
}

HRESULT CPokeDeck::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPokeDeck::Initialize(void * pArg)
{
	m_vecPoke.reserve(152);

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	for (_int i = 0; i < 152; ++i)
	{
		CGameObject* tInfo;

		if (i == 24)
		{
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Pikachu"), LEVEL_STATIC, TEXT("Layer_Pokemon"), &tInfo)))
				return E_FAIL;
			m_vecPoke.push_back(tInfo);
			continue;
		}
		if (i == 151)
		{
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Garomakguri"), LEVEL_STATIC, TEXT("Layer_Pokemon"), &tInfo)))
				return E_FAIL;
			m_vecPoke.push_back(tInfo);
			continue;
		}
		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_NonePoke"), LEVEL_STATIC, TEXT("Layer_Pokemon"), &tInfo)))
			return E_FAIL;
		m_vecPoke.push_back(tInfo);
	}

	RELEASE_INSTANCE(CGameInstance)

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_fSizeX = (_float)g_iWinSizeX;
	m_fSizeY = (_float)g_iWinSizeY;
	m_fX = g_iWinSizeX >> 1;
	m_fY = g_iWinSizeY >> 1;

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixTranspose(XMMatrixIdentity()));
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f)));

	_float3 vScale = { m_fSizeX,m_fSizeY,0.f };

	m_pTransformCom->Set_Scale(XMLoadFloat3(&vScale));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));

	m_fSizeX = 60.f;
	m_fSizeY = 60.f;
	m_fX = 770.f;
	m_fY = 140.f;
	vScale = { m_fSizeX,m_fSizeY,0.f };
	m_pTransformCom2->Set_Scale(XMLoadFloat3(&vScale));
	m_pTransformCom2->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));


	return S_OK;
}

void CPokeDeck::Tick(_float fTimeDelta)
{
	if (g_bPokeDeck)
	{
		Key_Input();
	}
}

void CPokeDeck::Late_Tick(_float fTimeDelta)
{
	if (nullptr != m_pRendererCom && g_bPokeDeck)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CPokeDeck::Render()
{
	if (nullptr == m_pShaderCom || nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	if (!m_bSelect)
		RenderFonts();

	return S_OK;
}

HRESULT CPokeDeck::Ready_Components()
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
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_PokeDeck"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture2"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI"), (CComponent**)&m_pTextureCom2)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture3"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_PokeNum"), (CComponent**)&m_pTextureCom3)))
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

HRESULT CPokeDeck::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_World4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;
	_int iTextrue = 0;
	if (!m_bSelect)
		iTextrue = 0;
	else
		iTextrue = 1;
	if (FAILED(m_pShaderCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_SRV(iTextrue))))
		return E_FAIL;

	m_pShaderCom->Begin();
	m_pVIBufferCom->Render();
	if (!m_bSelect)
	{
		/*if (FAILED(m_pShaderCom2->Set_RawValue("g_WorldMatrix", &m_pTransformCom3->Get_World4x4_TP(), sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderCom2->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderCom2->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderCom2->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom3->Get_SRV(dynamic_cast<CGameObj*>(m_vecPoke[m_iPokePos])->Get_PokeInfo().iPokeNum))))
			return E_FAIL;

		m_pShaderCom3->Begin();
		m_pVIBufferCom3->Render();*/



		if (FAILED(m_pShaderCom2->Set_RawValue("g_WorldMatrix", &m_pTransformCom2->Get_World4x4_TP(), sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderCom2->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderCom2->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderCom2->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom2->Get_SRV(0))))
			return E_FAIL;

		m_pShaderCom2->Begin();
		m_pVIBufferCom2->Render();
	}
	return S_OK;
}

HRESULT CPokeDeck::SetSelectButton(CBag::ButtonDir _eDir)
{
	switch (_eDir)
	{
	case CBag::DIR_UP:
		m_fY -= 90.f;
		m_pTransformCom2->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));
		break;
	case CBag::DIR_DOWN:
		m_fY += 90.f;
		m_pTransformCom2->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));
		break;
	default:
		break;
	}

	return S_OK;
}

void CPokeDeck::Key_Input()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (!m_bSelect && pGameInstance->Key_Pressing(DIK_LSHIFT) && pGameInstance->Key_Pressing(DIK_UP))
	{
		if (m_iSelect == 0)
		{
			if (m_iPokeScroll != 0)
			{
				--m_iPokeScroll;
				--m_iPokePos;
			}
		}
		else
		{
			SetSelectButton(CBag::DIR_UP);
			--m_iSelect;
			--m_iPokePos;
		}
	}
	else if (!m_bSelect && pGameInstance->Key_Down(DIK_UP))
	{
		if (m_iSelect == 0)
		{
			if (m_iPokeScroll != 0)
			{
				--m_iPokeScroll;
				--m_iPokePos;
			}
		}
		else
		{
			SetSelectButton(CBag::DIR_UP);
			--m_iSelect;
			--m_iPokePos;
		}
	}
	if (!m_bSelect && pGameInstance->Key_Pressing(DIK_LSHIFT) && pGameInstance->Key_Pressing(DIK_DOWN))
	{
		if (m_iSelect == 5)
		{
			if (m_iPokeScroll != 146)
			{
				++m_iPokeScroll;
				++m_iPokePos;
			}
		}
		else
		{
			SetSelectButton(CBag::DIR_DOWN);
			++m_iSelect;
			++m_iPokePos;
		}
	}
	else if (!m_bSelect && pGameInstance->Key_Down(DIK_DOWN))
	{
		if (m_iSelect == 5)
		{
			if (m_iPokeScroll != 146)
			{
				++m_iPokeScroll;
				++m_iPokePos;
			}
		}
		else
		{
			SetSelectButton(CBag::DIR_DOWN);
			++m_iSelect;
			++m_iPokePos;
		}
	}

	if (m_bUseKey && !m_bSelect && pGameInstance->Key_Down(DIK_RETURN))
	{
		m_bSelect = true;

	}
	if (!m_bSelect && pGameInstance->Key_Down(DIK_BACKSPACE))
	{
		g_bPokeDeck = false;
		m_bUseKey = false;
	}
	else if (m_bSelect && pGameInstance->Key_Down(DIK_BACKSPACE))
		m_bSelect = false;


	m_bUseKey = true;

	RELEASE_INSTANCE(CGameInstance);
}

void CPokeDeck::RenderFonts()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_vector vPos = { 920.f,130.f,0.f,1.f };
	_vector vPos2 = { 1070.f,130.f,0.f,1.f };
	wstring strNum = TEXT("No.");
	wstring strNum2 = TEXT("??????");

	for (_int i = 0; i < 6; ++i)
	{
		strNum = TEXT("No.");

		if (dynamic_cast<CGameObj*>(m_vecPoke[i + m_iPokeScroll])->Get_PokeInfo().iPokeNum != 999)
		{
			strNum += to_wstring(dynamic_cast<CGameObj*>(m_vecPoke[i + m_iPokeScroll])->Get_PokeInfo().iPokeNum);
			pGameInstance->Render_Font(TEXT("Font_Nexon"), dynamic_cast<CGameObj*>(m_vecPoke[i + m_iPokeScroll])->Get_PokeInfo().strName.c_str(), vPos2, XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f));
		}
		else
		{
			strNum += TEXT("???");;
			pGameInstance->Render_Font(TEXT("Font_Nexon"), strNum2.c_str(), vPos2, XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f));
		}
		pGameInstance->Render_Font(TEXT("Font_Nexon"), strNum.c_str(), vPos, XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f));

		vPos.m128_f32[1] += 90.f;
		vPos2.m128_f32[1] += 90.f;
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CPokeDeck::RenderFonts2()
{
}

CPokeDeck * CPokeDeck::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPokeDeck*	pInstance = new CPokeDeck(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CPokeDeck"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CPokeDeck::Clone(void * pArg)
{
	CGameObject*	pInstance = new CPokeDeck(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CPokeDeck"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPokeDeck::Free()
{
	__super::Free();

	if (!m_vecPoke.empty())
	{
		for (int i = 0; i < 152; ++i)
		{
			Safe_Release(m_vecPoke[i]);
		}
		m_vecPoke.clear();
	}
	for (int i = 0; i < 6; ++i)
	{
		Safe_Release(m_pVIBufferPoke[i]);
		Safe_Release(m_pTransformPoke[i]);
		Safe_Release(m_pShaderPoke[i]);
	}
	Safe_Release(m_pTransformCom2);
	Safe_Release(m_pTextureCom2);
	Safe_Release(m_pTextureCom3);
	Safe_Release(m_pShaderCom2);
	Safe_Release(m_pVIBufferCom2);
	Safe_Release(m_pShaderCom3);
	Safe_Release(m_pVIBufferCom3);
	Safe_Release(m_pTransformCom3);
}
