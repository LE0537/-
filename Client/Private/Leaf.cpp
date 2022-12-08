#include "stdafx.h"
#include "..\Public\Leaf.h"

#include "GameInstance.h"
#include "Level_GamePlay.h"

CLeaf::CLeaf(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
}

CLeaf::CLeaf(const CLeaf & rhs)
	: CGameObj(rhs)
{
}

HRESULT CLeaf::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLeaf::Initialize(void * pArg)
{

	if (FAILED(Ready_Components()))
		return E_FAIL;

//	m_pTransformCom->Set_Scale(XMVectorSet(0.055f, 0.055f, 0.055f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4((&((CLevel_GamePlay::LOADFILE*)pArg)->vPos)));

	return S_OK;
}

void CLeaf::Tick(_float fTimeDelta)
{
	_vector vRight = { -1.f,0.f,0.f,0.f };
	_vector vLook = { 0.f,0.f,1.f,0.f };
	m_pTransformCom->Turn(vRight,fTimeDelta);
	m_pTransformCom->Turn(vLook,fTimeDelta);
	_vector vUp = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	vUp.m128_f32[0] -= 0.2f;
	vUp.m128_f32[1] -= 0.3f;
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vUp);

	if (vUp.m128_f32[1] < -1.f)
		Set_Dead();
}

void CLeaf::Late_Tick(_float fTimeDelta)
{

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);


	if (pGameInstance->IsInFrustum(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION), 10.f))
	{
		if (!g_bEvolution && !g_bBag && !g_PokeInfo && !g_bPokeDeck && nullptr != m_pRendererCom)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}

	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CLeaf::Render()
{

	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;


	return S_OK;
}
HRESULT CLeaf::Ready_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(60.0f);

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Leaf"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;


	return S_OK;
}

HRESULT CLeaf::SetUp_ShaderResources()
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

	m_pShaderCom->Begin(11);
	m_pVIBufferCom->Render();

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CLeaf * CLeaf::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLeaf*	pInstance = new CLeaf(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CLeaf"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CLeaf::Clone(void * pArg)
{
	CGameObject*	pInstance = new CLeaf(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CLeaf"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLeaf::Free()
{
	__super::Free();


}
