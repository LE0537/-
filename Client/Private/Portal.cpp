#include "stdafx.h"
#include "..\Public\Portal.h"

#include "GameInstance.h"
#include "Level_GamePlay.h"
#include "Data_Manager.h"	// 추가
#include "Player.h"
#include "SoundMgr.h"

CPortal::CPortal(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
}

CPortal::CPortal(const CPortal & rhs)
	: CGameObj(rhs)
{
}

HRESULT CPortal::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPortal::Initialize(void * pArg)
{



	if (FAILED(Ready_Components()))
		return E_FAIL;

	_vector vPos = XMLoadFloat4((&((CLevel_GamePlay::LOADFILE*)pArg)->vPos));

	if (vPos.m128_f32[0] < 100.f)
	{
		m_pTransformCom->Turn2(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-90.f));
		m_iPortalIndex = 0;

	}
	else
	{
		m_pTransformCom->Turn2(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f));
		m_iPortalIndex = 1;
	}

	m_pTarget = *(&((CLevel_GamePlay::LOADFILE*)pArg)->pTarget);

	m_pTransformCom->Set_Scale(XMLoadFloat3((&((CLevel_GamePlay::LOADFILE*)pArg)->vScale)));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vPos);


	return S_OK;
}

void CPortal::Tick(_float fTimeDelta)
{
	//CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	//if (pGameInstance->Key_Down(DIK_F6))	// 추가 -> 저장하기
	//{
	//	CData_Manager* pData_Manager = GET_INSTANCE(CData_Manager);
	//	char cName[MAX_PATH];
	//	ZeroMemory(cName, sizeof(char) * MAX_PATH);
	//	pData_Manager->TCtoC(TEXT("Portal"), cName);
	//	pData_Manager->Conv_Bin_Model(m_pModelCom, cName, CData_Manager::DATA_NONANIM);
	//	ERR_MSG(TEXT("Save_Bin_Portal"));
	//	RELEASE_INSTANCE(CData_Manager);
	//}

	//RELEASE_INSTANCE(CGameInstance);


	m_pOBBCom->Update(m_pTransformCom->Get_WorldMatrix());
}

void CPortal::Late_Tick(_float fTimeDelta)
{
	if (!g_Battle)
	{
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
		if (!g_Battle && !g_bEvolution)
			Check_Coll();
		if (pGameInstance->IsInFrustum(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION), 10.f))
		{
			if (!g_bRace && !g_Battle && !g_bEvolution && !g_bBag && !g_PokeInfo && !g_bPokeDeck && nullptr != m_pRendererCom)
			{
				_vector vTargetPos = dynamic_cast<CGameObj*>(m_pTarget)->Get_Transfrom()->Get_State(CTransform::STATE_TRANSLATION);
				_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
				_vector vLook = vTargetPos - vPos;

				_float fDist = XMVectorGetX(XMVector3Length(vLook));

				if (fDist < 30.f)
					m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOWDEPTH, this);

				if (fDist < 500.f)
					m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
			}
		}
		RELEASE_INSTANCE(CGameInstance);
		if (g_CollBox)
		{
			m_pRendererCom->Add_Debug(m_pOBBCom);
		}
	}
}

HRESULT CPortal::Render()
{

	if (nullptr == m_pShaderCom ||
		nullptr == m_pModelCom)
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshContainers();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 2)))
			return E_FAIL;

	}

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}
HRESULT CPortal::Render_ShadowDepth()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pModelCom)
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_World4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;

	_vector			vLightEye = XMLoadFloat4(&pGameInstance->Get_ShadowLightDesc(LIGHTDESC::TYPE_FIELDSHADOW)->vDirection);
	_vector			vLightAt = XMLoadFloat4(&pGameInstance->Get_ShadowLightDesc(LIGHTDESC::TYPE_FIELDSHADOW)->vDiffuse);
	_vector			vLightUp = { 0.f, 1.f, 0.f ,0.f };
	_matrix			matLightView = XMMatrixLookAtLH(vLightEye, vLightAt, vLightUp);

	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixTranspose(matLightView), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;



	_uint		iNumMeshes = m_pModelCom->Get_NumMeshContainers();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 8)))
			return E_FAIL;

	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}
void CPortal::Check_Coll()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);


	CCollider*	pTargetCollider = (CCollider*)pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_OBB"));
	if (nullptr == pTargetCollider)
		return;

	if (m_pOBBCom->Collision(pTargetCollider))
	{
		if (!m_bButton)
		{
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_PortalButton"), LEVEL_GAMEPLAY, TEXT("Layer_UI"),&m_pButton)))
				return;
			m_bButton = true;
		}
		if (pGameInstance->Key_Down(DIK_F))
		{
			dynamic_cast<CPlayer*>(m_pTarget)->Find_Navi(m_iPortalIndex);
			CSoundMgr::Get_Instance()->PlayEffect(TEXT("Portal.mp3"),1.f);

			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_PortalUI"), LEVEL_GAMEPLAY, TEXT("Layer_UI"))))
				return;

			m_bButton = false;
			m_pButton->Set_Dead();
		}
	
	}
	else
	{
		if (m_bButton)
		{
			m_bButton = false;
			m_pButton->Set_Dead();
		}
	}
	RELEASE_INSTANCE(CGameInstance);
}
HRESULT CPortal::Ready_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 10.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model*/
	if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_STATIC, TEXT("Portal"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC		ColliderDesc;

	/* For.Com_OBB*/
	ColliderDesc.vScale = _float3(14.f, 10.f, 8.f);
	ColliderDesc.vPosition = _float3(0.f, 5.f, 0.f);
	if (FAILED(__super::Add_Components(TEXT("Com_OBB"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"), (CComponent**)&m_pOBBCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}


HRESULT CPortal::SetUp_ShaderResources()
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

CPortal * CPortal::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPortal*	pInstance = new CPortal(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CPortal"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CPortal::Clone(void * pArg)
{
	CGameObject*	pInstance = new CPortal(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CPortal"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPortal::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pOBBCom);
}
