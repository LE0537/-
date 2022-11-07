#include "stdafx.h"
#include "..\Public\BattleUI.h"

#include "GameInstance.h"
#include "MonsterBall.h"
#include "Player.h"

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
//	m_pPlayer = *(CGameObject**)pArg;

	if (FAILED(Ready_Components()))
		return E_FAIL;


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
	return S_OK;
}

void CBattleUI::Tick(_float fTimeDelta)
{
	Key_Input();

}

void CBattleUI::Late_Tick(_float fTimeDelta)
{

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
	
}

HRESULT CBattleUI::Render()
{

	if (nullptr == m_pShaderCom || nullptr == m_pVIBufferCom)
		return E_FAIL;
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
	if (FAILED(SetUp_ShaderPlayer()))
		return E_FAIL;
	if (FAILED(SetUp_ShaderTarget()))
		return E_FAIL;
	if (FAILED(SetUp_ShaderBall()))
		return E_FAIL;

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
		if (FAILED(m_pShaderSkill[i]->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom2->Get_SRV(8))))
			return E_FAIL;

		m_pShaderSkill[i]->Begin(7);
		m_pVIBufferSkill[i]->Render();
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

	if (FAILED(m_pShaderPlayer[2]->Set_RawValue("g_WorldMatrix", &m_pTransformPlayer[2]->Get_World4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderPlayer[2]->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderPlayer[2]->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderPlayer[2]->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom4->Get_SRV(5))))
		return E_FAIL;

	m_pShaderPlayer[2]->Begin(7);
	m_pVIBufferPlayer[2]->Render();

	if (FAILED(m_pShaderPlayer[3]->Set_RawValue("g_WorldMatrix", &m_pTransformPlayer[3]->Get_World4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderPlayer[3]->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderPlayer[3]->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderPlayer[3]->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom3->Get_SRV(7))))
		return E_FAIL;

	m_pShaderPlayer[3]->Begin(7);
	m_pVIBufferPlayer[3]->Render();

	if (FAILED(m_pShaderPlayer[4]->Set_RawValue("g_WorldMatrix", &m_pTransformPlayer[4]->Get_World4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderPlayer[4]->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderPlayer[4]->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderPlayer[4]->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom3->Get_SRV(7))))
		return E_FAIL;

	m_pShaderPlayer[4]->Begin(7);
	m_pVIBufferPlayer[4]->Render();

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

	if (FAILED(m_pShaderTarget[2]->Set_RawValue("g_WorldMatrix", &m_pTransformTarget[2]->Get_World4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderTarget[2]->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderTarget[2]->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderTarget[2]->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom4->Get_SRV(4))))
		return E_FAIL;

	m_pShaderTarget[2]->Begin(7);
	m_pVIBufferTarget[2]->Render();

	if (FAILED(m_pShaderTarget[3]->Set_RawValue("g_WorldMatrix", &m_pTransformTarget[3]->Get_World4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderTarget[3]->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderTarget[3]->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderTarget[3]->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom3->Get_SRV(7))))
		return E_FAIL;

	m_pShaderTarget[3]->Begin(7);
	m_pVIBufferTarget[3]->Render();

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
		if (FAILED(m_pShaderBall[i]->Set_RawValue("g_WorldMatrix", &m_pTransformBall[i]->Get_World4x4_TP(), sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderBall[i]->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderBall[i]->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderBall[i]->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_SRV(10))))
			return E_FAIL;

		m_pShaderBall[i]->Begin(7);
		m_pVIBufferBall[i]->Render();
	}

	for (_uint i = 6; i < 12; ++i)
	{
		if (FAILED(m_pShaderBall[i]->Set_RawValue("g_WorldMatrix", &m_pTransformBall[i]->Get_World4x4_TP(), sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderBall[i]->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderBall[i]->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderBall[i]->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_SRV(10))))
			return E_FAIL;

		m_pShaderBall[i]->Begin(7);
		m_pVIBufferBall[i]->Render();
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
			SetSelectButton(DIR_DOWN);
			++m_iSelect;
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

void CBattleUI::Check_Selected()
{
	switch (m_iSelect)
	{
	case 0:
		m_bSkill = true;
		Set_CheckPos(CHECKPOS_SKILL);
		break;
	case 1:
		break;
	case 2:
		break;
	case 3:
		break;
	default:
		break;
	}

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

	if (!m_vecPoke.empty())
	{
		for (int i = 0; i < 6; ++i)
		{
			Safe_Release(m_vecPoke[i]);
		}
		m_vecPoke.clear();
	}


}