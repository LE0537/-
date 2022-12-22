#include "stdafx.h"
#include "..\Public\EvolLight2.h"

#include "GameInstance.h"
#include "Data_Manager.h"	// 추가

CEvolLight2::CEvolLight2(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
}

CEvolLight2::CEvolLight2(const CEvolLight2 & rhs)
	: CGameObj(rhs)
{
}

HRESULT CEvolLight2::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEvolLight2::Initialize(void * pArg)
{

	if (FAILED(Ready_Components()))
		return E_FAIL;
	
	m_fSizeX = 5.f;
	m_fSizeY = 5.f;

	m_fX = g_iWinSizeX >> 1;
	m_fY = g_iWinSizeY >> 1;

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixTranspose(XMMatrixIdentity()));
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, -500.f, 100.f)));

	_float3 vScale = { m_fSizeX,m_fSizeY,m_fSizeY };

	m_pTransformCom->Set_Scale(XMLoadFloat3(&vScale));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, -200.f, 1.f));

	return S_OK;
}

void CEvolLight2::Tick(_float fTimeDelta)
{
	//CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	//if (pGameInstance->Key_Down(DIK_F2))	// 추가 -> 저장하기
	//{
	//	CData_Manager* pData_Manager = GET_INSTANCE(CData_Manager);
	//	char cName[MAX_PATH];
	//	ZeroMemory(cName, sizeof(char) * MAX_PATH);
	//	pData_Manager->TCtoC(TEXT("Cross"), cName);
	//	pData_Manager->Conv_Bin_Model(m_pModelCom, cName, CData_Manager::DATA_NONANIM);
	//	ERR_MSG(TEXT("Save_Bin_Cross"));
	//	RELEASE_INSTANCE(CData_Manager);
	//}

	//RELEASE_INSTANCE(CGameInstance);

	//Set_Pos(fTimeDelta);


	if (!g_bEvolution)
		Set_Dead();
}

void CEvolLight2::Late_Tick(_float fTimeDelta)
{
	if (g_bEvolution && nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UIPOKE, this);
}

HRESULT CEvolLight2::Render()
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

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 1)))
			return E_FAIL;
	}


	return S_OK;
}

HRESULT CEvolLight2::Render_ShadowDepth()
{
	return E_NOTIMPL;
}

HRESULT CEvolLight2::Ready_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;


	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxUIModel"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_STATIC, TEXT("Prototype_Component_Model_Cross"), (CComponent**)&m_pModelCom)))
		return E_FAIL;
	


	return S_OK;
}

HRESULT CEvolLight2::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	
	if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);
	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_World4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;


	return S_OK;
}

void CEvolLight2::Set_Pos(_float fTimeDelta)
{

	m_fSizeX += 1.f * fTimeDelta;
	m_fSizeY += 1.f * fTimeDelta;

	_float3 vScale = { m_fSizeX,m_fSizeY,m_fSizeY };

	
	m_pTransformCom->Set_Scale(XMLoadFloat3(&vScale));

}




CEvolLight2 * CEvolLight2::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CEvolLight2*	pInstance = new CEvolLight2(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CEvolLight2"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CEvolLight2::Clone(void * pArg)
{
	CGameObject*	pInstance = new CEvolLight2(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CEvolLight2"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvolLight2::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
}
