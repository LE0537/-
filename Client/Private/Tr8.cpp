#include "stdafx.h"
#include "..\Public\Tr8.h"

#include "GameInstance.h"
#include "Level_GamePlay.h"
#include "Camera_Dynamic.h"
#include "TextBox.h"
#include "SoundMgr.h"
#include "Player.h"
#include "Ball.h"
#include "Data_Manager.h"	// 추가
#include "VIBuffer_Navigation.h"
#include "Bag.h"

CTr8::CTr8(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
}

CTr8::CTr8(const CTr8 & rhs)
	: CGameObj(rhs)
{
}

HRESULT CTr8::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTr8::Initialize(void * pArg)
{
	m_vecPoke.reserve(6);
	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Parts()))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	m_pTarget = *(&((CLevel_GamePlay::LOADFILE*)pArg)->pTarget);
	m_pCamera = *(&((CLevel_GamePlay::LOADFILE*)pArg)->pCamera);
	m_vMyBattlePos = dynamic_cast<CGameObj*>(m_pTarget)->Get_TargetBattlePos();

	CGameObject* tInfo;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Pikachu"), LEVEL_STATIC, TEXT("Layer_Pokemon"), &tInfo)))
		return E_FAIL;
	m_vecPoke.push_back(tInfo);

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Pikachu"), LEVEL_STATIC, TEXT("Layer_Pokemon"), &tInfo)))
		return E_FAIL;
	m_vecPoke.push_back(tInfo);

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Pikachu"), LEVEL_STATIC, TEXT("Layer_Pokemon"), &tInfo)))
		return E_FAIL;
	m_vecPoke.push_back(tInfo);

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_NonePoke"), LEVEL_STATIC, TEXT("Layer_Pokemon"), &tInfo)))
		return E_FAIL;
	m_vecPoke.push_back(tInfo);

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_NonePoke"), LEVEL_STATIC, TEXT("Layer_Pokemon"), &tInfo)))
		return E_FAIL;
	m_vecPoke.push_back(tInfo);

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_NonePoke"), LEVEL_STATIC, TEXT("Layer_Pokemon"), &tInfo)))
		return E_FAIL;
	m_vecPoke.push_back(tInfo);

	RELEASE_INSTANCE(CGameInstance);

	m_iPokeMaxSize = 2;
	m_iPokeSize = 0;
	m_PlayerInfo.strName = L"두송";
	m_PlayerInfo.bEvent = false;
	m_PlayerInfo.bBattle = false;

	m_pModelCom->Set_CurrentAnimIndex(0);
	//m_pTransformCom->Turn2(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f));
	//m_pTransformCom->Set_Scale(XMLoadFloat3((&((CLevel_GamePlay::LOADFILE*)pArg)->vScale)));
	m_pTransformCom->Set_Scale(XMVectorSet(0.055f, 0.055f, 0.055f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4((&((CLevel_GamePlay::LOADFILE*)pArg)->vPos)));
	m_pTransformCom->LookAt(XMLoadFloat4((&((CLevel_GamePlay::LOADFILE*)pArg)->vMyPos)));
	Ready_Script();
	m_pNavigationCom->Find_CurrentCellIndex(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

	return S_OK;
}

void CTr8::Tick(_float fTimeDelta)
{
	if (m_OnOff && m_bBattleLose)
	{
		for (auto& iter : m_vecPoke)
		{
			iter->Set_Dead();
		}
		m_vecPoke.clear();
	}
	if (m_OnOff && !m_bBattleLose && g_Battle)
	{
		Check_Anim(fTimeDelta);
		if (!m_bChangeAnim && g_Battle)
			Battle(fTimeDelta);
	}
	else if (!g_bEvolution)
	{
		Ckeck_Dist();
		OnNavi();
		m_fEventTime += fTimeDelta;
		if (!m_bFindPlayer)
			m_pModelCom->Set_CurrentAnimIndex(0);
		if (m_bFindPlayer && !m_PlayerInfo.bEvent)
			Move(fTimeDelta);

		if (m_bFindPlayer && m_fEventTime > 1.5f)
		{
			m_PlayerInfo.bEvent = false;
		}
		if (m_fDist <= 30.f)
		{
			m_pAABBCom->Update(m_pTransformCom->Get_WorldMatrix());
			m_pOBBCom->Update(m_pTransformCom->Get_WorldMatrix());
			m_pModelCom->Play_Animation(fTimeDelta);
		}
	}

}

void CTr8::Late_Tick(_float fTimeDelta)
{
	if (!g_Battle && !g_bEvolution)
		Check_Coll();
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->IsInFrustum(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION), 10.f))
	{
		if (m_fDist <= 30.f && !g_bEvolution && !g_PokeInfo && !g_bPokeDeck && nullptr != m_pRendererCom)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}
	RELEASE_INSTANCE(CGameInstance);
	if (g_CollBox)
	{
		m_pRendererCom->Add_Debug(m_pAABBCom);
		m_pRendererCom->Add_Debug(m_pOBBCom);
	}
}

HRESULT CTr8::Render()
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
HRESULT CTr8::Ready_Components()
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
	if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_STATIC, TEXT("Tr8"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC		ColliderDesc;

	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vScale = _float3(20.f, 40.f, 20.f);
	ColliderDesc.vPosition = _float3(0.f, 20.f, 0.f);
	if (FAILED(__super::Add_Components(TEXT("Com_AABB"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"), (CComponent**)&m_pAABBCom, &ColliderDesc)))
		return E_FAIL;

	/* For.Com_OBB*/
	ColliderDesc.vScale = _float3(40.f, 40.f, 40.f);
	ColliderDesc.vPosition = _float3(0.f, 20.f, 20.f);
	if (FAILED(__super::Add_Components(TEXT("Com_OBB"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"), (CComponent**)&m_pOBBCom, &ColliderDesc)))
		return E_FAIL;

	CNavigation::NAVIDESC			NaviDesc;
	ZeroMemory(&NaviDesc, sizeof NaviDesc);

	NaviDesc.iCurrentCellIndex = 0;

	if (FAILED(__super::Add_Components(TEXT("Com_Navigation"), LEVEL_STATIC, TEXT("Prototype_Component_Navigation"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
		return E_FAIL;

	return S_OK;
}

void CTr8::Move(_float fTimeDelta)
{
	m_pModelCom->Set_CurrentAnimIndex(2);
	_vector vTargetPos = dynamic_cast<CGameObj*>(m_pTarget)->Get_Transfrom()->Get_State(CTransform::STATE_TRANSLATION);
	_vector vLook = vTargetPos - m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	vPos += XMVector3Normalize(vLook) * 4.f * fTimeDelta;
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vPos);
	m_pTransformCom->LookAt(vTargetPos);
}

void CTr8::Ready_Script()
{
	m_vNormalScript.push_back(TEXT("락큰롤!"));
	m_vNormalScript.push_back(TEXT("내 라이브를 방해하다니 간덩이가 부었구나!!"));

	wstring szScriptBegin = TEXT("용서못해 내 첫 라이브를!!\n가라!!     '");
	wstring szScriptEnd = TEXT("'!!     너로 정했다!");
	szScriptBegin += dynamic_cast<CGameObj*>(m_vecPoke[0])->Get_PokeInfo().strName;
	szScriptBegin += szScriptEnd;
	m_vBattleScript.push_back(szScriptBegin);
}

void CTr8::Ready_LoseText()
{
	for (auto iter = m_vBattleScript.begin(); iter != m_vBattleScript.end();)
		iter = m_vBattleScript.erase(iter);

	m_vBattleScript.clear();


	wstring strTextBegin = TEXT("이젠 라이브 다신 안할게요...");
	m_vBattleScript.push_back(strTextBegin);
	m_vBattleScript.push_back(TEXT("악의배지를 얻었습니다."));
}
void CTr8::OnNavi()
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
void CTr8::Ckeck_Dist()
{
	_vector vTargetPos = dynamic_cast<CGameObj*>(m_pTarget)->Get_Transfrom()->Get_State(CTransform::STATE_TRANSLATION);
	_vector vLook = vTargetPos - m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	m_fDist = XMVectorGetX(XMVector3Length(vLook));
}
void CTr8::Check_Coll()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CCollider*	pTargetCollider = (CCollider*)pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_AABB"));
	CCollider*	pTargetCollider2 = (CCollider*)pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_OBB"));
	if (nullptr == pTargetCollider)
		return;

	if (!m_bEvent && m_pOBBCom->Collision(pTargetCollider2))
	{
		CSoundMgr::Get_Instance()->SetSoundVolume(SOUND_BGM, 0.3f);
		CSoundMgr::Get_Instance()->PlayEffect(TEXT("Battle0.wav"), 1.f);
		m_bFindPlayer = true;
		m_PlayerInfo.bEvent = true;
		m_bEvent = true;
		m_OnOff = true;
		m_fEventTime = 0.f;
		dynamic_cast<CPlayer*>(m_pTarget)->Set_TargetPoke(&m_vecPoke);
		dynamic_cast<CPlayer*>(m_pTarget)->Set_BattleTarget(this, BATTLE_TRAINER);
		if (!dynamic_cast<CGameObj*>(m_pTarget)->Get_Event())
			dynamic_cast<CGameObj*>(m_pTarget)->OnOffEvent();
		dynamic_cast<CCamera_Dynamic*>(m_pCamera)->Set_Target(this);
		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_BattleEvent"), LEVEL_GAMEPLAY, TEXT("Layer_Effect"), this)))
			return;
		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Screen"), LEVEL_GAMEPLAY, TEXT("Layer_Effect"), this)))
			return;
		CTextBox::TINFO tTInfo;

		tTInfo.iScriptSize = (_int)m_vNormalScript.size();
		tTInfo.pTarget = this;
		tTInfo.pScript = new wstring[m_vNormalScript.size()];
		tTInfo.iType = 0;
		for (_int i = 0; i < m_vNormalScript.size(); ++i)
			tTInfo.pScript[i] = m_vNormalScript[i];

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_TextBox"), LEVEL_GAMEPLAY, TEXT("Layer_UI"), &tTInfo)))
			return;
	}
	if (m_pAABBCom->Collision(pTargetCollider))
	{
		m_bFindPlayer = false;

	}
	RELEASE_INSTANCE(CGameInstance);
}

void CTr8::BattleStart(_float fTimeDelta)
{
	if (!m_bBattle)
	{
		if (!m_bEffectEnd)
		{
			m_pModelCom->Set_Loop(1);
			m_pModelCom->Set_CurrentAnimIndex(1);
			m_bEffectEnd = true;
			m_fStartBattle = 0.f;
		}
		if (m_bMotionEnd && m_fStartBattle > 0.2f && m_fStartBattle > 3.25f)
		{
			m_bMotionEnd = false;
			m_bEffectEnd = false;
			m_bEffect = false;
			m_bBattle = true;
			m_pModelCom->Set_End(1);
			dynamic_cast<CPlayer*>(m_pTarget)->Set_BattleStart();
			dynamic_cast<CBall*>(m_pBall)->Set_Render(false, 0);
			return;
		}
		m_fStartBattle += fTimeDelta;
		m_pModelCom->Play_Animation(fTimeDelta * 1.25f);
		dynamic_cast<CBall*>(m_pBall)->Set_Render(true, 2);
		m_pBall->Tick(fTimeDelta);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, m_pBall);
		if (m_fStartBattle > 2.85f && !m_bEffect)
		{
			_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
			XMVector3Normalize(vLook);
			_vector vPos = XMLoadFloat4(&m_vMyBattlePos) + vLook * 45.f;
			_vector vTargetPos = XMLoadFloat4(&m_vMyBattlePos) + vLook * 200.f;

			CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

			CLevel_GamePlay::LOADFILE tInfo;

			XMStoreFloat4(&tInfo.vPos, vPos);

			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_BallEffect"), LEVEL_GAMEPLAY, TEXT("Layer_UI"), &tInfo)))
				return;

			RELEASE_INSTANCE(CGameInstance);
			m_bEffect = true;
		}
		if (m_fStartBattle > 3.25f)
		{
			_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
			XMVector3Normalize(vLook);
			_vector vPos = XMLoadFloat4(&m_vMyBattlePos) + vLook * 35.f;
			_vector vTargetPos = XMLoadFloat4(&m_vMyBattlePos) + vLook * 200.f;
			dynamic_cast<CGameObj*>(m_vecPoke[m_iPokeSize])->Get_Transfrom()->Set_State(CTransform::STATE_TRANSLATION, vPos);
			dynamic_cast<CGameObj*>(m_vecPoke[m_iPokeSize])->Get_Transfrom()->LookAt(vTargetPos);
			dynamic_cast<CGameObj*>(m_vecPoke[m_iPokeSize])->Set_AnimIndex(0);
			dynamic_cast<CGameObj*>(m_vecPoke[m_iPokeSize])->Set_BattleMap(true, 3.7f);
			m_bMotionEnd = true;
		}

		if (!m_bBattleText)
		{
			CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

			CTextBox::TINFO tTInfo;

			tTInfo.iScriptSize = (_int)m_vBattleScript.size();
			tTInfo.pTarget = this;
			tTInfo.pScript = new wstring[m_vBattleScript.size()];
			tTInfo.iType = 1;
			for (_int i = 0; i < m_vBattleScript.size(); ++i)
				tTInfo.pScript[i] = m_vBattleScript[i];

			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_TextBox"), LEVEL_GAMEPLAY, TEXT("Layer_UI"), &tTInfo)))
				return;

			RELEASE_INSTANCE(CGameInstance);
			m_bBattleText = true;
		}
	}
	else if (m_bBattle)
	{
		m_pModelCom->Set_CurrentAnimIndex(0);
		m_pModelCom->Play_Animation(fTimeDelta);
	}

}

void CTr8::Check_Anim(_float fTimeDelta)
{
	if (m_iAnimIndex == 1 || m_iAnimIndex == 4)
	{
		m_bChangeAnim = true;
		if (m_ChangePoke && m_fStartBattle > 3.25f)
		{
			m_bEffect = false;
			m_pModelCom->Set_End(m_iAnimIndex);
			m_iAnimIndex = 0;
			m_pModelCom->Set_CurrentAnimIndex(m_iAnimIndex);
			m_bChangeAnim = false;
			m_ChangePoke = false;
		}
		m_fStartBattle += fTimeDelta;
		if (!m_ChangePoke && m_iAnimIndex == 1)
		{
			m_pModelCom->Set_Loop(m_iAnimIndex);
			m_pModelCom->Set_CurrentAnimIndex(m_iAnimIndex);
			m_ChangePoke = true;
			m_fStartBattle = 0.f;
		}

		if (m_bLose && m_pModelCom->Get_End(m_iAnimIndex))
		{
			m_pModelCom->Set_End(m_iAnimIndex);
			m_iAnimIndex = 0;
			m_pModelCom->Set_CurrentAnimIndex(m_iAnimIndex);
			m_bChangeAnim = false;
			dynamic_cast<CPlayer*>(m_pTarget)->Battle_Win();
			m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&m_vPrevPos));
			m_pTransformCom->LookAt(dynamic_cast<CGameObj*>(m_pTarget)->Get_Transfrom()->Get_State(CTransform::STATE_TRANSLATION));
			m_bBattleText = false;
			m_bBattle = false;
			m_fStartBattle = 0.f;
			m_ChangePoke = false;
			g_Battle = false;
			m_bBattleLose = true;

			for (_int i = 0; i < 60; ++i)
			{
				if (dynamic_cast<CPlayer*>(m_pTarget)->Get_Bag()->Get_vecItemNum(i) == 99)
				{
					CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

					ITEMINFO* tInfo;
					if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_EvilBadge"), LEVEL_STATIC, TEXT("Layer_Item"), &tInfo)))
						return;
					dynamic_cast<CPlayer*>(m_pTarget)->Get_Bag()->Set_vecItem(i, tInfo);

					RELEASE_INSTANCE(CGameInstance);
					break;
				}
			}

			_int iIndex = dynamic_cast<CPlayer*>(m_pTarget)->Get_Bag()->Get_EvolIndex();
			if (iIndex != 99)
			{
				g_bEvolution = true;
				dynamic_cast<CGameObj*>(dynamic_cast<CPlayer*>(m_pTarget)->Get_Bag()->Get_vecPoke(iIndex))->Set_PokeUIOnOff();
				CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
				if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Evolution"), LEVEL_GAMEPLAY, TEXT("Layer_UI"))))
					return;
				RELEASE_INSTANCE(CGameInstance);
			}

			return;
		}
		if (!m_bLose && m_iAnimIndex == 4)
		{
			m_pModelCom->Set_Loop(m_iAnimIndex);
			m_pModelCom->Set_CurrentAnimIndex(m_iAnimIndex);
			m_bLose = true;
			CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
			Ready_LoseText();
			CTextBox::TINFO tTInfo;

			tTInfo.iScriptSize = (_int)m_vBattleScript.size();
			tTInfo.pTarget = this;
			tTInfo.pScript = new wstring[m_vBattleScript.size()];
			tTInfo.iType = 6;
			for (_int i = 0; i < m_vBattleScript.size(); ++i)
				tTInfo.pScript[i] = m_vBattleScript[i];

			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_TextBox"), LEVEL_GAMEPLAY, TEXT("Layer_UI"), &tTInfo)))
				return;

			RELEASE_INSTANCE(CGameInstance);
		}
	}

	if (m_ChangePoke && m_bChangeAnim)
	{
		m_pModelCom->Play_Animation(fTimeDelta * 1.25f);
		dynamic_cast<CBall*>(m_pBall)->Set_Render(true, 2);
		m_pBall->Tick(fTimeDelta);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, m_pBall);
		if (m_fStartBattle > 2.85f && !m_bEffect)
		{
			_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
			XMVector3Normalize(vLook);
			_vector vPos = XMLoadFloat4(&m_vMyBattlePos) + vLook * 45.f;
			_vector vTargetPos = XMLoadFloat4(&m_vMyBattlePos) + vLook * 200.f;

			CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

			CLevel_GamePlay::LOADFILE tInfo;

			XMStoreFloat4(&tInfo.vPos, vPos);

			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_BallEffect"), LEVEL_GAMEPLAY, TEXT("Layer_UI"), &tInfo)))
				return;

			RELEASE_INSTANCE(CGameInstance);
			m_bEffect = true;
		}
		if (m_fStartBattle > 3.25f)
		{
			_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
			XMVector3Normalize(vLook);
			_vector vPos = XMLoadFloat4(&m_vMyBattlePos) + vLook * 35.f;
			_vector vTargetPos = XMLoadFloat4(&m_vMyBattlePos) + vLook * 200.f;
			dynamic_cast<CGameObj*>(m_vecPoke[m_iPokeSize])->Get_Transfrom()->Set_State(CTransform::STATE_TRANSLATION, vPos);
			dynamic_cast<CGameObj*>(m_vecPoke[m_iPokeSize])->Get_Transfrom()->LookAt(vTargetPos);
			dynamic_cast<CGameObj*>(m_vecPoke[m_iPokeSize])->Set_AnimIndex(0);
			dynamic_cast<CGameObj*>(m_vecPoke[m_iPokeSize])->Set_BattleMap(true, 0.f);
		}
	}
	if (m_bLose && m_bChangeAnim)
	{
		m_pModelCom->Play_Animation(fTimeDelta * 1.2f);
	}
}

void CTr8::Battle(_float fTimeDelta)
{
	BattleStart(fTimeDelta);
	if (!m_bPrevPos)
	{
		XMStoreFloat4(&m_vPrevPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
		m_bPrevPos = true;
	}
	_vector vTargetPos = dynamic_cast<CGameObj*>(m_pTarget)->Get_Transfrom()->Get_State(CTransform::STATE_TRANSLATION);
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&m_vMyBattlePos));
	m_pTransformCom->LookAt(vTargetPos);
}
HRESULT CTr8::Ready_Parts()
{

	/* For.Weapon */
	CHierarchyNode*		pSocket = m_pModelCom->Get_BonePtr("loc_ob_ball");
	if (nullptr == pSocket)
		return E_FAIL;

	CBall::WEAPONDESC		WeaponDesc;
	WeaponDesc.pSocket = pSocket;
	WeaponDesc.SocketPivotMatrix = m_pModelCom->Get_PivotFloat4x4();
	WeaponDesc.pParentWorldMatrix = m_pTransformCom->Get_World4x4Ptr();
	Safe_AddRef(pSocket);

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	m_pBall = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Ball"), &WeaponDesc);
	if (nullptr == m_pBall)
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CTr8::SetUp_ShaderResources()
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

CTr8 * CTr8::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTr8*	pInstance = new CTr8(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CTr8"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CTr8::Clone(void * pArg)
{
	CGameObject*	pInstance = new CTr8(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CTr8"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTr8::Free()
{
	__super::Free();

	for (auto iter = m_vNormalScript.begin(); iter != m_vNormalScript.end();)
		iter = m_vNormalScript.erase(iter);

	m_vNormalScript.clear();


	for (auto iter = m_vBattleScript.begin(); iter != m_vBattleScript.end();)
		iter = m_vBattleScript.erase(iter);

	m_vBattleScript.clear();

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pBall);
	Safe_Release(m_pAABBCom);
	Safe_Release(m_pOBBCom);
	Safe_Release(m_pModelCom);
}
