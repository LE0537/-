#include "stdafx.h"
#include "..\Public\Lv_Up.h"

#include "GameInstance.h"
#include "Level_GamePlay.h"

CLv_Up::CLv_Up(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
}

CLv_Up::CLv_Up(const CLv_Up & rhs)
	: CGameObj(rhs)
{
}

HRESULT CLv_Up::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLv_Up::Initialize(void * pArg)
{
	ZeroMemory(&m_LvInfo, sizeof(LVUPINFO));
	memcpy(&m_LvInfo, pArg, sizeof(LVUPINFO));

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_fSizeX = 450.f;
	m_fSizeY = 340.f;
	m_fX = 225.f;
	m_fY = 360.f;

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixTranspose(XMMatrixIdentity()));
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, -200.f, 100.f)));

	_float3 vScale = { m_fSizeX,m_fSizeY,0.f };

	m_pTransformCom->Set_Scale(XMLoadFloat3(&vScale));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));

	m_fDeadTime = 0.f;
	return S_OK;
}

void CLv_Up::Tick(_float fTimeDelta)
{
	m_fDeadTime += fTimeDelta;
	if (m_fDeadTime > 2.f)
	{
		m_bDead = true;
	}

}

void CLv_Up::Late_Tick(_float fTimeDelta)
{

	if (!g_bBag && !g_bEvolution && nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

}

HRESULT CLv_Up::Render()
{

	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	Render_Fonts();

	return S_OK;
}
HRESULT CLv_Up::Ready_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Battle"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;


	return S_OK;
}

void CLv_Up::Render_Fonts()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	
	wstring szName = TEXT("");
	_vector vPos = { 225.f,250.f,0.f,1.f };
	//Hp
	szName = to_wstring(m_LvInfo.iPrevMaxHp);
	pGameInstance->Render_Font(TEXT("Font_Nexon"), szName.c_str(), vPos, XMVectorSet(0.f, 0.f, 0.f, 0.85f), XMVectorSet(1.f, 1.f, 1.f, 1.f));
	vPos = { 320.f,250.f,0.f,1.f };
	szName = TEXT("+  ");
	szName += to_wstring(m_LvInfo.iMaxHp - m_LvInfo.iPrevMaxHp);
	pGameInstance->Render_Font(TEXT("Font_Nexon"), szName.c_str(), vPos, XMVectorSet(1.f, 0.f, 0.f, 0.85f), XMVectorSet(1.f, 1.f, 1.f, 1.f));
	//DMG
	vPos = { 220.f,300.f,0.f,1.f };
	szName = to_wstring(m_LvInfo.iPrevDmg);
	pGameInstance->Render_Font(TEXT("Font_Nexon"), szName.c_str(), vPos, XMVectorSet(0.f, 0.f, 0.f, 0.85f), XMVectorSet(1.f, 1.f, 1.f, 1.f));
	vPos = { 320.f,300.f,0.f,1.f };
	szName = TEXT("+  ");
	szName += to_wstring(m_LvInfo.iDmg - m_LvInfo.iPrevDmg);
	pGameInstance->Render_Font(TEXT("Font_Nexon"), szName.c_str(), vPos, XMVectorSet(1.f, 0.f, 0.f, 0.85f), XMVectorSet(1.f, 1.f, 1.f, 1.f));
	//Def
	vPos = { 220.f,350.f,0.f,1.f };
	szName = to_wstring(m_LvInfo.iPrevDef);
	pGameInstance->Render_Font(TEXT("Font_Nexon"), szName.c_str(), vPos, XMVectorSet(0.f, 0.f, 0.f, 0.85f), XMVectorSet(1.f, 1.f, 1.f, 1.f));
	vPos = { 320.f,350.f,0.f,1.f };
	szName = TEXT("+  ");
	szName += to_wstring(m_LvInfo.iDef - m_LvInfo.iPrevDef);
	pGameInstance->Render_Font(TEXT("Font_Nexon"), szName.c_str(), vPos, XMVectorSet(1.f, 0.f, 0.f, 0.85f), XMVectorSet(1.f, 1.f, 1.f, 1.f));
	//SDMG
	vPos = { 220.f,400.f,0.f,1.f };
	szName = to_wstring(m_LvInfo.iPrevSDmg);
	pGameInstance->Render_Font(TEXT("Font_Nexon"), szName.c_str(), vPos, XMVectorSet(0.f, 0.f, 0.f, 0.85f), XMVectorSet(1.f, 1.f, 1.f, 1.f));
	vPos = { 320.f,400.f,0.f,1.f };
	szName = TEXT("+  ");
	szName += to_wstring(m_LvInfo.iSDmg - m_LvInfo.iPrevSDmg);
	pGameInstance->Render_Font(TEXT("Font_Nexon"), szName.c_str(), vPos, XMVectorSet(1.f, 0.f, 0.f, 0.85f), XMVectorSet(1.f, 1.f, 1.f, 1.f));
	//SDef
	vPos = { 220.f,450.f,0.f,1.f };
	szName = to_wstring(m_LvInfo.iPrevSDef);
	pGameInstance->Render_Font(TEXT("Font_Nexon"), szName.c_str(), vPos, XMVectorSet(0.f, 0.f, 0.f, 0.85f), XMVectorSet(1.f, 1.f, 1.f, 1.f));
	vPos = { 320.f,450.f,0.f,1.f };
	szName = TEXT("+  ");
	szName += to_wstring(m_LvInfo.iSDef - m_LvInfo.iPrevSDef);
	pGameInstance->Render_Font(TEXT("Font_Nexon"), szName.c_str(), vPos, XMVectorSet(1.f, 0.f, 0.f, 0.85f), XMVectorSet(1.f, 1.f, 1.f, 1.f));
	//Speed
	vPos = { 220.f,500.f,0.f,1.f };
	szName = to_wstring(m_LvInfo.iPrevSpeed);
	pGameInstance->Render_Font(TEXT("Font_Nexon"), szName.c_str(), vPos, XMVectorSet(0.f, 0.f, 0.f, 0.85f), XMVectorSet(1.f, 1.f, 1.f, 1.f));
	vPos = { 320.f,500.f,0.f,1.f };
	szName = TEXT("+  ");
	szName += to_wstring(m_LvInfo.iSpeed - m_LvInfo.iPrevSpeed);
	pGameInstance->Render_Font(TEXT("Font_Nexon"), szName.c_str(), vPos, XMVectorSet(1.f, 0.f, 0.f, 0.85f), XMVectorSet(1.f, 1.f, 1.f, 1.f));





	RELEASE_INSTANCE(CGameInstance);
}


HRESULT CLv_Up::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;


	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_World4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_SRV(20))))
		return E_FAIL;

	m_pShaderCom->Begin(7);
	m_pVIBufferCom->Render();



	return S_OK;
}

CLv_Up * CLv_Up::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLv_Up*	pInstance = new CLv_Up(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CLv_Up"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CLv_Up::Clone(void * pArg)
{
	CGameObject*	pInstance = new CLv_Up(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CLv_Up"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLv_Up::Free()
{
	__super::Free();


}
