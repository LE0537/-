#include "stdafx.h"
#include "..\Public\Player.h"
#include "GameInstance.h"
#include "Level_GamePlay.h"
#include "SoundMgr.h"
#include "Bag.h"
#include "TextBox.h"
#include "VIBuffer_Navigation.h"
#include "BattleUI.h"

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
	
	
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	m_PlayerInfo.strName = L"쥬신";
	m_PlayerInfo.bEvent = false;
	m_PlayerInfo.bBattle = false;
	m_PlayerInfo.bRide = false;

	RELEASE_INSTANCE(CGameInstance);
	m_pTransformCom->Set_Scale(XMLoadFloat3((&((CLevel_GamePlay::LOADFILE*)pArg)->vScale)));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4((&((CLevel_GamePlay::LOADFILE*)pArg)->vPos)));
	m_pModelCom->Set_CurrentAnimIndex(IDLE);
	
	return S_OK;
}

void CPlayer::Tick(_float fTimeDelta)
{
	if(g_Battle)
		Battle(fTimeDelta);
	else
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
	m_pModelCom->Play_Animation(fTimeDelta);
}

void CPlayer::Late_Tick(_float fTimeDelta)
{
	if (g_Battle && m_PlayerInfo.bRide)
		m_PlayerInfo.bRide = false;

	if (!g_PokeInfo && !g_bPokeDeck && nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
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

	if (g_CollBox)
	{
		m_pAABBCom->Render();
		m_pOBBCom->Render();
		m_pNavigationCom->Render_Navigation();
	}
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
	if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_STATIC, TEXT("Prototype_Component_Model_Player"), (CComponent**)&m_pModelCom)))
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
	case 143:
		m_pModelCom->Set_CurrentAnimIndex(RIDE_SNORLAX);
		break;
	default:
		break;
	}
}

void CPlayer::Key_Input(_float fTimeDelta)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	if (!m_PlayerInfo.bRide)
	{
		if (pGameInstance->Key_Pressing(DIK_LSHIFT) && pGameInstance->Key_Pressing(DIK_W))
		{
			m_pTransformCom->Go_Straight(fTimeDelta * 1.3f, m_pNavigationCom);
			m_pModelCom->Set_CurrentAnimIndex(RUN);
		}
		else if (pGameInstance->Key_Pressing(DIK_W))
		{
			m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);
			m_pModelCom->Set_CurrentAnimIndex(WALK);
		}

	}
	if (m_PlayerInfo.bRide)
	{
		if (pGameInstance->Key_Pressing(DIK_W))
		{
			m_pTransformCom->Go_Straight(fTimeDelta * 1.8f, m_pNavigationCom);
			switch (m_PlayerInfo.iRideNum)
			{
			case 143:
				m_pModelCom->Set_CurrentAnimIndex(RUN_SNORLAX);
				break;
			default:
				break;
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
	if (pGameInstance->Key_Pressing(DIK_O))
	{
		--m_iAnim;
		if (m_iAnim < 0)
			m_iAnim = 0;
		m_pModelCom->Set_CurrentAnimIndex(m_iAnim);
	}
	if (pGameInstance->Key_Pressing(DIK_P))
	{
		++m_iAnim;
		if (m_iAnim > 12)
			m_iAnim = 12;
		m_pModelCom->Set_CurrentAnimIndex(m_iAnim);
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
	m_fStartBattle += fTimeDelta;
	if (!m_bBattle)
	{
		m_pModelCom->Set_Loop(2);
		m_pModelCom->Set_CurrentAnimIndex(2);
		m_pModelCom->Play_Animation(fTimeDelta*0.8f);

		if (m_fStartBattle > 1.8f)
		{
			_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
			XMVector3Normalize(vLook);
			_vector vPos = XMLoadFloat4(&m_vMyBattlePos) + vLook * 35.f;
			_vector vTargetPos = XMLoadFloat4(&m_vMyBattlePos) + vLook * 200.f;
			dynamic_cast<CGameObj*>(m_pBag->Get_vecPoke(0))->Get_Transfrom()->Set_State(CTransform::STATE_TRANSLATION, vPos);
			dynamic_cast<CGameObj*>(m_pBag->Get_vecPoke(0))->Get_Transfrom()->LookAt(vTargetPos);
			dynamic_cast<CGameObj*>(m_pBag->Get_vecPoke(0))->Set_AnimIndex(0);
			dynamic_cast<CGameObj*>(m_pBag->Get_vecPoke(0))->Set_BattleMap(true,2.f);
		}
		if ((m_fStartBattle > 0.2f) && m_pModelCom->Get_End(2))
		{
			m_bBattle = true;
			m_pModelCom->Set_End(2);
			m_fBattleUITime = 0.f;
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
	wstring strTextBegin = TEXT("가랏 !!   '");
	wstring strTextEnd = TEXT("'   너로 정했다!!");

	
	strTextBegin += dynamic_cast<CGameObj*>(m_pBag->Get_SelectPoke())->Get_PokeInfo().strName;
	strTextBegin += strTextEnd;

	m_vBattleScript.push_back(strTextBegin);

}
void CPlayer::Battle_Win()
{
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
	if (m_iAnimIndex == 2)
	{
		m_fStartBattle += fTimeDelta;
		m_bChangeAnim = true;
		if (m_ChangePoke && m_pModelCom->Get_End(m_iAnimIndex))
		{
			m_pModelCom->Set_End(m_iAnimIndex);
			m_iAnimIndex = 0;
			m_pModelCom->Set_CurrentAnimIndex(m_iAnimIndex);
			m_bChangeAnim = false;
			m_ChangePoke = false;
		}
		if (!m_ChangePoke && m_iAnimIndex == 2)
		{
			m_pModelCom->Set_Loop(m_iAnimIndex);
			m_pModelCom->Set_CurrentAnimIndex(m_iAnimIndex);
			m_ChangePoke = true;
			m_fStartBattle = 0.f;
		}
	}
	if (m_ChangePoke && m_bChangeAnim)
	{
		m_pModelCom->Play_Animation(fTimeDelta*0.8f);
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

	if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_World4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
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

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pAABBCom);
	Safe_Release(m_pOBBCom);
	Safe_Release(m_pModelCom);
}
