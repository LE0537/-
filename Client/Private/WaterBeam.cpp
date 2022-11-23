#include "stdafx.h"
#include "..\Public\WaterBeam.h"

#include "GameInstance.h"


CWaterBeam::CWaterBeam(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
}

CWaterBeam::CWaterBeam(const CWaterBeam & rhs)
	: CGameObj(rhs)
{
}

HRESULT CWaterBeam::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWaterBeam::Initialize(void * pArg)
{
	m_SkillInfo.strName = TEXT("물대포");
	m_SkillInfo.strInfo = TEXT("강력한 물줄기로 상대를 공격한다.");
	m_SkillInfo.iSkillNum = 1;
	m_SkillInfo.iDmg = 0;
	m_SkillInfo.iSDmg = 40;
	m_SkillInfo.fHit = 100;
	m_SkillInfo.iMaxPoint = 25;
	m_SkillInfo.iPoint = m_SkillInfo.iMaxPoint;
	m_SkillInfo.eType = WATER;
	m_SkillInfo.bUseSkill = false;
	*(SKILLINFO**)pArg = &m_SkillInfo;


	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CWaterBeam::Tick(_float fTimeDelta)
{
	if (m_SkillInfo.bUseSkill)
	{
		Set_Pos(fTimeDelta);
		m_fDeadTime += fTimeDelta;
		if (m_fDeadTime > 2.f)
		{
			m_fDeadTime = 0.f;
			m_SkillInfo.bUseSkill = false;
			m_bSetPos = false;
		}
	}
}

void CWaterBeam::Late_Tick(_float fTimeDelta)
{
	if (m_SkillInfo.bUseSkill)
	{
		if (g_Battle && nullptr != m_pRendererCom)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
	}
}

HRESULT CWaterBeam::Render()
{
	if (m_SkillInfo.bUseSkill)
	{
		if (nullptr == m_pShaderCom)
			return E_FAIL;
		
		if (FAILED(SetUp_ShaderResources()))
			return E_FAIL;
		
		_uint		iNumMeshes = m_pModelCom->Get_NumMeshContainers();
		
		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			if (FAILED(m_pModelCom->SetUp_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
				return E_FAIL;
		
			if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 4)))
				return E_FAIL;
		}
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

		if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom2->Get_World4x4_TP(), sizeof(_float4x4))))
			return E_FAIL;
		RELEASE_INSTANCE(CGameInstance);
		_uint		iNumMeshes2 = m_pModelCom2->Get_NumMeshContainers();
		for (_uint i = 0; i < iNumMeshes2; ++i)
		{
			if (FAILED(m_pModelCom2->SetUp_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
				return E_FAIL;

			if (FAILED(m_pModelCom2->Render(m_pShaderCom, i, 4)))
				return E_FAIL;
		}

	}
	return S_OK;
}

HRESULT CWaterBeam::Ready_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Transform2"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom2, &TransformDesc)))
		return E_FAIL;
	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model*/

	if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_STATIC, TEXT("Prototype_Component_Model_Cylinder"), (CComponent**)&m_pModelCom)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Model2"), LEVEL_STATIC, TEXT("Prototype_Component_Model_Cone"), (CComponent**)&m_pModelCom2)))
		return E_FAIL;

	return S_OK;
}

HRESULT CWaterBeam::SetUp_ShaderResources()
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

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CWaterBeam * CWaterBeam::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CWaterBeam*	pInstance = new CWaterBeam(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CWaterBeam"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObj * CWaterBeam::Clone(void * pArg)
{
	CWaterBeam*	pInstance = new CWaterBeam(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CWaterBeam"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWaterBeam::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pModelCom2);
	Safe_Release(m_pTransformCom2);
}

void CWaterBeam::Set_Pos(_float fTimeDelta)
{
	if (!m_bSetPos)
	{
		_vector		vMyPos = XMLoadFloat4(&m_SkillInfo.vMyPos);
		_vector		vAt = XMLoadFloat4(&m_SkillInfo.vTargetPos);
		_vector		vDist = vAt - vMyPos;
		vMyPos += XMVector3Normalize(vDist) * 0.25f;
		vMyPos.m128_f32[1] += 1.f;
		vAt.m128_f32[1] += 1.f;
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vMyPos);
		m_pTransformCom2->Set_State(CTransform::STATE_TRANSLATION, vMyPos);

		_float3 vScale = { 0.3f,0.01f,0.3f };
		m_pTransformCom->Set_Scale(XMLoadFloat3(&vScale));
		vScale = { 1.f,1.f,1.f };
		m_pTransformCom2->Set_Scale(XMLoadFloat3(&vScale));

		m_pTransformCom->LookAt(vAt);
		m_pTransformCom2->LookAt(vAt);
		m_pTransformCom->Turn2(m_pTransformCom->Get_State(CTransform::STATE_RIGHT),XMConvertToRadians(90.f));
		m_pTransformCom2->Turn2(m_pTransformCom2->Get_State(CTransform::STATE_RIGHT), XMConvertToRadians(90.f));
	
		
		m_bSetPos = true;
	}
	_float3 vScaleUp = m_pTransformCom->Get_Scale();

	_vector		vRotUp = m_pTransformCom->Get_State(CTransform::STATE_UP);
	_vector		vRotUp2 = m_pTransformCom2->Get_State(CTransform::STATE_UP);
	vScaleUp.y +=  1.5f * fTimeDelta;

	if (vScaleUp.y > 0.8f)
		vScaleUp.y = 0.8f;

	m_pTransformCom->Turn(vRotUp, fTimeDelta * 2.f);
	m_pTransformCom2->Turn(vRotUp2, fTimeDelta * 3.f);
	m_pTransformCom->Set_Scale(XMLoadFloat3(&vScaleUp));
}
