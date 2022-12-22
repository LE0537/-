#include "stdafx.h"
#include "..\Public\CaptureStar.h"

#include "GameInstance.h"
#include "Level_GamePlay.h"

CCaptureStar::CCaptureStar(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
}

CCaptureStar::CCaptureStar(const CCaptureStar & rhs)
	: CGameObj(rhs)
{
}

HRESULT CCaptureStar::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCaptureStar::Initialize(void * pArg)
{

	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_fSize = 0.35f;
	_vector vScale = { m_fSize ,m_fSize ,m_fSize ,0.f };
	m_pTransformCom->Set_Scale(vScale);

	_float4 vPos = ((CLevel_GamePlay::LOADFILE*)pArg)->vPos;
	vPos.y += 0.5f;
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&vPos));

	_int iIndex = _int(((CLevel_GamePlay::LOADFILE*)pArg)->vScale.x);

	Check_Pos(iIndex);



	return S_OK;
}

void CCaptureStar::Tick(_float fTimeDelta)
{
	Set_Pos(fTimeDelta);

}

void CCaptureStar::Late_Tick(_float fTimeDelta)
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

HRESULT CCaptureStar::Render()
{

	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;


	return S_OK;
}
HRESULT CCaptureStar::Render_ShadowDepth()
{
	return E_NOTIMPL;
}
HRESULT CCaptureStar::Ready_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Star"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;


	return S_OK;
}

void CCaptureStar::OnBillboard()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_matrix		ViewMatrix = XMMatrixInverse(nullptr, pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));

	_vector vScale = XMLoadFloat3(&m_pTransformCom->Get_Scale());

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, ViewMatrix.r[0] * vScale.m128_f32[0]);

	m_pTransformCom->Set_State(CTransform::STATE_UP, ViewMatrix.r[1] * vScale.m128_f32[1]);

	m_pTransformCom->Set_State(CTransform::STATE_LOOK, ViewMatrix.r[2] * vScale.m128_f32[2]);

	RELEASE_INSTANCE(CGameInstance);
}

void CCaptureStar::Set_Pos(_float fTimeDelta)
{
	m_fSpeed += 4.f * fTimeDelta;

	_vector vPos = XMVectorCatmullRom(XMLoadFloat4(&m_vPos1), XMLoadFloat4(&m_vPos2)
		, XMLoadFloat4(&m_vPos3), XMLoadFloat4(&m_vPos4), m_fSpeed);

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vPos);

	m_fDeadTime += fTimeDelta;
	if (m_fDeadTime > 0.35f)
		Set_Dead();

}

void CCaptureStar::Check_Pos(_int _iIndex)
{
	_vector vTargetPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_vector vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
	XMVector3Normalize(vRight);

	switch (_iIndex)
	{
	case 0:
		vTargetPos -= vRight * 0.5f;
		XMStoreFloat4(&m_vPos1, vTargetPos);
		vTargetPos.m128_f32[1] += 0.5f;
		XMStoreFloat4(&m_vPos2, vTargetPos);
		vTargetPos -= vRight * 2.f;
		vTargetPos.m128_f32[1] += 0.5f;
		XMStoreFloat4(&m_vPos3, vTargetPos);
		vTargetPos -= vRight * 3.5f;
		vTargetPos.m128_f32[1] -= 0.75f;
		XMStoreFloat4(&m_vPos4, vTargetPos);
		break;
	case 1:
		vTargetPos += vRight * 0.3f;
		XMStoreFloat4(&m_vPos1, vTargetPos);
		vTargetPos.m128_f32[1] += 0.5f;
		XMStoreFloat4(&m_vPos2, vTargetPos);
		vTargetPos += vRight * 1.5f;
		vTargetPos.m128_f32[1] += 0.5f;
		XMStoreFloat4(&m_vPos3, vTargetPos);
		vTargetPos += vRight * 2.f;
		vTargetPos.m128_f32[1] -= 0.75f;
		XMStoreFloat4(&m_vPos4, vTargetPos);
		break;
	case 2:
		vTargetPos += vRight * 1.5f;
		XMStoreFloat4(&m_vPos1, vTargetPos);
		vTargetPos.m128_f32[1] += 0.5f;
		XMStoreFloat4(&m_vPos2, vTargetPos);
		vTargetPos += vRight * 3.5f;
		vTargetPos.m128_f32[1] += 0.5f;
		XMStoreFloat4(&m_vPos3, vTargetPos);
		vTargetPos += vRight * 5.5f;
		vTargetPos.m128_f32[1] -= 0.75f;
		XMStoreFloat4(&m_vPos4, vTargetPos);
		break;
	default:
		break;
	}

}

HRESULT CCaptureStar::SetUp_ShaderResources()
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

	if (FAILED(m_pShaderCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_SRV(0))))
		return E_FAIL;

	m_pShaderCom->Begin(13);
	m_pVIBufferCom->Render();

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CCaptureStar * CCaptureStar::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCaptureStar*	pInstance = new CCaptureStar(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CCaptureStar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CCaptureStar::Clone(void * pArg)
{
	CGameObject*	pInstance = new CCaptureStar(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CCaptureStar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCaptureStar::Free()
{
	__super::Free();


}
