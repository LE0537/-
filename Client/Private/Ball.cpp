#include "stdafx.h"
#include "..\Public\Ball.h"

#include "GameInstance.h"


CBall::CBall(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CBall::CBall(const CBall & rhs)
	: CGameObject(rhs)
{
}

HRESULT CBall::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBall::Initialize(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_WeaponDesc, pArg, sizeof(WEAPONDESC));

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(XMVectorSet(0.15f,0.15f,0.15f,0.f));

	m_pTransformCom->Turn2(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(-90.0f));
	m_pTransformCom->Turn2(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(180.0f));
	return S_OK;
}

void CBall::Tick(_float fTimeDelta)
{
	if (m_bRender)
	{
		
		_matrix		SocketMatrix = m_WeaponDesc.pSocket->Get_CombinedTransformationMatrix() *
			XMLoadFloat4x4(&m_WeaponDesc.SocketPivotMatrix) * XMLoadFloat4x4(m_WeaponDesc.pParentWorldMatrix);

		SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
		SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
		SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);


		XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix() * SocketMatrix);

	}
}

void CBall::Late_Tick(_float fTimeDelta)
{


}

HRESULT CBall::Render()
{
	if (m_bRender)
	{
		if (nullptr == m_pShaderCom)
			return E_FAIL;

		if (FAILED(SetUp_ShaderResources()))
			return E_FAIL;

		_uint		iNumMeshes = m_pModelCom[m_iBallIndex]->Get_NumMeshContainers();

		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			if (FAILED(m_pModelCom[m_iBallIndex]->SetUp_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
				return E_FAIL;

			if (FAILED(m_pModelCom[m_iBallIndex]->Render(m_pShaderCom, i, 0)))
				return E_FAIL;
		}

	}
	return S_OK;
}

HRESULT CBall::Ready_Components()
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
	wstring szModel[4];
	wstring szBall[4];
	szBall[0] = TEXT("MonsterBall");
	szBall[1] = TEXT("SuperBall");
	szBall[2] = TEXT("HyperBall");
	szBall[3] = TEXT("MasterBall");
	for (int i = 0; i < 4; ++i)
	{
		szModel[i] = TEXT("Com_ModelCom");
	}
	for (int i = 0; i < 4; ++i)
	{
		szModel[i] += to_wstring(i);
		if (FAILED(__super::Add_Components(szModel[i].c_str(), LEVEL_STATIC, szBall[i].c_str(), (CComponent**)&m_pModelCom[i])))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CBall::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	_float4x4		WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_CombinedWorldMatrix)));

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}


CBall * CBall::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBall*	pInstance = new CBall(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CBall"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CBall::Clone(void * pArg)
{
	CBall*	pInstance = new CBall(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CBall"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBall::Free()
{
	__super::Free();

	Safe_Release(m_WeaponDesc.pSocket);
	for (int i = 0; i < 4; ++i)
	{
		Safe_Release(m_pModelCom[i]);

	}
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
