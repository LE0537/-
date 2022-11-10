#include "stdafx.h"
#include "..\Public\Pikachu.h"
#include "GameInstance.h"
#include "Level_GamePlay.h"

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

	Set_Stats();

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

	m_pTransformCom->Set_Scale(XMLoadFloat3((&((CLevel_GamePlay::LOADFILE*)pArg)->vScale)));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4((&((CLevel_GamePlay::LOADFILE*)pArg)->vPos)));

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
	if (g_PokeInfo || g_bPokeDeck)
		m_pModelCom->Play_Animation(fTimeDelta);
	if(!m_bOnOff)
		m_bSetPos = false;

	if (m_bBattleMap)
	{
		Battle(fTimeDelta);
	}
}

void CPikachu::Late_Tick(_float fTimeDelta)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	
	if (pGameInstance->IsInFrustum(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION), m_pTransformCom->Get_Scale()))
	{

	}
	if ((g_PokeInfo || g_bPokeDeck) && m_bOnOff && nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UIPOKE, this);
	else if (m_bBattleMap && g_Battle && nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	RELEASE_INSTANCE(CGameInstance);
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
	
		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 0)))
			return E_FAIL;
		
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

	if (m_bDeckInfo)
	{
		if (!m_bSetPos)
		{
			_vector		vLook = { -0.3f,0.f,-1.f,0.f };

			_vector		vAxisY = XMVectorSet(0.f, 1.f, 0.f, 0.f);

			_vector		vRight = XMVector3Cross(vAxisY, vLook);

			_vector		vUp = XMVector3Cross(vLook, vRight);

			m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMVector3Normalize(vRight));
			m_pTransformCom->Set_State(CTransform::STATE_UP, XMVector3Normalize(vUp));
			m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVector3Normalize(vLook));
			m_bSetPos = true;
		}

		m_fSizeX = 20.f;
		m_fSizeY = 20.f;
		m_fX = 1000;
		m_fY = 550;

		XMStoreFloat4x4(&m_ViewMatrix, XMMatrixTranspose(XMMatrixIdentity()));
		XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, -500.f, 100.f)));
		_float3 vScale = { m_fSizeX,m_fSizeY,20.f };
		m_pTransformCom->Set_Scale(XMLoadFloat3(&vScale));
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, -200.f, 1.f));

	}
	else if (!m_bDeckInfo)
	{
		if (!m_bSetPos)
		{
			_vector		vLook = { 0.f,0.f,-1.f,0.f };

			_vector		vAxisY = XMVectorSet(0.f, 1.f, 0.f, 0.f);

			_vector		vRight = XMVector3Cross(vAxisY, vLook);

			_vector		vUp = XMVector3Cross(vLook, vRight);

			m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMVector3Normalize(vRight));
			m_pTransformCom->Set_State(CTransform::STATE_UP, XMVector3Normalize(vUp));
			m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVector3Normalize(vLook));
			m_bSetPos = true;
		}

		m_fSizeX =	20.f;
		m_fSizeY = 20.f;
		m_fX = 280;
		m_fY = 550;

		XMStoreFloat4x4(&m_ViewMatrix, XMMatrixTranspose(XMMatrixIdentity()));
		XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, -500.f, 100.f)));
		_float3 vScale = { m_fSizeX,m_fSizeY,20.f };
		m_pTransformCom->Set_Scale(XMLoadFloat3(&vScale));
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, -200.f, 1.f));

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
		m_pTransformCom->Turn2(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(3.f));
	}
	else if (pGameInstance->Key_Pressing(DIK_D))
	{
		m_pTransformCom->Turn2(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(-3.f));
	}

	RELEASE_INSTANCE(CGameInstance);
}
void CPikachu::Battle(_float fTimeDelta)
{
	if (!m_bBattle)
	{
		m_pTransformCom->Set_Scale(XMVectorSet(0.075f, 0.075f, 0.075f, 0.f));
		m_fStartBattle += fTimeDelta;
		if (m_iAnimIndex == 0)
		{
			m_pModelCom->Set_Loop(m_iAnimIndex);
			m_pModelCom->Set_CurrentAnimIndex(m_iAnimIndex);
		}
		if (!m_bBrath && (m_fStartBattle > m_fBattleMapTime) && m_pModelCom->Get_End(m_iAnimIndex))
		{
			m_pModelCom->Set_End(m_iAnimIndex);
			m_iAnimIndex = 1;
			m_pModelCom->Set_Loop(m_iAnimIndex);
			m_pModelCom->Set_CurrentAnimIndex(m_iAnimIndex);
			m_bBrath = true;
		}
		if (m_bBattleMap)
		{
			if (m_bBrath && m_pModelCom->Get_End(m_iAnimIndex))
			{
				m_pModelCom->Set_End(m_iAnimIndex);
				m_iAnimIndex = 2;
				m_pModelCom->Set_CurrentAnimIndex(m_iAnimIndex);
				m_bBattle = true;
			}

		}
	}
	if (m_bAttack && m_pModelCom->Get_End(m_iAnimIndex))
	{
		m_pModelCom->Set_End(m_iAnimIndex);
		m_iAnimIndex = 2;
		m_pModelCom->Set_CurrentAnimIndex(m_iAnimIndex);
		m_bAttack = false;
	}
	if (m_iAnimIndex == 3 || m_iAnimIndex == 4)
	{
		m_pModelCom->Set_Loop(m_iAnimIndex);
		m_pModelCom->Set_CurrentAnimIndex(m_iAnimIndex);
		m_bAttack = true;
	}
	if (m_bHit && m_pModelCom->Get_End(m_iAnimIndex))
	{
		m_pModelCom->Set_End(m_iAnimIndex);
		m_iAnimIndex = 2;
		m_pModelCom->Set_CurrentAnimIndex(m_iAnimIndex);
		m_bHit = false;
	}
	if (m_iAnimIndex == 5)
	{
		m_pModelCom->Set_Loop(m_iAnimIndex);
		m_pModelCom->Set_CurrentAnimIndex(m_iAnimIndex);
		m_bHit = true;
	}
	if (m_bDown && m_pModelCom->Get_End(m_iAnimIndex))
	{
		m_pModelCom->Set_End(m_iAnimIndex);
		m_bStopAnim = true;
	}
	if (!m_bDown && m_iAnimIndex == 6)
	{
		m_pModelCom->Set_Loop(m_iAnimIndex);
		m_pModelCom->Set_CurrentAnimIndex(m_iAnimIndex);
		m_bDown = true;
	}

	if (!m_bStopAnim)
		m_pModelCom->Play_Animation(fTimeDelta * 1.1f);
}
void CPikachu::Set_Stats()
{
	_float fHp = 35.f;
	_float fDmg = 55.f;
	_float fDef = 40.f;
	_float fSDmg = 50.f;
	_float fSDef = 50.f;
	_float fSpeed = 90.f;

	m_PokemonInfo.strName = TEXT("피카츄");
	m_PokemonInfo.strInfo = TEXT("재벌3세 석카츄...삼성게임즈를 위하여!\n최근 나이키 지디포스 화이트사고 자존감업됨.\n제발 코딩력도 업되자.");
	m_PokemonInfo.strChar = TEXT("부유함");
	m_PokemonInfo.iPokeNum = 25;
	m_PokemonInfo.iLv = 5;
	m_PokemonInfo.iMaxHp = _int(((fHp * 2.f) + 31.f + 100) * (m_PokemonInfo.iLv / 100.f) + 10.f);
	m_PokemonInfo.iHp = m_PokemonInfo.iMaxHp;
	m_PokemonInfo.iDmg = _int(((fDmg * 2.f) + 31.f) * (m_PokemonInfo.iLv / 100.f) + 5.f);
	m_PokemonInfo.iSDmg = _int(((fSDmg * 2.f) + 31.f) * (m_PokemonInfo.iLv / 100.f) + 5.f);
	m_PokemonInfo.iDef = _int(((fDef * 2.f) + 31.f) * (m_PokemonInfo.iLv / 100.f) + 5.f);
	m_PokemonInfo.iSDef = _int(((fSDef * 2.f) + 31.f) * (m_PokemonInfo.iLv / 100.f) + 5.f);
	m_PokemonInfo.iSpeed = _int(((fSpeed * 2.f) + 31.f) * (m_PokemonInfo.iLv / 100.f) + 5.f);
	m_PokemonInfo.iMaxExp = 20;
	m_PokemonInfo.iExp = 0;
	m_PokemonInfo.iSex = rand() % 2;
	m_PokemonInfo.iBallNum = 0;
	m_PokemonInfo.bRide = false;
	m_PokemonInfo.bEvolution = true;
}
HRESULT CPikachu::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	if (!m_bOnOff)
	{
		if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_World4x4_TP(), sizeof(_float4x4))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
			return E_FAIL;
	}
	else if (m_bOnOff)
	{
		
		if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_World4x4_TP(), sizeof(_float4x4))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
			return E_FAIL;

	}

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
