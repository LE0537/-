#include "stdafx.h"
#include "..\Public\Meowth.h"
#include "GameInstance.h"


CMeowth::CMeowth(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
}

CMeowth::CMeowth(const CMeowth & rhs)
	: CGameObj(rhs)
{
}

HRESULT CMeowth::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMeowth::Initialize(void * pArg)
{

	*(CGameObject**)pArg = this;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_PokemonInfo.strName = TEXT("나옹");
	m_PokemonInfo.strInfo = TEXT("가보자고가보자고가보자고가보자고가보자고가보자고\n가보자고가보자고가보자고가보자고가보자고가보자고\n가보자고가보자고가보자고가보자고가보자고가보자고");
	m_PokemonInfo.strChar = TEXT("민준나옹이다옹");
	m_PokemonInfo.iPokeNum = 52;
	m_PokemonInfo.iMaxHp = 1500;
	m_PokemonInfo.iHp = m_PokemonInfo.iMaxHp;
	m_PokemonInfo.iDmg = 80;
	m_PokemonInfo.iSDmg = 50;
	m_PokemonInfo.iDef = 50;
	m_PokemonInfo.iSDef = 40;
	m_PokemonInfo.iSpeed = 100;
	m_PokemonInfo.iLv = 5;
	m_PokemonInfo.iMaxExp = 20;
	m_PokemonInfo.iExp = 0;
	m_PokemonInfo.iSex = rand() % 2;
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

	m_PokemonInfo.eType = NORMAL;
	m_PokemonInfo.eType2 = POKETYPE_END;
	m_PokemonInfo.eStatInfo = STATINFO_END;

	m_pModelCom->Set_CurrentAnimIndex(2);

	return S_OK;
}

void CMeowth::Tick(_float fTimeDelta)
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
	if (!m_bOnOff)
		m_bSetPos = false;

}

void CMeowth::Late_Tick(_float fTimeDelta)
{
	if ((g_PokeInfo || g_bPokeDeck) && m_bOnOff && nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UIPOKE, this);
	//	else if (!g_PokeInfo && !g_bPokeDeck && !m_bDeckPoke && nullptr != m_pRendererCom)
	//		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CMeowth::Render()
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
HRESULT CMeowth::Ready_Components()
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
	if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_STATIC, TEXT("Prototype_Component_Model_Meowth"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}
void CMeowth::Set_DeckPos()
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

		m_fSizeX = 20.f;
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
void CMeowth::Key_Input(_float fTimeDelta)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Key_Down(DIK_W))
	{
		++m_iAnim;
		if (m_iAnim > 9)
			m_iAnim = 9;

		m_pModelCom->Set_CurrentAnimIndex(m_iAnim);
	}
	else if (pGameInstance->Key_Down(DIK_S))
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
HRESULT CMeowth::SetUp_ShaderResources()
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

CMeowth * CMeowth::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMeowth*	pInstance = new CMeowth(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CMeowth"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CMeowth::Clone(void * pArg)
{
	CGameObject*	pInstance = new CMeowth(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CMeowth"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMeowth::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
}
