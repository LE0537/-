#include "stdafx.h"
#include "..\Public\Field.h"

#include "GameInstance.h"
#include "Level_GamePlay.h"
#include "Data_Manager.h"	// Ãß°¡
#include "Cell.h"
#include "SoundMgr.h"


CField::CField(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
}

CField::CField(const CField & rhs)
	: CGameObj(rhs)
{
}

HRESULT CField::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CField::Initialize(void * pArg)
{

	 
	if (FAILED(Ready_Components()))
		return E_FAIL;



	m_pTransformCom->Set_Scale(XMLoadFloat3((&((CLevel_GamePlay::LOADFILE*)pArg)->vScale)));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4((&((CLevel_GamePlay::LOADFILE*)pArg)->vPos)));
	return S_OK;
}

void CField::Tick(_float fTimeDelta)
{
	//CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	//if(pGameInstance->Key_Down(DIK_F2))
	//{ 
	//	g_bEnding = !g_bEnding;
	//}
	//RELEASE_INSTANCE(CGameInstance);
	if (g_bEnding)
	{
		if (m_iEndinCell >= 3)
		{
			Ending(fTimeDelta);
			return;
		}
		if (m_bPicking)
		{
			m_pModelCom->Picking(m_pTransformCom, &m_vMousePos);
			
			Check_Range();
			
			switch (m_iCheckPoints)
			{
			case 0:
				vPoints[0] = m_vMousePos;
				++m_iCheckPoints;
				break;
			case 1:
				vPoints[1] = m_vMousePos;
				++m_iCheckPoints;
				break;
			case 2:
				vPoints[2] = m_vMousePos;
				m_iCheckPoints = 0;
				Sort_Cell();
				Create_Cell();
				break;
			default:
				break;
			}
			m_bPicking = false;
		}
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
		if (pGameInstance->Mouse_Down(DIMK_RBUTTON))
		{
			m_bPicking = true;
		}
		RELEASE_INSTANCE(CGameInstance);
	}
}

void CField::Late_Tick(_float fTimeDelta)
{
	if (!g_Battle)
	{
		if (!g_bEvolution && !g_bBag && !g_PokeInfo && !g_bPokeDeck && nullptr != m_pRendererCom)
		{
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
		}
		if (g_CollBox)
		{
			m_pRendererCom->Add_Debug(m_pNavigationCom);
		}

	}
}

HRESULT CField::Render()
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
		if (i == 150 || i == 151 || i == 152 || i == 153)
		{
			if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 5)))
				return E_FAIL;
		}
		else
		{
			if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 1)))
				return E_FAIL;
		}
	}

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}
HRESULT CField::Ready_Components()
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
	if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_STATIC, TEXT("Field"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Navigation */
	if (FAILED(__super::Add_Components(TEXT("Com_Navigation"), LEVEL_STATIC, TEXT("Prototype_Component_Navigation"), (CComponent**)&m_pNavigationCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CField::Create_Cell()
{
	CCell*			pCell = CCell::Create(m_pDevice, m_pContext, vPoints, m_pNavigationCom->Get_Cells()->size());
	if (nullptr == pCell)
		return E_FAIL;

	
	m_pNavigationCom->Get_Cells()->push_back(pCell);
	
	++m_iEndinCell;

	return S_OK;
}
void CField::Check_Range()
{
	_vector vPos;
	_vector vMousePos = XMLoadFloat3(&m_vMousePos);
	_float fDist = 0.f;
	_float fDist2 = 1.5f;
	for (auto& Cell : *m_pNavigationCom->Get_Cells())
	{
		vPos = XMLoadFloat3(Cell->Get_Point(CCell::POINT_A));
		fDist = XMVectorGetX(XMVector3Length(vPos - vMousePos));
		if (fDist2 > fDist)
			XMStoreFloat3(&m_vMousePos, vPos);
		vPos = XMLoadFloat3(Cell->Get_Point(CCell::POINT_B));
		fDist = XMVectorGetX(XMVector3Length(vPos - vMousePos));
		if (fDist2 > fDist)
			XMStoreFloat3(&m_vMousePos, vPos);
		vPos = XMLoadFloat3(Cell->Get_Point(CCell::POINT_C));
		fDist = XMVectorGetX(XMVector3Length(vPos - vMousePos));
		if (fDist2 > fDist)
			XMStoreFloat3(&m_vMousePos, vPos);
	}

}
void CField::Sort_Cell()
{

	_vector vABDir = XMLoadFloat3(&vPoints[1]) - XMLoadFloat3(&vPoints[0]);
	_vector vBCDir = XMLoadFloat3(&vPoints[2]) - XMLoadFloat3(&vPoints[1]);
	_vector vCloss = XMVector3Cross(vABDir, vBCDir);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_vector vMouseDir = pGameInstance->Get_RayDir() * -1.f;
	vCloss = XMVector3Normalize(vCloss);
	vMouseDir = XMVector3Normalize(vMouseDir);

	if (0.f > XMVectorGetX(XMVector3Dot(vCloss, vMouseDir)))
	{
		_float3 vTemp = vPoints[0];
		vPoints[0] = vPoints[2];
		vPoints[2] = vTemp;
	}
	RELEASE_INSTANCE(CGameInstance);

}
HRESULT CField::Ending(_float fTimeDelta)
{
	if (!m_bEnding)
	{
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_EndingErr"), LEVEL_GAMEPLAY, TEXT("Layer_UI"))))
			return E_FAIL;

		RELEASE_INSTANCE(CGameInstance);
		CSoundMgr::Get_Instance()->BGM_Stop();
		CSoundMgr::Get_Instance()->PlayEffect(TEXT("Err.wav"), 1.f);
		m_bEnding = true;
		m_fDeadtime = 0.f;
	}

	if (m_bEnding && !m_bDeadSound)
	{
		m_fDeadtime += fTimeDelta;
		if (m_fDeadtime > 3.f)
		{
			CSoundMgr::Get_Instance()->PlayEffect(TEXT("DeadEnding.wav"), 1.f);
			m_bDeadSound = true;
		}
	}

	return S_OK;
}
HRESULT CField::SetUp_ShaderResources()
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

CField * CField::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CField*	pInstance = new CField(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CField"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CField::Clone(void * pArg)
{
	CGameObject*	pInstance = new CField(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CField"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CField::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pModelCom);
}
