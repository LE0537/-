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

	Set_PokePos();
	Set_TypePos();

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
	else
		RenderFonts2();

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
	
	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Shader2"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), (CComponent**)&m_pShaderCom2)))
		return E_FAIL;
	
	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_PokeDeck"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture2"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI"), (CComponent**)&m_pTextureCom2)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture3"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_PokeNum"), (CComponent**)&m_pTextureCom3)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture4"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Type"), (CComponent**)&m_pTextureCom4)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer2"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom2)))
		return E_FAIL;

	wstring szBuffer[6];
	wstring szTrans[6];
	wstring szShader[6];
	for (int i = 0; i < 6; ++i)
	{
		szBuffer[i] = TEXT("Com_VIBufferPoke");
		szTrans[i] = TEXT("Com_TransformPoke");
		szShader[i] = TEXT("Com_ShaderPoke");
	}
	for (int i = 0; i < 6; ++i)
	{
		szBuffer[i] += to_wstring(i);
		szTrans[i] += to_wstring(i);
		szShader[i] += to_wstring(i);
		if (FAILED(__super::Add_Components(szBuffer[i].c_str(), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferPoke[i])))
			return E_FAIL;
		if (FAILED(__super::Add_Components(szTrans[i].c_str(), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformPoke[i])))
			return E_FAIL;
		if (FAILED(__super::Add_Components(szShader[i].c_str(), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), (CComponent**)&m_pShaderPoke[i])))
			return E_FAIL;
	}

	wstring szBufferType[3];
	wstring szTransType[3];
	wstring szShaderType[3];
	for (int i = 0; i < 3; ++i)
	{
		szBufferType[i] = TEXT("Com_VIBufferType");
		szTransType[i] = TEXT("Com_TransformType");
		szShaderType[i] = TEXT("Com_ShaderType");
	}
	for (int i = 0; i < 3; ++i)
	{
		szBufferType[i] += to_wstring(i);
		szTransType[i] += to_wstring(i);
		szShaderType[i] += to_wstring(i);
		if (FAILED(__super::Add_Components(szBufferType[i].c_str(), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferType[i])))
			return E_FAIL;
		if (FAILED(__super::Add_Components(szTransType[i].c_str(), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformType[i])))
			return E_FAIL;
		if (FAILED(__super::Add_Components(szShaderType[i].c_str(), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), (CComponent**)&m_pShaderType[i])))
			return E_FAIL;
	}

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
		for (_int i = 0; i < 6; ++i)
		{
			if (dynamic_cast<CGameObj*>(m_vecPoke[i + m_iPokeScroll])->Get_PokeInfo().iPokeNum != 999)
			{
				if (FAILED(m_pShaderPoke[i]->Set_RawValue("g_WorldMatrix", &m_pTransformPoke[i]->Get_World4x4_TP(), sizeof(_float4x4))))
					return E_FAIL;
				if (FAILED(m_pShaderPoke[i]->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
					return E_FAIL;
				if (FAILED(m_pShaderPoke[i]->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
					return E_FAIL;
				if (FAILED(m_pShaderPoke[i]->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom3->Get_SRV(dynamic_cast<CGameObj*>(m_vecPoke[i + m_iPokeScroll])->Get_PokeInfo().iPokeNum))))
					return E_FAIL;

				m_pShaderPoke[i]->Begin();
				m_pVIBufferPoke[i]->Render();
			}
		}

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
	if (m_bSelect)
	{

		if (FAILED(m_pShaderType[0]->Set_RawValue("g_WorldMatrix", &m_pTransformType[0]->Get_World4x4_TP(), sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderType[0]->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderType[0]->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderType[0]->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom3->Get_SRV(dynamic_cast<CGameObj*>(m_vecPoke[m_iPokePos])->Get_PokeInfo().iPokeNum))))
			return E_FAIL;

		m_pShaderType[0]->Begin();
		m_pVIBufferType[0]->Render();

		for (_int i = 1; i < 3; ++i)
		{
			if (FAILED(m_pShaderType[i]->Set_RawValue("g_WorldMatrix", &m_pTransformType[i]->Get_World4x4_TP(), sizeof(_float4x4))))
				return E_FAIL;
			if (FAILED(m_pShaderType[i]->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
				return E_FAIL;
			if (FAILED(m_pShaderType[i]->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
				return E_FAIL;
			if (i == 1)
			{
				if (FAILED(m_pShaderType[i]->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom4->Get_SRV(dynamic_cast<CGameObj*>(m_vecPoke[m_iPokePos])->Get_PokeInfo().eType))))
					return E_FAIL;
				m_pShaderType[i]->Begin();
				m_pVIBufferType[i]->Render();
			}
			else if(dynamic_cast<CGameObj*>(m_vecPoke[m_iPokePos])->Get_PokeInfo().eType2 != POKETYPE_END)
			{
				if (FAILED(m_pShaderType[i]->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom4->Get_SRV(dynamic_cast<CGameObj*>(m_vecPoke[m_iPokePos])->Get_PokeInfo().eType2))))
					return E_FAIL;
				m_pShaderType[i]->Begin();
				m_pVIBufferType[i]->Render();
			}
			
		}
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
		if(dynamic_cast<CGameObj*>(m_vecPoke[m_iPokePos])->Get_PokeInfo().iPokeNum != 999)
			m_bSelect = true;

	}
	m_bUseKey = true;
	if (!m_bSelect && pGameInstance->Key_Down(DIK_BACKSPACE))
	{
		g_bPokeDeck = false;
		m_bUseKey = false;
		m_iSelect = 0;
		m_iPokePos = 0;
		m_iPokeScroll = 0;
		m_fSizeX = 60.f;
		m_fSizeY = 60.f;
		m_fX = 770.f;
		m_fY = 140.f;
		_float3 vScale = { m_fSizeX,m_fSizeY,0.f };
		m_pTransformCom2->Set_Scale(XMLoadFloat3(&vScale));
		m_pTransformCom2->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));

	}
	else if (m_bSelect && pGameInstance->Key_Down(DIK_BACKSPACE))
		m_bSelect = false;



	RELEASE_INSTANCE(CGameInstance);
}

void CPokeDeck::Set_PokePos()
{
	_float fSizeX = 150.f;
	_float fSizeY = 150.f;
	_float fX = 850.f;
	_float fY = 100.f;
	_float3 vScale = { fSizeX,fSizeY,0.f };

	for (_int i = 0; i < 6; ++i)
	{
		m_pTransformPoke[i]->Set_Scale(XMLoadFloat3(&vScale));
		m_pTransformPoke[i]->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(fX - g_iWinSizeX * 0.5f, -fY + g_iWinSizeY * 0.5f, 0.f, 1.f));
		fY += 90.f;
	}
}

void CPokeDeck::Set_TypePos()
{
	_float fSizeX = 200.f;
	_float fSizeY = 200.f;
	_float fX = 600.f;
	_float fY = 70.f;
	_float3 vScale = { fSizeX,fSizeY,0.f };

	m_pTransformType[0]->Set_Scale(XMLoadFloat3(&vScale));
	m_pTransformType[0]->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(fX - g_iWinSizeX * 0.5f, -fY + g_iWinSizeY * 0.5f, 0.f, 1.f));

	fSizeX = 120.f;
	fSizeY = 30.f;
	fX = 960.f;
	fY = 280.f;
	vScale = { fSizeX,fSizeY,0.f };

	for (_int i = 1; i < 3; ++i)
	{

		m_pTransformType[i]->Set_Scale(XMLoadFloat3(&vScale));
		m_pTransformType[i]->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(fX - g_iWinSizeX * 0.5f, -fY + g_iWinSizeY * 0.5f, 0.f, 1.f));
		fX += 150.f;
	}
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
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_vector vPos = { 660.f,110.f,0.f,1.f };
	_vector vPos2 = { 900.f,110.f,0.f,1.f };
	wstring strNum = TEXT("No.");
	wstring strNum2 = TEXT("");

	strNum += to_wstring(dynamic_cast<CGameObj*>(m_vecPoke[m_iPokePos])->Get_PokeInfo().iPokeNum);
	pGameInstance->Render_Font(TEXT("Font_Nexon"), strNum.c_str(), vPos, XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(1.2f, 1.2f, 1.2f, 1.f));
	pGameInstance->Render_Font(TEXT("Font_Nexon"), dynamic_cast<CGameObj*>(m_vecPoke[m_iPokePos])->Get_PokeInfo().strName.c_str(), vPos2, XMVectorSet(1.f, 1.f, 1.f, 1.f), XMVectorSet(1.2f, 1.2f, 1.2f, 1.f));

	vPos = { 710.f,270.f,0.f,1.f };
	strNum = TEXT("타입");
	pGameInstance->Render_Font(TEXT("Font_Nexon"), strNum.c_str(), vPos, XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f));
	
	vPos = { 710.f,325.f,0.f,1.f };
	vPos2 = { 990.f,325.f,0.f,1.f };
	strNum = TEXT("성격");
	pGameInstance->Render_Font(TEXT("Font_Nexon"), strNum.c_str(), vPos, XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f));
	pGameInstance->Render_Font(TEXT("Font_Nexon"), dynamic_cast<CGameObj*>(m_vecPoke[m_iPokePos])->Get_PokeInfo().strChar.c_str(), vPos2, XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f));

	vPos = { 685.f,380.f,0.f,1.f };
	vPos2 = { 980.f,380.f,0.f,1.f };
	strNum = TEXT("탑승 여부");
	pGameInstance->Render_Font(TEXT("Font_Nexon"), strNum.c_str(), vPos, XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f));
	if(dynamic_cast<CGameObj*>(m_vecPoke[m_iPokePos])->Get_PokeInfo().bRide)
		strNum2 = TEXT("탑승 가능");
	else
		strNum2 = TEXT("탑승 불가");
	pGameInstance->Render_Font(TEXT("Font_Nexon"), strNum2.c_str(), vPos2, XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f));

	vPos = { 820.f,495.f,0.f,1.f };
	vPos2 = { 615.f,555.f,0.f,1.f };
	strNum = TEXT("포켓몬 설명");
	pGameInstance->Render_Font(TEXT("Font_Nexon"), strNum.c_str(), vPos, XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f));
	pGameInstance->Render_Font(TEXT("Font_Nexon"), dynamic_cast<CGameObj*>(m_vecPoke[m_iPokePos])->Get_PokeInfo().strInfo.c_str(), vPos2, XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f));

	RELEASE_INSTANCE(CGameInstance);
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
	for (int i = 0; i < 3; ++i)
	{
		Safe_Release(m_pVIBufferType[i]);
		Safe_Release(m_pTransformType[i]);
		Safe_Release(m_pShaderType[i]);
	}
	Safe_Release(m_pTransformCom2);
	Safe_Release(m_pTextureCom2);
	Safe_Release(m_pTextureCom3);
	Safe_Release(m_pTextureCom4);
	Safe_Release(m_pShaderCom2);
	Safe_Release(m_pVIBufferCom2);
	Safe_Release(m_pVIBufferCom3);
}
