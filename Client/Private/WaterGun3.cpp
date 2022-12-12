#include "stdafx.h"
#include "..\Public\WaterGun3.h"

#include "GameInstance.h"
#include "Level_GamePlay.h"

CWaterGun3::CWaterGun3(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
}

CWaterGun3::CWaterGun3(const CWaterGun3 & rhs)
	: CGameObj(rhs)
{
}

HRESULT CWaterGun3::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWaterGun3::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_fSize = 0.25f;
	_vector vScale = { m_fSize ,m_fSize ,1.f ,0.f };

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4((&((CLevel_GamePlay::LOADFILE*)pArg)->vPos)));

	_float fIndex = ((CLevel_GamePlay::LOADFILE*)pArg)->vScale.x;

	if (fIndex > 0.5f)
	{
		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

		vPos.m128_f32[0] += _float(rand() % 50 - 25.f) * 0.001f;
		vPos.m128_f32[1] += _float(rand() % 50 - 25.f) * 0.001f;
		vPos.m128_f32[2] += _float(rand() % 50 - 25.f) * 0.001f;

		_vector vLook = vPos - m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		XMVector3Normalize(vLook);
		m_pTransformCom->Set_State(CTransform::STATE_UP, vLook);
		m_pTransformCom->Set_Scale(vScale);

		_vector vPos2 = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_vector vUp = m_pTransformCom->Get_State(CTransform::STATE_UP);

		vPos2 += XMVector3Normalize(vUp) * _float(rand() % 30 * 0.1f);
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vPos2);
	}


	return S_OK;
}

void CWaterGun3::Tick(_float fTimeDelta)
{
	Set_Pos(fTimeDelta);

}

void CWaterGun3::Late_Tick(_float fTimeDelta)
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

HRESULT CWaterGun3::Render()
{

	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;


	return S_OK;
}
HRESULT CWaterGun3::Ready_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_WaterGun3"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;


	return S_OK;
}

void CWaterGun3::OnBillboard()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_matrix		ViewMatrix = XMMatrixInverse(nullptr, pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));

	_vector vScale = XMLoadFloat3(&m_pTransformCom->Get_Scale());

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, ViewMatrix.r[0] * vScale.m128_f32[0]);

	m_pTransformCom->Set_State(CTransform::STATE_UP, ViewMatrix.r[1] * vScale.m128_f32[1]);

	m_pTransformCom->Set_State(CTransform::STATE_LOOK, ViewMatrix.r[2] * vScale.m128_f32[2]);

	RELEASE_INSTANCE(CGameInstance);
}

void CWaterGun3::Set_Pos(_float fTimeDelta)
{
	m_fFrameTime += fTimeDelta;
	if (m_fFrameTime > 0.05f)
	{
		++m_iFrame;
		m_fFrameTime = 0.f;
	}
	if (m_iFrame > 7)
		Set_Dead();

}

HRESULT CWaterGun3::SetUp_ShaderResources()
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

	m_pShaderCom->Begin(15);
	m_pVIBufferCom->Render();

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CWaterGun3 * CWaterGun3::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CWaterGun3*	pInstance = new CWaterGun3(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CWaterGun3"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CWaterGun3::Clone(void * pArg)
{
	CGameObject*	pInstance = new CWaterGun3(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CWaterGun3"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWaterGun3::Free()
{
	__super::Free();


}
