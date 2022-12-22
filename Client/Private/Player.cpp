#include "stdafx.h"
#include "..\Public\Player.h"
#include "GameInstance.h"
#include "Level_GamePlay.h"
#include "SoundMgr.h"
#include "Bag.h"
#include "TextBox.h"
#include "VIBuffer_Navigation.h"
#include "BattleUI.h"
#include "Data_Manager.h"	// 추가
#include "Ball.h"
#include "Trail.h"
CPlayer::CPlayer(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
}

CPlayer::CPlayer(const CPlayer & rhs)
	: CGameObj(rhs)
{
}

HRESULT CPlayer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer::Initialize(void * pArg)
{

	*(CGameObject**)(&((CLevel_GamePlay::LOADFILE*)pArg)->pTarget) = this;

	m_vMyBattlePos = *(_float4*)&((CLevel_GamePlay::LOADFILE*)pArg)->vMyPos;
	m_vTargetBattlePos = *(_float4*)&((CLevel_GamePlay::LOADFILE*)pArg)->vTargetPos;
	if (FAILED(Ready_Components()))
		return E_FAIL;
	if (FAILED(Ready_Parts()))
		return E_FAIL;

	m_PlayerInfo.strName = L"쥬신";
	m_PlayerInfo.bEvent = false;
	m_PlayerInfo.bBattle = false;
	m_PlayerInfo.bRide = false;
	m_PlayerInfo.iMoney = 1000;
	
	m_pTransformCom->Set_Scale(XMLoadFloat3((&((CLevel_GamePlay::LOADFILE*)pArg)->vScale)));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4((&((CLevel_GamePlay::LOADFILE*)pArg)->vPos)));
	m_pModelCom->Set_CurrentAnimIndex(IDLE);
	
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Trail"), LEVEL_STATIC, TEXT("Layer_Effect"), &m_pTrail)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);


	return S_OK;
}

void CPlayer::Tick(_float fTimeDelta)
{
	if (!g_bRace)
	{
		if (g_Battle)
			Battle(fTimeDelta);
		else if (!g_bEvolution)
		{
			OnNavi();
			if (!m_PlayerInfo.bRide)
				m_pModelCom->Set_CurrentAnimIndex(IDLE);
			if (m_PlayerInfo.bRide)
				CheckRideIDLE();

			if (!m_PlayerInfo.bEvent)
				Key_Input(fTimeDelta);

			m_pAABBCom->Update(m_pTransformCom->Get_WorldMatrix());
			m_pOBBCom->Update(m_pTransformCom->Get_WorldMatrix());
		}
		if (!g_bEvolution && !m_PlayerInfo.bRide)
			m_pModelCom->Play_Animation(fTimeDelta);
		else if (m_PlayerInfo.bRide)
		{
			switch (m_PlayerInfo.iRideNum)
			{
			case 115:
				m_pModelCom->Play_Animation(fTimeDelta);
				break;
			case 143:
				m_pModelCom->Play_Animation(fTimeDelta * 1.5f);
				break;
			default:
				break;
			}
		}
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	
		_float4 vPos;
		XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
		_float4 vAt = vPos;
		if (!g_Battle)
		{
			vPos.x -= 15.f;
			vPos.y += 30.f;
			vPos.z -= 30.f;
		}
		else if (g_Battle)
		{
			vPos.x -= 5.f;
			vPos.y += 10.f;
			vPos.z -= 10.f;
		}
		pGameInstance->Set_ShadowLightDesc(LIGHTDESC::TYPE_FIELDSHADOW,vPos, vAt);

		RELEASE_INSTANCE(CGameInstance);
	}
}

void CPlayer::Late_Tick(_float fTimeDelta)
{
	if (!g_bRace)
	{
		if (g_Battle && m_PlayerInfo.bRide)
			m_PlayerInfo.bRide = false;

		if (!g_bEvolution && !g_PokeInfo && !g_bPokeDeck && nullptr != m_pRendererCom)
		{
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOWDEPTH, this);
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
		}
		if (g_CollBox)
		{
			m_pRendererCom->Add_Debug(m_pNavigationCom);
			m_pRendererCom->Add_Debug(m_pAABBCom);
			m_pRendererCom->Add_Debug(m_pOBBCom);
		}
	}
}

HRESULT CPlayer::Render()
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
HRESULT CPlayer::Render_ShadowDepth()
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

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 3)))
			return E_FAIL;

	}

	RELEASE_INSTANCE(CGameInstance);



	return S_OK;
}
HRESULT CPlayer::Ready_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom,&TransformDesc)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model*/
	if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_STATIC, TEXT("Player"), (CComponent**)&m_pModelCom)))
		return E_FAIL;


	CCollider::COLLIDERDESC		ColliderDesc;

	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vScale = _float3(20.f, 40.f, 20.f);
	ColliderDesc.vPosition = _float3(0.f, 20.f, 0.f);
	if (FAILED(__super::Add_Components(TEXT("Com_AABB"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"), (CComponent**)&m_pAABBCom, &ColliderDesc)))
		return E_FAIL;

	/* For.Com_OBB*/
	ColliderDesc.vScale = _float3(20.f, 40.f, 20.f);
	ColliderDesc.vPosition = _float3(0.f, 20.f, 0.f);
	if (FAILED(__super::Add_Components(TEXT("Com_OBB"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"), (CComponent**)&m_pOBBCom, &ColliderDesc)))
		return E_FAIL;

	CNavigation::NAVIDESC			NaviDesc;
	ZeroMemory(&NaviDesc, sizeof NaviDesc);

	NaviDesc.iCurrentCellIndex = 0;

	if (FAILED(__super::Add_Components(TEXT("Com_Navigation"), LEVEL_STATIC, TEXT("Prototype_Component_Navigation"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
		return E_FAIL;

	return S_OK;
}

void CPlayer::CheckRideIDLE()
{
	switch (m_PlayerInfo.iRideNum)
	{
	case 115:
		m_pModelCom->Set_CurrentAnimIndex(RIDE_KANGASKHAN);
		break;
	case 143:
		m_pModelCom->Set_CurrentAnimIndex(RIDE_SNORLAX);
		break;
	default:
		break;
	}
}

void CPlayer::Key_Input(_float fTimeDelta)
{
	m_fLandTime += fTimeDelta;
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (!m_PlayerInfo.bRide)
	{
		if (pGameInstance->Key_Pressing(DIK_LSHIFT) && pGameInstance->Key_Pressing(DIK_W))
		{
			m_pTransformCom->Go_Straight(fTimeDelta * 1.3f, m_pNavigationCom);
			m_pModelCom->Set_CurrentAnimIndex(RUN);
			if (m_fLandTime > 0.2f)
			{
				CLevel_GamePlay::LOADFILE tInfo;
				XMStoreFloat4(&tInfo.vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
				tInfo.vScale = { 0.5f,0.5f,0.5f};
				tInfo.vPos.y += 0.2f;
				if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Land"), LEVEL_GAMEPLAY, TEXT("Layer_UI"), &tInfo)))
					return;
				m_fLandTime = 0.f;
			}
		}
		else if (pGameInstance->Key_Pressing(DIK_W))
		{
			m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);
			m_pModelCom->Set_CurrentAnimIndex(WALK);
			if (m_fLandTime > 0.3f)
			{
				CLevel_GamePlay::LOADFILE tInfo;
				XMStoreFloat4(&tInfo.vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
				tInfo.vScale = { 0.5f,0.5f,0.5f };
				tInfo.vPos.y += 0.2f;
				if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Land"), LEVEL_GAMEPLAY, TEXT("Layer_UI"), &tInfo)))
					return;
				m_fLandTime = 0.f;
			}
		}

	}
	if (m_PlayerInfo.bRide)
	{
		if (pGameInstance->Key_Pressing(DIK_W))
		{
			m_pTransformCom->Go_Straight(fTimeDelta * 1.8f, m_pNavigationCom);
			switch (m_PlayerInfo.iRideNum)
			{
			case 115:
				m_pModelCom->Set_CurrentAnimIndex(RUN_KANGASKHAN);
				break;
			case 143:
				m_pModelCom->Set_CurrentAnimIndex(RUN_SNORLAX);
				break;
			default:
				break;
			}
			if (m_fLandTime > 0.1f)
			{
				CLevel_GamePlay::LOADFILE tInfo;
				_vector vRight, vPos;
				tInfo.vScale = { 1.1f,1.1f,1.1f };
				vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
				vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
				XMVector3Normalize(vRight);
				if (!m_bRideLand)
				{
					vPos += vRight * 5.f;
					vPos.m128_f32[1] += 0.4f;
					XMStoreFloat4(&tInfo.vPos, vPos);
					m_bRideLand = true;
				}
				else
				{
					vPos -= vRight * 5.f;
					vPos.m128_f32[1] += 0.4f;
					XMStoreFloat4(&tInfo.vPos, vPos);
					m_bRideLand = false;
				}
				if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Land"), LEVEL_GAMEPLAY, TEXT("Layer_UI"), &tInfo)))
					return;
				m_fLandTime = 0.f;
			}
		}
		if (pGameInstance->Key_Pressing(DIK_R))
		{
			m_PlayerInfo.bRide = false;
		}
	}

	if (pGameInstance->Key_Pressing(DIK_A))
	{
		m_pTransformCom->Turn2(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(-3.f));
	}
	else if (pGameInstance->Key_Pressing(DIK_D))
	{
		m_pTransformCom->Turn2(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(3.f));
	}


	RELEASE_INSTANCE(CGameInstance);
}
void CPlayer::Battle(_float fTimeDelta)
{
	Check_Anim(fTimeDelta);
	if (!m_bChangeAnim)
	{
		if (!m_bPrevPos)
		{
			XMStoreFloat4(&m_vPrevPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
			m_bPrevPos = true;
		}
		if (m_bBattleStart)
			BattleStart(fTimeDelta);
		else
		{
			m_pModelCom->Set_CurrentAnimIndex(0);
			m_pModelCom->Play_Animation(fTimeDelta);
		}
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&m_vMyBattlePos));
		m_pTransformCom->LookAt(XMLoadFloat4(&m_vTargetBattlePos));
	}
}
void CPlayer::BattleStart(_float fTimeDelta)
{
	
	if (!m_bBattle)
	{
		if (!m_bEffectEnd)
		{
			m_pModelCom->Set_Loop(2);
			m_pModelCom->Set_CurrentAnimIndex(2);
			m_bEffectEnd = true;
			m_fStartBattle = 0.f;
		}
		if (m_bMotionEnd && m_fStartBattle > 0.2f && m_fStartBattle > 3.5f)
		{
			m_bMotionEnd = false;
			m_bEffectEnd = false;
			m_bEffect = false;
			m_bBattle = true;
			m_pModelCom->Set_End(2);
			m_fBattleUITime = 0.f;
			dynamic_cast<CBall*>(m_pBall)->Set_Render(false, m_iBallIndex);
			dynamic_cast<CBall*>(m_pBall)->Set_Reset();
			return;
		}
		m_fStartBattle += fTimeDelta;
		m_pModelCom->Play_Animation(fTimeDelta*0.8f);
		m_iBallIndex = dynamic_cast<CGameObj*>(m_pBag->Get_vecPoke(0))->Get_PokeInfo().iBallNum;
		dynamic_cast<CBall*>(m_pBall)->Set_Render(true, m_iBallIndex);
		m_pBall->Tick(fTimeDelta);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, m_pBall);
		if (m_fStartBattle > 3.1f && !m_bEffect)
		{
			_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
			XMVector3Normalize(vLook);
			_vector vPos = XMLoadFloat4(&m_vMyBattlePos) + vLook * 35.f;
			_vector vTargetPos = XMLoadFloat4(&m_vMyBattlePos) + vLook * 200.f;

			CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

			CLevel_GamePlay::LOADFILE tInfo;

			XMStoreFloat4(&tInfo.vPos, vPos);

			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_BallEffect"), LEVEL_GAMEPLAY, TEXT("Layer_UI"), &tInfo)))
				return;

			RELEASE_INSTANCE(CGameInstance);
			m_bEffect = true;
		}
		if (m_fStartBattle > 3.5f)
		{
			_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
			XMVector3Normalize(vLook);
			_vector vPos = XMLoadFloat4(&m_vMyBattlePos) + vLook * 35.f;
			_vector vTargetPos = XMLoadFloat4(&m_vMyBattlePos) + vLook * 200.f;

			_int iPokeIndex = 0;

			for (_int i = 0; i < 6; ++i)
			{
				if (dynamic_cast<CGameObj*>(m_pBag->Get_vecPoke(i))->Get_PokeInfo().eStatInfo != STUN)
				{
					iPokeIndex = i;
					i = 6;
				}
			}
			
			dynamic_cast<CGameObj*>(m_pBag->Get_vecPoke(iPokeIndex))->Get_Transfrom()->Set_State(CTransform::STATE_TRANSLATION, vPos);
			dynamic_cast<CGameObj*>(m_pBag->Get_vecPoke(iPokeIndex))->Get_Transfrom()->LookAt(vTargetPos);
			dynamic_cast<CGameObj*>(m_pBag->Get_vecPoke(iPokeIndex))->Set_AnimIndex(0);
			dynamic_cast<CGameObj*>(m_pBag->Get_vecPoke(iPokeIndex))->Set_BattleMap(true,0.f);
			m_bMotionEnd = true;
		}
		if (!m_bBattleText)
		{
			CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
			Ready_Script();
			CTextBox::TINFO tTInfo;

			tTInfo.iScriptSize = (_int)m_vBattleScript.size();
			tTInfo.pTarget = this;
			tTInfo.pScript = new wstring[m_vBattleScript.size()];
			tTInfo.iType = 2;
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
		m_fBattleUITime += fTimeDelta;
		if (m_fBattleUITime > 1.5f)
		{
			if (!m_bBattleUI)
			{	
				CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

				CBattleUI::BATTLEINFO tInfo;
				tInfo.pPlayer = m_pBag;
				tInfo.pvecTargetPoke = m_pvecTargetPoke;
				tInfo.pPlayer_Orgin = this;
				tInfo.pBattleTarget = m_pBattleTarget;
				tInfo.eBattleType = m_eBattleType;
				if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_BattleUI"), LEVEL_GAMEPLAY, TEXT("Layer_UI"),&tInfo)))
					return;

				RELEASE_INSTANCE(CGameInstance);
				m_bBattleUI = true;
			}
		}
		
		m_pModelCom->Set_CurrentAnimIndex(0);
		m_pModelCom->Play_Animation(fTimeDelta);
	}
	//배틀 종료할때 m_bPrevPos,m_fStartBattle,m_bBattle,m_bBattleStart,m_bBattleText,m_bBattleUI 초기화 필수로해줘야함
}
void CPlayer::Ready_Script()
{
	for (auto iter = m_vBattleScript.begin(); iter != m_vBattleScript.end();)
		iter = m_vBattleScript.erase(iter);

	m_vBattleScript.clear();

	wstring strTextBegin = TEXT("가랏 !!   '");
	wstring strTextEnd = TEXT("'   너로 정했다!!");

	_int iPokeIndex = 0;

	for (_int i = 0; i < 6; ++i)
	{
		if (dynamic_cast<CGameObj*>(m_pBag->Get_vecPoke(i))->Get_PokeInfo().eStatInfo != STUN)
		{
			iPokeIndex = i;
			i = 6;
		}
	}

	

	strTextBegin += dynamic_cast<CGameObj*>(m_pBag->Get_vecPoke(iPokeIndex))->Get_PokeInfo().strName;
	strTextBegin += strTextEnd;

	m_vBattleScript.push_back(strTextBegin);

}
void CPlayer::Check_Ball()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CLevel_GamePlay::LOADFILE tInfo;

	tInfo.pTarget = this;
	tInfo.pCamera = m_pvecTargetPoke->front();
	tInfo.vPos = m_vTargetBattlePos;

	switch (m_iCaptureBall)
	{
	case 0:
		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_AnimMonsterBall"), LEVEL_GAMEPLAY, TEXT("CaptureBall"), &tInfo)))
			return;
		break;
	case 1:
		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_AnimSuperBall"), LEVEL_GAMEPLAY, TEXT("CaptureBall"), &tInfo)))
			return;
		break;
	case 2:
		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_AnimHyperBall"), LEVEL_GAMEPLAY, TEXT("CaptureBall"), &tInfo)))
			return;
		break;
	case 3:
		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_AnimMasterBall"), LEVEL_GAMEPLAY, TEXT("CaptureBall"), &tInfo)))
			return;
		break;
	default:
		break;
	}

	RELEASE_INSTANCE(CGameInstance);

}
void CPlayer::Battle_Win()
{
	for (_uint i = 0; i < 6; ++i)
	{
		dynamic_cast<CGameObj*>(m_pBag->Get_vecPoke(i))->Reset_Anim();
	}
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION,XMLoadFloat4(&m_vPrevPos));
	m_bBattleText = false;
	m_bBattleStart = false;
	m_bBattle = false;
	m_fStartBattle = 0.f;
	m_fBattleUITime = 0.f;
	m_bBattleUI = false;
	m_bPrevPos = false;
	m_PlayerInfo.bEvent = false;
}
void CPlayer::Check_Anim(_float fTimeDelta)
{
	if (m_iAnimIndex == 1 || m_iAnimIndex == 2 || m_iAnimIndex == 8)
	{
		m_bChangeAnim = true;
		if (m_ChangePoke && m_fStartBattle > 1.8f)
		{
			m_bEffect = false;
			m_pModelCom->Set_End(m_iAnimIndex);
			m_iAnimIndex = 0;
			m_pModelCom->Set_CurrentAnimIndex(m_iAnimIndex);
			m_bChangeAnim = false;
			m_ChangePoke = false;
			dynamic_cast<CBall*>(m_pBall)->Set_Reset();
		}
		if (!m_ChangePoke && m_iAnimIndex == 2)
		{
			m_pModelCom->Set_Loop(m_iAnimIndex);
			m_pModelCom->Set_CurrentAnimIndex(m_iAnimIndex);
			m_ChangePoke = true;
			m_fStartBattle = 0.f;
		}
		if (m_bCapture && m_pModelCom->Get_End(m_iAnimIndex))
		{
			m_pModelCom->Set_End(m_iAnimIndex);
			m_iAnimIndex = 0;
			m_pModelCom->Set_CurrentAnimIndex(m_iAnimIndex);
			m_bChangeAnim = false;
			m_bCapture = false;
			m_bCaptureBall = false;
			Check_Ball();
			dynamic_cast<CBall*>(m_pBall)->Set_Reset();
			g_bCaptureRender = true;
		}
		if (!m_bCapture && m_iAnimIndex == 1)
		{
			m_pModelCom->Set_Loop(m_iAnimIndex);
			m_pModelCom->Set_CurrentAnimIndex(m_iAnimIndex);
			m_bCapture = true;
			m_fStartBattle = 0.f;
		}
		if (m_bReturnPoke && m_pModelCom->Get_End(m_iAnimIndex))
		{
			m_pModelCom->Set_End(m_iAnimIndex);
			m_iAnimIndex = 0;
			m_pModelCom->Set_CurrentAnimIndex(m_iAnimIndex);
			m_bChangeAnim = false;
			m_bReturnPoke = false;
			m_bReturn = false;
			dynamic_cast<CBall*>(m_pBall)->Set_Reset();
		}
		if (!m_bReturnPoke && m_iAnimIndex == 8)
		{
			m_pModelCom->Set_Loop(m_iAnimIndex);
			m_pModelCom->Set_CurrentAnimIndex(m_iAnimIndex);
			m_bReturnPoke = true;
			m_bTakeBall = false;
			m_fStartBattle = 0.f;
		}

	}
	m_fStartBattle += fTimeDelta;
	if (m_ChangePoke && m_bChangeAnim)
	{
		m_pModelCom->Play_Animation(fTimeDelta*0.8f);
		dynamic_cast<CBall*>(m_pBall)->Set_Render(true, m_iBallIndex);
		m_pBall->Tick(fTimeDelta);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, m_pBall);
		if (m_fStartBattle > 1.4f && !m_bEffect)
		{
			_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
			XMVector3Normalize(vLook);
			_vector vPos = XMLoadFloat4(&m_vMyBattlePos) + vLook * 35.f;
			_vector vTargetPos = XMLoadFloat4(&m_vMyBattlePos) + vLook * 200.f;

			CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

			CLevel_GamePlay::LOADFILE tInfo;

			XMStoreFloat4(&tInfo.vPos, vPos);

			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_BallEffect"), LEVEL_GAMEPLAY, TEXT("Layer_UI"), &tInfo)))
				return;

			RELEASE_INSTANCE(CGameInstance);
			m_bEffect = true;
		}
		if (m_fStartBattle > 1.8f)
		{
			_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
			XMVector3Normalize(vLook);
			_vector vPos = XMLoadFloat4(&m_vMyBattlePos) + vLook * 35.f;
			_vector vTargetPos = XMLoadFloat4(&m_vMyBattlePos) + vLook * 200.f;

			dynamic_cast<CGameObj*>(m_pBag->Get_vecPoke(m_pBag->Get_iChangePoke()))->Get_Transfrom()->Set_State(CTransform::STATE_TRANSLATION, vPos);
			dynamic_cast<CGameObj*>(m_pBag->Get_vecPoke(m_pBag->Get_iChangePoke()))->Get_Transfrom()->LookAt(vTargetPos);
			dynamic_cast<CGameObj*>(m_pBag->Get_vecPoke(m_pBag->Get_iChangePoke()))->Set_AnimIndex(0);
			dynamic_cast<CGameObj*>(m_pBag->Get_vecPoke(m_pBag->Get_iChangePoke()))->Set_BattleMap(true, 0.f);
	
		}
	}
	if (m_bReturnPoke && m_bChangeAnim)
	{
		m_pModelCom->Play_Animation(fTimeDelta*0.6f);
		if (!m_bTakeBall)
		{
			_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
			XMVector3Normalize(vLook);
			_vector vPos = XMLoadFloat4(&m_vMyBattlePos) + vLook * 35.f;
			_float4 vPos2;
			XMStoreFloat4(&vPos2, vPos);
			dynamic_cast<CBall*>(m_pBall)->Set_Render2(true, m_iPrevBall, true, vPos2);
			m_bTakeBall = true;
		}
		m_pBall->Tick(fTimeDelta);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, m_pBall);
		if (m_fStartBattle > 1.0f)
			m_bReturn = true;
	}
	if (m_bCapture && m_bChangeAnim)
	{
		if (!m_bCaptureBall)
		{
			m_fCaptureTime += fTimeDelta;
			if (m_fCaptureTime > 2.2f)
			{
				CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

				CLevel_GamePlay::LOADFILE tInfo;

				_float4 vPos = m_vTargetBattlePos;

				vPos.y += 1.f;

				tInfo.vPos = vPos;

				if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_BallRed2"), LEVEL_GAMEPLAY, TEXT("Layer_Effect"), &tInfo)))
					return;

				RELEASE_INSTANCE(CGameInstance);
				m_bCaptureBall = true;
				m_fCaptureTime = 0.f;
			}
		}
		m_pModelCom->Play_Animation(fTimeDelta*0.8f);
		dynamic_cast<CBall*>(m_pBall)->Set_Render3(true, m_iCaptureBall);
		m_pBall->Tick(fTimeDelta);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, m_pBall);
	}
}
void CPlayer::OnNavi()
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
HRESULT CPlayer::SetUp_ShaderResources()
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
HRESULT CPlayer::Ready_Parts()
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

CPlayer * CPlayer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayer*	pInstance = new CPlayer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CPlayer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CPlayer::Clone(void * pArg)
{
	CGameObject*	pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CPlayer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();

	for (auto iter = m_vBattleScript.begin(); iter != m_vBattleScript.end();)
		iter = m_vBattleScript.erase(iter);

	m_vBattleScript.clear();
	Safe_Release(m_pBall);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pAABBCom);
	Safe_Release(m_pOBBCom);
	Safe_Release(m_pModelCom);
}
