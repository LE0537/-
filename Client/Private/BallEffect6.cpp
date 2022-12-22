#include "stdafx.h"
#include "..\Public\BallEffect6.h"

#include "GameInstance.h"
#include "Level_GamePlay.h"

CBallEffect6::CBallEffect6(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
}

CBallEffect6::CBallEffect6(const CBallEffect6 & rhs)
	: CGameObj(rhs)
{
}

HRESULT CBallEffect6::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBallEffect6::Initialize(void * pArg)
{

	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_fSize = 0.5f;//_float(rand() % 10 * 0.1f);
	_vector vScale = { m_fSize ,m_fSize ,1.f ,0.f };
	m_pTransformCom->Set_Scale(vScale);

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4((&((CLevel_GamePlay::LOADFILE*)pArg)->vPos)));

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	vPos.m128_f32[1] -= 0.5f;
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vPos);

	vPos.m128_f32[0] += _float(rand() % 200 - 100.f);
	vPos.m128_f32[2] += _float(rand() % 200 - 100.f);

	XMStoreFloat4(&m_vPos, vPos);

	return S_OK;
}

void CBallEffect6::Tick(_float fTimeDelta)
{
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_vector vLook = XMLoadFloat4(&m_vPos) - vPos;
	XMVector3Normalize(vLook);
	vPos += vLook * 0.03f * fTimeDelta;
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vPos);
	m_fFrameTime += fTimeDelta;
	if (m_fFrameTime > 0.05f)
	{
		m_fFrameTime = 0.f;
		++m_iFrame;
	}
	if (m_iFrame > 15)
		Set_Dead();
}

void CBallEffect6::Late_Tick(_float fTimeDelta)
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

HRESULT CBallEffect6::Render()
{

	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;


	return S_OK;
}
HRESULT CBallEffect6::Render_ShadowDepth()
{
	return E_NOTIMPL;
}
HRESULT CBallEffect6::Ready_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 3.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(60.0f);

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Land"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;


	return S_OK;
}

void CBallEffect6::OnBillboard()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_matrix		ViewMatrix = XMMatrixInverse(nullptr, pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));

	_vector	vScale = XMLoadFloat3(&m_pTransformCom->Get_Scale());

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, ViewMatrix.r[0] * vScale.m128_f32[0]);

	m_pTransformCom->Set_State(CTransform::STATE_UP, ViewMatrix.r[1] * vScale.m128_f32[1]);

	m_pTransformCom->Set_State(CTransform::STATE_LOOK, ViewMatrix.r[2] * vScale.m128_f32[2]);

	RELEASE_INSTANCE(CGameInstance);
}



HRESULT CBallEffect6::SetUp_ShaderResources()
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

	if (FAILED(m_pShaderCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_SRV(m_iFrame))))
		return E_FAIL;

	m_pShaderCom->Begin(9);
	m_pVIBufferCom->Render();

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CBallEffect6 * CBallEffect6::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBallEffect6*	pInstance = new CBallEffect6(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CBallEffect6"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CBallEffect6::Clone(void * pArg)
{
	CGameObject*	pInstance = new CBallEffect6(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CBallEffect6"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBallEffect6::Free()
{
	__super::Free();


}
