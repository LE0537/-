#include "stdafx.h"
#include "..\Public\Bite1.h"

#include "GameInstance.h"
#include "Level_GamePlay.h"
#include "Data_Manager.h"	// 추가

CBite1::CBite1(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
}

CBite1::CBite1(const CBite1 & rhs)
	: CGameObj(rhs)
{
}

HRESULT CBite1::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBite1::Initialize(void * pArg)
{
	m_vMyBattlePos = ((SKILLINFO*)pArg)->vMyPos;
	m_vTargetBattlePos = ((SKILLINFO*)pArg)->vTargetPos;

	m_vMyBattlePos.y += 1.2f;
	m_vTargetBattlePos.y += 1.2f;

	_vector vMyPos = XMLoadFloat4(&m_vMyBattlePos);
	_vector vTargetPos = XMLoadFloat4(&m_vTargetBattlePos);
	_vector		vDist = vTargetPos - vMyPos;
	XMVector3Normalize(vDist);

	if (FAILED(Ready_Components()))
		return E_FAIL;


	_float3 vScale = { 0.2f,0.2f,0.2f };

	m_pTransformCom->Set_Scale(XMLoadFloat3(&vScale));
	vTargetPos -= vDist * 0.25f;
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vTargetPos);
	vTargetPos += vDist * 0.5f;
	m_pTransformCom->LookAt(vTargetPos);
	m_pTransformCom->Turn2(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(180.f));

	m_pModelCom->Set_CurrentAnimIndex(0);

	return S_OK;
}

void CBite1::Tick(_float fTimeDelta)
{
	//CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	//if (pGameInstance->Key_Down(DIK_F6))	// 추가 -> 저장하기
	//{
	//	CData_Manager* pData_Manager = GET_INSTANCE(CData_Manager);
	//	char cName[MAX_PATH];
	//	ZeroMemory(cName, sizeof(char) * MAX_PATH);
	//	pData_Manager->TCtoC(TEXT("Bite"), cName);
	//	pData_Manager->Conv_Bin_Model(m_pModelCom, cName, CData_Manager::DATA_ANIM);
	//	ERR_MSG(TEXT("Save_Bin_Bite"));
	//	RELEASE_INSTANCE(CData_Manager);
	//}

	//RELEASE_INSTANCE(CGameInstance);

	m_pModelCom->Play_Animation(fTimeDelta * 1.5f);

	m_fDeadTime += fTimeDelta;
	if (m_fDeadTime > 1.5f)
	{
		Set_Dead();

	}
}

void CBite1::Late_Tick(_float fTimeDelta)
{

	if (g_Battle && nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);

}

HRESULT CBite1::Render()
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

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 2)))
			return E_FAIL;

	}

	return S_OK;
}

HRESULT CBite1::Render_ShadowDepth()
{
	return E_NOTIMPL;
}

HRESULT CBite1::Ready_Components()
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

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model*/

	if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_STATIC, TEXT("Bite"), (CComponent**)&m_pModelCom)))
		return E_FAIL;


	return S_OK;
}

HRESULT CBite1::SetUp_ShaderResources()
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

CBite1 * CBite1::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBite1*	pInstance = new CBite1(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CBite1"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObj * CBite1::Clone(void * pArg)
{
	CBite1*	pInstance = new CBite1(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CBite1"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBite1::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);

}
