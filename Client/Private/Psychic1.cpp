#include "stdafx.h"
#include "..\Public\Psychic1.h"

#include "GameInstance.h"
#include "Level_GamePlay.h"
#include "Data_Manager.h"	// 추가

CPsychic1::CPsychic1(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
}

CPsychic1::CPsychic1(const CPsychic1 & rhs)
	: CGameObj(rhs)
{
}

HRESULT CPsychic1::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPsychic1::Initialize(void * pArg)
{
	m_vMyBattlePos = ((SKILLINFO*)pArg)->vMyPos;
	m_vTargetBattlePos = ((SKILLINFO*)pArg)->vTargetPos;

	m_vMyBattlePos.y += 1.2f;
	m_vTargetBattlePos.y += 1.2f;

	_vector vMyPos = XMLoadFloat4(&m_vMyBattlePos);
	_vector vTargetPos = XMLoadFloat4(&m_vTargetBattlePos);


	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_fSize = 0.8f;

	_float3 vScale = { m_fSize,m_fSize,m_fSize };

	m_pTransformCom->Set_Scale(XMLoadFloat3(&vScale));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vTargetPos);


	return S_OK;
}

void CPsychic1::Tick(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Key_Down(DIK_F5))	// 추가 -> 저장하기
	{
		CData_Manager* pData_Manager = GET_INSTANCE(CData_Manager);
		char cName[MAX_PATH];
		ZeroMemory(cName, sizeof(char) * MAX_PATH);
		pData_Manager->TCtoC(TEXT("Psychic"), cName);
		pData_Manager->Conv_Bin_Model(m_pModelCom, cName, CData_Manager::DATA_NONANIM);
		ERR_MSG(TEXT("Save_Bin_Psychic"));
		RELEASE_INSTANCE(CData_Manager);
	}

	RELEASE_INSTANCE(CGameInstance);

	Set_Pos(fTimeDelta);


}

void CPsychic1::Late_Tick(_float fTimeDelta)
{

	if (g_Battle && nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);

}

HRESULT CPsychic1::Render()
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

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 7)))
			return E_FAIL;

	}

	return S_OK;
}

HRESULT CPsychic1::Ready_Components()
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
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model*/

	if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_STATIC, TEXT("Prototype_Component_Model_Psychic"), (CComponent**)&m_pModelCom)))
		return E_FAIL;


	return S_OK;
}

HRESULT CPsychic1::SetUp_ShaderResources()
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

void CPsychic1::Set_Pos(_float fTimeDelta)
{
	m_fSize -= 1.5f * fTimeDelta;

	_vector vScale = { m_fSize ,m_fSize ,m_fSize ,0.f };
	m_pTransformCom->Set_Scale(vScale);

	m_pTransformCom->Turn2(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(6.f));


	m_fDeadTime += fTimeDelta;
	if (m_fDeadTime > 0.6f)
	{
		Set_Dead();
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

		CLevel_GamePlay::LOADFILE tInfo;

		XMStoreFloat4(&tInfo.vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Psychic2"), LEVEL_GAMEPLAY, TEXT("Effect"), &tInfo)))
			return;

		for (_int i = 0; i < 100; ++i)
		{
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Psychic3"), LEVEL_GAMEPLAY, TEXT("Effect"), &tInfo)))
				return;
		}
		RELEASE_INSTANCE(CGameInstance);
	}
}

CPsychic1 * CPsychic1::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPsychic1*	pInstance = new CPsychic1(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CPsychic1"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObj * CPsychic1::Clone(void * pArg)
{
	CPsychic1*	pInstance = new CPsychic1(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CPsychic1"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPsychic1::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);

}
