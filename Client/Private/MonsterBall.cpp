#include "stdafx.h"
#include "..\Public\MonsterBall.h"

#include "GameInstance.h"

CMonsterBall::CMonsterBall(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
}

CMonsterBall::CMonsterBall(const CMonsterBall & rhs)
	: CGameObj(rhs)
{
}

HRESULT CMonsterBall::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMonsterBall::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;
	*(CGameObject**)pArg = this;

	m_ItemInfo.strName = TEXT("몬스터볼");
	m_ItemInfo.strInfo = TEXT("포켓몬을 잡을 수 있다.");
	m_ItemInfo.iItemNum = 0;
	m_ItemInfo.iNum = 1;
	m_ItemInfo.iPrice = 200;

	m_fSizeX = 40.f;
	m_fSizeY = 40.f;
	m_fX = 0.f;
	m_fY = 0.f;

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixTranspose(XMMatrixIdentity()));
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f)));

	_float3 vScale = { m_fSizeX,m_fSizeY,0.f };

	m_pTransformCom->Set_Scale(XMLoadFloat3(&vScale));
	
	return S_OK;
}

void CMonsterBall::Tick(_float fTimeDelta)
{

}

void CMonsterBall::Late_Tick(_float fTimeDelta)
{
	if (nullptr != m_pRendererCom && m_bInven)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CMonsterBall::Render()
{
	if (nullptr == m_pShaderCom || nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonsterBall::Ready_Components()
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
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Item"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonsterBall::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_World4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_SRV(3))))
		return E_FAIL;

	m_pShaderCom->Begin();
	m_pVIBufferCom->Render();

	return S_OK;
}


CMonsterBall * CMonsterBall::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMonsterBall*	pInstance = new CMonsterBall(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CMonsterBall"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CMonsterBall::Clone(void * pArg)
{
	CGameObject*	pInstance = new CMonsterBall(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CMonsterBall"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonsterBall::Free()
{
	__super::Free();

}
