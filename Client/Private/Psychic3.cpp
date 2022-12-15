#include "stdafx.h"
#include "..\Public\Psychic3.h"

#include "GameInstance.h"
#include "Level_GamePlay.h"

CPsychic3::CPsychic3(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
}

CPsychic3::CPsychic3(const CPsychic3 & rhs)
	: CGameObj(rhs)
{
}

HRESULT CPsychic3::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPsychic3::Initialize(void * pArg)
{

	if (FAILED(Ready_Components()))
		return E_FAIL;

	_float fIndex = ((CLevel_GamePlay::LOADFILE*)pArg)->vScale.x;

	if(fIndex > 0.f)
		m_fSize = (_float(rand() % 40 + 1) * 0.1f);
	else
		m_fSize = (_float(rand() % 10 + 1) * 0.1f);
	_vector vScale = { m_fSize ,m_fSize ,m_fSize ,0.f };
	m_pTransformCom->Set_Scale(vScale);

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4((&((CLevel_GamePlay::LOADFILE*)pArg)->vPos)));

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

	vPos.m128_f32[0] += _float(rand() % 100 - 50.f) * 0.01f;
	vPos.m128_f32[1] += _float(rand() % 100 - 50.f) * 0.01f;
	vPos.m128_f32[2] += _float(rand() % 100 - 50.f) * 0.01f;

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vPos);


	return S_OK;
}

void CPsychic3::Tick(_float fTimeDelta)
{
	Set_Pos(fTimeDelta);

}

void CPsychic3::Late_Tick(_float fTimeDelta)
{

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	

	if (pGameInstance->IsInFrustum(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION), 10.f))
	{
		if (!g_PokeInfo && !g_bPokeDeck && nullptr != m_pRendererCom)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
	}

	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CPsychic3::Render()
{

	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;


	return S_OK;
}
HRESULT CPsychic3::Ready_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Psychic2"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;


	return S_OK;
}

void CPsychic3::OnBillboard()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_matrix		ViewMatrix = XMMatrixInverse(nullptr, pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));

	_vector vScale = XMLoadFloat3(&m_pTransformCom->Get_Scale());

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, ViewMatrix.r[0] * vScale.m128_f32[0]);

	m_pTransformCom->Set_State(CTransform::STATE_UP, ViewMatrix.r[1] * vScale.m128_f32[1]);

	m_pTransformCom->Set_State(CTransform::STATE_LOOK, ViewMatrix.r[2] * vScale.m128_f32[2]);

	RELEASE_INSTANCE(CGameInstance);
}

void CPsychic3::Set_Pos(_float fTimeDelta)
{
	m_fFrame += fTimeDelta;
	if (m_fFrame > 0.03f)
	{
		++m_iFrame;
		if(m_iFrame > 15)
			Set_Dead();

		m_fFrame = 0.f;
	}
	
}

HRESULT CPsychic3::SetUp_ShaderResources()
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

	m_pShaderCom->Begin(19);
	m_pVIBufferCom->Render();

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CPsychic3 * CPsychic3::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPsychic3*	pInstance = new CPsychic3(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CPsychic3"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CPsychic3::Clone(void * pArg)
{
	CGameObject*	pInstance = new CPsychic3(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CPsychic3"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPsychic3::Free()
{
	__super::Free();


}
