#include "stdafx.h"
#include "..\Public\Dragonite.h"

#include "GameInstance.h"
#include "Lv_Up.h"
#include "SoundMgr.h"
#include "TextBox.h"
#include "Camera_Dynamic.h"
#include "Player.h"
#include "VIBuffer_Navigation.h"
#include "Data_Manager.h"	// 추가
#include "Bag.h"

CDragonite::CDragonite(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
}

CDragonite::CDragonite(const CDragonite & rhs)
	: CGameObj(rhs)
{
}

HRESULT CDragonite::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDragonite::Initialize(void * pArg)
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

	m_PokemonInfo.eType = DRAGON;
	m_PokemonInfo.eType2 = FLY;
	m_PokemonInfo.eStatInfo = STATINFO_END;
	m_PokemonInfo.bLvUp = false;
	m_bWildPoke = false;
	m_pModelCom->Set_CurrentAnimIndex(2);
	m_pTransformCom->Set_Scale(XMVectorSet(0.04f, 0.04f, 0.04f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(-50000.f, -50000.f, -50000.f, 1.f));

	return S_OK;
}

void CDragonite::Tick(_float fTimeDelta)
{
	if (m_bOnOff)
	{
		if (!g_bEvolution)
		{
			Set_DeckPos();
			if (!m_bDeckInfo)
			{
				Key_Input(fTimeDelta);
			}
		}
		else if (g_bEvolution)
		{
			Set_EvolPos(fTimeDelta);
		}
		if (g_PokeInfo || g_bPokeDeck || g_bEvolution)
			m_pModelCom->Play_Animation(fTimeDelta);
	}

	if (!m_bOnOff)
		m_bSetPos = false;

	if (m_bBattleMap)
	{
		Battle(fTimeDelta);
		if (m_PokemonInfo.bLvUp)
			LvUp();
	}
	if (m_bAnimReset)
		Reset_Battle();
	if (m_bWildPoke && !g_bEvolution)
	{
		if (!m_bReadyWild)
		{
			Ready_WildBattle();
			m_bReadyWild = true;
		}
		if (g_Battle)
		{
			if (!m_bBattleStart && m_bCollCheck)
				WildBattle();
		}


		if (m_bWildPoke && !g_Battle && !g_bBag && !g_PokeInfo && !g_bPokeDeck && !dynamic_cast<CGameObj*>(m_pTarget)->Get_Event())
		{
			Move(fTimeDelta);
		}
	}
}

void CDragonite::Late_Tick(_float fTimeDelta)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (!g_bEvolution && !g_Battle && m_bWildPoke)
		Check_Coll();

	if (pGameInstance->IsInFrustum(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION), 10.f))
	{
		if (m_fDist < 30.f && !g_bEvolution && !g_bBag && !g_PokeInfo && !g_bPokeDeck && m_bWildPoke && !m_bBattleMap && !g_Battle && nullptr != m_pRendererCom)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}
	if ((g_PokeInfo || g_bPokeDeck || g_bEvolution) && m_bOnOff && nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UIPOKE, this);
	else if (m_bBattleMap && g_Battle && nullptr != m_pRendererCom)
	{
		if (g_bCaptureRender && !m_bWildPoke)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
		else if (!g_bCaptureRender)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}
	RELEASE_INSTANCE(CGameInstance);

	if (g_CollBox)
		m_pRendererCom->Add_Debug(m_pAABBCom);
}

HRESULT CDragonite::Render()
{

	if (nullptr == m_pShaderCom ||
		nullptr == m_pModelCom)
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshContainers();

	if (!m_bOnOff)
	{
		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			if (FAILED(m_pModelCom->SetUp_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
				return E_FAIL;

			if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 0)))
				return E_FAIL;
		}
	}
	else if (m_bOnOff)
	{
		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			if (FAILED(m_pModelCom->SetUp_Material(m_pShaderUICom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
				return E_FAIL;
			if (!m_bEvol)
			{
				if (FAILED(m_pModelCom->Render(m_pShaderUICom, i, 0)))
					return E_FAIL;
			}
			else if (m_bEvol && m_bRender)
			{
				if (FAILED(m_pModelCom->Render(m_pShaderUICom, i, 1)))
					return E_FAIL;
			}
		}
	}
	RELEASE_INSTANCE(CGameInstance);


	return S_OK;
}
HRESULT CDragonite::Ready_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 3.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_ShaderUI"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxUIModel"), (CComponent**)&m_pShaderUICom)))
		return E_FAIL;
	/* For.Com_Model*/
	if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_STATIC, TEXT("Dragonite"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC		ColliderDesc;

	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vScale = _float3(20.f, 40.f, 20.f);
	ColliderDesc.vPosition = _float3(0.f, 20.f, 0.f);
	if (FAILED(__super::Add_Components(TEXT("Com_AABB"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"), (CComponent**)&m_pAABBCom, &ColliderDesc)))
		return E_FAIL;

	CNavigation::NAVIDESC			NaviDesc;
	ZeroMemory(&NaviDesc, sizeof NaviDesc);

	NaviDesc.iCurrentCellIndex = 0;

	if (FAILED(__super::Add_Components(TEXT("Com_Navigation"), LEVEL_STATIC, TEXT("Prototype_Component_Navigation"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
		return E_FAIL;


	return S_OK;
}
void CDragonite::Check_Coll()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CCollider*	pTargetCollider = (CCollider*)pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_AABB"));

	if (nullptr == pTargetCollider)
		return;


	if (m_pAABBCom->Collision(pTargetCollider))
	{
		if (!m_bCollCheck)
		{
			dynamic_cast<CPlayer*>(m_pTarget)->Set_TargetPoke(&m_vecPoke);
			dynamic_cast<CPlayer*>(m_pTarget)->Set_BattleTarget(this, BATTLE_WILD);
			if (!dynamic_cast<CGameObj*>(m_pTarget)->Get_Event())
				dynamic_cast<CGameObj*>(m_pTarget)->OnOffEvent();
			dynamic_cast<CCamera_Dynamic*>(m_pCamera)->Set_Target(this);

			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_BattleIntro"), LEVEL_GAMEPLAY, TEXT("Layer_Effect"))))
				return;
			CSoundMgr::Get_Instance()->BGM_Stop();
			CSoundMgr::Get_Instance()->PlayEffect(TEXT("Battle1.wav"), 0.75f);
			m_bCollCheck = true;
		}
	}

	RELEASE_INSTANCE(CGameInstance);
}
void CDragonite::Ready_WildBattle()
{
	m_vecPoke.reserve(6);;
	m_vecPoke.push_back(this);
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	CGameObject* tInfo;
	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_NonePoke"), LEVEL_STATIC, TEXT("Layer_Pokemon"), &tInfo)))
		return;
	m_vecPoke.push_back(tInfo);
	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_NonePoke"), LEVEL_STATIC, TEXT("Layer_Pokemon"), &tInfo)))
		return;
	m_vecPoke.push_back(tInfo);
	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_NonePoke"), LEVEL_STATIC, TEXT("Layer_Pokemon"), &tInfo)))
		return;
	m_vecPoke.push_back(tInfo);
	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_NonePoke"), LEVEL_STATIC, TEXT("Layer_Pokemon"), &tInfo)))
		return;
	m_vecPoke.push_back(tInfo);
	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_NonePoke"), LEVEL_STATIC, TEXT("Layer_Pokemon"), &tInfo)))
		return;
	m_vecPoke.push_back(tInfo);
	RELEASE_INSTANCE(CGameInstance);

	m_pNavigationCom->Find_CurrentCellIndex(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	XMStoreFloat4(&m_vOriginPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
}
void CDragonite::WildBattle()
{
	//	_vector vLook = XMLoadFloat4(&dynamic_cast<CGameObj*>(m_pTarget)->Get_MyBattlePos()) - XMLoadFloat4(&m_vMyBattlePos);
	//	XMVector3Normalize(vLook);
	_vector vPos = XMLoadFloat4(&m_vMyBattlePos);
	//	_vector vTargetPos = XMLoadFloat4(&m_vMyBattlePos) + vLook * 200.f;
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vPos);
	m_pTransformCom->LookAt(XMLoadFloat4(&dynamic_cast<CGameObj*>(m_pTarget)->Get_MyBattlePos()));
	Set_BattleMap(true, 0.f);
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	Ready_Script();
	CTextBox::TINFO tTInfo;

	tTInfo.iScriptSize = (_int)m_vNormalScript.size();
	tTInfo.pTarget = this;
	tTInfo.pScript = new wstring[m_vNormalScript.size()];
	tTInfo.iType = 1;
	for (_int i = 0; i < m_vNormalScript.size(); ++i)
		tTInfo.pScript[i] = m_vNormalScript[i];

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_TextBox"), LEVEL_GAMEPLAY, TEXT("Layer_UI"), &tTInfo)))
		return;

	RELEASE_INSTANCE(CGameInstance);

	m_bBattleStart = true;
	m_PlayerInfo.bEvent = false;

}
void CDragonite::Ready_Script()
{
	wstring szScriptBegin = TEXT("앗!! 야생의 '");
	wstring szScriptEnd = TEXT("'(이)가   나타났다!");
	szScriptBegin += m_PokemonInfo.strName;
	szScriptBegin += szScriptEnd;
	m_vNormalScript.push_back(szScriptBegin);
}
void CDragonite::Move(_float fTimeDelta)
{
	_vector vTargetPos = dynamic_cast<CGameObj*>(m_pTarget)->Get_Transfrom()->Get_State(CTransform::STATE_TRANSLATION);
	_vector vLook = vTargetPos - m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_float fOriginPosDist = XMVectorGetX(XMVector3Length(XMLoadFloat4(&m_vOriginPos) - m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION)));
	m_fDist = XMVectorGetX(XMVector3Length(vLook));

	if (m_fDist < 30.f)
	{
		OnNavi();
		if (m_fDist < 10.f)
		{
			if (!m_bFindPlayer)
			{
				m_PlayerInfo.bEvent = true;
				CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
				if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_BattleEvent"), LEVEL_GAMEPLAY, TEXT("Layer_Effect"), this)))
					return;
				RELEASE_INSTANCE(CGameInstance);
				m_bFindPlayer = true;
			}
			m_pModelCom->Set_CurrentAnimIndex(8);
			m_pTransformCom->Go_MonsterStraight(fTimeDelta * 1.3f, m_pNavigationCom, vTargetPos);
			m_pTransformCom->LookAt(vTargetPos);
		}
		else
		{
			m_fMoveTime += fTimeDelta;
			m_PlayerInfo.bEvent = false;
			m_bFindPlayer = false;

			if (m_fMoveTime > 3.f)
			{
				m_iMoveIndex = rand() % 2;
				if (m_iMoveIndex != 0)
					m_pTransformCom->Turn2(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(_float(rand() % 181)));

				m_fMoveTime = 0.f;
			}
			if (fOriginPosDist > 15.f && m_iMoveIndex == 1)
			{
				m_iMoveIndex = 2;
			}

			switch (m_iMoveIndex)
			{
			case 0:
				m_pModelCom->Set_CurrentAnimIndex(2);
				break;
			case 1:
				m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);
				m_pModelCom->Set_CurrentAnimIndex(7);
				break;
			case 2:
				if (fOriginPosDist > 1.f)
				{
					m_pTransformCom->Go_MonsterStraight(fTimeDelta, m_pNavigationCom, XMLoadFloat4(&m_vOriginPos));
					m_pTransformCom->LookAt(XMLoadFloat4(&m_vOriginPos));
					m_pModelCom->Set_CurrentAnimIndex(7);
				}
				else
					m_pModelCom->Set_CurrentAnimIndex(2);
				break;
			default:
				break;
			}

		}

		m_pModelCom->Play_Animation(fTimeDelta);
		m_pAABBCom->Update(m_pTransformCom->Get_WorldMatrix());
	}
}
void CDragonite::Ready_EvolScript()
{
	for (auto iter = m_vNormalScript.begin(); iter != m_vNormalScript.end();)
		iter = m_vNormalScript.erase(iter);

	m_vNormalScript.clear();

	wstring strTextBegin = TEXT("...오잉 ?!   '");
	wstring strTextEnd = TEXT("' 의 모습이...!");

	strTextBegin += m_PokemonInfo.strName;
	strTextBegin += strTextEnd;

	m_vNormalScript.push_back(strTextBegin);
}
void CDragonite::Ready_ClearEvolScript()
{
	for (auto iter = m_vNormalScript.begin(); iter != m_vNormalScript.end();)
		iter = m_vNormalScript.erase(iter);

	m_vNormalScript.clear();

	wstring strTextBegin = TEXT("축하합니다! 신뇽은  \n'");
	wstring strTextEnd = TEXT("' 로 진화했습니다!");

	strTextBegin += m_PokemonInfo.strName;
	strTextBegin += strTextEnd;

	m_vNormalScript.push_back(strTextBegin);
}
void CDragonite::Set_EvolPos(_float fTimeDelta)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	if (m_bSetPos)
	{
		m_EvolTime += fTimeDelta;
	}
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

		if (m_PokemonInfo.bEvolution)
		{
			Ready_Script();
			CTextBox::TINFO tTInfo;

			tTInfo.iScriptSize = (_int)m_vNormalScript.size();
			tTInfo.pTarget = this;
			tTInfo.pScript = new wstring[m_vNormalScript.size()];
			tTInfo.iType = 1;
			for (_int i = 0; i < m_vNormalScript.size(); ++i)
				tTInfo.pScript[i] = m_vNormalScript[i];

			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_TextBox"), LEVEL_GAMEPLAY, TEXT("Layer_UI"), &tTInfo)))
				return;

			m_iAnimIndex = 1;
			m_pModelCom->Set_CurrentAnimIndex(m_iAnimIndex);
		}
		else
		{
			m_bEvol = true;
			m_iAnimIndex = 2;
			m_pModelCom->Set_CurrentAnimIndex(m_iAnimIndex);
		}
	}
	if (m_PokemonInfo.bEvolution)
	{
		if (m_bSetPos && m_iAnimIndex == 1 && m_pModelCom->Get_End(m_iAnimIndex))
		{
			m_bEvol = true;
			m_EvolTime = 0.f;
			m_iAnimIndex = 2;
			m_pModelCom->Set_CurrentAnimIndex(m_iAnimIndex);

			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Wartortle"), LEVEL_STATIC, TEXT("Layer_Pokemon"), &m_EvolPoke)))
				return;
			_int iIndex = dynamic_cast<CPlayer*>(m_pTarget)->Get_Bag()->Get_EvolIndex();
			dynamic_cast<CGameObj*>(m_EvolPoke)->Set_Target(m_pTarget);
			dynamic_cast<CPlayer*>(m_pTarget)->Get_Bag()->Set_vecPoke(iIndex, m_EvolPoke);
			dynamic_cast<CGameObj*>(m_EvolPoke)->Set_PokeUIOnOff();
		}
	}
	else if (m_bClearEvol)
	{
		m_bEvol = false;
		m_iAnimIndex = 1;
		m_pModelCom->Set_CurrentAnimIndex(m_iAnimIndex);

		Ready_ClearEvolScript();
		CTextBox::TINFO tTInfo;

		tTInfo.iScriptSize = (_int)m_vNormalScript.size();
		tTInfo.pTarget = this;
		tTInfo.pScript = new wstring[m_vNormalScript.size()];
		tTInfo.iType = 1;
		for (_int i = 0; i < m_vNormalScript.size(); ++i)
			tTInfo.pScript[i] = m_vNormalScript[i];

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_TextBox"), LEVEL_GAMEPLAY, TEXT("Layer_UI"), &tTInfo)))
			return;

		m_bClearEvol = false;
		m_iAnimIndex = 1;
		m_pModelCom->Set_CurrentAnimIndex(m_iAnimIndex);
		m_EvolTime = 0.f;
	}
	else if (!m_bEvol && !m_bClearEvol && m_iAnimIndex == 1 && m_pModelCom->Get_End(m_iAnimIndex))
	{
		m_iAnimIndex = 2;
		m_pModelCom->Set_CurrentAnimIndex(m_iAnimIndex);
	}
	else if (!m_bEvol && !m_bClearEvol && m_EvolTime > 3.f)
	{
		LvUp();
		g_bEvolution = false;
		m_bSetPos = false;
		m_bOnOff = false;
	}
	m_fSizeX = 6.f;
	m_fSizeY = 6.f;
	m_fX = 640.f;
	m_fY = 450.f;

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixTranspose(XMMatrixIdentity()));
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, -500.f, 100.f)));
	_float3 vScale = { m_fSizeX,m_fSizeY,6.f };
	m_pTransformCom->Set_Scale(XMLoadFloat3(&vScale));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, -200.f, 1.f));


	RELEASE_INSTANCE(CGameInstance);
}
void CDragonite::Set_DeckPos()
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

		m_fSizeX = 6.f;
		m_fSizeY = 6.f;
		m_fX = 1000;
		m_fY = 600;

		XMStoreFloat4x4(&m_ViewMatrix, XMMatrixTranspose(XMMatrixIdentity()));
		XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, -500.f, 100.f)));
		_float3 vScale = { m_fSizeX,m_fSizeY,6.f };
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

		m_fSizeX = 6.f;
		m_fSizeY = 6.f;
		m_fX = 280;
		m_fY = 600;

		XMStoreFloat4x4(&m_ViewMatrix, XMMatrixTranspose(XMMatrixIdentity()));
		XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, -500.f, 100.f)));
		_float3 vScale = { m_fSizeX,m_fSizeY,6.f };
		m_pTransformCom->Set_Scale(XMLoadFloat3(&vScale));
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, -200.f, 1.f));

	}

	RELEASE_INSTANCE(CGameInstance);
}
void CDragonite::Key_Input(_float fTimeDelta)
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
void CDragonite::Battle(_float fTimeDelta)
{
	if (!m_bBattle)
	{
		m_pTransformCom->Set_Scale(XMVectorSet(0.04f, 0.04f, 0.04f, 0.f));
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
				if (m_bWildPoke)
					dynamic_cast<CPlayer*>(m_pTarget)->Set_BattleStart();
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
	if ((m_iAnimIndex == 3 || m_iAnimIndex == 4) && !m_bAttack)
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
	if (!m_bHit && m_iAnimIndex == 5)
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
void CDragonite::Set_Stats()
{
	_float fHp = 91.f;
	_float fDmg = 134.f;
	_float fDef = 95.f;
	_float fSDmg = 100.f;
	_float fSDef = 100.f;
	_float fSpeed = 80.f;

	m_PlayerInfo.strName = TEXT("망나뇽");
	m_PlayerInfo.bEvent = false;


	m_PokemonInfo.strName = TEXT("망나뇽");
	m_PokemonInfo.strInfo = TEXT("바다의 화신이라고 불린다.\n망나뇽 형태의 조각상을 뱃머리에 두는 배도 많다.");
	m_PokemonInfo.strChar = TEXT("화신");
	m_PokemonInfo.iPokeNum = 149;
	m_PokemonInfo.iLv = rand() % 10 + 40;
	m_PokemonInfo.iMaxHp = _int(((fHp * 2.f) + 31.f + 100) * (m_PokemonInfo.iLv / 100.f) + 10.f);
	m_PokemonInfo.iHp = m_PokemonInfo.iMaxHp;
	m_PokemonInfo.iDmg = _int(((fDmg * 2.f) + 31.f) * (m_PokemonInfo.iLv / 100.f) + 5.f);
	m_PokemonInfo.iSDmg = _int(((fSDmg * 2.f) + 31.f) * (m_PokemonInfo.iLv / 100.f) + 5.f);
	m_PokemonInfo.iDef = _int(((fDef * 2.f) + 31.f) * (m_PokemonInfo.iLv / 100.f) + 5.f);
	m_PokemonInfo.iSDef = _int(((fSDef * 2.f) + 31.f) * (m_PokemonInfo.iLv / 100.f) + 5.f);
	m_PokemonInfo.iSpeed = _int(((fSpeed * 2.f) + 31.f) * (m_PokemonInfo.iLv / 100.f) + 5.f);
	m_PokemonInfo.iMaxExp = m_PokemonInfo.iLv * 2;
	m_PokemonInfo.iExp = 0;
	m_PokemonInfo.iSex = rand() % 2;
	m_PokemonInfo.iBallNum = 0;
	m_PokemonInfo.bRide = false;
	m_PokemonInfo.bEvolution = false;

}
void CDragonite::OnNavi()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CVIBuffer_Navigation*		pVIBuffer_Navigation = (CVIBuffer_Navigation*)pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Field"), TEXT("Com_Navigation"), 0);
	if (nullptr == pVIBuffer_Navigation)
		return;

	CTransform*		pTransform_Navigation = (CTransform*)pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Field"), TEXT("Com_Transform"), 0);
	if (nullptr == pTransform_Navigation)
		return;

	_vector		vPosition = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

	vPosition.m128_f32[1] = pVIBuffer_Navigation->Compute_Height(vPosition, pTransform_Navigation->Get_WorldMatrix(), m_pNavigationCom->Get_CellPoints());

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vPosition);

	RELEASE_INSTANCE(CGameInstance);
}
void CDragonite::LvUp()
{
	_float fHp = 91.f;
	_float fDmg = 134.f;
	_float fDef = 95.f;
	_float fSDmg = 100.f;
	_float fSDef = 100.f;
	_float fSpeed = 80.f;

	_int iPrevMaxHp = m_PokemonInfo.iMaxHp;
	_int iPrevHp = m_PokemonInfo.iHp;
	_int iPrevDmg = m_PokemonInfo.iDmg;
	_int iPrevDef = m_PokemonInfo.iDef;
	_int iPrevSDmg = m_PokemonInfo.iSDmg;
	_int iPrevSDef = m_PokemonInfo.iSDef;
	_int iPrevSpeed = m_PokemonInfo.iSpeed;

	m_PokemonInfo.iMaxHp = _int(((fHp * 2.f) + 31.f + 100) * (m_PokemonInfo.iLv / 100.f) + 10.f);
	m_PokemonInfo.iHp = m_PokemonInfo.iMaxHp - iPrevMaxHp + iPrevHp;
	m_PokemonInfo.iDmg = _int(((fDmg * 2.f) + 31.f) * (m_PokemonInfo.iLv / 100.f) + 5.f);
	m_PokemonInfo.iSDmg = _int(((fSDmg * 2.f) + 31.f) * (m_PokemonInfo.iLv / 100.f) + 5.f);
	m_PokemonInfo.iDef = _int(((fDef * 2.f) + 31.f) * (m_PokemonInfo.iLv / 100.f) + 5.f);
	m_PokemonInfo.iSDef = _int(((fSDef * 2.f) + 31.f) * (m_PokemonInfo.iLv / 100.f) + 5.f);
	m_PokemonInfo.iSpeed = _int(((fSpeed * 2.f) + 31.f) * (m_PokemonInfo.iLv / 100.f) + 5.f);

	if (!g_bEvolution)
	{
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

		CLv_Up::LVUPINFO tInfo;
		tInfo.iPrevMaxHp = iPrevMaxHp;
		tInfo.iPrevDmg = iPrevDmg;
		tInfo.iPrevDef = iPrevDef;
		tInfo.iPrevSDmg = iPrevSDmg;
		tInfo.iPrevSDef = iPrevSDef;
		tInfo.iPrevSpeed = iPrevSpeed;

		tInfo.iMaxHp = m_PokemonInfo.iMaxHp;
		tInfo.iDmg = m_PokemonInfo.iDmg;
		tInfo.iDef = m_PokemonInfo.iDef;
		tInfo.iSDmg = m_PokemonInfo.iSDmg;
		tInfo.iSDef = m_PokemonInfo.iSDef;
		tInfo.iSpeed = m_PokemonInfo.iSpeed;


		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Lv_Up"), LEVEL_GAMEPLAY, TEXT("Layer_UI"), &tInfo)))
			return;

		RELEASE_INSTANCE(CGameInstance);
	}


	m_PokemonInfo.bLvUp = false;
}
void CDragonite::Reset_Battle()
{
	m_iAnim = 0;
	m_bSetPos = false;
	m_bBrath = false;
	m_bBattle = false;
	m_fStartBattle = 0.f;
	m_bAttack = false;
	m_bHit = false;
	m_bDown = false;
	m_bStopAnim = false;
	m_bAnimReset = false;
	m_bBattleMap = false;
}
HRESULT CDragonite::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

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

		if (FAILED(m_pShaderUICom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4))))
			return E_FAIL;

		if (FAILED(m_pShaderUICom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_World4x4_TP(), sizeof(_float4x4))))
			return E_FAIL;

		if (FAILED(m_pShaderUICom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
			return E_FAIL;

		if (FAILED(m_pShaderUICom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
			return E_FAIL;

	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CDragonite * CDragonite::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CDragonite*	pInstance = new CDragonite(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CDragonite"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CDragonite::Clone(void * pArg)
{
	CGameObject*	pInstance = new CDragonite(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CDragonite"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDragonite::Free()
{
	__super::Free();

	if (m_bWildPoke)
		m_vecPoke.clear();

	for (auto iter = m_vNormalScript.begin(); iter != m_vNormalScript.end();)
		iter = m_vNormalScript.erase(iter);

	m_vNormalScript.clear();
	Safe_Release(m_pShaderUICom);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pAABBCom);
}
