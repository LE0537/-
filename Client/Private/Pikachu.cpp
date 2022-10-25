#include "stdafx.h"
#include "..\Public\Pikachu.h"
#include "GameInstance.h"


CPikachu::CPikachu(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
}

CPikachu::CPikachu(const CPikachu & rhs)
	: CGameObj(rhs)
{
}

HRESULT CPikachu::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPikachu::Initialize(void * pArg)
{

	*(CGameObject**)pArg = this;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_PokemonInfo.strName = TEXT("피카츄");
	m_PokemonInfo.strInfo = TEXT("볼에 전기를 저장하는 주머니가 있다. 숲을 거처로 삼으며,\n 단단한 나무열매를 전기로 익혀 먹는 등의 지혜를 가졌다.");
	m_PokemonInfo.strChar = TEXT("온순함");
	m_PokemonInfo.iPokeNum = 25;
	m_PokemonInfo.iMaxHp = 200;
	m_PokemonInfo.iHp = m_PokemonInfo.iMaxHp;
	m_PokemonInfo.iDmg = 70;
	m_PokemonInfo.iSDmg = 100;
	m_PokemonInfo.iDef = 50;
	m_PokemonInfo.iSDef = 70;
	m_PokemonInfo.iSpeed = 140;
	m_PokemonInfo.iLv = 5;
	m_PokemonInfo.iMaxExp = 20;
	m_PokemonInfo.iExp = 0;
	m_PokemonInfo.iSex = 0;
	m_PokemonInfo.iBallNum = 0;
	m_PokemonInfo.bRide = false;
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Tackle"), LEVEL_STATIC, TEXT("Layer_Skill"), &m_PokemonInfo.eSkillNum1)))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_NoneSkill"), LEVEL_STATIC, TEXT("Layer_Skill"), &m_PokemonInfo.eSkillNum2)))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_NoneSkill"), LEVEL_STATIC, TEXT("Layer_Skill"), &m_PokemonInfo.eSkillNum3)))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_NoneSkill"), LEVEL_STATIC, TEXT("Layer_Skill"), &m_PokemonInfo.eSkillNum4)))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_None"), LEVEL_STATIC, TEXT("Layer_Item"), &m_PokemonInfo.eItem)))
		return E_FAIL;
	
	
	RELEASE_INSTANCE(CGameInstance);

	

	m_PokemonInfo.eType = THUNDER;
	m_PokemonInfo.eType2 = POKETYPE_END;
	m_PokemonInfo.eStatInfo = STATINFO_END;

	m_pModelCom->Set_CurrentAnimIndex(2);

	return S_OK;
}

void CPikachu::Tick(_float fTimeDelta)
{
	if (m_bOnOff)
	{
		Set_DeckPos();
		if (!m_bDeckInfo)
		{
			Key_Input(fTimeDelta);
		}
	}
	m_pModelCom->Play_Animation(fTimeDelta);
}

void CPikachu::Late_Tick(_float fTimeDelta)
{
	if((g_PokeInfo || g_bPokeDeck) && m_bOnOff && nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UIPOKE, this);
//	else if (!g_PokeInfo && !g_bPokeDeck && nullptr != m_pRendererCom)
	//	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CPikachu::Render()
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
		if ((g_PokeInfo || g_bPokeDeck))
		{
			if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 1)))
				return E_FAIL;
		}
		else
		{
			if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 0)))
				return E_FAIL;
		}
	}

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}
HRESULT CPikachu::Ready_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model*/
	if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_STATIC, TEXT("Prototype_Component_Model_Pikachu"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}
void CPikachu::Set_DeckPos()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	
	_vector vPos = XMLoadFloat4(&pGameInstance->Get_CamPosition());
	vPos.m128_f32[3] = 1.f;
	_vector vRight = XMLoadFloat4(&pGameInstance->Get_CamRight());
	_vector vUp = XMLoadFloat4(&pGameInstance->Get_CamUp());
	_vector vLook = XMLoadFloat4(&pGameInstance->Get_CamLook());


	XMVector4Normalize(vRight);
	XMVector4Normalize(vUp);
	XMVector4Normalize(vLook);
	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight);
	m_pTransformCom->Set_State(CTransform::STATE_UP, vUp);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook);

	if (m_bDeckInfo)
	{
		vRight *= -10.f;
		vUp *= 15.f;
		vLook *= 30.f;
		vPos += vRight;
		vPos += vUp;
		vPos += vLook;

		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vPos);
		m_pTransformCom->Turn2(m_pTransformCom->Get_State(CTransform::STATE_LOOK), XMConvertToRadians(90.f));
		m_pModelCom->Set_CurrentAnimIndex(2);
	}
	else if (!m_bDeckInfo)
	{
		vRight *= -10.f;
		vUp *= -17.f;
		vLook *= 30.f;
		vPos += vRight;
		vPos += vUp;
		vPos += vLook;

		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vPos);
		m_pTransformCom->Turn2(m_pTransformCom->Get_State(CTransform::STATE_LOOK), XMConvertToRadians(90.f));
		
	}
	
	RELEASE_INSTANCE(CGameInstance);
}
void CPikachu::Key_Input(_float fTimeDelta)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Key_Down(DIK_W))
	{
		++m_iAnim;
		if (m_iAnim > 9)
			m_iAnim = 9;
	
		m_pModelCom->Set_CurrentAnimIndex(m_iAnim);
	}
	else if(pGameInstance->Key_Down(DIK_S))
	{
		--m_iAnim;
		if (m_iAnim < 0)
			m_iAnim = 0;

		m_pModelCom->Set_CurrentAnimIndex(m_iAnim);
	}
	else if (pGameInstance->Key_Pressing(DIK_A))
	{
		m_pTransformCom->Turn2(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(130.f));
	}
	else if (pGameInstance->Key_Pressing(DIK_D))
	{
		m_pTransformCom->Turn2(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(-30.f));
	}

	RELEASE_INSTANCE(CGameInstance);
}
HRESULT CPikachu::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_World4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CPikachu * CPikachu::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPikachu*	pInstance = new CPikachu(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CPikachu"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CPikachu::Clone(void * pArg)
{
	CGameObject*	pInstance = new CPikachu(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CPikachu"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPikachu::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
}
