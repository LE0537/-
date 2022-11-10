#include "stdafx.h"
#include "..\Public\TextBox.h"

#include "GameInstance.h"
#include "SoundMgr.h"

CTextBox::CTextBox(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
	ZeroMemory(&m_tTInfo, sizeof(TINFO));
}

CTextBox::CTextBox(const CTextBox & rhs)
	: CGameObj(rhs)
	,m_tTInfo(rhs.m_tTInfo)
{
}

HRESULT CTextBox::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTextBox::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;
	
	memcpy(&m_tTInfo, pArg, sizeof(TINFO));
	
	m_fSizeX = 1000.f;
	m_fSizeY = 260.f;
	m_fX = 640.f;
	m_fY = 570.f;

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixTranspose(XMMatrixIdentity()));
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, -200.f, 100.f)));

	_float3 vScale = { m_fSizeX,m_fSizeY,0.f };

	m_pTransformCom->Set_Scale(XMLoadFloat3(&vScale));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));

	m_pScript = m_tTInfo.pScript;

	return S_OK;
}

void CTextBox::Tick(_float fTimeDelta)
{
	Running_TextBox();
	Print_Text();
	m_fTimeDelta += fTimeDelta;
	m_fDeadTime += fTimeDelta;
}

void CTextBox::Late_Tick(_float fTimeDelta)
{
	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CTextBox::Render()
{
	if (nullptr == m_pShaderCom || nullptr == m_pVIBufferCom )
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTextBox::Ready_Components()
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

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTextBox::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_World4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_SRV(15))))
		return E_FAIL;

	m_pShaderCom->Begin(6);
	m_pVIBufferCom->Render();

	RenderFonts();
	
	return S_OK;
}

void CTextBox::Running_TextBox()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	if (m_tTInfo.iType == 0)
	{
		if (pGameInstance->Key_Down(DIK_SPACE))
		{
			if (m_iScriptIndex < m_tTInfo.iScriptSize - 1)
			{
				++m_iScriptIndex;
				m_wstr = TEXT("");
				m_iIndex = 0;
			}
			else if (m_iScriptIndex == m_tTInfo.iScriptSize - 1)
			{
				if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_BattleIntro"), LEVEL_GAMEPLAY, TEXT("Layer_Effect"))))
					return;
				CSoundMgr::Get_Instance()->BGM_Stop();
				CSoundMgr::Get_Instance()->PlayEffect(TEXT("Battle1.wav"), 1.f);
				m_bDead = true;
			}
		}
	}
	else if (m_tTInfo.iType == 1)
	{
		if (m_fDeadTime > 3.f)
			m_bDead = true;
	}
	else if (m_tTInfo.iType == 2)
	{
		if (m_fDeadTime > 2.5f)
			m_bDead = true;
	}
	else if (m_tTInfo.iType == 3)
	{
		if (m_fDeadTime > 2.0f)
		{
			if (m_iScriptIndex < m_tTInfo.iScriptSize - 1)
			{
				++m_iScriptIndex;
				m_wstr = TEXT("");
				m_iIndex = 0;
			}
			else if (m_iScriptIndex == m_tTInfo.iScriptSize - 1)
			{
				m_bDead = true;
			}
			m_fDeadTime = 0.f;
		}
	}
	else if (m_tTInfo.iType == 4)
	{
		if (m_fDeadTime > 1.5f)
		{
		
			m_bDead = true;
			
			m_fDeadTime = 0.f;
		}
	}
	RELEASE_INSTANCE(CGameInstance);
}

void CTextBox::RenderFonts()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	wstring szName = dynamic_cast<CGameObj*>(m_tTInfo.pTarget)->Get_PalyerInfo().strName;
	_vector vPos = { 210.f,550.f,0.f,1.f };
	pGameInstance->Render_Font(TEXT("Font_Nexon"), m_wstr.c_str(), vPos, XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(1.2f, 1.2f, 1.2f, 1.f));
	vPos = { 220.f,460.f,0.f,1.f };
	pGameInstance->Render_Font(TEXT("Font_Nexon"), szName.c_str(), vPos, XMVectorSet(1.f, 1.f, 1.f, 1.f), XMVectorSet(1.5f, 1.5f, 1.5f, 1.f));
	
	RELEASE_INSTANCE(CGameInstance);
}

void CTextBox::Print_Text(void)
{
	wstring wstr = m_pScript[m_iScriptIndex];

	if (m_iIndex < wstr.length() && m_fTimeDelta > 0.03f)
	{
		m_wstr += wstr[m_iIndex];
		++m_iIndex;
		m_fTimeDelta = 0.f;
		m_bFontRender = true;
	}
}

CTextBox * CTextBox::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTextBox*	pInstance = new CTextBox(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CTextBox"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CTextBox::Clone(void * pArg)
{
	CGameObject*	pInstance = new CTextBox(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CTextBox"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTextBox::Free()
{
	if (nullptr != m_pScript)
	{
		for (_int i = 0; i < m_tTInfo.iScriptSize; ++i)
			m_pScript[i].clear();
		Safe_Delete_Array(m_pScript);
	}

	__super::Free();

}
