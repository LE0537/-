#include "stdafx.h"
#include "..\Public\BattleUI.h"

#include "GameInstance.h"
#include "MonsterBall.h"
#include "Player.h"
#include "Bag.h"
#include "TextBox.h"
#include "SoundMgr.h"

CBattleUI::CBattleUI(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
}

CBattleUI::CBattleUI(const CBattleUI & rhs)
	: CGameObj(rhs)
{
}

HRESULT CBattleUI::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBattleUI::Initialize(void * pArg)
{
	ZeroMemory(&m_tInfo, sizeof(BATTLEINFO));
	memcpy(&m_tInfo, pArg, sizeof(BATTLEINFO));

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_szPlayerName = TEXT("쥬신");

	m_fSizeX = 50.f;
	m_fSizeY = 50.f;
	m_fX = 1000.f;
	m_fY = 450.f;

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixTranspose(XMMatrixIdentity()));
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f)));

	_float3 vScale = { m_fSizeX,m_fSizeY,0.f };

	m_pTransformCom->Set_Scale(XMLoadFloat3(&vScale));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));

	Set_Pos();
	Set_PlayerPos();
	Set_TargetPos();
	Set_BallPos();

	for (_int i = 0; i < 6; ++i)
	{
		if (dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(i))->Get_PokeInfo().eStatInfo != STUN)
		{
			m_iPlayerIndex = i;
			m_iPlayerHPIndex = i;
			i = 6;
		}
	}
	
	return S_OK;
}

void CBattleUI::Tick(_float fTimeDelta)
{
	if (m_bWildWin)
		Wild_Win(fTimeDelta);
	if (!m_tInfo.pPlayer->Get_BattlePokeDead() && !m_tInfo.pPlayer->Get_BattleChangePoke() && !m_tInfo.pPlayer->Get_BattleUseItem())
	{
		if (m_bPlayerAttack)
		{
			m_fHPTime += fTimeDelta;
			BattleFrame();
			BattleDelay(fTimeDelta);
		}
		else if (m_bBattleChangePoke)
		{
			if (m_tInfo.pPlayer->Get_Close())
			{
				m_bBattleChangePoke = false;
				m_tInfo.pPlayer->Set_Close();
				return;
			}
			Change_Poke(fTimeDelta);
		}
		else if (m_bBattleUseItem)
		{
			if (m_tInfo.pPlayer->Get_Close())
			{
				m_bBattleUseItem = false;
				m_tInfo.pPlayer->Set_Close();
				return;
			}
			Use_Item(fTimeDelta);
		}
		else
		{
			Key_Input();
		}
		if (!g_Battle)
		{
			m_bDead = true;
			CSoundMgr::Get_Instance()->BGM_Stop();
			CSoundMgr::Get_Instance()->PlayBGM(TEXT("hov.wav"), 0.75f);
		}
	}
}

void CBattleUI::Late_Tick(_float fTimeDelta)
{
	if (!m_tInfo.pPlayer->Get_BattlePokeDead() && !m_tInfo.pPlayer->Get_BattleChangePoke() && !m_tInfo.pPlayer->Get_BattleUseItem() && nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup_Front(CRenderer::RENDER_UI, this);
	
}

HRESULT CBattleUI::Render()
{

	if (nullptr == m_pShaderCom || nullptr == m_pVIBufferCom)
		return E_FAIL;
	if (!m_bPlayerAttack)
	{
		if (!m_bSkill)
		{
			if (FAILED(SetUp_ShaderResources()))
				return E_FAIL;
		}
		else if (m_bSkill)
		{
			if (FAILED(SetUp_ShaderSkill()))
				return E_FAIL;
		}
	}
	if (FAILED(SetUp_ShaderPlayer()))
		return E_FAIL;
	if (FAILED(SetUp_ShaderTarget()))
		return E_FAIL;
	if (FAILED(SetUp_ShaderBall()))
		return E_FAIL;
	
	Render_Fonts();
	

	return S_OK;
}

HRESULT CBattleUI::Ready_Components()
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
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Battle"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture2"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_TypeSmall"), (CComponent**)&m_pTextureCom2)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture3"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI"), (CComponent**)&m_pTextureCom3)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture4"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_StatInfo"), (CComponent**)&m_pTextureCom4)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	wstring szInfoBuffer[4];
	wstring szInfoTrans[4];
	wstring szInfoShader[4];
	for (int i = 0; i < 4; ++i)
	{
		szInfoBuffer[i] = TEXT("Com_VIBufferInfo");
		szInfoTrans[i] = TEXT("Com_TransformInfo");
		szInfoShader[i] = TEXT("Com_ShaderInfo");
	}
	for (int i = 0; i < 4; ++i)
	{
		szInfoBuffer[i] += to_wstring(i);
		szInfoTrans[i] += to_wstring(i);
		szInfoShader[i] += to_wstring(i);
		if (FAILED(__super::Add_Components(szInfoBuffer[i].c_str(), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferInfo[i])))
			return E_FAIL;
		if (FAILED(__super::Add_Components(szInfoTrans[i].c_str(), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformInfo[i])))
			return E_FAIL;
		if (FAILED(__super::Add_Components(szInfoShader[i].c_str(), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), (CComponent**)&m_pShaderInfo[i])))
			return E_FAIL;
	}

	wstring szBallBuffer[12];
	wstring szBallTrans[12];
	wstring szBallShader[12];
	for (int i = 0; i < 12; ++i)
	{
		szBallBuffer[i] = TEXT("Com_VIBufferBall");
		szBallTrans[i] = TEXT("Com_TransformBall");
		szBallShader[i] = TEXT("Com_ShaderBall");
	}
	for (int i = 0; i < 12; ++i)
	{
		szBallBuffer[i] += to_wstring(i);
		szBallTrans[i] += to_wstring(i);
		szBallShader[i] += to_wstring(i);
		if (FAILED(__super::Add_Components(szBallBuffer[i].c_str(), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferBall[i])))
			return E_FAIL;
		if (FAILED(__super::Add_Components(szBallTrans[i].c_str(), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformBall[i])))
			return E_FAIL;
		if (FAILED(__super::Add_Components(szBallShader[i].c_str(), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), (CComponent**)&m_pShaderBall[i])))
			return E_FAIL;
	}

	wstring szSkillBuffer[8];
	wstring szSkillTrans[8];
	wstring szSkillShader[8];
	for (int i = 0; i < 8; ++i)
	{
		szSkillBuffer[i] = TEXT("Com_VIBufferSkill");
		szSkillTrans[i] = TEXT("Com_TransformSkill");
		szSkillShader[i] = TEXT("Com_ShaderSkill");
	}
	for (int i = 0; i < 8; ++i)
	{
		szSkillBuffer[i] += to_wstring(i);
		szSkillTrans[i] += to_wstring(i);
		szSkillShader[i] += to_wstring(i);
		if (FAILED(__super::Add_Components(szSkillBuffer[i].c_str(), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferSkill[i])))
			return E_FAIL;
		if (FAILED(__super::Add_Components(szSkillTrans[i].c_str(), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformSkill[i])))
			return E_FAIL;
		if (FAILED(__super::Add_Components(szSkillShader[i].c_str(), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), (CComponent**)&m_pShaderSkill[i])))
			return E_FAIL;
	}

	wstring szTargetBuffer[5];
	wstring szTargetTrans[5];
	wstring szTargetShader[5];
	for (int i = 0; i < 5; ++i)
	{
		szTargetBuffer[i] = TEXT("Com_VIBufferTarget");
		szTargetTrans[i] = TEXT("Com_TransformTarget");
		szTargetShader[i] = TEXT("Com_ShaderTarget");
	}
	for (int i = 0; i < 5; ++i)
	{
		szTargetBuffer[i] += to_wstring(i);
		szTargetTrans[i] += to_wstring(i);
		szTargetShader[i] += to_wstring(i);
		if (FAILED(__super::Add_Components(szTargetBuffer[i].c_str(), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferTarget[i])))
			return E_FAIL;
		if (FAILED(__super::Add_Components(szTargetTrans[i].c_str(), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformTarget[i])))
			return E_FAIL;
		if (FAILED(__super::Add_Components(szTargetShader[i].c_str(), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), (CComponent**)&m_pShaderTarget[i])))
			return E_FAIL;
	}

	wstring szPlayerBuffer[6];
	wstring szPlayerTrans[6];
	wstring szPlayerShader[6];
	for (int i = 0; i < 6; ++i)
	{
		szPlayerBuffer[i] = TEXT("Com_VIBufferPlayer");
		szPlayerTrans[i] = TEXT("Com_TransformPlayer");
		szPlayerShader[i] = TEXT("Com_ShaderPlayer");
	}
	for (int i = 0; i < 6; ++i)
	{
		szPlayerBuffer[i] += to_wstring(i);
		szPlayerTrans[i] += to_wstring(i);
		szPlayerShader[i] += to_wstring(i);
		if (FAILED(__super::Add_Components(szPlayerBuffer[i].c_str(), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferPlayer[i])))
			return E_FAIL;
		if (FAILED(__super::Add_Components(szPlayerTrans[i].c_str(), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformPlayer[i])))
			return E_FAIL;
		if (FAILED(__super::Add_Components(szPlayerShader[i].c_str(), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), (CComponent**)&m_pShaderPlayer[i])))
			return E_FAIL;
	}
	return S_OK;
}

HRESULT CBattleUI::SetUp_ShaderResources()
{
	_int iSkillIndex = 2;
	
	for (_uint i = 0; i < 4; ++i)
	{
		if (FAILED(m_pShaderInfo[i]->Set_RawValue("g_WorldMatrix", &m_pTransformInfo[i]->Get_World4x4_TP(), sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderInfo[i]->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderInfo[i]->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderInfo[i]->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_SRV(iSkillIndex))))
			return E_FAIL;

		m_pShaderInfo[i]->Begin(7);
		m_pVIBufferInfo[i]->Render();
		++iSkillIndex;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_World4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom3->Get_SRV(0))))
		return E_FAIL;

	m_pShaderCom->Begin();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CBattleUI::SetUp_ShaderSkill()
{
	for (_uint i = 0; i < 4; ++i)
	{
		if (FAILED(m_pShaderSkill[i]->Set_RawValue("g_WorldMatrix", &m_pTransformSkill[i]->Get_World4x4_TP(), sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderSkill[i]->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderSkill[i]->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderSkill[i]->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_SRV(8))))
			return E_FAIL;

		m_pShaderSkill[i]->Begin(7);
		m_pVIBufferSkill[i]->Render();
	}

	for (_uint i = 4; i < 8; ++i)
	{
		if (FAILED(m_pShaderSkill[i]->Set_RawValue("g_WorldMatrix", &m_pTransformSkill[i]->Get_World4x4_TP(), sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderSkill[i]->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderSkill[i]->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
			return E_FAIL;
		if (i == 4)
		{
			if (FAILED(m_pShaderSkill[i]->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom2->Get_SRV(dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eSkillNum1->eType))))
				return E_FAIL;
			m_pShaderSkill[i]->Begin(7);
			m_pVIBufferSkill[i]->Render();
		}
		if ((dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eSkillNum2->iSkillNum != 99) && i == 5)
		{
			if (FAILED(m_pShaderSkill[i]->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom2->Get_SRV(dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eSkillNum2->eType))))
				return E_FAIL;
			m_pShaderSkill[i]->Begin(7);
			m_pVIBufferSkill[i]->Render();
		}
		if ((dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eSkillNum3->iSkillNum != 99) && i == 6)
		{
			if (FAILED(m_pShaderSkill[i]->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom2->Get_SRV(dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eSkillNum3->eType))))
				return E_FAIL;
			m_pShaderSkill[i]->Begin(7);
			m_pVIBufferSkill[i]->Render();
		}
		if ((dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eSkillNum4->iSkillNum != 99) && i == 7)
		{
			if (FAILED(m_pShaderSkill[i]->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom2->Get_SRV(dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eSkillNum4->eType))))
				return E_FAIL;
			m_pShaderSkill[i]->Begin(7);
			m_pVIBufferSkill[i]->Render();
		}
		
	}

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_World4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom3->Get_SRV(0))))
		return E_FAIL;

	m_pShaderCom->Begin();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CBattleUI::SetUp_ShaderPlayer()
{

	if (FAILED(m_pShaderPlayer[0]->Set_RawValue("g_WorldMatrix", &m_pTransformPlayer[0]->Get_World4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderPlayer[0]->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderPlayer[0]->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderPlayer[0]->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_SRV(6))))
		return E_FAIL;

	m_pShaderPlayer[0]->Begin(7);
	m_pVIBufferPlayer[0]->Render();
	

	if (dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().iSex == 0)
	{
		if (FAILED(m_pShaderPlayer[1]->Set_RawValue("g_WorldMatrix", &m_pTransformPlayer[1]->Get_World4x4_TP(), sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderPlayer[1]->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderPlayer[1]->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderPlayer[1]->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom3->Get_SRV(5))))
			return E_FAIL;
		m_pShaderPlayer[1]->Begin(7);
		m_pVIBufferPlayer[1]->Render();
	}
	else if (dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().iSex == 1)
	{
		if (FAILED(m_pShaderPlayer[1]->Set_RawValue("g_WorldMatrix", &m_pTransformPlayer[1]->Get_World4x4_TP(), sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderPlayer[1]->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderPlayer[1]->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderPlayer[1]->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom3->Get_SRV(6))))
			return E_FAIL;
		m_pShaderPlayer[1]->Begin(7);
		m_pVIBufferPlayer[1]->Render();
	}
	if (dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eStatInfo != STATINFO_END)
	{
		if (FAILED(m_pShaderPlayer[2]->Set_RawValue("g_WorldMatrix", &m_pTransformPlayer[2]->Get_World4x4_TP(), sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderPlayer[2]->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderPlayer[2]->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderPlayer[2]->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom4->Get_SRV(dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eStatInfo))))
			return E_FAIL;

		m_pShaderPlayer[2]->Begin(7);
		m_pVIBufferPlayer[2]->Render();
	}
	if (dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerHPIndex))->Get_PokeInfo().iHp > 0)
	{
		_float fHp = 0.f;
		_float HpfSizeX = 258.f;
		_float HpfSizeY = 16.f;
		_float HpfX = 153.f;
		_float HpfY = 639.f;
		_float3 HpvScale = { HpfSizeX,HpfSizeY,0.f };
		_float fShaderHp = (_float)dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerHPIndex))->Get_PokeInfo().iHp / (_float)dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerHPIndex))->Get_PokeInfo().iMaxHp;
		fHp = HpfSizeX * fShaderHp;
	
		HpfX -= (HpfSizeX - fHp) / 2.f;

		HpvScale = { fHp,HpfSizeY,1.f };
		m_pTransformPlayer[3]->Set_Scale(XMLoadFloat3(&HpvScale));
		m_pTransformPlayer[3]->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(HpfX - (_float)g_iWinSizeX * 0.5f, -HpfY + (_float)g_iWinSizeY * 0.5f, 0.f, 1.f));

		if (FAILED(m_pShaderPlayer[3]->Set_RawValue("g_fHP", &fShaderHp, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderPlayer[3]->Set_RawValue("g_WorldMatrix", &m_pTransformPlayer[3]->Get_World4x4_TP(), sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderPlayer[3]->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderPlayer[3]->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderPlayer[3]->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom3->Get_SRV(7))))
			return E_FAIL;

		m_pShaderPlayer[3]->Begin(1);
		m_pVIBufferPlayer[3]->Render();
	}
	if (dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().iExp > 0)
	{
		_float fExp = 0.f;
		_float ExpfSizeX = 116.f;
		_float ExpfSizeY = 6.f;
		_float ExpfX = 224.f;
		_float ExpfY = 655.f;
		_float3 ExpvScale = { ExpfSizeX,ExpfSizeY,0.f };
		_float fShaderExp = (_float)dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().iExp / (_float)dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().iMaxExp;

		fExp = ExpfSizeX * fShaderExp;

		ExpfX -= (ExpfSizeX - fExp) / 2.f;

		ExpvScale = { fExp,ExpfSizeY,0.f };
		m_pTransformPlayer[4]->Set_Scale(XMLoadFloat3(&ExpvScale));
		m_pTransformPlayer[4]->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(ExpfX - g_iWinSizeX * 0.5f, -ExpfY + g_iWinSizeY * 0.5f, 0.f, 1.f));

		if (FAILED(m_pShaderPlayer[4]->Set_RawValue("g_WorldMatrix", &m_pTransformPlayer[4]->Get_World4x4_TP(), sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderPlayer[4]->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderPlayer[4]->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderPlayer[4]->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom3->Get_SRV(7))))
			return E_FAIL;

		m_pShaderPlayer[4]->Begin(8);
		m_pVIBufferPlayer[4]->Render();
	}
	if (FAILED(m_pShaderPlayer[5]->Set_RawValue("g_WorldMatrix", &m_pTransformPlayer[5]->Get_World4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderPlayer[5]->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderPlayer[5]->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderPlayer[5]->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_SRV(19))))
		return E_FAIL;

	m_pShaderPlayer[5]->Begin(7);
	m_pVIBufferPlayer[5]->Render();

	return S_OK;
}

HRESULT CBattleUI::SetUp_ShaderTarget()
{

	if (FAILED(m_pShaderTarget[0]->Set_RawValue("g_WorldMatrix", &m_pTransformTarget[0]->Get_World4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderTarget[0]->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderTarget[0]->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderTarget[0]->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_SRV(7))))
		return E_FAIL;

	m_pShaderTarget[0]->Begin(7);
	m_pVIBufferTarget[0]->Render();

	if (dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().iSex == 0)
	{
		if (FAILED(m_pShaderTarget[1]->Set_RawValue("g_WorldMatrix", &m_pTransformTarget[1]->Get_World4x4_TP(), sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderTarget[1]->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderTarget[1]->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderTarget[1]->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom3->Get_SRV(5))))
			return E_FAIL;

		m_pShaderTarget[1]->Begin(7);
		m_pVIBufferTarget[1]->Render();
	}
	else if (dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().iSex == 1)
	{
		if (FAILED(m_pShaderTarget[1]->Set_RawValue("g_WorldMatrix", &m_pTransformTarget[1]->Get_World4x4_TP(), sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderTarget[1]->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderTarget[1]->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderTarget[1]->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom3->Get_SRV(6))))
			return E_FAIL;

		m_pShaderTarget[1]->Begin(7);
		m_pVIBufferTarget[1]->Render();
	}


	//상태이상 
	if (dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().eStatInfo != STATINFO_END)
	{
		if (FAILED(m_pShaderTarget[2]->Set_RawValue("g_WorldMatrix", &m_pTransformTarget[2]->Get_World4x4_TP(), sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderTarget[2]->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderTarget[2]->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderTarget[2]->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom4->Get_SRV(dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().eStatInfo))))
			return E_FAIL;
		
		m_pShaderTarget[2]->Begin(7);
		m_pVIBufferTarget[2]->Render();
	}
	if (dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().iHp > 0)
	{
		_float fHp = 0.f;
		_float HpfSizeX = 258.f;
		_float HpfSizeY = 16.f;
		_float HpfX = 1127.f;
		_float HpfY = 79.f;
		_float3 HpvScale = { HpfSizeX,HpfSizeY,0.f };
		_float fShaderHp = (_float)dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().iHp / (_float)dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().iMaxHp;
		fHp = HpfSizeX * fShaderHp;

		HpfX -= (HpfSizeX - fHp) / 2.f;

		HpvScale = { fHp,HpfSizeY,0.f };
		m_pTransformTarget[3]->Set_Scale(XMLoadFloat3(&HpvScale));
		m_pTransformTarget[3]->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(HpfX - g_iWinSizeX * 0.5f, -HpfY + g_iWinSizeY * 0.5f, 0.f, 1.f));

		if (FAILED(m_pShaderTarget[3]->Set_RawValue("g_fHP", &fShaderHp, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderTarget[3]->Set_RawValue("g_WorldMatrix", &m_pTransformTarget[3]->Get_World4x4_TP(), sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderTarget[3]->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderTarget[3]->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderTarget[3]->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom3->Get_SRV(7))))
			return E_FAIL;

		m_pShaderTarget[3]->Begin(1);
		m_pVIBufferTarget[3]->Render();
	}

	if (FAILED(m_pShaderTarget[4]->Set_RawValue("g_WorldMatrix", &m_pTransformTarget[4]->Get_World4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderTarget[4]->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderTarget[4]->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderTarget[4]->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_SRV(9))))
		return E_FAIL;

	m_pShaderTarget[4]->Begin(7);
	m_pVIBufferTarget[4]->Render();

	return S_OK;
}

HRESULT CBattleUI::SetUp_ShaderBall()
{
	
	for (_uint i = 0; i < 6; ++i)
	{
		if (dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(i))->Get_PokeInfo().iPokeNum != 999)
		{
			if (FAILED(m_pShaderBall[i]->Set_RawValue("g_WorldMatrix", &m_pTransformBall[i]->Get_World4x4_TP(), sizeof(_float4x4))))
				return E_FAIL;
			if (FAILED(m_pShaderBall[i]->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
				return E_FAIL;
			if (FAILED(m_pShaderBall[i]->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
				return E_FAIL;
			if (dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(i))->Get_PokeInfo().iHp > 0)
			{
				if (FAILED(m_pShaderBall[i]->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_SRV(10))))
					return E_FAIL;
			}
			else
			{
				if (FAILED(m_pShaderBall[i]->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_SRV(11))))
					return E_FAIL;
			}
			m_pShaderBall[i]->Begin(7);
			m_pVIBufferBall[i]->Render();
		}
	}
	if (m_tInfo.eBattleType == BATTLE_TRAINER)
	{
		for (_uint i = 6; i < 12; ++i)
		{
			if (dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[i - 6])->Get_PokeInfo().iPokeNum != 999)
			{
				if (FAILED(m_pShaderBall[i]->Set_RawValue("g_WorldMatrix", &m_pTransformBall[i]->Get_World4x4_TP(), sizeof(_float4x4))))
					return E_FAIL;
				if (FAILED(m_pShaderBall[i]->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
					return E_FAIL;
				if (FAILED(m_pShaderBall[i]->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
					return E_FAIL;
				if (dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[i - 6])->Get_PokeInfo().iHp > 0)
				{
					if (FAILED(m_pShaderBall[i]->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_SRV(10))))
						return E_FAIL;
				}
				else
				{
					if (FAILED(m_pShaderBall[i]->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_SRV(11))))
						return E_FAIL;
				}
				m_pShaderBall[i]->Begin(7);
				m_pVIBufferBall[i]->Render();
			}
		}
	}
	return S_OK;
}


HRESULT CBattleUI::SetSelectButton(ButtonDir _eDir)
{

	switch (_eDir)
	{
	case CBattleUI::DIR_UP:
		if (!m_bSkill)
		{
			m_fY -= 75.f;
			m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));
		}
		else if (m_bSkill)
		{
			m_fY -= 62.f;
			m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));
		}
		break;
	case CBattleUI::DIR_DOWN:
		if (!m_bSkill)
		{
			m_fY += 75.f;
			m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));
		}
		else if (m_bSkill)
		{
			m_fY += 62.f;
			m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));
		}
		break;
	default:
		break;
	}



	return S_OK;
}

void CBattleUI::Wild_Win(_float fTimeDelta)
{
	m_WinTime += fTimeDelta;

	if (m_WinTime > 0.5f)
	{
		dynamic_cast<CPlayer*>(m_tInfo.pPlayer_Orgin)->Battle_Win();
		m_tInfo.pBattleTarget->Set_Dead();
		g_Battle = false;
	}
}

void CBattleUI::Ready_Run()
{
	for (auto iter = m_vBattleScript.begin(); iter != m_vBattleScript.end();)
		iter = m_vBattleScript.erase(iter);

	m_vBattleScript.clear();

	wstring strTextBegin = TEXT("도망쳤습니다.....");


	m_vBattleScript.push_back(strTextBegin);
}

void CBattleUI::Ready_RunFAILScript()
{
	for (auto iter = m_vBattleScript.begin(); iter != m_vBattleScript.end();)
		iter = m_vBattleScript.erase(iter);

	m_vBattleScript.clear();

	wstring strTextBegin = TEXT("트레이너와 전투 중에는 도망칠 수 없습니다.");


	m_vBattleScript.push_back(strTextBegin);
}

void CBattleUI::Ready_ChangePokeScript()
{
	for (auto iter = m_vBattleScript.begin(); iter != m_vBattleScript.end();)
		iter = m_vBattleScript.erase(iter);

	m_vBattleScript.clear();

	wstring strTextBegin = TEXT("'");
	wstring strTextSkillName = TEXT("'  고생했어 이만 돌아와! ");

	strTextBegin += dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().strName;
	strTextBegin += strTextSkillName;

	m_vBattleScript.push_back(strTextBegin);
}

void CBattleUI::Render_Fonts()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	_vector vPos = { 10.f,605.f,0.f,1.f };
	_vector vScale = { 0.85f,0.85f,0.85f,1.f };
	wstring strNum = TEXT("");

	//플레이어 포켓몬 이름
	pGameInstance->Render_Font(TEXT("Font_Nexon"), dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().strName.c_str(), vPos, XMVectorSet(0.f, 0.f, 0.f, 1.f), vScale);
	//Lv
	if (dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eStatInfo == STATINFO_END)
	{
		strNum = TEXT("Lv.");
		strNum += to_wstring(dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().iLv);
		vPos.m128_f32[0] = 200.f;
		vPos.m128_f32[1] = 605.f;
		pGameInstance->Render_Font(TEXT("Font_Nexon"), strNum.c_str(), vPos, XMVectorSet(0.f, 0.f, 0.f, 1.f), vScale);
	}
	//HP
	strNum = TEXT("");
	strNum += to_wstring(dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().iHp);
	strNum += TEXT("/");
	strNum += to_wstring(dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().iMaxHp);
	vPos.m128_f32[0] = 25.f;
	vPos.m128_f32[1] = 654.f;
	pGameInstance->Render_Font(TEXT("Font_Nexon"), strNum.c_str(), vPos, XMVectorSet(0.f, 0.f, 0.f, 1.f), vScale);
	
	
	vPos.m128_f32[0] = 975.f;
	vPos.m128_f32[1] = 45.f;
	//상대 포켓몬 이름
	pGameInstance->Render_Font(TEXT("Font_Nexon"), dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().strName.c_str(), vPos, XMVectorSet(0.f, 0.f, 0.f, 1.f), vScale);
	//Lv
	if (dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().eStatInfo == STATINFO_END)
	{
		strNum = TEXT("Lv.");
		strNum += to_wstring(dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().iLv);
		vPos.m128_f32[0] = 1180.f;
		vPos.m128_f32[1] = 45.f;
		pGameInstance->Render_Font(TEXT("Font_Nexon"), strNum.c_str(), vPos, XMVectorSet(0.f, 0.f, 0.f, 1.f), vScale);
	}
	//HP
	strNum = TEXT("");
	strNum += to_wstring(dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().iHp);
	strNum += TEXT("/");
	strNum += to_wstring(dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().iMaxHp);
	vPos.m128_f32[0] = 995.f;
	vPos.m128_f32[1] = 92.f;
	pGameInstance->Render_Font(TEXT("Font_Nexon"), strNum.c_str(), vPos, XMVectorSet(0.f, 0.f, 0.f, 1.f), vScale);
	
	if (!m_bPlayerAttack && m_bSkill)
	{//1번스킬
		vScale = { 1.f,1.f,1.f,1.f };
		vPos.m128_f32[0] = 960.f;
		vPos.m128_f32[1] = 485.f;
		pGameInstance->Render_Font(TEXT("Font_Nexon"), dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eSkillNum1->strName.c_str(), vPos, XMVectorSet(0.f, 0.f, 0.f, 1.f), vScale);
		vPos.m128_f32[0] = 1165.f;
		strNum = to_wstring(dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eSkillNum1->iPoint);
		strNum += TEXT("/");
		strNum += to_wstring(dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eSkillNum1->iMaxPoint);
		pGameInstance->Render_Font(TEXT("Font_Nexon"), strNum.c_str(), vPos, XMVectorSet(1.f, 1.f, 1.f, 1.f), vScale);
		//2번스킬
		if (dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eSkillNum2->iSkillNum != 99)
		{
			vPos.m128_f32[0] = 960.f;
			vPos.m128_f32[1] = 547.f;
			pGameInstance->Render_Font(TEXT("Font_Nexon"), dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eSkillNum2->strName.c_str(), vPos, XMVectorSet(0.f, 0.f, 0.f, 1.f), vScale);
			vPos.m128_f32[0] = 1165.f;
			strNum = to_wstring(dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eSkillNum2->iPoint);
			strNum += TEXT("/");
			strNum += to_wstring(dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eSkillNum2->iMaxPoint);
			pGameInstance->Render_Font(TEXT("Font_Nexon"), strNum.c_str(), vPos, XMVectorSet(1.f, 1.f, 1.f, 1.f), vScale);
		}
		//3번스킬
		if (dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eSkillNum3->iSkillNum != 99)
		{
			vPos.m128_f32[0] = 960.f;
			vPos.m128_f32[1] = 609.f;
			pGameInstance->Render_Font(TEXT("Font_Nexon"), dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eSkillNum3->strName.c_str(), vPos, XMVectorSet(0.f, 0.f, 0.f, 1.f), vScale);
			vPos.m128_f32[0] = 1165.f;
			strNum = to_wstring(dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eSkillNum3->iPoint);
			strNum += TEXT("/");
			strNum += to_wstring(dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eSkillNum3->iMaxPoint);
			pGameInstance->Render_Font(TEXT("Font_Nexon"), strNum.c_str(), vPos, XMVectorSet(1.f, 1.f, 1.f, 1.f), vScale);
		}
		//4번스킬
		if (dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eSkillNum4->iSkillNum != 99)
		{
			vPos.m128_f32[0] = 960.f;
			vPos.m128_f32[1] = 671.f;
			pGameInstance->Render_Font(TEXT("Font_Nexon"), dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eSkillNum4->strName.c_str(), vPos, XMVectorSet(0.f, 0.f, 0.f, 1.f), vScale);
			vPos.m128_f32[0] = 1165.f;
			strNum = to_wstring(dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eSkillNum4->iPoint);
			strNum += TEXT("/");
			strNum += to_wstring(dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eSkillNum4->iMaxPoint);
			pGameInstance->Render_Font(TEXT("Font_Nexon"), strNum.c_str(), vPos, XMVectorSet(1.f, 1.f, 1.f, 1.f), vScale);
		}
	}
	RELEASE_INSTANCE(CGameInstance);
}

void CBattleUI::Change_Poke(_float fTimeDelta)
{
	if (m_bChangePokeStart)
	{
		m_fDelayTime += fTimeDelta;
		m_fHPTime += fTimeDelta;
	}
	//포켓몬 집어넣기
	if (!m_bChangePokeStart)
	{
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
		CTextBox::TINFO tTInfo;
		Ready_ChangePokeScript();
		tTInfo.iScriptSize = (_int)m_vBattleScript.size();
		tTInfo.pTarget = dynamic_cast<CGameObj*>(m_tInfo.pPlayer_Orgin);
		tTInfo.pScript = new wstring[m_vBattleScript.size()];
		tTInfo.iType = 3;
		for (_int i = 0; i < m_vBattleScript.size(); ++i)
			tTInfo.pScript[i] = m_vBattleScript[i];

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_TextBox"), LEVEL_GAMEPLAY, TEXT("Layer_UI"), &tTInfo)))
			return;
		RELEASE_INSTANCE(CGameInstance);

		m_fTextSizeTime = (_float)m_vBattleScript.size() * 2.f;

		dynamic_cast<CGameObj*>(m_tInfo.pPlayer_Orgin)->Set_AnimIndex(8);
		m_bChangePokeStart = true;
		m_fDelayTime = 0.f;
	}
	//포켓몬 집어넣기 확인
	if (!m_bChangeAttack && m_bChangePokeStart && dynamic_cast<CPlayer*>(m_tInfo.pPlayer_Orgin)->Get_Return())
	{
		dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_Transfrom()->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(-50000.f, -50000.f, -50000.f, 1.f));
		dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Reset_Anim();
	}
	//교체 포켓몬 내보내기
	if (!m_bChangeAttack && m_bChangePokeStart && m_fDelayTime > m_fTextSizeTime)
	{
		m_iPlayerIndex = m_tInfo.pPlayer->Get_iChangePoke();
		m_iPlayerHPIndex = m_tInfo.pPlayer->Get_iChangePoke();
		Ready_PlayerChange_Poke();
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
		CTextBox::TINFO tTInfo;

		tTInfo.iScriptSize = (_int)m_vBattleScript.size();
		tTInfo.pTarget = dynamic_cast<CGameObj*>(m_tInfo.pPlayer_Orgin);
		tTInfo.pScript = new wstring[m_vBattleScript.size()];
		tTInfo.iType = 3;
		for (_int i = 0; i < m_vBattleScript.size(); ++i)
			tTInfo.pScript[i] = m_vBattleScript[i];

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_TextBox"), LEVEL_GAMEPLAY, TEXT("Layer_UI"), &tTInfo)))
			return;
		RELEASE_INSTANCE(CGameInstance);
		m_bChangeAttack = true;
		m_fTextSizeTime = (_float)m_vBattleScript.size() * 5.f;
		m_fDelayTime = 0.f;
		dynamic_cast<CGameObj*>(m_tInfo.pPlayer_Orgin)->Set_AnimIndex(2);

	}
	//상대포켓몬 공격준비
	if (!m_bCreateTextBox && m_bChangeAttack && m_fDelayTime > m_fTextSizeTime)
	{
		_int iTargetSkill = rand() % Check_TargetSkill();

		Use_TargetSkill(iTargetSkill);

		Ready_TargetScript();
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
		CTextBox::TINFO tTInfo;

		tTInfo.iScriptSize = (_int)m_vBattleScript.size();
		tTInfo.pTarget = dynamic_cast<CGameObj*>(m_tInfo.pBattleTarget);
		tTInfo.pScript = new wstring[m_vBattleScript.size()];
		tTInfo.iType = 3;
		for (_int i = 0; i < m_vBattleScript.size(); ++i)
			tTInfo.pScript[i] = m_vBattleScript[i];

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_TextBox"), LEVEL_GAMEPLAY, TEXT("Layer_UI"), &tTInfo)))
			return;
		RELEASE_INSTANCE(CGameInstance);
		m_bCreateTextBox = true;
		m_fTextSizeTime = (_float)m_vBattleScript.size() * 1.5f;
		m_fDelayTime = 0.f;
		_int iAnim = 0;

		switch (m_bTargetSkillIndex)
		{
		case 0:
			if (dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().eSkillNum1->iDmg == 0)
				iAnim = 4;
			else
				iAnim = 3;
			break;
		case 1:
			if (dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().eSkillNum2->iDmg == 0)
				iAnim = 4;
			else
				iAnim = 3;
			break;
		case 2:
			if (dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().eSkillNum3->iDmg == 0)
				iAnim = 4;
			else
				iAnim = 3;
			break;
		case 3:
			if (dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().eSkillNum4->iDmg == 0)
				iAnim = 4;
			else
				iAnim = 3;
			break;
		default:
			break;
		}

		dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Set_AnimIndex(iAnim);
		m_bCheckAttack = false;
		m_fDotDeal = 0.f;
		m_fHPTime = 0.f;
	}
	//상대포켓몬 공격
	if (m_bCreateTextBox && m_bTargetHit && m_fDelayTime > m_fTextSizeTime && m_fHPTime > 0.05f)
	{
		if (m_fDotDeal < m_iTargetFinalDmg)
		{
			dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Set_PokeHp(-1);
			if (!m_bPokeDead && dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().iHp <= 0)
			{
				dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Set_StatInfo(STUN);
				dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Set_AnimIndex(6);

				m_bPokeDead = true;
				m_fDotDeal = m_iTargetFinalDmg - 1;
				m_fDelayTime = 0.f;
			}
			++m_fDotDeal;
		}
		if (m_fDotDeal == m_iTargetFinalDmg)
		{
			if (!m_bPokeDead && m_fDelayTime > m_fTextSizeTime)
			{
				m_bPokeDead = false;

				m_bCheckAttack = false;
				m_bChangePokeStart = false;
				m_bChangeAttack = false;
				m_bBattleChangePoke = false;
				m_bCreateTextBox = false;
				m_fTextSizeTime = 0.f;

				m_fDotDeal = 0;
				m_fHPTime = 0.f;
				m_fDelayTime = 0.f;
				Set_CheckPos(CHECKPOS_INFO);
				m_bSkill = false;
			}
		}
		m_fHPTime = 0.f;
	}
	//포켓몬 히트모션
	if (!m_bCheckAttack && m_fDelayTime > m_fTextSizeTime)
	{
		if (m_bTargetHit)
			dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Set_AnimIndex(5);

		m_bCheckAttack = true;
	}
	//플레이어 포켓몬 죽어서 교체
	if (m_bBattleBagPoke && !m_tInfo.pPlayer->Get_BattlePokeDead())
	{
		m_iPlayerIndex = m_tInfo.pPlayer->Get_iChangePoke();
		m_iPlayerHPIndex = m_tInfo.pPlayer->Get_iChangePoke();
		Ready_PlayerChange_Poke();
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
		CTextBox::TINFO tTInfo;

		tTInfo.iScriptSize = (_int)m_vBattleScript.size();
		tTInfo.pTarget = dynamic_cast<CGameObj*>(m_tInfo.pPlayer_Orgin);
		tTInfo.pScript = new wstring[m_vBattleScript.size()];
		tTInfo.iType = 3;
		for (_int i = 0; i < m_vBattleScript.size(); ++i)
			tTInfo.pScript[i] = m_vBattleScript[i];

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_TextBox"), LEVEL_GAMEPLAY, TEXT("Layer_UI"), &tTInfo)))
			return;
		RELEASE_INSTANCE(CGameInstance);
		m_bBattleBagPoke = false;
		m_fTextSizeTime = (_float)m_vBattleScript.size() * 2.f;
		m_fDelayTime = 0.f;
		dynamic_cast<CGameObj*>(m_tInfo.pPlayer_Orgin)->Set_AnimIndex(2);
	}
	//플레이어 포켓몬 죽었는지 확인
	if (m_bPokeDead && m_fDelayTime > m_fTextSizeTime)
	{
		if (dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eStatInfo == STUN)
			Change_PlayerPoke(fTimeDelta);
	}
	//플레이어 포켓몬 죽어서 교체 후 모션 딜레이 확인후 턴종료
	if (!m_bBattleBagPoke && m_bPokeDead && m_fDelayTime > m_fTextSizeTime)
	{
		m_bPokeDead = false;

		m_bCheckAttack = false;
		m_bChangePokeStart = false;
		m_bChangeAttack = false;
		m_bBattleChangePoke = false;
		m_bCreateTextBox = false;
		m_fTextSizeTime = 0.f;

		m_fDotDeal = 0;
		m_fHPTime = 0.f;
		m_fDelayTime = 0.f;
		Set_CheckPos(CHECKPOS_INFO);
		m_bSkill = false;
	}
}

void CBattleUI::Use_Item(_float fTimeDelta)
{
	m_fDelayTime += fTimeDelta;
	if(m_bCreateTextBox)
		m_fHPTime += fTimeDelta;

	//상대포켓몬 공격준비
	if (!m_bCreateTextBox && m_fDelayTime > 1.5f)
	{
		_int iTargetSkill = rand() % Check_TargetSkill();

		Use_TargetSkill(iTargetSkill);

		Ready_TargetScript();
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
		CTextBox::TINFO tTInfo;

		tTInfo.iScriptSize = (_int)m_vBattleScript.size();
		tTInfo.pTarget = dynamic_cast<CGameObj*>(m_tInfo.pBattleTarget);
		tTInfo.pScript = new wstring[m_vBattleScript.size()];
		tTInfo.iType = 3;
		for (_int i = 0; i < m_vBattleScript.size(); ++i)
			tTInfo.pScript[i] = m_vBattleScript[i];

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_TextBox"), LEVEL_GAMEPLAY, TEXT("Layer_UI"), &tTInfo)))
			return;
		RELEASE_INSTANCE(CGameInstance);
		m_bCreateTextBox = true;
		m_fTextSizeTime = (_float)m_vBattleScript.size() * 1.5f;
		m_fDelayTime = 0.f;
		_int iAnim = 0;

		switch (m_bTargetSkillIndex)
		{
		case 0:
			if (dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().eSkillNum1->iDmg == 0)
				iAnim = 4;
			else
				iAnim = 3;
			break;
		case 1:
			if (dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().eSkillNum2->iDmg == 0)
				iAnim = 4;
			else
				iAnim = 3;
			break;
		case 2:
			if (dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().eSkillNum3->iDmg == 0)
				iAnim = 4;
			else
				iAnim = 3;
			break;
		case 3:
			if (dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().eSkillNum4->iDmg == 0)
				iAnim = 4;
			else
				iAnim = 3;
			break;
		default:
			break;
		}

		dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Set_AnimIndex(iAnim);
		m_bCheckAttack = false;
		m_fDotDeal = 0;
		m_fHPTime = 0.f;
	}
	//상대포켓몬 공격
	if (m_bCreateTextBox && m_bTargetHit && m_fDelayTime > m_fTextSizeTime && m_fHPTime > 0.05f)
	{
		if (m_fDotDeal < m_iTargetFinalDmg)
		{
			dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Set_PokeHp(-1);
			if (!m_bPokeDead && dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().iHp <= 0)
			{
				dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Set_StatInfo(STUN);
				dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Set_AnimIndex(6);

				m_bPokeDead = true;
				m_fDotDeal = m_iTargetFinalDmg - 1;
				m_fDelayTime = 0.f;
			}
			++m_fDotDeal;
		}
		if (m_fDotDeal == m_iTargetFinalDmg)
		{
			if (!m_bPokeDead && m_fDelayTime > m_fTextSizeTime)
			{
				m_bPokeDead = false;

				m_bCheckAttack = false;
				m_bChangePokeStart = false;
				m_bChangeAttack = false;
				m_bBattleUseItem = false;
				m_bCreateTextBox = false;
				m_fTextSizeTime = 0.f;

				m_fDotDeal = 0;
				m_fHPTime = 0.f;
				m_fDelayTime = 0.f;
				Set_CheckPos(CHECKPOS_INFO);
				m_bSkill = false;
			}
		}
		m_fHPTime = 0.f;
	}
	//포켓몬 히트모션
	if (m_bCreateTextBox && !m_bCheckAttack && m_fDelayTime > m_fTextSizeTime)
	{
		if (m_bTargetHit)
			dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Set_AnimIndex(5);

		m_bCheckAttack = true;
	}
	//플레이어 포켓몬 죽어서 교체
	if (m_bBattleBagPoke && !m_tInfo.pPlayer->Get_BattlePokeDead())
	{
		m_iPlayerIndex = m_tInfo.pPlayer->Get_iChangePoke();
		m_iPlayerHPIndex = m_tInfo.pPlayer->Get_iChangePoke();
		Ready_PlayerChange_Poke();
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
		CTextBox::TINFO tTInfo;

		tTInfo.iScriptSize = (_int)m_vBattleScript.size();
		tTInfo.pTarget = dynamic_cast<CGameObj*>(m_tInfo.pPlayer_Orgin);
		tTInfo.pScript = new wstring[m_vBattleScript.size()];
		tTInfo.iType = 3;
		for (_int i = 0; i < m_vBattleScript.size(); ++i)
			tTInfo.pScript[i] = m_vBattleScript[i];

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_TextBox"), LEVEL_GAMEPLAY, TEXT("Layer_UI"), &tTInfo)))
			return;
		RELEASE_INSTANCE(CGameInstance);
		m_bBattleBagPoke = false;
		m_fTextSizeTime = (_float)m_vBattleScript.size() * 2.f;
		m_fDelayTime = 0.f;
		dynamic_cast<CGameObj*>(m_tInfo.pPlayer_Orgin)->Set_AnimIndex(2);
	}
	//플레이어 포켓몬 죽었는지 확인
	if (m_bPokeDead && m_fDelayTime > m_fTextSizeTime)
	{
		if (dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eStatInfo == STUN)
			Change_PlayerPoke(fTimeDelta);
	}
	//플레이어 포켓몬 죽어서 교체 후 모션 딜레이 확인후 턴종료
	if (!m_bBattleBagPoke && m_bPokeDead && m_fDelayTime > m_fTextSizeTime)
	{
		m_bPokeDead = false;

		m_bCheckAttack = false;
		m_bChangePokeStart = false;
		m_bChangeAttack = false;
		m_bBattleUseItem = false;
		m_bCreateTextBox = false;
		m_fTextSizeTime = 0.f;

		m_fDotDeal = 0;
		m_fHPTime = 0.f;
		m_fDelayTime = 0.f;
		Set_CheckPos(CHECKPOS_INFO);
		m_bSkill = false;
	}
}

void CBattleUI::Set_Pos()
{
	_float InfoSizeX = 233.f;
	_float InfofSizeY = 67.f;
	_float InfofX = 1140.f;
	_float InfofY = 450.f;
	_float3 InfoScale = { InfoSizeX,InfofSizeY,0.f };

	_float SkillSizeX = 373.f;
	_float SkillfSizeY = 54.f;
	_float SkillfX = 1080.f;
	_float SkillfY = 500.f;
	_float3 SkillScale = { SkillSizeX,SkillfSizeY,0.f };

	_float TypeSmallSizeX = 54.f;
	_float TypeSmallfSizeY = 54.f;
	_float TypeSmallfX = 920.f;
	_float TypeSmallfY = 500.f;
	_float3 TypeSmallScale = { TypeSmallSizeX,TypeSmallfSizeY,0.f };

	for (_int i = 0; i < 4; ++i)
	{
		m_pTransformInfo[i]->Set_Scale(XMLoadFloat3(&InfoScale));
		m_pTransformInfo[i]->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(InfofX - g_iWinSizeX * 0.5f, -InfofY + g_iWinSizeY * 0.5f, 0.f, 1.f));
		InfofY += 75.f;
	}
	for (_int i = 0; i < 4; ++i)
	{
		m_pTransformSkill[i]->Set_Scale(XMLoadFloat3(&SkillScale));
		m_pTransformSkill[i]->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(SkillfX - g_iWinSizeX * 0.5f, -SkillfY + g_iWinSizeY * 0.5f, 0.f, 1.f));
		SkillfY += 62.f;
	}
	for (_int i = 4; i < 8; ++i)
	{
		m_pTransformSkill[i]->Set_Scale(XMLoadFloat3(&TypeSmallScale));
		m_pTransformSkill[i]->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(TypeSmallfX - g_iWinSizeX * 0.5f, -TypeSmallfY + g_iWinSizeY * 0.5f, 0.f, 1.f));
		TypeSmallfY += 62.f;
	}
	
}

void CBattleUI::Set_PlayerPos()
{
	_float fSizeX = 332.f;
	_float fSizeY = 79.f;
	_float fX = 166.f;
	_float fY = 640.f;
	_float3 vScale = { fSizeX,fSizeY,0.f };

	//프레임
	m_pTransformPlayer[0]->Set_Scale(XMLoadFloat3(&vScale));
	m_pTransformPlayer[0]->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(fX - g_iWinSizeX * 0.5f, -fY + g_iWinSizeY * 0.5f, 0.f, 1.f));
	
	fSizeX = 20.f;
	fSizeY = 20.f;
	fX = 160.f;
	fY = 616.f;
	vScale = { fSizeX,fSizeY,0.f };
	//성별
	m_pTransformPlayer[1]->Set_Scale(XMLoadFloat3(&vScale));
	m_pTransformPlayer[1]->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(fX - g_iWinSizeX * 0.5f, -fY + g_iWinSizeY * 0.5f, 0.f, 1.f));

	fSizeX = 100.f;
	fSizeY = 25.f;
	fX = 250.f;
	fY = 616.f;
	vScale = { fSizeX,fSizeY,0.f };
	//상태이상
	m_pTransformPlayer[2]->Set_Scale(XMLoadFloat3(&vScale));
	m_pTransformPlayer[2]->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(fX - g_iWinSizeX * 0.5f, -fY + g_iWinSizeY * 0.5f, 0.f, 1.f));

	fSizeX = 273.f;
	fSizeY = 30.f;
	fX = 154.f;
	fY = 639.f;
	vScale = { fSizeX,fSizeY,0.f };
	//HP
	m_pTransformPlayer[3]->Set_Scale(XMLoadFloat3(&vScale));
	m_pTransformPlayer[3]->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(fX - g_iWinSizeX * 0.5f, -fY + g_iWinSizeY * 0.5f, 0.f, 1.f));

	fSizeX = 120.f;
	fSizeY = 10.f;
	fX = 224.f;
	fY = 655.f;
	vScale = { fSizeX,fSizeY,0.f };
	//EXP
	m_pTransformPlayer[4]->Set_Scale(XMLoadFloat3(&vScale));
	m_pTransformPlayer[4]->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(fX - g_iWinSizeX * 0.5f, -fY + g_iWinSizeY * 0.5f, 0.f, 1.f));

	fSizeX = 158.f;
	fSizeY = 24.f;
	fX = 79.f;
	fY = 700.f;
	vScale = { fSizeX,fSizeY,0.f };
	//볼프레임
	m_pTransformPlayer[5]->Set_Scale(XMLoadFloat3(&vScale));
	m_pTransformPlayer[5]->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(fX - g_iWinSizeX * 0.5f, -fY + g_iWinSizeY * 0.5f, 0.f, 1.f));

	
}

void CBattleUI::Set_TargetPos()
{
	_float fSizeX = 332.f;
	_float fSizeY = 79.f;
	_float fX = 1114.f;
	_float fY = 80.f;
	_float3 vScale = { fSizeX,fSizeY,0.f };

	//프레임
	m_pTransformTarget[0]->Set_Scale(XMLoadFloat3(&vScale));
	m_pTransformTarget[0]->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(fX - g_iWinSizeX * 0.5f, -fY + g_iWinSizeY * 0.5f, 0.f, 1.f));

	fSizeX = 20.f;
	fSizeY = 20.f;
	fX = 1130.f;
	fY = 56.f;
	vScale = { fSizeX,fSizeY,0.f };
	//성별
	m_pTransformTarget[1]->Set_Scale(XMLoadFloat3(&vScale));
	m_pTransformTarget[1]->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(fX - g_iWinSizeX * 0.5f, -fY + g_iWinSizeY * 0.5f, 0.f, 1.f));

	fSizeX = 100.f;
	fSizeY = 25.f;
	fX = 1220.f;
	fY = 56.f;
	vScale = { fSizeX,fSizeY,0.f };
	//상태이상
	m_pTransformTarget[2]->Set_Scale(XMLoadFloat3(&vScale));
	m_pTransformTarget[2]->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(fX - g_iWinSizeX * 0.5f, -fY + g_iWinSizeY * 0.5f, 0.f, 1.f));

	fSizeX = 273.f;
	fSizeY = 30.f;
	fX = 1127.f;
	fY = 79.f;
	vScale = { fSizeX,fSizeY,0.f };
	//HP
	m_pTransformTarget[3]->Set_Scale(XMLoadFloat3(&vScale));
	m_pTransformTarget[3]->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(fX - g_iWinSizeX * 0.5f, -fY + g_iWinSizeY * 0.5f, 0.f, 1.f));

	fSizeX = 158.f;
	fSizeY = 24.f;
	fX = 1201.f;
	fY = 20.f;
	vScale = { fSizeX,fSizeY,0.f };
	//볼프레임
	m_pTransformTarget[4]->Set_Scale(XMLoadFloat3(&vScale));
	m_pTransformTarget[4]->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(fX - g_iWinSizeX * 0.5f, -fY + g_iWinSizeY * 0.5f, 0.f, 1.f));

}

void CBattleUI::Set_BallPos()
{
	_float 	fSizeX = 15.f;
	_float 	fSizeY = 15.f;
	_float 	fX = 14.f;
	_float 	fY = 700.f;
	_float3	vScale = { fSizeX,fSizeY,0.f };
	//플레이어 볼
	for (_uint i = 0; i < 6; ++i)
	{
		m_pTransformBall[i]->Set_Scale(XMLoadFloat3(&vScale));
		m_pTransformBall[i]->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(fX - g_iWinSizeX * 0.5f, -fY + g_iWinSizeY * 0.5f, 0.f, 1.f));
		fX += 24.f;
	}

	fSizeX = 15.f;
	fSizeY = 15.f;
	fX = 1146.f;
	fY = 20.f;
	vScale = { fSizeX,fSizeY,0.f };
	//상대 볼
	for (_uint i = 6; i < 12; ++i)
	{
		m_pTransformBall[i]->Set_Scale(XMLoadFloat3(&vScale));
		m_pTransformBall[i]->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(fX - g_iWinSizeX * 0.5f, -fY + g_iWinSizeY * 0.5f, 0.f, 1.f));
		fX += 24.f;
	}


}

void CBattleUI::Key_Input()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	
	if (pGameInstance->Key_Down(DIK_UP))
	{
		if (m_iSelect != 0)
		{
			SetSelectButton(DIR_UP);
			--m_iSelect;
		}
	
	}
	if (pGameInstance->Key_Down(DIK_DOWN))
	{
		if (m_iSelect != 3)
		{
			if (m_bSkill)
			{
				if (dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eSkillNum2->iSkillNum != 99 && m_iSelect == 0)
				{
					SetSelectButton(DIR_DOWN);
					++m_iSelect;
				}
				else if (dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eSkillNum3->iSkillNum != 99 && m_iSelect == 1)
				{
					SetSelectButton(DIR_DOWN);
					++m_iSelect;
				}
				else if (dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eSkillNum4->iSkillNum != 99 && m_iSelect == 2)
				{
					SetSelectButton(DIR_DOWN);
					++m_iSelect;
				}
			}
			else if (!m_bSkill)
			{
				SetSelectButton(DIR_DOWN);
				++m_iSelect;
			}
		}
	}
	if (pGameInstance->Key_Down(DIK_RETURN))
	{
		Check_Selected();
	}
	if (m_bSkill && pGameInstance->Key_Down(DIK_BACKSPACE))
	{
		Set_CheckPos(CHECKPOS_INFO);
		m_bSkill = false;
	}
	RELEASE_INSTANCE(CGameInstance);
}

void CBattleUI::BattleFrame()
{
	if (!m_bCreateTextBox && (dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().iSpeed >= dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().iSpeed))
	{
		_int iTargetSkill = rand() % Check_TargetSkill();

		Use_TargetSkill(iTargetSkill);

		Ready_PlayerScript();
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
		CTextBox::TINFO tTInfo;

		tTInfo.iScriptSize = (_int)m_vBattleScript.size();
		tTInfo.pTarget = m_tInfo.pPlayer_Orgin;
		tTInfo.pScript = new wstring[m_vBattleScript.size()];
		tTInfo.iType = 3;
		for (_int i = 0; i < m_vBattleScript.size(); ++i)
			tTInfo.pScript[i] = m_vBattleScript[i];

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_TextBox"), LEVEL_GAMEPLAY, TEXT("Layer_UI"), &tTInfo)))
			return;
		RELEASE_INSTANCE(CGameInstance);
		m_bCreateTextBox = true;
		m_fTextSizeTime = m_vBattleScript.size() * 2.f;

		_int iAnim = 0;

		switch (m_bPlayerSkillIndex)
		{
		case 0:
			if (dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eSkillNum1->iDmg == 0)
				iAnim = 4;
			else
				iAnim = 3;
			break;
		case 1:
			if (dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eSkillNum2->iDmg == 0)
				iAnim = 4;
			else
				iAnim = 3;
			break;
		case 2:
			if (dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eSkillNum3->iDmg == 0)
				iAnim = 4;
			else
				iAnim = 3;
			break;
		case 3:
			if (dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eSkillNum4->iDmg == 0)
				iAnim = 4;
			else
				iAnim = 3;
			break;
		default:
			break;
		}

		dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Set_AnimIndex(iAnim);
		m_bCheckAttack = false;
		m_fDotDeal = 0;
		m_fHPTime = 0.f;
	}
	else if (!m_bCreateTextBox && (dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().iSpeed < dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().iSpeed))
	{
		_int iTargetSkill = rand() % Check_TargetSkill();

		Use_TargetSkill(iTargetSkill);

		Ready_TargetScript();
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
		CTextBox::TINFO tTInfo;

		tTInfo.iScriptSize = (_int)m_vBattleScript.size();
		tTInfo.pTarget = dynamic_cast<CGameObj*>(m_tInfo.pBattleTarget);
		tTInfo.pScript = new wstring[m_vBattleScript.size()];
		tTInfo.iType = 3;
		for (_int i = 0; i < m_vBattleScript.size(); ++i)
			tTInfo.pScript[i] = m_vBattleScript[i];

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_TextBox"), LEVEL_GAMEPLAY, TEXT("Layer_UI"), &tTInfo)))
			return;
		RELEASE_INSTANCE(CGameInstance);
		m_bCreateTextBox = true;
		m_fTextSizeTime = m_vBattleScript.size() * 2.f;

		_int iAnim = 0;

		switch (m_bTargetSkillIndex)
		{
		case 0:
			if (dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().eSkillNum1->iDmg == 0)
				iAnim = 4;
			else
				iAnim = 3;
			break;
		case 1:
			if (dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().eSkillNum2->iDmg == 0)
				iAnim = 4;
			else
				iAnim = 3;
			break;
		case 2:
			if (dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().eSkillNum3->iDmg == 0)
				iAnim = 4;
			else
				iAnim = 3;
			break;
		case 3:
			if (dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().eSkillNum4->iDmg == 0)
				iAnim = 4;
			else
				iAnim = 3;
			break;
		default:
			break;
		}

		dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Set_AnimIndex(iAnim);
		m_bCheckAttack = false;
		m_fDotDeal = 0;
		m_fHPTime = 0.f;
	}

	if (!m_bCreateTextBox2 && m_bCheckAttack2)
	{
		if (!m_bCreateTextBox2 && (dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().iSpeed < dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().iSpeed))
		{
			_int iTargetSkill = rand() % Check_TargetSkill();

			Use_TargetSkill(iTargetSkill);

			Ready_PlayerScript();
			CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
			CTextBox::TINFO tTInfo;

			tTInfo.iScriptSize = (_int)m_vBattleScript.size();
			tTInfo.pTarget = m_tInfo.pPlayer_Orgin;
			tTInfo.pScript = new wstring[m_vBattleScript.size()];
			tTInfo.iType = 3;
			for (_int i = 0; i < m_vBattleScript.size(); ++i)
				tTInfo.pScript[i] = m_vBattleScript[i];

			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_TextBox"), LEVEL_GAMEPLAY, TEXT("Layer_UI"), &tTInfo)))
				return;
			RELEASE_INSTANCE(CGameInstance);
			m_bCreateTextBox2 = true;
			m_fTextSizeTime = m_vBattleScript.size() * 2.f;

			_int iAnim = 0;

			switch (m_bPlayerSkillIndex)
			{
			case 0:
				if (dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eSkillNum1->iDmg == 0)
					iAnim = 4;
				else
					iAnim = 3;
				break;
			case 1:
				if (dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eSkillNum2->iDmg == 0)
					iAnim = 4;
				else
					iAnim = 3;
				break;
			case 2:
				if (dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eSkillNum3->iDmg == 0)
					iAnim = 4;
				else
					iAnim = 3;
				break;
			case 3:
				if (dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eSkillNum4->iDmg == 0)
					iAnim = 4;
				else
					iAnim = 3;
				break;
			default:
				break;
			}

			dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Set_AnimIndex(iAnim);
			m_fDotDeal = 0;
			m_fHPTime = 0.f;
		}
		else if (!m_bCreateTextBox2 && (dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().iSpeed >= dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().iSpeed))
		{
			_int iTargetSkill = rand() % Check_TargetSkill();

			Use_TargetSkill(iTargetSkill);

			Ready_TargetScript();
			CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
			CTextBox::TINFO tTInfo;

			tTInfo.iScriptSize = (_int)m_vBattleScript.size();
			tTInfo.pTarget = dynamic_cast<CGameObj*>(m_tInfo.pBattleTarget);
			tTInfo.pScript = new wstring[m_vBattleScript.size()];
			tTInfo.iType = 3;
			for (_int i = 0; i < m_vBattleScript.size(); ++i)
				tTInfo.pScript[i] = m_vBattleScript[i];

			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_TextBox"), LEVEL_GAMEPLAY, TEXT("Layer_UI"), &tTInfo)))
				return;
			RELEASE_INSTANCE(CGameInstance);
			m_bCreateTextBox2 = true;
			m_fTextSizeTime = m_vBattleScript.size() * 2.f;

			_int iAnim = 0;

			switch (m_bTargetSkillIndex)
			{
			case 0:
				if (dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().eSkillNum1->iDmg == 0)
					iAnim = 4;
				else
					iAnim = 3;
				break;
			case 1:
				if (dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().eSkillNum2->iDmg == 0)
					iAnim = 4;
				else
					iAnim = 3;
				break;
			case 2:
				if (dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().eSkillNum3->iDmg == 0)
					iAnim = 4;
				else
					iAnim = 3;
				break;
			case 3:
				if (dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().eSkillNum4->iDmg == 0)
					iAnim = 4;
				else
					iAnim = 3;
				break;
			default:
				break;
			}

			dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Set_AnimIndex(iAnim);
			m_fDotDeal = 0;
			m_fHPTime = 0.f;
		}
	}

}

void CBattleUI::BattleDelay(_float fTimeDelta)
{
	//선공
	if (!m_bBattleBagPoke && !m_bCreateTextBox2 && m_bCheckAttack && m_fHPTime > 0.05f)
	{
		if (dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().iSpeed >= dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().iSpeed)
		{
			if (m_bPlayerHit)
			{
				if (m_fDotDeal < m_iPlayerFinalDmg)
				{
					dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Set_PokeHp(-1);
					if (!m_bPokeDead && dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().iHp <= 0)
					{
						dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Set_StatInfo(STUN);
						dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Set_AnimIndex(6);
	
						m_bPokeDead = true;
						m_fDotDeal = m_iPlayerFinalDmg - 1;
						m_fDelayTime = 0.f;
						if (dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().iLv < 100)
						{
							m_fEXPTime = 0.f;
							m_bGet_EXP = true;
							m_iDotEXP = 0;
							m_iTotalEXP = dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().iLv;
						}
					}
					++m_fDotDeal;
				}
				if (m_fDotDeal == m_iPlayerFinalDmg)
				{
					if (!m_bPokeDead && m_fDelayTime > m_fTextSizeTime)
					{
						m_bCheckAttack2 = true;
						m_fDelayTime = 0.f;
					}
				}
			}
		}
		else if (dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().iSpeed < dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().iSpeed)
		{
			if (m_bTargetHit)
			{
				if (m_fDotDeal < m_iTargetFinalDmg)
				{
					dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Set_PokeHp(-1);
					if (!m_bPokeDead && dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().iHp <= 0)
					{
						dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Set_StatInfo(STUN);
						dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Set_AnimIndex(6);
					
						m_bPokeDead = true;
						m_fDotDeal = m_iTargetFinalDmg - 1;
						m_fDelayTime = 0.f;
					}
					++m_fDotDeal;
				}
				if (m_fDotDeal == m_iTargetFinalDmg)
				{
					if (!m_bPokeDead && m_fDelayTime > m_fTextSizeTime)
					{
						m_bCheckAttack2 = true;
						m_fDelayTime = 0.f;
					}
				}
			}
		}
		m_fHPTime = 0.f;
	}
	//선공 히트모션
	if (!m_bCheckAttack && m_fHPTime > 1.f)
	{
		if (dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().iSpeed >= dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().iSpeed)
		{
			if (m_bPlayerHit)
				dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Set_AnimIndex(5);
		}
		else if (dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().iSpeed < dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().iSpeed)
		{
			if (m_bTargetHit)
				dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Set_AnimIndex(5);
		}
		m_bCheckAttack = true;
	}
	//후공
	if (!m_bBattleBagPoke &&m_bCheckAttack3 && m_fHPTime > 0.05f)
	{
		if (dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().iSpeed < dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().iSpeed)
		{
			if (m_bPlayerHit)
			{
				if (m_fDotDeal < m_iPlayerFinalDmg)
				{
					dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Set_PokeHp(-1);
					if (!m_bPokeDead && dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().iHp <= 0)
					{
						dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Set_StatInfo(STUN);
						dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Set_AnimIndex(6);
	
						m_bPokeDead = true;
						m_fDotDeal = m_iPlayerFinalDmg - 1;
						m_fDelayTime = 0.f;
						if (dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().iLv < 100)
						{
							m_fEXPTime = 0.f;
							m_bGet_EXP = true;
							m_iDotEXP = 0;
							m_iTotalEXP = dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().iLv;
						}
					}
					++m_fDotDeal;
				}
				if (m_fDotDeal == m_iPlayerFinalDmg)
				{
					if (!m_bPokeDead && m_fDelayTime > 0.8f)
					{
						m_bPlayerAttack = false;
						m_bCheckAttack = false;
						m_bCheckAttack2 = false;
						m_bCheckAttack3 = false;

						m_bCreateTextBox = false;
						m_bCreateTextBox2 = false;

						m_fDotDeal = 0;
						m_fHPTime = 0.f;
						m_fDelayTime = 0.f;
						Set_CheckPos(CHECKPOS_INFO);
						m_bSkill = false;
					}
				}
			}
		}
		else if (dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().iSpeed >= dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().iSpeed)
		{
			if (m_bTargetHit)
			{
				if (m_fDotDeal < m_iTargetFinalDmg)
				{
					dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Set_PokeHp(-1);
					if (!m_bPokeDead && dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().iHp <= 0)
					{
						dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Set_StatInfo(STUN);
						dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Set_AnimIndex(6);
					
						m_bPokeDead = true;
						m_fDotDeal = m_iTargetFinalDmg - 1;
						m_fDelayTime = 0.f;
					}
					++m_fDotDeal;
				}
				if (m_fDotDeal == m_iTargetFinalDmg)
				{
					if (!m_bPokeDead && m_fDelayTime > 0.8f)
					{
						m_bPokeDead = false;
						m_bPlayerAttack = false;
						m_bCheckAttack = false;
						m_bCheckAttack2 = false;
						m_bCheckAttack3 = false;

						m_bCreateTextBox = false;
						m_bCreateTextBox2 = false;

						m_fDotDeal = 0;
						m_fHPTime = 0.f;
						m_fDelayTime = 0.f;
						Set_CheckPos(CHECKPOS_INFO);
						m_bSkill = false;
					}
				}
			}
		}
		m_fHPTime = 0.f;
	}
	//경치획득
	if (m_bGet_EXP)
	{
		m_fEXPTime += fTimeDelta;
		if (m_fEXPTime > 0.05f)
		{
			if (m_iDotEXP < m_iTotalEXP)
				dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Set_PokeEXP(1);
			
			if (m_iDotEXP == m_iTotalEXP)
				m_bGet_EXP = false;
			
			++m_iDotEXP;
			m_fEXPTime = 0.f;
		}
	}
	//후공 히트모션
	if (m_bCreateTextBox2 && !m_bCheckAttack3 && m_fHPTime > 1.f)
	{
		if (dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().iSpeed < dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().iSpeed)
		{
			if (m_bPlayerHit)
				dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Set_AnimIndex(5);
		}
		else if (dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().iSpeed >= dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().iSpeed)
		{
			if (m_bTargetHit)
				dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Set_AnimIndex(5);
		}
		m_bCheckAttack3 = true;
	}
	//플레이어 포켓몬 죽어서 포켓몬 교체
	if (m_bBattleBagPoke && !m_tInfo.pPlayer->Get_BattlePokeDead())
	{
		m_iPlayerIndex = m_tInfo.pPlayer->Get_iChangePoke();
		m_iPlayerHPIndex = m_tInfo.pPlayer->Get_iChangePoke();
		Ready_PlayerChange_Poke();
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
		CTextBox::TINFO tTInfo;

		tTInfo.iScriptSize = (_int)m_vBattleScript.size();
		tTInfo.pTarget = dynamic_cast<CGameObj*>(m_tInfo.pPlayer_Orgin);
		tTInfo.pScript = new wstring[m_vBattleScript.size()];
		tTInfo.iType = 3;
		for (_int i = 0; i < m_vBattleScript.size(); ++i)
			tTInfo.pScript[i] = m_vBattleScript[i];

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_TextBox"), LEVEL_GAMEPLAY, TEXT("Layer_UI"), &tTInfo)))
			return;
		RELEASE_INSTANCE(CGameInstance);
		m_bBattleBagPoke = false;
		m_fDelayTime = 0.f;

		dynamic_cast<CGameObj*>(m_tInfo.pPlayer_Orgin)->Set_AnimIndex(2);
	}
	//서로 포켓몬 죽었는지 확인
	m_fDelayTime += fTimeDelta;
	if (m_bPokeDead && m_fDelayTime > m_fTextSizeTime)
	{
		if (dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().eStatInfo == STUN)
			Change_TargetPoke(fTimeDelta);
		if (dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eStatInfo == STUN)
			Change_PlayerPoke(fTimeDelta);
	}
	//플레이어 포켓몬 죽어서 교체후 모션 딜레이 확인후 턴종료
	if (!m_bBattleBagPoke && m_bPokeDead && m_fDelayTime > m_fTextSizeTime)
	{
		m_bPokeDead = false;
		m_bPlayerAttack = false;
		m_bCheckAttack = false;
		m_bCheckAttack2 = false;
		m_bCheckAttack3 = false;

		m_bCreateTextBox = false;
		m_bCreateTextBox2 = false;

		m_fDotDeal = 0;
		m_fHPTime = 0.f;
		m_fDelayTime = 0.f;
		Set_CheckPos(CHECKPOS_INFO);
		m_bSkill = false;
	}
}

void CBattleUI::Check_Selected()
{
	switch (m_iSelect)
	{
	case 0:
		if (!m_bSkill)
		{
			m_bSkill = true;
			Set_CheckPos(CHECKPOS_SKILL);
		}
		else
		{
			if (dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eSkillNum1->iPoint > 0)
			{
				Use_PlayerSkill(m_iSelect);
				dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Set_PokePP1(-1);
			}
		}
		break;
	case 1:
		if (!m_bSkill)
		{
			if (!m_bBattleBagPoke)
			{
				m_tInfo.pPlayer->Set_BattleChangePoke();
				m_bBattleChangePoke = true;
			}
		}
		else
		{
			if (dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eSkillNum2->iPoint > 0)
			{
				Use_PlayerSkill(m_iSelect);
				dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Set_PokePP2(-1);
			}
		}
		break;
	case 2:
		if (!m_bSkill)
		{
			if (!m_bBattleUseItem)
			{
				m_tInfo.pPlayer->Set_BattleUseItem();
				m_bBattleUseItem = true;
			}
			m_fDelayTime = 0.f;
			m_fHPTime = 0.f;
		}
		else
		{
			if (dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eSkillNum3->iPoint > 0)
			{
				Use_PlayerSkill(m_iSelect);
				dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Set_PokePP3(-1);
			}
		}
		break;
	case 3:
		if (!m_bSkill)
		{
			if (m_tInfo.eBattleType == BATTLE_TRAINER)
			{
				Ready_RunFAILScript();
				CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
				CTextBox::TINFO tTInfo;

				tTInfo.iScriptSize = (_int)m_vBattleScript.size();
				tTInfo.pTarget = dynamic_cast<CGameObj*>(m_tInfo.pPlayer_Orgin);
				tTInfo.pScript = new wstring[m_vBattleScript.size()];
				tTInfo.iType = 3;
				for (_int i = 0; i < m_vBattleScript.size(); ++i)
					tTInfo.pScript[i] = m_vBattleScript[i];

				if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_TextBox"), LEVEL_GAMEPLAY, TEXT("Layer_UI"), &tTInfo)))
					return;
				RELEASE_INSTANCE(CGameInstance);
			}
			else
			{ 
				Ready_Run();
				CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
				CTextBox::TINFO tTInfo;

				tTInfo.iScriptSize = (_int)m_vBattleScript.size();
				tTInfo.pTarget = dynamic_cast<CGameObj*>(m_tInfo.pPlayer_Orgin);
				tTInfo.pScript = new wstring[m_vBattleScript.size()];
				tTInfo.iType = 5;
				for (_int i = 0; i < m_vBattleScript.size(); ++i)
					tTInfo.pScript[i] = m_vBattleScript[i];

				if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_TextBox"), LEVEL_GAMEPLAY, TEXT("Layer_UI"), &tTInfo)))
					return;
				RELEASE_INSTANCE(CGameInstance);
				dynamic_cast<CPlayer*>(m_tInfo.pPlayer_Orgin)->Battle_Win();
				m_tInfo.pBattleTarget->Set_Dead();
				g_Battle = false;
			}
		}
		else
		{
			if (dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eSkillNum4->iPoint > 0)
			{
				Use_PlayerSkill(m_iSelect);
				dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Set_PokePP4(-1);
			}
		}
		break;
	default:
		break;
	}

}

_int CBattleUI::Check_TargetSkill()
{
	if (dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().eSkillNum4->iSkillNum != 99)
		return 4;
	if (dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().eSkillNum3->iSkillNum != 99)
		return 3;
	if (dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().eSkillNum2->iSkillNum != 99)
		return 2;

		return 1;
}


void CBattleUI::Set_CheckPos(CHECKPOS eType)
{
	_float3 vScale;
	switch (eType)
	{
	case CHECKPOS_INFO:
		m_fSizeX = 50.f;
		m_fSizeY = 50.f;
		m_fX = 1000.f;
		m_fY = 450.f;

		vScale = { m_fSizeX,m_fSizeY,0.f };

		m_pTransformCom->Set_Scale(XMLoadFloat3(&vScale));
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));
		m_iSelect = 0;
		break;
	case CHECKPOS_SKILL:
		m_fSizeX = 50.f;
		m_fSizeY = 50.f;
		m_fX = 870.f;
		m_fY = 500.f;

		vScale = { m_fSizeX,m_fSizeY,0.f };

		m_pTransformCom->Set_Scale(XMLoadFloat3(&vScale));
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));
		m_iSelect = 0;
		break;
	default:
		break;
	}
}

void CBattleUI::Use_PlayerSkill(_int _iSkillIndex)
{
	_int iSkillDmg = 0;
	_int iDmg = 0;
	_int iDef = 0;
	_int iLv = dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().iLv;
	POKETYPE eType1 = dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eType;
	POKETYPE eType2 = dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eType2;
	POKETYPE eTargetType1 = dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().eType;
	POKETYPE eTargetType2 = dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().eType2;
	_float fRand = _float(rand() % 38 + 217.f) / 255.f;
	_float	fCri = 1.f;
	_float	fTypeBonus = 1.f;
	_float	fTargetType1 = 1.f;
	_float	fTargetType2 = 1.f;
	_float	_fHit = 1.f;
	switch (_iSkillIndex)
	{
	case 0:
		if (dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eSkillNum1->iDmg == 0)
		{
			iSkillDmg = dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eSkillNum1->iSDmg;
			iDmg = dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().iSDmg;
			iDef = dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().iSDef;
		}
		else
		{
			iSkillDmg = dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eSkillNum1->iDmg;
			iDmg = dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().iDmg;
			iDef = dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().iDef;
		}
		if (eType1 == dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eSkillNum1->eType)
			fTypeBonus = 1.5f;
		else if (eType2 == dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eSkillNum1->eType)
			fTypeBonus = 1.5f;		
		fTargetType1 = Check_Type(dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eSkillNum1->eType, eTargetType1);
		fTargetType2 = Check_Type(dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eSkillNum1->eType, eTargetType2);
		_fHit = dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eSkillNum1->fHit;
		m_bPlayerSkillIndex = 0;
		break;
	case 1:
		if (dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eSkillNum2->iDmg == 0)
		{
			iSkillDmg = dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eSkillNum2->iSDmg;
			iDmg = dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().iSDmg;
			iDef = dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().iSDef;
		}
		else
		{
			iSkillDmg = dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eSkillNum2->iDmg;
			iDmg = dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().iDmg;
			iDef = dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().iDef;
		}
		if (eType1 == dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eSkillNum2->eType)
			fTypeBonus = 1.5f;
		else if (eType2 == dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eSkillNum2->eType)
			fTypeBonus = 1.5f;
		fTargetType1 = Check_Type(dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eSkillNum2->eType, eTargetType1);
		fTargetType2 = Check_Type(dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eSkillNum2->eType, eTargetType2);
		_fHit = dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eSkillNum2->fHit;
		m_bPlayerSkillIndex = 1;
		break;
	case 2:
		if (dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eSkillNum3->iDmg == 0)
		{
			iSkillDmg = dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eSkillNum3->iSDmg;
			iDmg = dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().iSDmg;
			iDef = dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().iSDef;
		}
		else
		{
			iSkillDmg = dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eSkillNum3->iDmg;
			iDmg = dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().iDmg;
			iDef = dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().iDef;
		}
		if (eType1 == dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eSkillNum3->eType)
			fTypeBonus = 1.5f;
		else if (eType2 == dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eSkillNum3->eType)
			fTypeBonus = 1.5f;
		fTargetType1 = Check_Type(dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eSkillNum3->eType, eTargetType1);
		fTargetType2 = Check_Type(dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eSkillNum3->eType, eTargetType2);
		_fHit = dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eSkillNum3->fHit;
		m_bPlayerSkillIndex = 2;
		break;
	case 3:
		if (dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eSkillNum4->iDmg == 0)
		{
			iSkillDmg = dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eSkillNum4->iSDmg;
			iDmg = dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().iSDmg;
			iDef = dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().iSDef;
		}
		else
		{
			iSkillDmg = dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eSkillNum4->iDmg;
			iDmg = dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().iDmg;
			iDef = dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().iDef;
		}
		if (eType1 == dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eSkillNum4->eType)
			fTypeBonus = 1.5f;
		else if (eType2 == dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eSkillNum4->eType)
			fTypeBonus = 1.5f;
		fTargetType1 = Check_Type(dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eSkillNum4->eType, eTargetType1);
		fTargetType2 = Check_Type(dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eSkillNum4->eType, eTargetType2);
		_fHit = dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eSkillNum4->fHit;
		m_bPlayerSkillIndex = 3;
		break;
	default:
		break;
	}
	
	_int iCri = rand() % 10;
	if (iCri == 7)
	{
		fCri = 1.5f;
		m_bPlayerCri = true;
	}
	else
		m_bPlayerCri = false;

	_float fHit = rand() % 100 / 100.f;
	//(데미지 = 
	//(위력 × 공격 ×(레벨 × 2 ÷ 5 + 2) ÷ 방어 ÷ 50 ×[[급소]] + 2) ×[[자속 보정]] × 타입상성1 × 타입상성2 × 랜덤수 / 255)

	m_iPlayerFinalDmg = _int(((_float)iSkillDmg * (_float)iDmg * ((_float)iLv * 2.f / 5.f + 2.f) / (_float)iDef / 50.f * fCri + 2.f) * fTypeBonus * fTargetType1 * fTargetType2 * fRand);

	if (_fHit >= fHit)
		m_bPlayerHit = true;
	else
		m_bPlayerHit = false;

	m_fPlayerType = fTargetType1 * fTargetType2;

	m_bPlayerAttack = true;
}
void CBattleUI::Use_TargetSkill(_int _iSkillIndex)
{
	_int iSkillDmg = 0;
	_int iDmg = 0;
	_int iDef = 0;
	_int iLv = dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().iLv;
	POKETYPE eTargetType1 = dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eType;
	POKETYPE eTargetType2 = dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eType2;
	POKETYPE eType1 = dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().eType;
	POKETYPE eType2 = dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().eType2;
	_float fRand = _float(rand() % 38 + 217.f) / 255.f;
	_float	fCri = 1.f;
	_float	fTypeBonus = 1.f;
	_float	fTargetType1 = 1.f;
	_float	fTargetType2 = 1.f;
	_float	_fHit = 1.f;
	switch (_iSkillIndex)
	{
	case 0:
		if (dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().eSkillNum1->iDmg == 0)
		{
			iSkillDmg = dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().eSkillNum1->iSDmg;
			iDmg = dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().iSDmg;
			iDef = dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().iSDef;
		}
		else
		{
			iSkillDmg = dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().eSkillNum1->iDmg;
			iDmg = dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().iDmg;
			iDef = dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().iDef;
		}
		if (eType1 == dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().eSkillNum1->eType)
			fTypeBonus = 1.5f;
		else if (eType2 == dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().eSkillNum1->eType)
			fTypeBonus = 1.5f;
		fTargetType1 = Check_Type(dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().eSkillNum1->eType, eTargetType1);
		fTargetType2 = Check_Type(dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().eSkillNum1->eType, eTargetType2);
		_fHit = dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().eSkillNum1->fHit;
		m_bTargetSkillIndex = 0;
		break;
	case 1:
		if (dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().eSkillNum2->iDmg == 0)
		{
			iSkillDmg = dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().eSkillNum2->iSDmg;
			iDmg = dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().iSDmg;
			iDef = dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().iSDef;
		}
		else
		{
			iSkillDmg = dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().eSkillNum2->iDmg;
			iDmg = dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().iDmg;
			iDef = dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().iDef;
		}
		if (eType1 == dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().eSkillNum2->eType)
			fTypeBonus = 1.5f;
		else if (eType2 == dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().eSkillNum2->eType)
			fTypeBonus = 1.5f;
		fTargetType1 = Check_Type(dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().eSkillNum2->eType, eTargetType1);
		fTargetType2 = Check_Type(dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().eSkillNum2->eType, eTargetType2);
		_fHit = dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().eSkillNum2->fHit;
		m_bTargetSkillIndex = 1;
		break;
	case 2:
		if (dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().eSkillNum3->iDmg == 0)
		{
			iSkillDmg = dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().eSkillNum3->iSDmg;
			iDmg = dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().iSDmg;
			iDef = dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().iSDef;
		}
		else
		{
			iSkillDmg = dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().eSkillNum3->iDmg;
			iDmg = dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().iDmg;
			iDef = dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().iDef;
		}
		if (eType1 == dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().eSkillNum3->eType)
			fTypeBonus = 1.5f;
		else if (eType2 == dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().eSkillNum3->eType)
			fTypeBonus = 1.5f;
		fTargetType1 = Check_Type(dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().eSkillNum3->eType, eTargetType1);
		fTargetType2 = Check_Type(dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().eSkillNum3->eType, eTargetType2);
		_fHit = dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().eSkillNum3->fHit;
		m_bTargetSkillIndex = 2;
		break;
	case 3:
		if (dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().eSkillNum4->iDmg == 0)
		{
			iSkillDmg = dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().eSkillNum4->iSDmg;
			iDmg = dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().iSDmg;
			iDef = dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().iSDef;
		}
		else
		{
			iSkillDmg = dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().eSkillNum4->iDmg;
			iDmg = dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().iDmg;
			iDef = dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().iDef;
		}
		if (eType1 == dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().eSkillNum4->eType)
			fTypeBonus = 1.5f;
		else if (eType2 == dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().eSkillNum4->eType)
			fTypeBonus = 1.5f;
		fTargetType1 = Check_Type(dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().eSkillNum4->eType, eTargetType1);
		fTargetType2 = Check_Type(dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().eSkillNum4->eType, eTargetType2);
		_fHit = dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().eSkillNum4->fHit;
		m_bTargetSkillIndex = 3;
		break;
	default:
		break;
	}

	_int iCri = rand() % 10;
	if (iCri == 7)
	{
		fCri = 1.5f;
		m_bTargetCri = true;
	}
	else
		m_bTargetCri = false;

	_float fHit = rand() % 100 / 100.f;
	//(데미지 = 
	//(위력 × 공격 ×(레벨 × 2 ÷ 5 + 2) ÷ 방어 ÷ 50 ×[[급소]] + 2) ×[[자속 보정]] × 타입상성1 × 타입상성2 × 랜덤수 / 255)

	m_iTargetFinalDmg = _int(((_float)iSkillDmg * (_float)iDmg * ((_float)iLv * 2.f / 5.f + 2.f) / (_float)iDef / 50.f * fCri + 2.f) * fTypeBonus * fTargetType1 * fTargetType2 * fRand);

	if (_fHit >= fHit)
		m_bTargetHit = true;
	else
		m_bTargetHit = false;

	m_fTargetType = fTargetType1 * fTargetType2;
}
void CBattleUI::Ready_PlayerScript()
{
	for (auto iter = m_vBattleScript.begin(); iter != m_vBattleScript.end();)
		iter = m_vBattleScript.erase(iter);

	m_vBattleScript.clear();


	wstring strTextBegin = TEXT("'");
	wstring strTextSkillName = TEXT("'");
	wstring strTextEnd = TEXT("' !!!!");

	switch (m_bPlayerSkillIndex)
	{
	case 0:
		strTextSkillName = dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eSkillNum1->strName;
		break;
	case 1:
		strTextSkillName = dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eSkillNum2->strName;
		break;
	case 2:
		strTextSkillName = dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eSkillNum3->strName;
		break;
	case 3:
		strTextSkillName = dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().eSkillNum4->strName;
		break;
	default:
		break;
	}

	strTextBegin += dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().strName;
	strTextBegin += TEXT("'  ");
	strTextBegin += strTextSkillName;
	strTextBegin += strTextEnd;

	m_vBattleScript.push_back(strTextBegin);

	wstring strHit = TEXT("");
	wstring strCri = TEXT("");
	wstring strType = TEXT("");
	_bool	bPlayerType = false;


	if (m_fPlayerType >= 2.f)
		strType = TEXT("효과가 굉장했다!!");
	else if (m_fPlayerType == 1.f)
		bPlayerType = true;
	else if (m_fPlayerType <= 0.75f && m_fPlayerType > 0.f)
		strType = TEXT("효과가 별로인 것 같다...");
	else if (m_fPlayerType == 0.f)
		strType = TEXT("효과가 없는 것 같다...");

	if (!m_bPlayerHit)
	{
		strHit = TEXT("공격이 빗나갔다...");
		m_vBattleScript.push_back(strHit);
	}
	else
	{
		if (m_bPlayerCri)
		{
			strCri = TEXT("급소에 맞았다!!!");
			m_vBattleScript.push_back(strCri);
			if(!bPlayerType)
				m_vBattleScript.push_back(strType);
		}
		else
		{
			if (!bPlayerType)
				m_vBattleScript.push_back(strType);
		}

		_int iHp = dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().iHp - m_iPlayerFinalDmg;
		if (iHp <= 0)
		{
			wstring strTextBegin2 = TEXT("'");
			wstring strTextSkillName2 = TEXT("' (이)가 쓰러졌습니다... ");

			strTextBegin2 += dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().strName;
			strTextBegin2 += strTextSkillName2;

			m_vBattleScript.push_back(strTextBegin2);
		}
	}



}
void CBattleUI::Ready_TargetScript()
{
	for (auto iter = m_vBattleScript.begin(); iter != m_vBattleScript.end();)
		iter = m_vBattleScript.erase(iter);

	m_vBattleScript.clear();


	wstring strTextBegin = TEXT("'");
	wstring strTextSkillName = TEXT("'");
	wstring strTextEnd = TEXT("' !!!!");

	switch (m_bTargetSkillIndex)
	{
	case 0:
		strTextSkillName = dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().eSkillNum1->strName;
		break;
	case 1:
		strTextSkillName = dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().eSkillNum2->strName;
		break;
	case 2:
		strTextSkillName = dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().eSkillNum3->strName;
		break;
	case 3:
		strTextSkillName = dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().eSkillNum4->strName;
		break;
	default:
		break;
	}

	strTextBegin += dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().strName;
	strTextBegin += TEXT("'  ");
	strTextBegin += strTextSkillName;
	strTextBegin += strTextEnd;

	m_vBattleScript.push_back(strTextBegin);

	wstring strHit = TEXT("");
	wstring strCri = TEXT("");
	wstring strTargetType = TEXT("");
	_bool	bTargetType = false;


	if (m_fTargetType >= 2.f)
		strTargetType = TEXT("효과가 굉장했다!!");
	else if (m_fTargetType == 1.f)
		bTargetType = true;
	else if (m_fTargetType <= 0.75f && m_fTargetType > 0.f)
		strTargetType = TEXT("효과가 별로인 것 같다...");
	else if (m_fTargetType == 0.f)
		strTargetType = TEXT("효과가 없는 것 같다...");

	if (!m_bTargetHit)
	{
		strHit = TEXT("공격이 빗나갔다...");
		m_vBattleScript.push_back(strHit);
	}
	else
	{
		if (m_bTargetCri)
		{
			strCri = TEXT("급소에 맞았다!!!");
			m_vBattleScript.push_back(strCri);
			if (!bTargetType)
				m_vBattleScript.push_back(strTargetType);
		}
		else
		{
			if (!bTargetType)
				m_vBattleScript.push_back(strTargetType);
		}

		_int iHp = dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().iHp - m_iTargetFinalDmg;
		if (iHp <= 0)
		{
			wstring strTextBegin2 = TEXT("'");
			wstring strTextSkillName2 = TEXT("' (이)가 쓰러졌습니다... ");

			strTextBegin2 += dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().strName;
			strTextBegin2 += strTextSkillName2;

			m_vBattleScript.push_back(strTextBegin2);
		}
	}
}

void CBattleUI::Ready_PlayerChange_Poke()
{
	for (auto iter = m_vBattleScript.begin(); iter != m_vBattleScript.end();)
		iter = m_vBattleScript.erase(iter);

	m_vBattleScript.clear();

	wstring strTextBegin = TEXT("가랏!!  '");
	wstring strTextSkillName = TEXT("'  너로 정했다!! ");

	strTextBegin += dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_PokeInfo().strName;
	strTextBegin += strTextSkillName;

	m_vBattleScript.push_back(strTextBegin);
}
void CBattleUI::Ready_TargetChange_Poke()
{
	for (auto iter = m_vBattleScript.begin(); iter != m_vBattleScript.end();)
		iter = m_vBattleScript.erase(iter);

	m_vBattleScript.clear();

	wstring strTextBegin = TEXT("가랏!!  '");
	wstring strTextSkillName = TEXT("'  너로 정했다!! ");

	strTextBegin += dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_PokeInfo().strName;
	strTextBegin += strTextSkillName;

	m_vBattleScript.push_back(strTextBegin);
}
_float CBattleUI::Check_Type(POKETYPE _eType, POKETYPE _eTargetType)
{	
	if (_eType == NORMAL)
	{
		if (_eTargetType == ROCK)
			return 0.5f;
		if (_eTargetType == GHOST)
			return 0.f;
		if (_eTargetType == STEEL)
			return 0.5f;
	}
	if (_eType == FIRE)
	{
		if (_eTargetType == FIRE)
			return 0.5f;
		if (_eTargetType == WATER)
			return 0.5f;
		if (_eTargetType == GRASS)
			return 2.f;
		if (_eTargetType == ICE)
			return 2.f;
		if (_eTargetType == BUG)
			return 2.f;
		if (_eTargetType == ROCK)
			return 0.5f;
		if (_eTargetType == DRAGON)
			return 0.5f;
		if (_eTargetType == STEEL)
			return 2.f;
	}
	if (_eType == WATER)
	{
		if (_eTargetType == FIRE)
			return 2.f;
		if (_eTargetType == WATER)
			return 0.5f;
		if (_eTargetType == GRASS)
			return 0.5f;
		if (_eTargetType == TERRAIN)
			return 2.f;
		if (_eTargetType == ROCK)
			return 2.f;
		if (_eTargetType == DRAGON)
			return 0.5f;
	}
	if (_eType == GRASS)
	{
		if (_eTargetType == FIRE)
			return 0.5f;
		if (_eTargetType == WATER)
			return 2.f;
		if (_eTargetType == GRASS)
			return 0.5f;
		if (_eTargetType == POISON)
			return 0.5f;
		if (_eTargetType == TERRAIN)
			return 2.f;
		if (_eTargetType == FLY)
			return 0.5f;
		if (_eTargetType == BUG)
			return 0.5f;
		if (_eTargetType == ROCK)
			return 2.f;
		if (_eTargetType == DRAGON)
			return 0.5f;
		if (_eTargetType == STEEL)
			return 0.5f;
	}
	if (_eType == THUNDER)
	{
		if (_eTargetType == WATER)
			return 2.f;
		if (_eTargetType == GRASS)
			return 0.5f;
		if (_eTargetType == THUNDER)
			return 0.5f;
		if (_eTargetType == TERRAIN)
			return 0.f;
		if (_eTargetType == FLY)
			return 2.f;
		if (_eTargetType == DRAGON)
			return 0.5f;
	}
	if (_eType == ICE)
	{
		if (_eTargetType == FIRE)
			return 0.5f;
		if (_eTargetType == WATER)
			return 0.5f;
		if (_eTargetType == GRASS)
			return 2.f;
		if (_eTargetType == ICE)
			return 0.5f;
		if (_eTargetType == TERRAIN)
			return 2.f;
		if (_eTargetType == FLY)
			return 2.f;
		if (_eTargetType == DRAGON)
			return 2.f;
		if (_eTargetType == STEEL)
			return 0.5f;
	}
	if (_eType == FIGHT)
	{
		if (_eTargetType == NORMAL)
			return 2.f;
		if (_eTargetType == ICE)
			return 2.f;
		if (_eTargetType == POISON)
			return 0.5f;
		if (_eTargetType == FLY)
			return 0.5f;
		if (_eTargetType == ESPER)
			return 0.5f;
		if (_eTargetType == BUG)
			return 0.5f;
		if (_eTargetType == ROCK)
			return 2.f;
		if (_eTargetType == GHOST)
			return 0.f;
		if (_eTargetType == EVIL)
			return 2.f;
		if (_eTargetType == STEEL)
			return 2.f;
		if (_eTargetType == FAIRY)
			return 0.5f;
	}
	if (_eType == POISON)
	{
		if (_eTargetType == GRASS)
			return 2.f;
		if (_eTargetType == POISON)
			return 0.5f;
		if (_eTargetType == TERRAIN)
			return 0.5f;
		if (_eTargetType == ROCK)
			return 0.5f;
		if (_eTargetType == GHOST)
			return 0.5f;
		if (_eTargetType == STEEL)
			return 0.f;
		if (_eTargetType == FAIRY)
			return 2.f;
	}
	if (_eType == TERRAIN)
	{
		if (_eTargetType == FIRE)
			return 2.f;
		if (_eTargetType == GRASS)
			return 0.5f;
		if (_eTargetType == THUNDER)
			return 2.f;
		if (_eTargetType == POISON)
			return 2.f;
		if (_eTargetType == FLY)
			return 0.f;
		if (_eTargetType == BUG)
			return 0.5f;
		if (_eTargetType == ROCK)
			return 2.f;
		if (_eTargetType == STEEL)
			return 2.f;
	}
	if (_eType == FLY)
	{
		if (_eTargetType == GRASS)
			return 2.f;
		if (_eTargetType == THUNDER)
			return 0.5f;
		if (_eTargetType == FIGHT)
			return 2.f;
		if (_eTargetType == BUG)
			return 2.f;
		if (_eTargetType == ROCK)
			return 0.5f;
		if (_eTargetType == STEEL)
			return 0.5f;
	}
	if (_eType == ESPER)
	{
		if (_eTargetType == FIGHT)
			return 2.f;
		if (_eTargetType == POISON)
			return 2.f;
		if (_eTargetType == ESPER)
			return 0.5f;
		if (_eTargetType == EVIL)
			return 0.f;
		if (_eTargetType == STEEL)
			return 0.5f;
	}
	if (_eType == BUG)
	{
		if (_eTargetType == FIRE)
			return 0.5f;
		if (_eTargetType == GRASS)
			return 2.f;
		if (_eTargetType == FIGHT)
			return 0.5f;
		if (_eTargetType == POISON)
			return 0.5f;
		if (_eTargetType == FLY)
			return 0.5f;
		if (_eTargetType == ESPER)
			return 2.f;
		if (_eTargetType == GHOST)
			return 0.5f;
		if (_eTargetType == EVIL)
			return 2.f;
		if (_eTargetType == STEEL)
			return 0.5f;
		if (_eTargetType == FAIRY)
			return 0.5f;
	}
	if (_eType == ROCK)
	{
		if (_eTargetType == FIRE)
			return 2.f;
		if (_eTargetType == ICE)
			return 2.f;
		if (_eTargetType == FIGHT)
			return 0.5f;
		if (_eTargetType == TERRAIN)
			return 0.5f;
		if (_eTargetType == FLY)
			return 2.f;
		if (_eTargetType == BUG)
			return 2.f;
		if (_eTargetType == STEEL)
			return 0.5f;
	}
	if (_eType == GHOST)
	{
		if (_eTargetType == NORMAL)
			return 0.f;
		if (_eTargetType == ESPER)
			return 2.f;
		if (_eTargetType == GHOST)
			return 2.f;
		if (_eTargetType == EVIL)
			return 0.5f;
	}
	if (_eType == DRAGON)
	{
		if (_eTargetType == DRAGON)
			return 2.f;
		if (_eTargetType == STEEL)
			return 0.5f;
		if (_eTargetType == FAIRY)
			return 0.f;
	}
	if (_eType == EVIL)
	{
		if (_eTargetType == FIGHT)
			return 0.5f;
		if (_eTargetType == ESPER)
			return 2.f;
		if (_eTargetType == GHOST)
			return 2.f;
		if (_eTargetType == EVIL)
			return 0.5f;
		if (_eTargetType == FAIRY)
			return 0.5f;
	}
	if (_eType == STEEL)
	{
		if (_eTargetType == FIRE)
			return 0.5f;
		if (_eTargetType == WATER)
			return 0.5f;
		if (_eTargetType == THUNDER)
			return 0.5f;
		if (_eTargetType == ICE)
			return 2.f;
		if (_eTargetType == ROCK)
			return 2.f;
		if (_eTargetType == STEEL)
			return 0.5f;
		if (_eTargetType == FAIRY)
			return 2.f;
	}
	if (_eType == FAIRY)
	{
		if (_eTargetType == FIRE)
			return 0.5f;
		if (_eTargetType == FIGHT)
			return 2.f;
		if (_eTargetType == POISON)
			return 0.5f;
		if (_eTargetType == DRAGON)
			return 2.f;
		if (_eTargetType == EVIL)
			return 2.f;
		if (_eTargetType == STEEL)
			return 0.5f;
	}
	
	return 1.f;
}

void CBattleUI::Change_PlayerPoke(_float fTimeDelta)
{
	if (!m_tInfo.pPlayer->Get_BattlePokeDead())
	{
		dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Get_Transfrom()->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(-50000.f, -50000.f, -50000.f, 1.f));
		dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(m_iPlayerIndex))->Reset_Anim();
		_int iHP = 0;
		for (_int i = 0; i < 6; ++i)
		{
			if (iHP < dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(i))->Get_PokeInfo().iHp)
				iHP = dynamic_cast<CGameObj*>(m_tInfo.pPlayer->Get_vecPoke(i))->Get_PokeInfo().iHp;
		}
		if (iHP <= 0)
		{
			//전투 종료 패배
		}
		else
		{
			m_tInfo.pPlayer->Set_BattlePokeDead();
			m_bBattleBagPoke = true;
		}
	}
}

void CBattleUI::Change_TargetPoke(_float fTimeDelta)
{
	dynamic_cast<CGameObj*>((*m_tInfo.pvecTargetPoke)[m_iTargetIndex])->Get_Transfrom()->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(-50000.f, -50000.f, -50000.f, 1.f));
	if (m_iTargetIndex < dynamic_cast<CGameObj*>(m_tInfo.pBattleTarget)->Get_PokeMaxSize())
	{
		++m_iTargetIndex;
		dynamic_cast<CGameObj*>(m_tInfo.pBattleTarget)->Set_AnimIndex(1);
		dynamic_cast<CGameObj*>(m_tInfo.pBattleTarget)->Set_PokeSize();
		Ready_TargetChange_Poke();
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
		CTextBox::TINFO tTInfo;

		tTInfo.iScriptSize = (_int)m_vBattleScript.size();
		tTInfo.pTarget = dynamic_cast<CGameObj*>(m_tInfo.pBattleTarget);
		tTInfo.pScript = new wstring[m_vBattleScript.size()];
		tTInfo.iType = 3;
		for (_int i = 0; i < m_vBattleScript.size(); ++i)
			tTInfo.pScript[i] = m_vBattleScript[i];

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_TextBox"), LEVEL_GAMEPLAY, TEXT("Layer_UI"), &tTInfo)))
			return;
		RELEASE_INSTANCE(CGameInstance);
	}
	else
	{
		if (m_tInfo.eBattleType == BATTLE_TRAINER)
			dynamic_cast<CGameObj*>(m_tInfo.pBattleTarget)->Set_AnimIndex(4);
		else
		{
			m_bWildWin = true;
			m_WinTime = 0.f;
		}
	}
}


CBattleUI * CBattleUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBattleUI*	pInstance = new CBattleUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CBattleUI"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CBattleUI::Clone(void * pArg)
{
	CGameObject*	pInstance = new CBattleUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CBattleUI"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBattleUI::Free()
{
	__super::Free();


	for (auto iter = m_vBattleScript.begin(); iter != m_vBattleScript.end();)
		iter = m_vBattleScript.erase(iter);

	m_vBattleScript.clear();

	for (int i = 0; i < 4; ++i)
	{
		Safe_Release(m_pVIBufferInfo[i]);
		Safe_Release(m_pTransformInfo[i]);
		Safe_Release(m_pShaderInfo[i]);
	}
	for (int i = 0; i < 5; ++i)
	{
		Safe_Release(m_pVIBufferTarget[i]);
		Safe_Release(m_pTransformTarget[i]);
		Safe_Release(m_pShaderTarget[i]);
	}
	for (int i = 0; i < 6; ++i)
	{
		Safe_Release(m_pVIBufferPlayer[i]);
		Safe_Release(m_pTransformPlayer[i]);
		Safe_Release(m_pShaderPlayer[i]);
	}
	for (int i = 0; i < 8; ++i)
	{
		Safe_Release(m_pVIBufferSkill[i]);
		Safe_Release(m_pTransformSkill[i]);
		Safe_Release(m_pShaderSkill[i]);
	}
	for (int i = 0; i < 12; ++i)
	{
		Safe_Release(m_pVIBufferBall[i]);
		Safe_Release(m_pTransformBall[i]);
		Safe_Release(m_pShaderBall[i]);
	}
	Safe_Release(m_pTextureCom2);
	Safe_Release(m_pTextureCom3);
	Safe_Release(m_pTextureCom4);

}