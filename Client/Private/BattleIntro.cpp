#include "stdafx.h"
#include "..\Public\BattleIntro.h"

#include "GameInstance.h"
#include "SoundMgr.h"

CBattleIntro::CBattleIntro(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
}

CBattleIntro::CBattleIntro(const CBattleIntro & rhs)
	: CGameObj(rhs)
{
}

HRESULT CBattleIntro::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBattleIntro::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTarget = (CGameObject*)pArg;

	Set_Pos2();

	return S_OK;
}

void CBattleIntro::Tick(_float fTimeDelta)
{
	Set_Pos();
	RatBall(fTimeDelta);
}

void CBattleIntro::Late_Tick(_float fTimeDelta)
{
	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup_Front(CRenderer::RENDER_UI, this);
}

HRESULT CBattleIntro::Render()
{
	if (nullptr == m_pShaderCom || nullptr == m_pVIBufferCom )
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBattleIntro::Ready_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_TransformCom2"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom2)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_TransformCom3"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom3)))
		return E_FAIL;
	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_ShaderCom2"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), (CComponent**)&m_pShaderCom2)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_ShaderCom3"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), (CComponent**)&m_pShaderCom3)))
		return E_FAIL;
	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Battle"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_VIBufferCom2"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom2)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_VIBufferCom3"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom3)))
		return E_FAIL;
	wstring szBuffer[5];
	wstring szTrans[5];
	wstring szShader[5];
	for (int i = 0; i < 5; ++i)
	{
		szBuffer[i] = TEXT("Com_VIBuffer");
		szTrans[i] = TEXT("Com_Transform");
		szShader[i] = TEXT("Com_Shader");
	}
	for (int i = 0; i < 5; ++i)
	{
		szBuffer[i] += to_wstring(i);
		szTrans[i] += to_wstring(i);
		szShader[i] += to_wstring(i);
		if (FAILED(__super::Add_Components(szBuffer[i].c_str(), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBuffer[i])))
			return E_FAIL;
		if (FAILED(__super::Add_Components(szTrans[i].c_str(), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransform[i])))
			return E_FAIL;
		if (FAILED(__super::Add_Components(szShader[i].c_str(), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), (CComponent**)&m_pShader[i])))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CBattleIntro::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom )
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_World4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_SRV(16))))
		return E_FAIL;

	m_pShaderCom->Begin(4);
	m_pVIBufferCom->Render();
	for (_int i = 0; i < 5; ++i)
	{
		if (FAILED(m_pShader[i]->Set_RawValue("g_WorldMatrix", &m_pTransform[i]->Get_World4x4_TP(), sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShader[i]->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShader[i]->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShader[i]->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_SRV(16))))
			return E_FAIL;

		m_pShader[i]->Begin(4);
		m_pVIBuffer[i]->Render();
	}

	if (FAILED(m_pShaderCom2->Set_RawValue("g_WorldMatrix", &m_pTransformCom2->Get_World4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom2->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom2->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom2->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_SRV(17))))
		return E_FAIL;

	m_pShaderCom2->Begin(0);
	m_pVIBufferCom2->Render();

	if (m_bEnd)
	{
		if (FAILED(m_pShaderCom3->Set_RawValue("g_WorldMatrix", &m_pTransformCom3->Get_World4x4_TP(), sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderCom3->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
			return E_FAIL;
		if (FAILED(m_pShaderCom3->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
			return E_FAIL;
	
		m_pShaderCom3->Begin(5);
		m_pVIBufferCom3->Render();
	}

	return S_OK;
}

void CBattleIntro::Set_Pos()
{
	m_fX[0] += 16.f;
	if (m_fX[0] > 1920)
		m_fX[0] = -1904.f;

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fX[0] - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));
	m_pTransform[2]->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fX[0] - g_iWinSizeX * 0.5f, -m_fY2 + g_iWinSizeY * 0.5f, 0.f, 1.f));

	m_fX[1] += 16.f;
	if (m_fX[1] > 1920)
		m_fX[1] = -1904.f;

	m_pTransform[0]->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fX[1] - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));
	m_pTransform[3]->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fX[1] - g_iWinSizeX * 0.5f, -m_fY2 + g_iWinSizeY * 0.5f, 0.f, 1.f));

	m_fX[2] += 16.f;
	if (m_fX[2] > 1920)
		m_fX[2] = -1904.f;

	m_pTransform[1]->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fX[2] - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));
	m_pTransform[4]->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fX[2] - g_iWinSizeX * 0.5f, -m_fY2 + g_iWinSizeY * 0.5f, 0.f, 1.f));

}

void CBattleIntro::Set_Pos2()
{
	m_fSizeX = 1280.f;
	m_fSizeY = 200.f;
	m_fX[0] = 640.f;
	m_fY = 100.f;
	m_fY2 = 620.f;

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixTranspose(XMMatrixIdentity()));
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, -200.f, 100.f)));

	_float3 vScale = { m_fSizeX,m_fSizeY,0.f };

	m_pTransformCom->Set_Scale(XMLoadFloat3(&vScale));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fX[0] - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));
	m_pTransform[2]->Set_Scale(XMLoadFloat3(&vScale));
	m_pTransform[2]->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fX[0] - g_iWinSizeX * 0.5f, -m_fY2 + g_iWinSizeY * 0.5f, 0.f, 1.f));

	m_fX[1] = -640.f;

	m_pTransform[0]->Set_Scale(XMLoadFloat3(&vScale));
	m_pTransform[0]->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fX[1] - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));
	m_pTransform[3]->Set_Scale(XMLoadFloat3(&vScale));
	m_pTransform[3]->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fX[1] - g_iWinSizeX * 0.5f, -m_fY2 + g_iWinSizeY * 0.5f, 0.f, 1.f));

	m_fX[2] = -1920.f;

	m_pTransform[1]->Set_Scale(XMLoadFloat3(&vScale));
	m_pTransform[1]->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fX[2] - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));
	m_pTransform[4]->Set_Scale(XMLoadFloat3(&vScale));
	m_pTransform[4]->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fX[2] - g_iWinSizeX * 0.5f, -m_fY2 + g_iWinSizeY * 0.5f, 0.f, 1.f));

	m_fBallSizeX = 446.f;
	m_fBallSizeY = 342.f;
	m_fBallX = 1100.f;
	m_fBallY = 200.f;
	m_fDist = 480.f;

	vScale = { m_fBallSizeX,m_fBallSizeY,0.f };
	m_pTransformCom2->Set_Scale(XMLoadFloat3(&vScale));
	m_pTransformCom2->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fBallX - g_iWinSizeX * 0.5f, -m_fBallY + g_iWinSizeY * 0.5f, 0.f, 1.f));
	m_pTransformCom3->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(0.f, 0.f, 0.f, 1.f));
}

void CBattleIntro::RatBall(_float fTimeDelta)
{
	_float fAngleZ = 9.f;
	m_fDist -= 4.f;
	if (m_fDist < 0.f)
	{
		m_fDist = 0.f;
		fAngleZ = 0.f;
		m_fDeadtime += fTimeDelta;
	}
	if (m_fDeadtime == 0.f)
	{
		m_pTransformCom3->Turn2(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(fAngleZ));
		_vector vRight = m_pTransformCom3->Get_State(CTransform::STATE_RIGHT);
		XMVector3Normalize(vRight);
		_vector vPos = m_pTransformCom3->Get_State(CTransform::STATE_TRANSLATION) + (vRight * m_fDist);

		m_pTransformCom2->Turn2(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(fAngleZ + 6.f));
		m_pTransformCom2->Set_State(CTransform::STATE_TRANSLATION, vPos);
	}
	if (!m_bEnd && m_fDeadtime > 0.35f)
	{
		_float3 vScale2 = { 5000.f,5000.f,0.f };
		m_pTransformCom3->Set_Scale(XMLoadFloat3(&vScale2));
		m_bEnd = true;
	}
	if (m_fDeadtime > 0.5f)
	{
		CSoundMgr::Get_Instance()->BGM_Stop();
		CSoundMgr::Get_Instance()->PlayBGM(TEXT("Battle.wav"), 0.5f);
		g_Battle = true;
		m_bDead = true;
	}
}


CBattleIntro * CBattleIntro::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBattleIntro*	pInstance = new CBattleIntro(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CBattleIntro"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CBattleIntro::Clone(void * pArg)
{
	CGameObject*	pInstance = new CBattleIntro(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CBattleIntro"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBattleIntro::Free()
{
	__super::Free();

	for (int i = 0; i < 5; ++i)
	{
		Safe_Release(m_pTransform[i]);
		Safe_Release(m_pShader[i]);
		Safe_Release(m_pVIBuffer[i]);
	}
	Safe_Release(m_pTransformCom2);
	Safe_Release(m_pTransformCom3);
	Safe_Release(m_pShaderCom2);
	Safe_Release(m_pVIBufferCom2);
	Safe_Release(m_pShaderCom3);
	Safe_Release(m_pVIBufferCom3);
}
