#include "stdafx.h"
#include "..\Public\WaterGun1.h"

#include "GameInstance.h"
#include "Level_GamePlay.h"
#include "SoundMgr.h"

CWaterGun1::CWaterGun1(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
}

CWaterGun1::CWaterGun1(const CWaterGun1 & rhs)
	: CGameObj(rhs)
{
}

HRESULT CWaterGun1::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWaterGun1::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_fSize = 1.3f;
	_vector vScale = { m_fSize ,m_fSize ,m_fSize ,0.f };
	m_pTransformCom->Set_Scale(vScale);

	m_vMyBattlePos = ((SKILLINFO*)pArg)->vMyPos;
	m_vTargetBattlePos = ((SKILLINFO*)pArg)->vTargetPos;
	
	m_vMyBattlePos.y += 1.f;
	m_vTargetBattlePos.y += 1.f;

	_vector vMyPos = XMLoadFloat4(&m_vMyBattlePos);
	_vector vTargetPos = XMLoadFloat4(&m_vTargetBattlePos);
	_vector		vDist = vTargetPos - vMyPos;
	XMVector3Normalize(vDist);

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vMyPos);
	XMStoreFloat4(&m_vPos1, vMyPos);

	vMyPos.m128_f32[1] += 1.f;
	vMyPos += vDist * 0.25f;
	XMStoreFloat4(&m_vPos2, vMyPos);

	vMyPos.m128_f32[1] += 0.3f;
	vMyPos += vDist * 0.25f;
	XMStoreFloat4(&m_vPos3, vMyPos);

	CSoundMgr::Get_Instance()->PlayEffect(TEXT("WaterGun.mp3"), 1.f);



	return S_OK;
}

void CWaterGun1::Tick(_float fTimeDelta)
{
	Set_Pos(fTimeDelta);

}

void CWaterGun1::Late_Tick(_float fTimeDelta)
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

HRESULT CWaterGun1::Render()
{

	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;


	return S_OK;
}
HRESULT CWaterGun1::Render_ShadowDepth()
{
	return E_NOTIMPL;
}
HRESULT CWaterGun1::Ready_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_WaterGun1"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;


	return S_OK;
}

void CWaterGun1::OnBillboard()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_matrix		ViewMatrix = XMMatrixInverse(nullptr, pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));

	_vector vScale = XMLoadFloat3(&m_pTransformCom->Get_Scale());

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, ViewMatrix.r[0] * vScale.m128_f32[0]);

	m_pTransformCom->Set_State(CTransform::STATE_UP, ViewMatrix.r[1] * vScale.m128_f32[1]);

	m_pTransformCom->Set_State(CTransform::STATE_LOOK, ViewMatrix.r[2] * vScale.m128_f32[2]);

	RELEASE_INSTANCE(CGameInstance);
}

void CWaterGun1::Set_Pos(_float fTimeDelta)
{
	m_fSpeed += 3.5f * fTimeDelta;

	_vector vPos = XMVectorCatmullRom(XMLoadFloat4(&m_vPos1), XMLoadFloat4(&m_vPos2)
		, XMLoadFloat4(&m_vPos3), XMLoadFloat4(&m_vTargetBattlePos), m_fSpeed);

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vPos);

	m_fFrameTime += fTimeDelta;
	if (m_fFrameTime > 0.06f)
	{
		++m_iFrame;
		m_fFrameTime = 0.f;
	}
	if (m_iFrame == 7 && !m_bSkill)
	{
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

		CLevel_GamePlay::LOADFILE tInfo;
		XMStoreFloat4(&tInfo.vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
		tInfo.vScale = { 1.f,0.f,0.f };
		for (_int i = 0; i < 30; ++i)
		{
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_WaterGun3"), LEVEL_GAMEPLAY, TEXT("Layer_Effect"), &tInfo)))
				return;
		}
		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_WaterGun2"), LEVEL_GAMEPLAY, TEXT("Layer_Effect"), &tInfo)))
			return;

		RELEASE_INSTANCE(CGameInstance);
		m_bSkill = true;
	}
	if (m_iFrame > 7)
	{
		Set_Dead();
	}
}

HRESULT CWaterGun1::SetUp_ShaderResources()
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

CWaterGun1 * CWaterGun1::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CWaterGun1*	pInstance = new CWaterGun1(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CWaterGun1"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CWaterGun1::Clone(void * pArg)
{
	CGameObject*	pInstance = new CWaterGun1(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CWaterGun1"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWaterGun1::Free()
{
	__super::Free();


}
