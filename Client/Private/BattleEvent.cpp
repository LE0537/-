#include "stdafx.h"
#include "..\Public\BattleEvent.h"

#include "GameInstance.h"
#include "Level_GamePlay.h"

CBattleEvent::CBattleEvent(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
}

CBattleEvent::CBattleEvent(const CBattleEvent & rhs)
	: CGameObj(rhs)
{
}

HRESULT CBattleEvent::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBattleEvent::Initialize(void * pArg)
{

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTarget = (CGameObject*)pArg;

	OffSet();


	
	return S_OK;
}

void CBattleEvent::Tick(_float fTimeDelta)
{
	if (!dynamic_cast<CGameObj*>(m_pTarget)->Get_Event())
	{
		m_bDead = true;
		return;
	}
	_vector vTargetPos = dynamic_cast<CGameObj*>(m_pTarget)->Get_Transfrom()->Get_State(CTransform::STATE_TRANSLATION);
	vTargetPos.m128_f32[1] += 1.5f + m_fOffSet;
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vTargetPos);
}

void CBattleEvent::Late_Tick(_float fTimeDelta)
{

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	OnBillboard();

	if (pGameInstance->IsInFrustum(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION), 10.f))
	{
		if (!g_PokeInfo && !g_bPokeDeck && nullptr != m_pRendererCom)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
	}

	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CBattleEvent::Render()
{

	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;


	return S_OK;
}
HRESULT CBattleEvent::Ready_Components()
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

void CBattleEvent::OnBillboard()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_matrix		ViewMatrix = XMMatrixInverse(nullptr, pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, ViewMatrix.r[0]);

	//m_pTransformCom->Set_State(CTransform::STATE_UP, *(_float3*)&ViewMatrix.m[1][0] * vScale.y);

	m_pTransformCom->Set_State(CTransform::STATE_LOOK, ViewMatrix.r[2]);

	RELEASE_INSTANCE(CGameInstance);
}

void CBattleEvent::OffSet()
{
	if (dynamic_cast<CGameObj*>(m_pTarget)->Get_PalyerInfo().strName == TEXT("¸¶¸®"))
		m_fOffSet = 1.3f;

	if (dynamic_cast<CGameObj*>(m_pTarget)->Get_PalyerInfo().strName == TEXT("ºñÆ®"))
		m_fOffSet = 1.5f;

	if (dynamic_cast<CGameObj*>(m_pTarget)->Get_PalyerInfo().strName == TEXT("¼Ò´Ï¾Æ"))
		m_fOffSet = 1.5f;

	if (dynamic_cast<CGameObj*>(m_pTarget)->Get_PalyerInfo().strName == TEXT("´Üµ¨"))
		m_fOffSet = 1.5f;

	if (dynamic_cast<CGameObj*>(m_pTarget)->Get_PalyerInfo().strName == TEXT("¿Ã¸®ºê"))
		m_fOffSet = 1.5f;

	if (dynamic_cast<CGameObj*>(m_pTarget)->Get_PalyerInfo().strName == TEXT("¾ÆÅ³"))
		m_fOffSet = 1.5f;

	if (dynamic_cast<CGameObj*>(m_pTarget)->Get_PalyerInfo().strName == TEXT("¾ßÃ»"))
		m_fOffSet = 1.5f;

	if (dynamic_cast<CGameObj*>(m_pTarget)->Get_PalyerInfo().strName == TEXT("¼ø¹«"))
		m_fOffSet = 1.5f;

	if (dynamic_cast<CGameObj*>(m_pTarget)->Get_PalyerInfo().strName == TEXT("Ã¤µÎ"))
		m_fOffSet = 1.5f;

	if (dynamic_cast<CGameObj*>(m_pTarget)->Get_PalyerInfo().strName == TEXT("¾î´Ï¾ð"))
		m_fOffSet = 1.5f;

	if (dynamic_cast<CGameObj*>(m_pTarget)->Get_PalyerInfo().strName == TEXT("Æ÷ÇÃ·¯"))
		m_fOffSet = 1.5f;

	if (dynamic_cast<CGameObj*>(m_pTarget)->Get_PalyerInfo().strName == TEXT("¸¶Äí¿Í"))
		m_fOffSet = 1.5f;

	if (dynamic_cast<CGameObj*>(m_pTarget)->Get_PalyerInfo().strName == TEXT("¸á·Ð"))
		m_fOffSet = 1.5f;

	if (dynamic_cast<CGameObj*>(m_pTarget)->Get_PalyerInfo().strName == TEXT("µÎ¼Û"))
		m_fOffSet = 1.5f;

	if (dynamic_cast<CGameObj*>(m_pTarget)->Get_PalyerInfo().strName == TEXT("±Ý¶û"))
		m_fOffSet = 1.5f;

	if (dynamic_cast<CGameObj*>(m_pTarget)->Get_PalyerInfo().strName == TEXT("²¿ºÎ±â"))
		m_fOffSet = 0.f;

	if (dynamic_cast<CGameObj*>(m_pTarget)->Get_PalyerInfo().strName == TEXT("¾î´ÏºÎ±â"))
		m_fOffSet = 1.f;

	if (dynamic_cast<CGameObj*>(m_pTarget)->Get_PalyerInfo().strName == TEXT("°ÅºÏ¿Õ"))
		m_fOffSet = 1.5f;

	if (dynamic_cast<CGameObj*>(m_pTarget)->Get_PalyerInfo().strName == TEXT("Ä³ÅÍÇÇ"))
		m_fOffSet = 0.f;

	if (dynamic_cast<CGameObj*>(m_pTarget)->Get_PalyerInfo().strName == TEXT("´Üµ¥±â"))
		m_fOffSet = 0.5f;

	if (dynamic_cast<CGameObj*>(m_pTarget)->Get_PalyerInfo().strName == TEXT("¹öÅÍÇÃ"))
		m_fOffSet = 1.f;

	if (dynamic_cast<CGameObj*>(m_pTarget)->Get_PalyerInfo().strName == TEXT("²¿·¿"))
		m_fOffSet = 0.5f;

	if (dynamic_cast<CGameObj*>(m_pTarget)->Get_PalyerInfo().strName == TEXT("·¹Æ®¶ó"))
		m_fOffSet = 1.f;

	if (dynamic_cast<CGameObj*>(m_pTarget)->Get_PalyerInfo().strName == TEXT("ÇÇÄ«Ãò"))
		m_fOffSet = 0.f;

	if (dynamic_cast<CGameObj*>(m_pTarget)->Get_PalyerInfo().strName == TEXT("Çª¸°"))
		m_fOffSet = 1.f;

	if (dynamic_cast<CGameObj*>(m_pTarget)->Get_PalyerInfo().strName == TEXT("ÇªÅ©¸°"))
		m_fOffSet = 1.5f;

	if (dynamic_cast<CGameObj*>(m_pTarget)->Get_PalyerInfo().strName == TEXT("³ª¿Ë"))
		m_fOffSet = 0.f;

	if (dynamic_cast<CGameObj*>(m_pTarget)->Get_PalyerInfo().strName == TEXT("Æä¸£½Ã¿Â"))
		m_fOffSet = 1.f;

	if (dynamic_cast<CGameObj*>(m_pTarget)->Get_PalyerInfo().strName == TEXT("¾ßµ·"))
		m_fOffSet = 0.5f;

	if (dynamic_cast<CGameObj*>(m_pTarget)->Get_PalyerInfo().strName == TEXT("¾ßµµ¶õ"))
		m_fOffSet = 1.f;

	if (dynamic_cast<CGameObj*>(m_pTarget)->Get_PalyerInfo().strName == TEXT("·Õ½ºÅæ"))
		m_fOffSet = 2.1f;

	if (dynamic_cast<CGameObj*>(m_pTarget)->Get_PalyerInfo().strName == TEXT("³»·ç¹Ì"))
		m_fOffSet = 1.f;

	if (dynamic_cast<CGameObj*>(m_pTarget)->Get_PalyerInfo().strName == TEXT("·°Å°"))
		m_fOffSet = 1.f;

	if (dynamic_cast<CGameObj*>(m_pTarget)->Get_PalyerInfo().strName == TEXT("Ä»Ä«"))
		m_fOffSet = 2.5f;

	if (dynamic_cast<CGameObj*>(m_pTarget)->Get_PalyerInfo().strName == TEXT("½º¶óÅ©"))
		m_fOffSet = 1.5f;

	if (dynamic_cast<CGameObj*>(m_pTarget)->Get_PalyerInfo().strName == TEXT("À×¾îÅ·"))
		m_fOffSet = 0.5f;

	if (dynamic_cast<CGameObj*>(m_pTarget)->Get_PalyerInfo().strName == TEXT("°¼¶óµµ½º"))
		m_fOffSet = 1.5f;

	if (dynamic_cast<CGameObj*>(m_pTarget)->Get_PalyerInfo().strName == TEXT("Àá¸¸º¸"))
		m_fOffSet = 2.5f;

	if (dynamic_cast<CGameObj*>(m_pTarget)->Get_PalyerInfo().strName == TEXT("¹Ì´¨"))
		m_fOffSet = 0.5f;

	if (dynamic_cast<CGameObj*>(m_pTarget)->Get_PalyerInfo().strName == TEXT("½Å´¨"))
		m_fOffSet = 1.f;

	if (dynamic_cast<CGameObj*>(m_pTarget)->Get_PalyerInfo().strName == TEXT("¸Á³ª´¨"))
		m_fOffSet = 2.f;

	if (dynamic_cast<CGameObj*>(m_pTarget)->Get_PalyerInfo().strName == TEXT("¹ÂÃ÷"))
		m_fOffSet = 2.f;

	if (dynamic_cast<CGameObj*>(m_pTarget)->Get_PalyerInfo().strName == TEXT("¹Â"))
		m_fOffSet = 1.f;

	if (dynamic_cast<CGameObj*>(m_pTarget)->Get_PalyerInfo().strName == TEXT("ÇØ¿ù¸·±¸¸®"))
		m_fOffSet = 1.f;
}

HRESULT CBattleEvent::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_World4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_SRV(12))))
		return E_FAIL;

	m_pShaderCom->Begin();
	m_pVIBufferCom->Render();

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CBattleEvent * CBattleEvent::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBattleEvent*	pInstance = new CBattleEvent(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CBattleEvent"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CBattleEvent::Clone(void * pArg)
{
	CGameObject*	pInstance = new CBattleEvent(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CBattleEvent"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBattleEvent::Free()
{
	__super::Free();

	
}
