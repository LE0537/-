#include "stdafx.h"
#include "..\Public\PokeInfo.h"

#include "GameInstance.h"
#include "Bag.h"

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
	m_pBag = (CBag*)pArg;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_fSizeX = g_iWinSizeX;
	m_fSizeY = g_iWinSizeY;
	m_fX = g_iWinSizeX >> 1;
	m_fY = g_iWinSizeY >> 1;

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixTranspose(XMMatrixIdentity()));
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, -800.f, 500.f)));

	_float3 vScale = { m_fSizeX,m_fSizeY,0.f };

	m_pTransformCom->Set_Scale(XMLoadFloat3(&vScale));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 300.f, 1.f));

	m_fSizeX = 60.f;
	m_fSizeY = 60.f;
	m_fButtonX = 45.f;
	m_fButtonY = 112.f;
	vScale = { m_fSizeX,m_fSizeY,0.f };
	m_pTransformCom2->Set_Scale(XMLoadFloat3(&vScale));
	m_pTransformCom2->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fButtonX - g_iWinSizeX * 0.5f, -m_fButtonY + g_iWinSizeY * 0.5f, 0.f, 1.f));

	m_fSizeX = 116.f;
	m_fSizeY = 116.f;
	_float fX = 306.f;
	_float fY = 270.f;
	vScale = { m_fSizeX,m_fSizeY,0.f };
	m_pTransformCom4->Set_Scale(XMLoadFloat3(&vScale));
	m_pTransformCom4->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(fX - g_iWinSizeX * 0.5f, -fY + g_iWinSizeY * 0.5f, 0.f, 1.f));

	Set_SkillPos();
	Set_InfoPos();

	return S_OK;
}

void CPokeInfo::Tick(_float fTimeDelta)
{
	if (g_PokeInfo)
	{
		Key_Input();
	}
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
		if (FAILED(SetUp_InfoResources()))
			return E_FAIL;

		if (m_bSelect)
		{
			if (FAILED(SetUp_SkillResources()))
				return E_FAIL;
			RenderFonts();
		}
		else
		{
			RenderFonts2();
		}
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
	if (FAILED(__super::Add_Components(TEXT("Com_Transform4"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom4)))
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
	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture2"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Battle"), (CComponent**)&m_pTextureCom2)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture3"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Type"), (CComponent**)&m_pTextureCom3)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture4"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Item"), (CComponent**)&m_pTextureCom4)))
		return E_FAIL;
	
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer2"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom2)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer3"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom3)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_VIBufferHexagon"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Hexagon"), (CComponent**)&m_pVIBufferHexagonCom)))
		return E_FAIL;

	wstring szBuffer[4];
	wstring szTrans[4];
	wstring szShader[4];
	for (int i = 0; i < 4; ++i)
	{
		szBuffer[i] = TEXT("Com_VIBufferSkill");
		szTrans[i] = TEXT("Com_TransformSkill");
		szShader[i] = TEXT("Com_ShaderSkill");
	}
	for (int i = 0; i < 4; ++i)
	{
		szBuffer[i] += to_wstring(i);
		szTrans[i] += to_wstring(i);
		szShader[i] += to_wstring(i);
		if (FAILED(__super::Add_Components(szBuffer[i].c_str(), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferSkill[i])))
			return E_FAIL;
		if (FAILED(__super::Add_Components(szTrans[i].c_str(), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformSkill[i])))
			return E_FAIL;
		if (FAILED(__super::Add_Components(szShader[i].c_str(), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), (CComponent**)&m_pShaderSkill[i])))
			return E_FAIL;
	}
	wstring szBufferInfo[3];
	wstring szTransInfo[3];
	wstring szShaderInfo[3];
	for (int i = 0; i < 3; ++i)
	{
		szBufferInfo[i] = TEXT("Com_VIBufferInfo");
		szTransInfo[i] = TEXT("Com_TransformInfo");
		szShaderInfo[i] = TEXT("Com_ShaderInfo");
	}
	for (int i = 0; i < 3; ++i)
	{
		szBufferInfo[i] += to_wstring(i);
		szTransInfo[i] += to_wstring(i);
		szShaderInfo[i] += to_wstring(i);
		if (FAILED(__super::Add_Components(szBufferInfo[i].c_str(), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferInfo[i])))
			return E_FAIL;
		if (FAILED(__super::Add_Components(szTransInfo[i].c_str(), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformInfo[i])))
			return E_FAIL;
		if (FAILED(__super::Add_Components(szShaderInfo[i].c_str(), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), (CComponent**)&m_pShaderInfo[i])))
			return E_FAIL;
	}

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

		
		

		_float fHP = (_float)dynamic_cast<CGameObj*>(m_pBag->Get_SelectPoke())->Get_PokeInfo().iMaxHp / (((250.f * 2.f) + 31.f + 100.f) * (dynamic_cast<CGameObj*>(m_pBag->Get_SelectPoke())->Get_PokeInfo().iLv / 100.f) + 10.f);
		_float fDmg = (_float)dynamic_cast<CGameObj*>(m_pBag->Get_SelectPoke())->Get_PokeInfo().iDmg / (((180.f * 2.f) + 31.f) * (dynamic_cast<CGameObj*>(m_pBag->Get_SelectPoke())->Get_PokeInfo().iLv / 100.f) + 5.f);
		_float fSDmg = (_float)dynamic_cast<CGameObj*>(m_pBag->Get_SelectPoke())->Get_PokeInfo().iSDmg / (((180.f * 2.f) + 31.f) * (dynamic_cast<CGameObj*>(m_pBag->Get_SelectPoke())->Get_PokeInfo().iLv / 100.f) + 5.f);
		_float fDef = (_float)dynamic_cast<CGameObj*>(m_pBag->Get_SelectPoke())->Get_PokeInfo().iDef / (((230.f * 2.f) + 31.f) * (dynamic_cast<CGameObj*>(m_pBag->Get_SelectPoke())->Get_PokeInfo().iLv / 100.f) + 5.f);
		_float fSDef = (_float)dynamic_cast<CGameObj*>(m_pBag->Get_SelectPoke())->Get_PokeInfo().iSDef / (((230.f * 2.f) + 31.f) * (dynamic_cast<CGameObj*>(m_pBag->Get_SelectPoke())->Get_PokeInfo().iLv / 100.f) + 5.f);
		_float fSpeed = (_float)dynamic_cast<CGameObj*>(m_pBag->Get_SelectPoke())->Get_PokeInfo().iSpeed / (((160.f * 2.f) + 31.f) * (dynamic_cast<CGameObj*>(m_pBag->Get_SelectPoke())->Get_PokeInfo().iLv / 100.f) + 5.f);

		if (FAILED(m_pShaderCom4->Set_RawValue("g_fStatHp", &fHP, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom4->Set_RawValue("g_fStatDmg", &fDmg, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom4->Set_RawValue("g_fStatSDmg", &fSDmg, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom4->Set_RawValue("g_fStatDef", &fDef, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom4->Set_RawValue("g_fStatSDef", &fSDef, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom4->Set_RawValue("g_fStatSpeed", &fSpeed, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom4->Set_RawValue("g_WorldMatrix", &m_pTransformCom4->Get_World4x4_TP(), sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderCom4->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderCom4->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
			return E_FAIL;
	
		m_pShaderCom4->Begin(2);
		m_pVIBufferHexagonCom->Render();
	}
	else if (m_bSelect)
	{
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
	}
	return S_OK;
}

HRESULT CPokeInfo::SetUp_SkillResources()
{
	for (_int i = 0; i < 4; ++i)
	{
		if (FAILED(m_pShaderSkill[i]->Set_RawValue("g_WorldMatrix", &m_pTransformSkill[i]->Get_World4x4_TP(), sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderSkill[i]->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderSkill[i]->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
			return E_FAIL;
		
		if ((dynamic_cast<CGameObj*>(m_pBag->Get_SelectPoke())->Get_PokeInfo().eSkillNum1->iSkillNum != 99) && i == 0)
		{
			if (FAILED(m_pShaderSkill[i]->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom3->Get_SRV(dynamic_cast<CGameObj*>(m_pBag->Get_SelectPoke())->Get_PokeInfo().eSkillNum1->eType))))
				return E_FAIL;
			m_pShaderSkill[i]->Begin();
			m_pVIBufferSkill[i]->Render();
		}
		if ((dynamic_cast<CGameObj*>(m_pBag->Get_SelectPoke())->Get_PokeInfo().eSkillNum2->iSkillNum != 99) && i == 1)
		{
			if (FAILED(m_pShaderSkill[i]->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom3->Get_SRV(dynamic_cast<CGameObj*>(m_pBag->Get_SelectPoke())->Get_PokeInfo().eSkillNum2->eType))))
				return E_FAIL;
			m_pShaderSkill[i]->Begin();
			m_pVIBufferSkill[i]->Render();
		}
		if ((dynamic_cast<CGameObj*>(m_pBag->Get_SelectPoke())->Get_PokeInfo().eSkillNum3->iSkillNum != 99) && i == 2)
		{
			if (FAILED(m_pShaderSkill[i]->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom3->Get_SRV(dynamic_cast<CGameObj*>(m_pBag->Get_SelectPoke())->Get_PokeInfo().eSkillNum3->eType))))
				return E_FAIL;
			m_pShaderSkill[i]->Begin();
			m_pVIBufferSkill[i]->Render();
		}
		if ((dynamic_cast<CGameObj*>(m_pBag->Get_SelectPoke())->Get_PokeInfo().eSkillNum4->iSkillNum != 99) && i == 3)
		{
			if (FAILED(m_pShaderSkill[i]->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom3->Get_SRV(dynamic_cast<CGameObj*>(m_pBag->Get_SelectPoke())->Get_PokeInfo().eSkillNum4->eType))))
				return E_FAIL;
			m_pShaderSkill[i]->Begin();
			m_pVIBufferSkill[i]->Render();
		}
	}
	return S_OK;
}

HRESULT CPokeInfo::SetUp_InfoResources()
{
	for (_int i = 0; i < 3; ++i)
	{
		if (FAILED(m_pShaderInfo[i]->Set_RawValue("g_WorldMatrix", &m_pTransformInfo[i]->Get_World4x4_TP(), sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderInfo[i]->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderInfo[i]->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
			return E_FAIL;

		if ((dynamic_cast<CGameObj*>(m_pBag->Get_SelectPoke())->Get_PokeInfo().iBallNum != 99) && i == 0)
		{
			if (FAILED(m_pShaderInfo[i]->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom4->Get_SRV(dynamic_cast<CGameObj*>(m_pBag->Get_SelectPoke())->Get_PokeInfo().iBallNum))))
				return E_FAIL;
			m_pShaderInfo[i]->Begin();
			m_pVIBufferInfo[i]->Render();
		}
		if ((dynamic_cast<CGameObj*>(m_pBag->Get_SelectPoke())->Get_PokeInfo().iSex == 0) && i == 1)
		{
			if (FAILED(m_pShaderInfo[i]->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_SRV(5))))
				return E_FAIL;
			m_pShaderInfo[i]->Begin();
			m_pVIBufferInfo[i]->Render();
		}
		else if ((dynamic_cast<CGameObj*>(m_pBag->Get_SelectPoke())->Get_PokeInfo().iSex == 1) && i == 1)
		{
			if (FAILED(m_pShaderInfo[i]->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_SRV(6))))
				return E_FAIL;
			m_pShaderInfo[i]->Begin();
			m_pVIBufferInfo[i]->Render();
		}
		
		if (m_bSelect && i == 2)
		{
			CheckSkillType(m_iSelect);
		}
	
	}
	return S_OK;
}

HRESULT CPokeInfo::SetSelectButton(ButtonDir _eDir)
{
	switch (_eDir)
	{
	case CPokeInfo::DIR_UP:
		m_fButtonY -= 62.f;
		m_pTransformCom2->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fButtonX - g_iWinSizeX * 0.5f, -m_fButtonY + g_iWinSizeY * 0.5f, 0.f, 1.f));
		break;
	case CPokeInfo::DIR_DOWN:
		m_fButtonY += 62.f;
		m_pTransformCom2->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fButtonX - g_iWinSizeX * 0.5f, -m_fButtonY + g_iWinSizeY * 0.5f, 0.f, 1.f));
		break;
	default:
		break;
	}
	return S_OK;
}

void CPokeInfo::Key_Input()
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);
	if (m_bSelect && pGameInstance->Key_Down(DIK_LEFT))
	{
		m_bSelect = !m_bSelect;
		m_fButtonX = 45.f;
		m_fButtonY = 112.f;
		m_pTransformCom2->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fButtonX - g_iWinSizeX * 0.5f, -m_fButtonY + g_iWinSizeY * 0.5f, 0.f, 1.f));
		m_iSelect = 0;
	}
	else if (!m_bSelect && pGameInstance->Key_Down(DIK_RIGHT))
	{
		m_bSelect = !m_bSelect;
	}

	if (pGameInstance->Key_Down(DIK_BACKSPACE))
	{
		dynamic_cast<CGameObj*>(m_pBag->Get_SelectPoke())->Set_PokeUIOnOff();
		m_bSelect = false;
		g_PokeInfo = false;
		g_bBag = true;

	}
	if (pGameInstance->Key_Down(DIK_UP))
	{
		if (m_bSelect && m_iSelect != 0)
		{
			SetSelectButton(DIR_UP);
			--m_iSelect;
		}
	}
	if (pGameInstance->Key_Down(DIK_DOWN))
	{
		if(m_bSelect && m_iSelect != 3)
		{ 
			if (dynamic_cast<CGameObj*>(m_pBag->Get_SelectPoke())->Get_PokeInfo().eSkillNum2->iSkillNum != 99 && m_iSelect == 0)
			{
				SetSelectButton(DIR_DOWN);
				++m_iSelect;
			}
			else if (dynamic_cast<CGameObj*>(m_pBag->Get_SelectPoke())->Get_PokeInfo().eSkillNum3->iSkillNum != 99 && m_iSelect == 1)
			{
				SetSelectButton(DIR_DOWN);
				++m_iSelect;
			}
			else if (dynamic_cast<CGameObj*>(m_pBag->Get_SelectPoke())->Get_PokeInfo().eSkillNum4->iSkillNum != 99 && m_iSelect == 2)
			{
				SetSelectButton(DIR_DOWN);
				++m_iSelect;
			}
		}
	}
	Safe_Release(pGameInstance);
}

void CPokeInfo::Set_SkillPos()
{
	_float fSizeX = 150.f;
	_float fSizeY = 30.f;
	_float fX = 360.f;
	_float fY = 112.f;
	_float3 vScale = { fSizeX,fSizeY,0.f };

	for (_int i = 0; i < 4; ++i)
	{
		m_pTransformSkill[i]->Set_Scale(XMLoadFloat3(&vScale));
		m_pTransformSkill[i]->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(fX - g_iWinSizeX * 0.5f, -fY + g_iWinSizeY * 0.5f, 0.f, 1.f));
		fY += 62.f;
	}
}

void CPokeInfo::Set_InfoPos()
{
	_float fSizeX = 50.f;
	_float fSizeY = 50.f;
	_float fX = 700.f;
	_float fY = 57.f;
	_float3 vScale = { fSizeX,fSizeY,0.f };

	m_pTransformInfo[0]->Set_Scale(XMLoadFloat3(&vScale));
	m_pTransformInfo[0]->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(fX - g_iWinSizeX * 0.5f, -fY + g_iWinSizeY * 0.5f, 0.f, 1.f));
	
	fSizeX = 35.f;
	fSizeY = 35.f;
	vScale = { fSizeX,fSizeY,0.f };
	fX = 1100.f;
	m_pTransformInfo[1]->Set_Scale(XMLoadFloat3(&vScale));
	m_pTransformInfo[1]->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(fX - g_iWinSizeX * 0.5f, -fY + g_iWinSizeY * 0.5f, 0.f, 1.f));

	fSizeX = 50.f;
	fSizeY = 40.f;
	vScale = { fSizeX,fSizeY,0.f };
	fX = 128.f;
	fY = 584.f;
	m_pTransformInfo[2]->Set_Scale(XMLoadFloat3(&vScale));
	m_pTransformInfo[2]->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(fX - g_iWinSizeX * 0.5f, -fY + g_iWinSizeY * 0.5f, 0.f, 1.f));

}

void CPokeInfo::RenderFonts()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	
	_vector vPos = {70.f,100.f,0.f,1.f };
	_vector vPos2 = { 515.f,100.f,0.f,1.f };
	_vector vDmgPos = { 108.f,614.f,0.f,1.f };
	_vector	vHitPos = { 108.f,650.f,0.f,1.f };
	_vector vScale = { 0.8f,0.8f,0.8f,1.f };
	wstring strNum = TEXT("");
	wstring strDmg = TEXT("");
	wstring strHit = TEXT("");
	for (_int i = 0; i < 4; ++i)
	{
		if (dynamic_cast<CGameObj*>(m_pBag->Get_SelectPoke())->Get_PokeInfo().eSkillNum1->iSkillNum != 99 && i == 0)
		{
			strNum = TEXT("");
		
			strNum += to_wstring(dynamic_cast<CGameObj*>(m_pBag->Get_SelectPoke())->Get_PokeInfo().eSkillNum1->iPoint);
			strNum += TEXT("/");
			strNum += to_wstring(dynamic_cast<CGameObj*>(m_pBag->Get_SelectPoke())->Get_PokeInfo().eSkillNum1->iMaxPoint);
			pGameInstance->Render_Font(TEXT("Font_Nexon"), dynamic_cast<CGameObj*>(m_pBag->Get_SelectPoke())->Get_PokeInfo().eSkillNum1->strName.c_str(), vPos, XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f));
			pGameInstance->Render_Font(TEXT("Font_Nexon"), strNum.c_str(), vPos2, XMVectorSet(1.f, 1.f, 1.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f));
			if (m_iSelect == 0)
			{
				strDmg = TEXT("");
				strHit = TEXT("");
				if (dynamic_cast<CGameObj*>(m_pBag->Get_SelectPoke())->Get_PokeInfo().eSkillNum1->iDmg == 0)
					strDmg = to_wstring(dynamic_cast<CGameObj*>(m_pBag->Get_SelectPoke())->Get_PokeInfo().eSkillNum1->iSDmg);
				else
					strDmg = to_wstring(dynamic_cast<CGameObj*>(m_pBag->Get_SelectPoke())->Get_PokeInfo().eSkillNum1->iDmg);
				strHit = to_wstring((_int)dynamic_cast<CGameObj*>(m_pBag->Get_SelectPoke())->Get_PokeInfo().eSkillNum1->fHit);
				pGameInstance->Render_Font(TEXT("Font_Nexon"), strDmg.c_str(), vDmgPos, XMVectorSet(0.f, 0.f, 0.f, 1.f), vScale);
				pGameInstance->Render_Font(TEXT("Font_Nexon"), strHit.c_str(), vHitPos, XMVectorSet(0.f, 0.f, 0.f, 1.f), vScale);
			}
		}
		if (dynamic_cast<CGameObj*>(m_pBag->Get_SelectPoke())->Get_PokeInfo().eSkillNum2->iSkillNum != 99 && i == 1)
		{
			strNum = TEXT("");

			strNum += to_wstring(dynamic_cast<CGameObj*>(m_pBag->Get_SelectPoke())->Get_PokeInfo().eSkillNum2->iPoint);
			strNum += TEXT("/");
			strNum += to_wstring(dynamic_cast<CGameObj*>(m_pBag->Get_SelectPoke())->Get_PokeInfo().eSkillNum2->iMaxPoint);
			pGameInstance->Render_Font(TEXT("Font_Nexon"), dynamic_cast<CGameObj*>(m_pBag->Get_SelectPoke())->Get_PokeInfo().eSkillNum2->strName.c_str(), vPos, XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f));
			pGameInstance->Render_Font(TEXT("Font_Nexon"), strNum.c_str(), vPos2, XMVectorSet(1.f, 1.f, 1.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f));
			if (m_iSelect == 1)
			{
				strDmg = TEXT("");
				strHit = TEXT("");
				if (dynamic_cast<CGameObj*>(m_pBag->Get_SelectPoke())->Get_PokeInfo().eSkillNum2->iDmg == 0)
					strDmg = to_wstring(dynamic_cast<CGameObj*>(m_pBag->Get_SelectPoke())->Get_PokeInfo().eSkillNum2->iSDmg);
				else
					strDmg = to_wstring(dynamic_cast<CGameObj*>(m_pBag->Get_SelectPoke())->Get_PokeInfo().eSkillNum2->iDmg);
				strHit = to_wstring((_int)dynamic_cast<CGameObj*>(m_pBag->Get_SelectPoke())->Get_PokeInfo().eSkillNum2->fHit);
				pGameInstance->Render_Font(TEXT("Font_Nexon"), strDmg.c_str(), vDmgPos, XMVectorSet(0.f, 0.f, 0.f, 1.f), vScale);
				pGameInstance->Render_Font(TEXT("Font_Nexon"), strHit.c_str(), vHitPos, XMVectorSet(0.f, 0.f, 0.f, 1.f), vScale);
			}
		}
		if (dynamic_cast<CGameObj*>(m_pBag->Get_SelectPoke())->Get_PokeInfo().eSkillNum3->iSkillNum != 99 && i == 2)
		{
			strNum = TEXT("");

			strNum += to_wstring(dynamic_cast<CGameObj*>(m_pBag->Get_SelectPoke())->Get_PokeInfo().eSkillNum3->iPoint);
			strNum += TEXT("/");
			strNum += to_wstring(dynamic_cast<CGameObj*>(m_pBag->Get_SelectPoke())->Get_PokeInfo().eSkillNum3->iMaxPoint);
			pGameInstance->Render_Font(TEXT("Font_Nexon"), dynamic_cast<CGameObj*>(m_pBag->Get_SelectPoke())->Get_PokeInfo().eSkillNum3->strName.c_str(), vPos, XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f));
			pGameInstance->Render_Font(TEXT("Font_Nexon"), strNum.c_str(), vPos2, XMVectorSet(1.f, 1.f, 1.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f));
			if (m_iSelect == 2)
			{
				strDmg = TEXT("");
				strHit = TEXT("");
				if (dynamic_cast<CGameObj*>(m_pBag->Get_SelectPoke())->Get_PokeInfo().eSkillNum3->iDmg == 0)
					strDmg = to_wstring(dynamic_cast<CGameObj*>(m_pBag->Get_SelectPoke())->Get_PokeInfo().eSkillNum3->iSDmg);
				else
					strDmg = to_wstring(dynamic_cast<CGameObj*>(m_pBag->Get_SelectPoke())->Get_PokeInfo().eSkillNum3->iDmg);
				strHit = to_wstring((_int)dynamic_cast<CGameObj*>(m_pBag->Get_SelectPoke())->Get_PokeInfo().eSkillNum3->fHit);
				pGameInstance->Render_Font(TEXT("Font_Nexon"), strDmg.c_str(), vDmgPos, XMVectorSet(0.f, 0.f, 0.f, 1.f), vScale);
				pGameInstance->Render_Font(TEXT("Font_Nexon"), strHit.c_str(), vHitPos, XMVectorSet(0.f, 0.f, 0.f, 1.f), vScale);
			}
		}
		if (dynamic_cast<CGameObj*>(m_pBag->Get_SelectPoke())->Get_PokeInfo().eSkillNum4->iSkillNum != 99 && i == 3)
		{
			strNum = TEXT("");

			strNum += to_wstring(dynamic_cast<CGameObj*>(m_pBag->Get_SelectPoke())->Get_PokeInfo().eSkillNum4->iPoint);
			strNum += TEXT("/");
			strNum += to_wstring(dynamic_cast<CGameObj*>(m_pBag->Get_SelectPoke())->Get_PokeInfo().eSkillNum4->iMaxPoint);
			pGameInstance->Render_Font(TEXT("Font_Nexon"), dynamic_cast<CGameObj*>(m_pBag->Get_SelectPoke())->Get_PokeInfo().eSkillNum4->strName.c_str(), vPos, XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f));
			pGameInstance->Render_Font(TEXT("Font_Nexon"), strNum.c_str(), vPos2, XMVectorSet(1.f, 1.f, 1.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f));
			if (m_iSelect == 3)
			{
				strDmg = TEXT("");
				strHit = TEXT("");
				if (dynamic_cast<CGameObj*>(m_pBag->Get_SelectPoke())->Get_PokeInfo().eSkillNum4->iDmg == 0)
					strDmg = to_wstring(dynamic_cast<CGameObj*>(m_pBag->Get_SelectPoke())->Get_PokeInfo().eSkillNum4->iSDmg);
				else
					strDmg = to_wstring(dynamic_cast<CGameObj*>(m_pBag->Get_SelectPoke())->Get_PokeInfo().eSkillNum4->iDmg);
				strHit = to_wstring((_int)dynamic_cast<CGameObj*>(m_pBag->Get_SelectPoke())->Get_PokeInfo().eSkillNum4->fHit);
				pGameInstance->Render_Font(TEXT("Font_Nexon"), strDmg.c_str(), vDmgPos, XMVectorSet(0.f, 0.f, 0.f, 1.f), vScale);
				pGameInstance->Render_Font(TEXT("Font_Nexon"), strHit.c_str(), vHitPos, XMVectorSet(0.f, 0.f, 0.f, 1.f), vScale);
			}
		}
		vPos.m128_f32[1] += 62.f;
		vPos2.m128_f32[1] += 62.f;
	}
	strNum = TEXT("Lv.");
	vPos = { 750.f,42.f,0.f,1.f };
	pGameInstance->Render_Font(TEXT("Font_Nexon"), dynamic_cast<CGameObj*>(m_pBag->Get_SelectPoke())->Get_PokeInfo().strName.c_str(), vPos, XMVectorSet(1.f, 1.f, 1.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f));
	vPos.m128_f32[0] += 400.f;
	strNum += to_wstring(dynamic_cast<CGameObj*>(m_pBag->Get_SelectPoke())->Get_PokeInfo().iLv);
	pGameInstance->Render_Font(TEXT("Font_Nexon"), strNum.c_str(), vPos, XMVectorSet(1.f, 1.f, 1.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f));

	vPos = { 50.f,525.f,0.f,1.f };
	vPos2 = { 250.f,580.f,0.f,1.f };
	switch (m_iSelect)
	{
	case 0:
		pGameInstance->Render_Font(TEXT("Font_Nexon"), dynamic_cast<CGameObj*>(m_pBag->Get_SelectPoke())->Get_PokeInfo().eSkillNum1->strName.c_str(), vPos, XMVectorSet(1.f, 1.f, 1.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f));
		pGameInstance->Render_Font(TEXT("Font_Nexon"), dynamic_cast<CGameObj*>(m_pBag->Get_SelectPoke())->Get_PokeInfo().eSkillNum1->strInfo.c_str(), vPos2, XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f));
		break;
	case 1:
		pGameInstance->Render_Font(TEXT("Font_Nexon"), dynamic_cast<CGameObj*>(m_pBag->Get_SelectPoke())->Get_PokeInfo().eSkillNum2->strName.c_str(), vPos, XMVectorSet(1.f, 1.f, 1.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f));
		pGameInstance->Render_Font(TEXT("Font_Nexon"), dynamic_cast<CGameObj*>(m_pBag->Get_SelectPoke())->Get_PokeInfo().eSkillNum2->strInfo.c_str(), vPos2, XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f));
		break;
	case 2:
		pGameInstance->Render_Font(TEXT("Font_Nexon"), dynamic_cast<CGameObj*>(m_pBag->Get_SelectPoke())->Get_PokeInfo().eSkillNum3->strName.c_str(), vPos, XMVectorSet(1.f, 1.f, 1.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f));
		pGameInstance->Render_Font(TEXT("Font_Nexon"), dynamic_cast<CGameObj*>(m_pBag->Get_SelectPoke())->Get_PokeInfo().eSkillNum3->strInfo.c_str(), vPos2, XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f));
		break;
	case 3:
		pGameInstance->Render_Font(TEXT("Font_Nexon"), dynamic_cast<CGameObj*>(m_pBag->Get_SelectPoke())->Get_PokeInfo().eSkillNum4->strName.c_str(), vPos, XMVectorSet(1.f, 1.f, 1.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f));
		pGameInstance->Render_Font(TEXT("Font_Nexon"), dynamic_cast<CGameObj*>(m_pBag->Get_SelectPoke())->Get_PokeInfo().eSkillNum4->strInfo.c_str(), vPos2, XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f));
		break;
	default:
		break;
	}
	
	RELEASE_INSTANCE(CGameInstance);
}

void CPokeInfo::RenderFonts2()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	
	_vector vPos = { 0.f,0.f,0.f,1.f };
	_vector vPos2 = { 0.f,0.f,0.f,1.f };
	wstring strNum = TEXT("");

	strNum = TEXT("Lv.");
	vPos = { 750.f,42.f,0.f,1.f };
	pGameInstance->Render_Font(TEXT("Font_Nexon"), dynamic_cast<CGameObj*>(m_pBag->Get_SelectPoke())->Get_PokeInfo().strName.c_str(), vPos, XMVectorSet(1.f, 1.f, 1.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f));
	vPos.m128_f32[0] += 400.f;
	strNum += to_wstring(dynamic_cast<CGameObj*>(m_pBag->Get_SelectPoke())->Get_PokeInfo().iLv);
	pGameInstance->Render_Font(TEXT("Font_Nexon"), strNum.c_str(), vPos, XMVectorSet(1.f, 1.f, 1.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f));
	
	strNum = TEXT("성격");
	vPos = { 100.f,470.f,0.f,1.f };
	vPos2 = { 390.f,470.f,0.f,1.f };
	pGameInstance->Render_Font(TEXT("Font_Nexon"), strNum.c_str(), vPos, XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f));
	pGameInstance->Render_Font(TEXT("Font_Nexon"), dynamic_cast<CGameObj*>(m_pBag->Get_SelectPoke())->Get_PokeInfo().strChar.c_str(), vPos2, XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f));

	strNum = TEXT("지닌 물건");
	vPos = { 75.f,515.f,0.f,1.f };
	vPos2 = { 390.f,515.f,0.f,1.f };
	pGameInstance->Render_Font(TEXT("Font_Nexon"), strNum.c_str(), vPos, XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f));
	pGameInstance->Render_Font(TEXT("Font_Nexon"), dynamic_cast<CGameObj*>(m_pBag->Get_SelectPoke())->Get_PokeInfo().eItem->strName.c_str(), vPos2, XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f));
	vPos2 = { 10.f,575.f,0.f,1.f };
	pGameInstance->Render_Font(TEXT("Font_Nexon"), dynamic_cast<CGameObj*>(m_pBag->Get_SelectPoke())->Get_PokeInfo().eItem->strInfo.c_str(), vPos2, XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f));

	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CPokeInfo::CheckSkillType(_int _iIndex)
{
	switch (_iIndex)
	{
	case 0:
		if (dynamic_cast<CGameObj*>(m_pBag->Get_SelectPoke())->Get_PokeInfo().eSkillNum1->iDmg == 0)
		{
			if (FAILED(m_pShaderInfo[2]->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom2->Get_SRV(0))))
				return E_FAIL;
			m_pShaderInfo[2]->Begin();
			m_pVIBufferInfo[2]->Render();
		}
		else
		{
			if (FAILED(m_pShaderInfo[2]->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom2->Get_SRV(1))))
				return E_FAIL;
			m_pShaderInfo[2]->Begin();
			m_pVIBufferInfo[2]->Render();
		}
		break;
	case 1:
		if (dynamic_cast<CGameObj*>(m_pBag->Get_SelectPoke())->Get_PokeInfo().eSkillNum2->iDmg == 0)
		{
			if (FAILED(m_pShaderInfo[2]->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom2->Get_SRV(0))))
				return E_FAIL;
			m_pShaderInfo[2]->Begin();
			m_pVIBufferInfo[2]->Render();
		}
		else
		{
			if (FAILED(m_pShaderInfo[2]->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom2->Get_SRV(1))))
				return E_FAIL;
			m_pShaderInfo[2]->Begin();
			m_pVIBufferInfo[2]->Render();
		}
		break;
	case 2:
		if (dynamic_cast<CGameObj*>(m_pBag->Get_SelectPoke())->Get_PokeInfo().eSkillNum3->iDmg == 0)
		{
			if (FAILED(m_pShaderInfo[2]->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom2->Get_SRV(0))))
				return E_FAIL;
			m_pShaderInfo[2]->Begin();
			m_pVIBufferInfo[2]->Render();
		}
		else
		{
			if (FAILED(m_pShaderInfo[2]->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom2->Get_SRV(1))))
				return E_FAIL;
			m_pShaderInfo[2]->Begin();
			m_pVIBufferInfo[2]->Render();
		}
		break;
	case 3:
		if (dynamic_cast<CGameObj*>(m_pBag->Get_SelectPoke())->Get_PokeInfo().eSkillNum4->iDmg == 0)
		{
			if (FAILED(m_pShaderInfo[2]->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom2->Get_SRV(0))))
				return E_FAIL;
			m_pShaderInfo[2]->Begin();
			m_pVIBufferInfo[2]->Render();
		}
		else
		{
			if (FAILED(m_pShaderInfo[2]->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom2->Get_SRV(1))))
				return E_FAIL;
			m_pShaderInfo[2]->Begin();
			m_pVIBufferInfo[2]->Render();
		}
		break;
	default:
		break;
	}
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

	for (int i = 0; i < 4; ++i)
	{
		Safe_Release(m_pVIBufferSkill[i]);
		Safe_Release(m_pTransformSkill[i]);
		Safe_Release(m_pShaderSkill[i]);
	}
	for (int i = 0; i < 3; ++i)
	{
		Safe_Release(m_pVIBufferInfo[i]);
		Safe_Release(m_pTransformInfo[i]);
		Safe_Release(m_pShaderInfo[i]);
	}
	Safe_Release(m_pTextureCom2);
	Safe_Release(m_pTextureCom3);
	Safe_Release(m_pTextureCom4);
	Safe_Release(m_pTransformCom2);
	Safe_Release(m_pTransformCom3);
	Safe_Release(m_pTransformCom4);
	Safe_Release(m_pShaderCom2);
	Safe_Release(m_pShaderCom3);
	Safe_Release(m_pShaderCom4);
	Safe_Release(m_pVIBufferCom2);
	Safe_Release(m_pVIBufferCom3);
	Safe_Release(m_pVIBufferHexagonCom);
}
