#include "stdafx.h"
#include "..\Public\Bag.h"

#include "GameInstance.h"
#include "MonsterBall.h"

CBag::CBag(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
}

CBag::CBag(const CBag & rhs)
	: CGameObj(rhs)
{
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

	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);
	
	for (_int i = 0; i < 5; ++i)
	{
		CGameObject* tInfo;
		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_MonsterBall"), LEVEL_STATIC, TEXT("Layer_Item"),&tInfo)))
			return E_FAIL;
		m_vecItem.push_back(tInfo);
	}
	for (_int i = 5; i < 15; ++i)
	{
		CGameObject* tInfo;
		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_HpPotion"), LEVEL_STATIC, TEXT("Layer_Item"), &tInfo)))
			return E_FAIL;
		m_vecItem.push_back(tInfo);
	}
	for (_int i = 15; i < 20; ++i)
	{
		CGameObject* tInfo;
		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_ExpShare"), LEVEL_STATIC, TEXT("Layer_Item"), &tInfo)))
			return E_FAIL;
		m_vecItem.push_back(tInfo);
	}
	for (_int i = 20; i < 50; ++i)
	{
		CGameObject* tInfo;
		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_None"), LEVEL_STATIC, TEXT("Layer_Item"), &tInfo)))
			return E_FAIL;
		m_vecItem.push_back(tInfo);
	}
	for (_int i = 0; i < 3; ++i)
	{
		CGameObject* tInfo;
		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Pikachu"), LEVEL_STATIC, TEXT("Layer_Pokemon"), &tInfo)))
			return E_FAIL;
		m_vecPoke.push_back(tInfo);
	}

	for (_int i = 3; i < 6; ++i)
	{
		CGameObject* tInfo;
		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Garomakguri"), LEVEL_STATIC, TEXT("Layer_Pokemon"), &tInfo)))
			return E_FAIL;
		m_vecPoke.push_back(tInfo);
	}

	
	m_fSizeX = g_iWinSizeX;
	m_fSizeY = g_iWinSizeY;
	m_fX = g_iWinSizeX >> 1;
	m_fY = g_iWinSizeX >> 1;

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixTranspose(XMMatrixIdentity()));
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f)));

	_float3 vScale = { m_fSizeX,m_fSizeY,0.f };

	m_pTransformCom->Set_Scale(XMLoadFloat3(&vScale));

	m_fSizeX = 500;
	m_fSizeY = 45;
	m_fX = 900;
	m_fY = 170;
	vScale = { m_fSizeX,m_fSizeY,0.f };

	m_pTransformCom2->Set_Scale(XMLoadFloat3(&vScale));
	m_pTransformCom2->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));
	
	m_PokefSizeX = 60;
	m_PokefSizeY = 60;
	m_PokefX = 50;
	m_PokefY = 100;

	Safe_Release(pGameInstance);
	Set_ItemPos();
	Set_PokePos();
	Set_PokeItemPos();
	return S_OK;
}

void CBag::Tick(_float fTimeDelta)
{
	if (g_bBag)
	{
		if (!m_bItem && !m_bSwap)
			Key_Input();
		else if (m_bItem)
			Key_UseInput();
		else if (m_bSwap)
			Key_PokeInput();
	}
}

void CBag::Late_Tick(_float fTimeDelta)
{
	if (g_bBag)
	{
		if (nullptr != m_pRendererCom)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
	}
}

HRESULT CBag::Render()
{
	if (g_bBag)
	{
		if (nullptr == m_pShaderCom || nullptr == m_pVIBufferCom || nullptr == m_pShaderCom2 || nullptr == m_pVIBufferCom2 || nullptr == m_pShaderCom3 || nullptr == m_pVIBufferCom3)
			return E_FAIL;
		if (m_bUseItem || m_bUsePoke)
		{
			if (FAILED(SetUp_UseResources()))
				return E_FAIL;
		}
		if (FAILED(SetUp_PokeItemResources()))
			return E_FAIL;

		if (FAILED(SetUp_PokeResources()))
			return E_FAIL;

		if (FAILED(SetUp_ItemResources()))
			return E_FAIL;

		if (FAILED(SetUp_ShaderResources()))
			return E_FAIL;
	
	}
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
	if (FAILED(__super::Add_Components(TEXT("Com_Transform4"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom4)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Transform5"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom5)))
		return E_FAIL;
	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Shader2"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), (CComponent**)&m_pShaderCom2)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Shader3"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), (CComponent**)&m_pShaderCom3)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Shader4"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), (CComponent**)&m_pShaderCom4)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Shader5"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), (CComponent**)&m_pShaderCom5)))
		return E_FAIL;
	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture2"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Item"), (CComponent**)&m_pTextureCom2)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture3"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_PokeNum"), (CComponent**)&m_pTextureCom3)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture4"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_StatInfo"), (CComponent**)&m_pTextureCom4)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer2"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom2)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer3"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom3)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer4"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom4)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer5"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom5)))
		return E_FAIL;
	wstring szBuffer[8];
	wstring szTrans[8];
	wstring szShader[8];
	for (int i = 0; i < 8; ++i)
	{
		szBuffer[i] = TEXT("Com_VIBufferItem");
		szTrans[i] = TEXT("Com_TransformItem");
		szShader[i] = TEXT("Com_ShaderItem");
	}
	for (int i = 0; i < 8; ++i)
	{
		szBuffer[i] += to_wstring(i);
		szTrans[i] += to_wstring(i);
		szShader[i] += to_wstring(i);
		if (FAILED(__super::Add_Components(szBuffer[i].c_str(), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferItem[i])))
			return E_FAIL;
		if (FAILED(__super::Add_Components(szTrans[i].c_str(), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformItem[i])))
			return E_FAIL;
		if (FAILED(__super::Add_Components(szShader[i].c_str(), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), (CComponent**)&m_pShaderItem[i])))
			return E_FAIL;
	}
	wstring szPokeBuffer[24];
	wstring szPokeTrans[24];
	wstring szPokeShader[24];
	for (int i = 0; i < 24; ++i)
	{
		szPokeBuffer[i] = TEXT("Com_VIBufferPoke");
		szPokeTrans[i] = TEXT("Com_TransformPoke");
		szPokeShader[i] = TEXT("Com_ShaderPoke");
	}
	for (int i = 0; i < 24; ++i)
	{
		szPokeBuffer[i] += to_wstring(i);
		szPokeTrans[i] += to_wstring(i);
		szPokeShader[i] += to_wstring(i);
		if (FAILED(__super::Add_Components(szPokeBuffer[i].c_str(), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferPoke[i])))
			return E_FAIL;
		if (FAILED(__super::Add_Components(szPokeTrans[i].c_str(), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformPoke[i])))
			return E_FAIL;
		if (FAILED(__super::Add_Components(szPokeShader[i].c_str(), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), (CComponent**)&m_pShaderPoke[i])))
			return E_FAIL;
	}
	wstring szPokeItemBuffer[6];
	wstring szPokeItemTrans[6];
	wstring szPokeItemShader[6];
	for (int i = 0; i < 6; ++i)
	{
		szPokeItemBuffer[i] = TEXT("Com_VIBufferPokeItem");
		szPokeItemTrans[i] = TEXT("Com_TransformPokeItem");
		szPokeItemShader[i] = TEXT("Com_ShaderPokeItem");
	}
	for (int i = 0; i < 6; ++i)
	{
		szPokeItemBuffer[i] += to_wstring(i);
		szPokeItemTrans[i] += to_wstring(i);
		szPokeItemShader[i] += to_wstring(i);
		if (FAILED(__super::Add_Components(szPokeItemBuffer[i].c_str(), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferPokeItem[i])))
			return E_FAIL;
		if (FAILED(__super::Add_Components(szPokeItemTrans[i].c_str(), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformPokeItem[i])))
			return E_FAIL;
		if (FAILED(__super::Add_Components(szPokeItemShader[i].c_str(), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), (CComponent**)&m_pShaderPokeItem[i])))
			return E_FAIL;
	}
	return S_OK;
}

HRESULT CBag::SetUp_ShaderResources()
{

	if (m_bItemSelect)
	{
		if (nullptr == m_pShaderCom || nullptr == m_pShaderCom2 || nullptr == m_pShaderCom3)
			return E_FAIL;

		if (FAILED(m_pShaderCom2->Set_RawValue("g_WorldMatrix", &m_pTransformCom2->Get_World4x4_TP(), sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderCom2->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderCom2->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderCom2->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_SRV(2))))
			return E_FAIL;

		m_pShaderCom2->Begin();
		m_pVIBufferCom2->Render();
	}
	else if (!m_bItemSelect)
	{
		if (FAILED(m_pShaderCom3->Set_RawValue("g_WorldMatrix", &m_pTransformCom3->Get_World4x4_TP(), sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderCom3->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderCom3->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderCom3->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_SRV(0))))
			return E_FAIL;

		m_pShaderCom3->Begin();
		m_pVIBufferCom3->Render();
	}
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

	

	return S_OK;
}

HRESULT CBag::SetUp_ItemResources()
{
	for (_int i = 0; i < 8; ++i)
	{
		if (FAILED(m_pShaderItem[i]->Set_RawValue("g_WorldMatrix", &m_pTransformItem[i]->Get_World4x4_TP(), sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderItem[i]->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderItem[i]->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
			return E_FAIL;
		if (dynamic_cast<CGameObj*>(m_vecItem[i + m_iItemScoll])->Get_ItemInfo().iItemNum != 99)
		{
			if (FAILED(m_pShaderItem[i]->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom2->Get_SRV(dynamic_cast<CGameObj*>(m_vecItem[i + m_iItemScoll])->Get_ItemInfo().iItemNum))))
				return E_FAIL;
			m_pShaderItem[i]->Begin();
			m_pVIBufferItem[i]->Render();
		}
	}
	return S_OK;
}

HRESULT CBag::SetUp_PokeResources()
{
	_float fHp = 0.f;
	_float HpfSizeX = 230.f;
	_float HpfSizeY = 8.f;
	_float HpfX = 255.f;
	_float HpfY = 155.f;
	_float3 HpvScale = { HpfSizeX,HpfSizeY,0.f };

	for (_int i = 0; i < 6; ++i)
	{
		if (FAILED(m_pShaderPoke[i]->Set_RawValue("g_WorldMatrix", &m_pTransformPoke[i]->Get_World4x4_TP(), sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderPoke[i]->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderPoke[i]->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
			return E_FAIL;
		if (dynamic_cast<CGameObj*>(m_vecPoke[i])->Get_PokeInfo().iPokeNum != 999)
		{
			if (FAILED(m_pShaderPoke[i]->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom3->Get_SRV(dynamic_cast<CGameObj*>(m_vecPoke[i])->Get_PokeInfo().iPokeNum))))
				return E_FAIL;
			m_pShaderPoke[i]->Begin();
			m_pVIBufferPoke[i]->Render();
		}
	}
	for (_int i = 6; i < 12; ++i)
	{
		if (FAILED(m_pShaderPoke[i]->Set_RawValue("g_WorldMatrix", &m_pTransformPoke[i]->Get_World4x4_TP(), sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderPoke[i]->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderPoke[i]->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
			return E_FAIL;
		
		if (dynamic_cast<CGameObj*>(m_vecPoke[i - 6])->Get_PokeInfo().iPokeNum != 999)
		{
			_float fShaderHp = (_float)dynamic_cast<CGameObj*>(m_vecPoke[i - 6])->Get_PokeInfo().iHp / (_float)dynamic_cast<CGameObj*>(m_vecPoke[i - 6])->Get_PokeInfo().iMaxHp;
			fHp = HpfSizeX * fShaderHp;
		
			HpfX -= (HpfSizeX - fHp) / 2.f;
			
			HpvScale = { fHp,HpfSizeY,0.f };
			m_pTransformPoke[i]->Set_Scale(XMLoadFloat3(&HpvScale));
			m_pTransformPoke[i]->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(HpfX - g_iWinSizeX * 0.5f, -HpfY + g_iWinSizeY * 0.5f, 0.f, 1.f));
			HpfY += 96.f;
			HpfX = 255.f;
			
			if (FAILED(m_pShaderPoke[i]->Set_RawValue("g_fHP", &fShaderHp, sizeof(_float))))
				return E_FAIL;
			if (FAILED(m_pShaderPoke[i]->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_SRV(7))))
				return E_FAIL;
			m_pShaderPoke[i]->Begin(1);
			m_pVIBufferPoke[i]->Render();
		}
	}
	for (_int i = 12; i < 18; ++i)
	{
		if (FAILED(m_pShaderPoke[i]->Set_RawValue("g_WorldMatrix", &m_pTransformPoke[i]->Get_World4x4_TP(), sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderPoke[i]->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderPoke[i]->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
			return E_FAIL;
		if (dynamic_cast<CGameObj*>(m_vecPoke[i -12])->Get_PokeInfo().iPokeNum != 999 && dynamic_cast<CGameObj*>(m_vecPoke[i - 12])->Get_PokeInfo().iSex != 99)
		{
			if (dynamic_cast<CGameObj*>(m_vecPoke[i - 12])->Get_PokeInfo().iSex == 0)
			{
				if (FAILED(m_pShaderPoke[i]->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_SRV(5))))
					return E_FAIL;
			}
			else
			{
				if (FAILED(m_pShaderPoke[i]->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_SRV(6))))
					return E_FAIL;
			}
			m_pShaderPoke[i]->Begin();
			m_pVIBufferPoke[i]->Render();
		}
	}
	for (_int i = 18; i < 24; ++i)
	{
		if (dynamic_cast<CGameObj*>(m_vecPoke[i - 18])->Get_PokeInfo().eStatInfo != STATINFO_END)
		{
			if (FAILED(m_pShaderPoke[i]->Set_RawValue("g_WorldMatrix", &m_pTransformPoke[i]->Get_World4x4_TP(), sizeof(_float4x4))))
				return E_FAIL;
			if (FAILED(m_pShaderPoke[i]->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
				return E_FAIL;
			if (FAILED(m_pShaderPoke[i]->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
				return E_FAIL;
			if (dynamic_cast<CGameObj*>(m_vecPoke[i - 18])->Get_PokeInfo().iPokeNum != 999)
			{
				if (FAILED(m_pShaderPoke[i]->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom4->Get_SRV(dynamic_cast<CGameObj*>(m_vecPoke[i - 18])->Get_PokeInfo().eStatInfo))))
					return E_FAIL;
				m_pShaderPoke[i]->Begin();
				m_pVIBufferPoke[i]->Render();
			}
		}
	}
	return S_OK;
}

HRESULT CBag::SetUp_PokeItemResources()
{
	for (_int i = 0; i < 6; ++i)
	{
		if (FAILED(m_pShaderPokeItem[i]->Set_RawValue("g_WorldMatrix", &m_pTransformPokeItem[i]->Get_World4x4_TP(), sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderPokeItem[i]->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderPokeItem[i]->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
			return E_FAIL;
		if (dynamic_cast<CGameObj*>(m_vecPoke[i])->Get_PokeInfo().iItem != 99)
		{
			if (FAILED(m_pShaderPokeItem[i]->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom2->Get_SRV(dynamic_cast<CGameObj*>(m_vecPoke[i])->Get_PokeInfo().iItem))))
				return E_FAIL;
			m_pShaderPokeItem[i]->Begin();
			m_pVIBufferPokeItem[i]->Render();
		}
	}
	return S_OK;
}

HRESULT CBag::SetUp_UseResources()
{
	if (FAILED(m_pShaderCom5->Set_RawValue("g_WorldMatrix", &m_pTransformCom5->Get_World4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom5->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom5->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom5->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_SRV(0))))
		return E_FAIL;

	m_pShaderCom5->Begin();
	m_pVIBufferCom5->Render();

	if (m_bUseItem)
	{
		if (FAILED(m_pShaderCom4->Set_RawValue("g_WorldMatrix", &m_pTransformCom4->Get_World4x4_TP(), sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderCom4->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderCom4->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderCom4->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_SRV(9))))
			return E_FAIL;

		m_pShaderCom4->Begin();
		m_pVIBufferCom4->Render();
	}
	if (m_bUsePoke)
	{
		if (FAILED(m_pShaderCom4->Set_RawValue("g_WorldMatrix", &m_pTransformCom4->Get_World4x4_TP(), sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderCom4->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderCom4->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderCom4->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_SRV(8))))
			return E_FAIL;

		m_pShaderCom4->Begin();
		m_pVIBufferCom4->Render();
	}
	return S_OK;
}

HRESULT CBag::SetSelectButton(_int iIndex, ButtonDir _eDir)
{

	switch (_eDir)
	{
	case CBag::DIR_UP:
		m_fY -= 50;
		m_pTransformCom2->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));
		break;
	case CBag::DIR_DOWN:
		m_fY += 50;
		m_pTransformCom2->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));
		break;
	default:
		break;
	}
	
	

	return S_OK;
}

HRESULT CBag::SetSelectButtonPoke(_int iIndex, ButtonDir _eDir)
{
	_float3 vScale;
	switch (_eDir)
	{
	case CBag::DIR_UP:
		m_PokefY -= 96;
		m_pTransformCom3->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_PokefX - g_iWinSizeX * 0.5f, -m_PokefY + g_iWinSizeY * 0.5f, 0.f, 1.f));
		break;
	case CBag::DIR_DOWN:
		m_PokefY += 96;
		m_pTransformCom3->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_PokefX - g_iWinSizeX * 0.5f, -m_PokefY + g_iWinSizeY * 0.5f, 0.f, 1.f));
		break;
	case CBag::DIR_LEFT:
		 m_PokefSizeX = 60;
		 m_PokefSizeY = 60;
		 m_PokefX = 40;
		 m_PokefY = 150 + (iIndex * 96);
		 vScale = { m_PokefSizeX,m_PokefSizeY,0.f };

		m_pTransformCom3->Set_Scale(XMLoadFloat3(&vScale));
		m_pTransformCom3->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_PokefX - g_iWinSizeX * 0.5f, -m_PokefY + g_iWinSizeY * 0.5f, 0.f, 1.f));
		break;
	default:
		break;
	}


	return S_OK;
}

HRESULT CBag::SetSelectButtonUse(_int iIndex, ButtonDir _eDir)
{
	_vector vPos = m_pTransformCom5->Get_State(CTransform::STATE_TRANSLATION);
	switch (_eDir)
	{
	case CBag::DIR_UP:
		vPos.m128_f32[1] += 22;
		m_pTransformCom5->Set_State(CTransform::STATE_TRANSLATION, vPos);
		break;
	case CBag::DIR_DOWN:
		vPos.m128_f32[1] -= 22;
		m_pTransformCom5->Set_State(CTransform::STATE_TRANSLATION, vPos);
		break;
	default:
		break;
	}
	return S_OK;
}

void CBag::Key_Input()
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);
	if (pGameInstance->Key_Down(DIK_BACKSPACE) && !m_bUseItem && !m_bUsePoke)
	{
		m_bItem = false;
		m_bUseItem = false;
		m_bUsePoke = false;
		m_bUseKey = false;
		m_UsePos = 0;
		m_bItemSelect = true;
		m_iPokeSelect = 0;
		m_iSelect = 0;
		m_iItemPos = 0;
		m_iItemScoll = 0;
		m_fSizeX = 500;
		m_fSizeY = 45;
		m_fX = 900;
		m_fY = 170;
		_float3 vScale = { m_fSizeX,m_fSizeY,0.f };

		m_pTransformCom2->Set_Scale(XMLoadFloat3(&vScale));
		m_pTransformCom2->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));

		g_bBag = false;
	}
	if (pGameInstance->Key_Down(DIK_UP))
	{
		if (!m_bUseItem && !m_bUsePoke)
		{
			if (m_bItemSelect)
			{
				if (m_iSelect == 0)
				{
					if (m_iItemScoll != 0)
					{
						--m_iItemScoll;
						--m_iItemPos;
					}
				}
				else
				{
					SetSelectButton(m_iSelect, DIR_UP);
					--m_iSelect;
					--m_iItemPos;
				}
			}
			else
			{
				if (m_iPokeSelect != 0)
				{
					SetSelectButtonPoke(m_iPokeSelect, DIR_UP);
					--m_iPokeSelect;
				}
			}
		}
		if (m_bUseItem || m_bUsePoke)
		{
			if (m_UsePos != 0)
			{
				SetSelectButtonUse(m_UsePos, DIR_UP);
				--m_UsePos;
			}
		}
	}

	if (pGameInstance->Key_Down(DIK_DOWN))
	{
		if (!m_bUseItem && !m_bUsePoke)
		{
			if (m_bItemSelect)
			{
				if (m_iSelect == 7)
				{
					if (dynamic_cast<CGameObj*>(m_vecItem[m_iItemPos + 1])->Get_ItemInfo().iItemNum != 99 && m_iItemScoll != 50)
					{
						++m_iItemScoll;
						++m_iItemPos;
					}
				}
				else
				{
					if (dynamic_cast<CGameObj*>(m_vecItem[m_iItemPos + 1])->Get_ItemInfo().iItemNum != 99)
					{
						SetSelectButton(m_iSelect, DIR_DOWN);
						++m_iSelect;
						++m_iItemPos;
					}
				}
			}
			else
			{
				if (m_iPokeSelect != 5 && dynamic_cast<CGameObj*>(m_vecPoke[m_iPokeSelect + 1])->Get_PokeInfo().iPokeNum != 999)
				{
					SetSelectButtonPoke(m_iPokeSelect, DIR_DOWN);
					++m_iPokeSelect;
				}
			}
		}
		if (m_bUseItem || m_bUsePoke)
		{
			if (m_UsePos != 2)
			{
				SetSelectButtonUse(m_UsePos, DIR_DOWN);
				++m_UsePos;
			}
		}
	}
	if (pGameInstance->Key_Down(DIK_RETURN))
	{
		if (!m_bUseItem && !m_bUsePoke)
		{
			if (m_bItemSelect && m_bUseKey)
			{
				m_bUseItem = true;
				Set_UseItemPos(m_iSelect);
			}
			else if (!m_bItemSelect && m_bUseKey)
			{
				m_bUsePoke = true;
				Set_UsePokePos(m_iPokeSelect);
			}
			m_bUseKey = true;
		}
		if (m_bUseItem)
		{
			if (m_bUse)
			{
				switch (m_UsePos)
				{
				case 0:
					if (CheckUseItem())
					{
						SetSelectButtonPoke(m_iPokeSelect, DIR_LEFT);
						m_bItem = true;
						m_bItemSelect = false;
						m_bUseItem = false;
					}
					m_bUse = false;
					break;
				case 1:
					if (CheckGiveItem())
					{
						SetSelectButtonPoke(m_iPokeSelect, DIR_LEFT);
						m_bItem = true;
						m_bGiveItem = true;
						m_bItemSelect = false;
						m_bUseItem = false;
					}
					m_bUse = false;
					break;
				case 2:
					m_bUseItem = false;
					m_UsePos = 0;
					m_bUse = false;
					Safe_Release(pGameInstance);
					return;
					break;
				default:
					break;
				}
			}
			m_bUse = true;
		}
		if (m_bUsePoke)
		{
			if (m_bUse)
			{
				switch (m_UsePos)
				{
				case 0:
					LookStats();
					m_bUse = false;
					break;
				case 1:
					SetSelectButtonPoke(m_iPokeSelect, DIR_LEFT);
					m_iSwapPoke = m_iPokeSelect;
					m_bSwap = true;
					m_bItemSelect = false;
					m_bUsePoke = false;
					m_bUse = false;
					Safe_Release(pGameInstance);
					return;
					break;
				case 2:
					m_bUsePoke = false;
					m_UsePos = 0;
					m_bUse = false;
					Safe_Release(pGameInstance);
					return;
				default:
					break;
				}
			}
			m_bUse = true;
		}
	}
	if (!m_bUseItem && !m_bUsePoke)
	{
		if (m_bItemSelect && pGameInstance->Key_Down(DIK_LEFT))
		{
			m_bItemSelect = !m_bItemSelect;
			SetSelectButtonPoke(m_iPokeSelect, DIR_LEFT);
		}
		else if (!m_bItemSelect && pGameInstance->Key_Down(DIK_RIGHT))
		{
			m_iPokeSelect = 0;
			m_bItemSelect = !m_bItemSelect;
		}
	}
	Safe_Release(pGameInstance);
}

void CBag::Key_UseInput()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Key_Down(DIK_BACKSPACE) && !m_bUseItem && !m_bUsePoke)
	{
		m_iPokeSelect = 0;
		m_bItem = false;
		m_bUsePoke = false;
		m_bItemSelect = true;
		m_bUseItem = false;
		m_UsePos = 0;
		m_bUse = false;
	}
	if (pGameInstance->Key_Down(DIK_UP))
	{
		if (m_iPokeSelect != 0)
		{
			SetSelectButtonPoke(m_iPokeSelect, DIR_UP);
			--m_iPokeSelect;
		}
	}

	if (pGameInstance->Key_Down(DIK_DOWN))
	{
		if (m_iPokeSelect != 5 && dynamic_cast<CGameObj*>(m_vecPoke[m_iPokeSelect + 1])->Get_PokeInfo().iPokeNum != 999)
		{
			SetSelectButtonPoke(m_iPokeSelect, DIR_DOWN);
			++m_iPokeSelect;
		}
	}
	if (pGameInstance->Key_Down(DIK_RETURN))
	{
		if (!m_bGiveItem)
			UseItem();
		else if (m_bGiveItem)
			GiveItem();
		m_bItemSelect = true;
		m_bItem = false;
		m_bGiveItem = false;
		m_bUsePoke = false;
		m_bUseItem = false;
		m_UsePos = 0;
		m_bUse = false;
		m_iPokeSelect = 0;
	}
	RELEASE_INSTANCE(CGameInstance);
}

void CBag::Key_PokeInput()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	
	if (pGameInstance->Key_Down(DIK_BACKSPACE) && !m_bUseItem && !m_bUsePoke)
	{
		if (m_iSwapPoke != 99)
			m_iSwapPoke = 99;
		else
		{
			m_iPokeSelect = 0;
			m_bSwap = false;
			m_bUsePoke = false;
			m_bItemSelect = true;
			m_bUseItem = false;
			m_UsePos = 0;
			m_bUse = false;
		}
	}
	if (pGameInstance->Key_Down(DIK_UP))
	{
		if (m_iPokeSelect != 0)
		{
			SetSelectButtonPoke(m_iPokeSelect, DIR_UP);
			--m_iPokeSelect;
		}
	}

	if (pGameInstance->Key_Down(DIK_DOWN))
	{
		if (m_iPokeSelect != 5 && dynamic_cast<CGameObj*>(m_vecPoke[m_iPokeSelect + 1])->Get_PokeInfo().iPokeNum != 999)
		{
			SetSelectButtonPoke(m_iPokeSelect, DIR_DOWN);
			++m_iPokeSelect;
		}
	}
	if (pGameInstance->Key_Down(DIK_RETURN))
	{
		if (m_bSwap && m_iSwapPoke == 99)
		{
			m_iSwapPoke = m_iPokeSelect;
		}
		else if (m_iSwapPoke != 99 && m_iSwapPoke != m_iPokeSelect)
		{
			SwapPoke(m_iSwapPoke);
			m_bSwap = false;
			m_bUsePoke = false;
			m_bUseItem = false;
			m_UsePos = 0;
			m_bUse = false;
		}
	}
	RELEASE_INSTANCE(CGameInstance);
}

void CBag::Set_ItemPos()
{
	_float fSizeX = 40.f;
	_float fSizeY = 40.f;
	_float fX = 700.f;
	_float fY = 170.f;
	_float3 vScale = { fSizeX,fSizeY,0.f };

	for (_int i = 0; i < 8; ++i)
	{
		m_pTransformItem[i]->Set_Scale(XMLoadFloat3(&vScale));
		m_pTransformItem[i]->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(fX - g_iWinSizeX * 0.5f, -fY + g_iWinSizeY * 0.5f, 0.f, 1.f));
		fY += 50.f;
	}
}

void CBag::Set_PokePos()
{
	_float PokefSizeX = 110.f;
	_float PokefSizeY = 110.f;
	_float PokefX = 100.f;
	_float POkefY = 140.f;
	_float3 PokevScale = { PokefSizeX,PokefSizeY,0.f };

	_float HpfSizeX = 230.f;
	_float HpfSizeY = 6.f;
	_float HpfX = 255.f;
	_float HpfY = 170.f;
	_float3 HpvScale = { HpfSizeX,HpfSizeY,0.f };

	_float SexfSizeX = 25.f;
	_float SexfSizeY = 25.f;
	_float SexfX = 350.f;
	_float SexfY = 135.f;
	_float3 SexvScale = { SexfSizeX,SexfSizeY,0.f };

	_float StatsSizeX = 100.f;
	_float StatsfSizeY = 25.f;
	_float StatsfX = 360.f;
	_float StatsfY = 176.f;
	_float3 StatsvScale = { StatsSizeX,StatsfSizeY,0.f };

	for (_int i = 0; i < 6; ++i)
	{
		m_pTransformPoke[i]->Set_Scale(XMLoadFloat3(&PokevScale));
		m_pTransformPoke[i]->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(PokefX - g_iWinSizeX * 0.5f, -POkefY + g_iWinSizeY * 0.5f, 0.f, 1.f));
		POkefY += 96.f;
	}
	for (_int i = 6; i < 12; ++i)
	{
		m_pTransformPoke[i]->Set_Scale(XMLoadFloat3(&HpvScale));
		m_pTransformPoke[i]->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(HpfX - g_iWinSizeX * 0.5f, -HpfY + g_iWinSizeY * 0.5f, 0.f, 1.f));
		HpfY += 96.f;
	}
	for (_int i = 12; i < 18; ++i)
	{
		m_pTransformPoke[i]->Set_Scale(XMLoadFloat3(&SexvScale));
		m_pTransformPoke[i]->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(SexfX - g_iWinSizeX * 0.5f, -SexfY + g_iWinSizeY * 0.5f, 0.f, 1.f));
		SexfY += 96.f;
	}
	for (_int i = 18; i < 24; ++i)
	{
		m_pTransformPoke[i]->Set_Scale(XMLoadFloat3(&StatsvScale));
		m_pTransformPoke[i]->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(StatsfX - g_iWinSizeX * 0.5f, -StatsfY + g_iWinSizeY * 0.5f, 0.f, 1.f));
		StatsfY += 96.f;
	}
}

void CBag::Set_PokeItemPos()
{
	_float fSizeX = 50.f;
	_float fSizeY = 50.f;
	_float fX = 125.f;
	_float fY = 180.f;
	_float3 vScale = { fSizeX,fSizeY,0.f };

	for (_int i = 0; i < 6; ++i)
	{
		m_pTransformPokeItem[i]->Set_Scale(XMLoadFloat3(&vScale));
		m_pTransformPokeItem[i]->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(fX - g_iWinSizeX * 0.5f, -fY + g_iWinSizeY * 0.5f, 0.f, 1.f));
		fY += 96.f;
	}
}

void CBag::Set_UseItemPos(_int _iIndex)
{
	_vector vScale = { 120.f,80.f,0.f,0.f };
	_vector vPos = m_pTransformItem[_iIndex]->Get_State(CTransform::STATE_TRANSLATION);

	vPos.m128_f32[0] += 200.f;
	vPos.m128_f32[1] -= 20.f;
	m_pTransformCom4->Set_Scale(vScale);
	m_pTransformCom4->Set_State(CTransform::STATE_TRANSLATION, vPos);

	vPos.m128_f32[0] -= 70.f;
	vPos.m128_f32[1] += 20.f;
	vScale = { 20.f,20.f,0.f,0.f };
	m_pTransformCom5->Set_Scale(vScale);
	m_pTransformCom5->Set_State(CTransform::STATE_TRANSLATION, vPos);
}

void CBag::Set_UsePokePos(_int _iIndex)
{
	_vector vScale = { 120.f,80.f,0.f,0.f };
	_vector vPos = m_pTransformPoke[_iIndex]->Get_State(CTransform::STATE_TRANSLATION);

	vPos.m128_f32[0] += 350.f;
	vPos.m128_f32[1] -= 10.f;
	m_pTransformCom4->Set_Scale(vScale);
	m_pTransformCom4->Set_State(CTransform::STATE_TRANSLATION, vPos);

	vPos.m128_f32[0] -= 70.f;
	vPos.m128_f32[1] += 20.f;
	vScale = { 20.f,20.f,0.f,0.f };
	m_pTransformCom5->Set_Scale(vScale);
	m_pTransformCom5->Set_State(CTransform::STATE_TRANSLATION, vPos);
}

void CBag::UseItem()
{
	auto& iter = m_vecItem.begin();
	iter += m_iItemPos;

	switch (dynamic_cast<CGameObj*>((*iter))->Get_ItemInfo().iItemNum)
	{
	case 4:
		dynamic_cast<CGameObj*>(m_vecPoke[m_iPokeSelect])->Set_PokeHp(30);
		break;
	default:
		break;
	}
	(*iter)->m_bDead = true;
	m_vecItem.erase(iter);

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	CGameObject* tInfo;
	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_None"), LEVEL_STATIC, TEXT("Layer_Item"), &tInfo)))
		return;
	m_vecItem.push_back(tInfo);
	
	RELEASE_INSTANCE(CGameInstance);

	
	if (m_iSelect == 0)
	{
		if (m_iItemScoll != 0)
		{
			--m_iItemScoll;
			--m_iItemPos;
		}
	}
	else if(dynamic_cast<CGameObj*>(m_vecItem[m_iItemPos])->Get_ItemInfo().iItemNum == 99)
	{
		SetSelectButton(m_iSelect, DIR_UP);
		--m_iSelect;
		--m_iItemPos;
	}
	
}

void CBag::GiveItem()
{
	if (dynamic_cast<CGameObj*>(m_vecPoke[m_iPokeSelect])->Get_PokeInfo().iItem == 99)
	{
		dynamic_cast<CGameObj*>(m_vecPoke[m_iPokeSelect])->Set_PokeItem(dynamic_cast<CGameObj*>(m_vecItem[m_iItemPos])->Get_ItemInfo().iItemNum);

		auto& iter = m_vecItem.begin();
		iter += m_iItemPos;
		(*iter)->m_bDead = true;
		m_vecItem.erase(iter);

		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
		CGameObject* tInfo;
		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_None"), LEVEL_STATIC, TEXT("Layer_Item"), &tInfo)))
			return;
		m_vecItem.push_back(tInfo);
		
		RELEASE_INSTANCE(CGameInstance);
	}
}

void CBag::LookStats()
{
}

void CBag::SwapPoke(_int _iSwapPoke)
{
	CGameObject* tInfo;
	tInfo = m_vecPoke[_iSwapPoke];
	m_vecPoke[_iSwapPoke] = m_vecPoke[m_iPokeSelect];
	m_vecPoke[m_iPokeSelect] = tInfo;
}

_bool CBag::CheckUseItem()
{
	switch (dynamic_cast<CGameObj*>(m_vecItem[m_iItemPos])->Get_ItemInfo().iItemNum)
	{
	case 4:
		return true;
		break;
	default:
		break;
	}
	return false;
}

_bool CBag::CheckGiveItem()
{
	switch (dynamic_cast<CGameObj*>(m_vecItem[m_iItemPos])->Get_ItemInfo().iItemNum)
	{
	case 30:
		return true;
		break;
	default:
		break;
	}
	return false;
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

	for (int i = 0; i < 6; ++i)
	{
		Safe_Release(m_pVIBufferPokeItem[i]);
		Safe_Release(m_pTransformPokeItem[i]);
		Safe_Release(m_pShaderPokeItem[i]);
	}
	for (int i = 0; i < 8; ++i)
	{
		Safe_Release(m_pVIBufferItem[i]);
		Safe_Release(m_pTransformItem[i]);
		Safe_Release(m_pShaderItem[i]);
	}
	for (int i = 0; i < 24; ++i)
	{
		Safe_Release(m_pVIBufferPoke[i]);
		Safe_Release(m_pTransformPoke[i]);
		Safe_Release(m_pShaderPoke[i]);
	}
	if (!m_vecItem.empty())
	{
		for (int i = 0; i < 50; ++i)
		{
			Safe_Release(m_vecItem[i]);
		}
		m_vecItem.clear();
	}
	if (!m_vecPoke.empty())
	{
		for (int i = 0; i < 6; ++i)
		{
			Safe_Release(m_vecPoke[i]);
		}
		m_vecPoke.clear();
	}
	Safe_Release(m_pTextureCom2);
	Safe_Release(m_pTextureCom3);
	Safe_Release(m_pTextureCom4);
	Safe_Release(m_pTransformCom2);
	Safe_Release(m_pTransformCom3);
	Safe_Release(m_pTransformCom4);
	Safe_Release(m_pTransformCom5);
	Safe_Release(m_pShaderCom2);
	Safe_Release(m_pShaderCom3);
	Safe_Release(m_pShaderCom4);
	Safe_Release(m_pShaderCom5);
	Safe_Release(m_pVIBufferCom2);
	Safe_Release(m_pVIBufferCom3);
	Safe_Release(m_pVIBufferCom4);
	Safe_Release(m_pVIBufferCom5);
}
