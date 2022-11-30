#include "stdafx.h"
#include "..\Public\Sonia.h"

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

CSonia::CSonia(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
}

CSonia::CSonia(const CSonia & rhs)
	: CGameObj(rhs)
{
}

HRESULT CSonia::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSonia::Initialize(void * pArg)
{

	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_pTarget = *(&((CLevel_GamePlay::LOADFILE*)pArg)->pTarget);
	m_pCamera = *(&((CLevel_GamePlay::LOADFILE*)pArg)->pCamera);
	m_vMyBattlePos = dynamic_cast<CGameObj*>(m_pTarget)->Get_TargetBattlePos();


	m_iPokeMaxSize = 0;
	m_iPokeSize = 0;
	m_PlayerInfo.strName = L"소니아";
	m_PlayerInfo.bEvent = false;
	m_PlayerInfo.bBattle = false;

	m_pModelCom->Set_CurrentAnimIndex(0);
	m_pTransformCom->Turn2(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f));
	//m_pTransformCom->Set_Scale(XMLoadFloat3((&((CLevel_GamePlay::LOADFILE*)pArg)->vScale)));
	m_pTransformCom->Set_Scale(XMVectorSet(0.055f, 0.055f, 0.055f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4((&((CLevel_GamePlay::LOADFILE*)pArg)->vPos)));

	Ready_Script();
	m_pNavigationCom->Find_CurrentCellIndex(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

	return S_OK;
}

void CSonia::Tick(_float fTimeDelta)
{
	if (!g_bEvolution)
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

void CSonia::Late_Tick(_float fTimeDelta)
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

HRESULT CSonia::Render()
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
HRESULT CSonia::Ready_Components()
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
	if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_STATIC, TEXT("Sonia"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC		ColliderDesc;

	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vScale = _float3(20.f, 40.f, 20.f);
	ColliderDesc.vPosition = _float3(0.f, 20.f, 0.f);
	if (FAILED(__super::Add_Components(TEXT("Com_AABB"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"), (CComponent**)&m_pAABBCom, &ColliderDesc)))
		return E_FAIL;

	/* For.Com_OBB*/
	ColliderDesc.vScale = _float3(40.f, 40.f, 200.f);
	ColliderDesc.vPosition = _float3(0.f, 20.f, 80.f);
	if (FAILED(__super::Add_Components(TEXT("Com_OBB"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"), (CComponent**)&m_pOBBCom, &ColliderDesc)))
		return E_FAIL;

	CNavigation::NAVIDESC			NaviDesc;
	ZeroMemory(&NaviDesc, sizeof NaviDesc);

	NaviDesc.iCurrentCellIndex = 0;

	if (FAILED(__super::Add_Components(TEXT("Com_Navigation"), LEVEL_STATIC, TEXT("Prototype_Component_Navigation"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
		return E_FAIL;

	return S_OK;
}

void CSonia::Move(_float fTimeDelta)
{
	m_pModelCom->Set_CurrentAnimIndex(2);
	_vector vTargetPos = dynamic_cast<CGameObj*>(m_pTarget)->Get_Transfrom()->Get_State(CTransform::STATE_TRANSLATION);
	_vector vLook = vTargetPos - m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	vPos += XMVector3Normalize(vLook) * 4.f * fTimeDelta;
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vPos);
	m_pTransformCom->LookAt(vTargetPos);
}

void CSonia::Ready_Script()
{
	m_vNormalScript.push_back(TEXT("앗! 잠깐만!!"));
	m_vNormalScript.push_back(TEXT("넌 이세카이 사람이 아니구나?\n어이쿠... 어쩌다 이런 험난한 세카이로 오게된거야."));
	m_vNormalScript.push_back(TEXT("이 너머에는 다양한 포켓몬들이 존재하지"));
	m_vNormalScript.push_back(TEXT("아무런 준비없이 갔다간 죽음뿐이야..."));
	m_vNormalScript.push_back(TEXT("원래 세카이로 돌아가고 싶으면\n배지 10개를 모아서 챔피온에게 가봐."));
	m_vNormalScript.push_back(TEXT("챔피온이라면 뭔가 방법을 알고 있을지도 몰라..."));
	m_vNormalScript.push_back(TEXT("인심 썼다...\n이걸 줄게 이 녀석이 널 지켜줄지도 몰라."));

}


void CSonia::OnNavi()
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
void CSonia::Ckeck_Dist()
{
	_vector vTargetPos = dynamic_cast<CGameObj*>(m_pTarget)->Get_Transfrom()->Get_State(CTransform::STATE_TRANSLATION);
	_vector vLook = vTargetPos - m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	m_fDist = XMVectorGetX(XMVector3Length(vLook));
}
void CSonia::Check_Coll()
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
		m_fEventTime = 0.f;
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
		tTInfo.iType = 3;
		for (_int i = 0; i < m_vNormalScript.size(); ++i)
			tTInfo.pScript[i] = m_vNormalScript[i];

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_TextBox"), LEVEL_GAMEPLAY, TEXT("Layer_UI"), &tTInfo)))
			return;
	}
	if (m_pAABBCom->Collision(pTargetCollider))
	{
		m_bFindPlayer = false;

		if (!m_bGiveItem)
		{
			if (dynamic_cast<CGameObj*>(m_pTarget)->Get_Event())
				dynamic_cast<CGameObj*>(m_pTarget)->OnOffEvent();

			m_bGiveItem = true;

			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Mew"), LEVEL_STATIC, TEXT("Layer_Pokemon"), &m_GivePoke)))
				return;
			for (_int i = 0; i < 6; ++i)
			{
				if (dynamic_cast<CGameObj*>(dynamic_cast<CPlayer*>(m_pTarget)->Get_Bag()->Get_vecPoke(i))->Get_PokeInfo().iPokeNum == 999)
				{
					dynamic_cast<CPlayer*>(m_pTarget)->Get_Bag()->Set_vecPoke(i, m_GivePoke);
					break;
				}
			}
		}
	}
	RELEASE_INSTANCE(CGameInstance);
}



HRESULT CSonia::SetUp_ShaderResources()
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

CSonia * CSonia::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSonia*	pInstance = new CSonia(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CSonia"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CSonia::Clone(void * pArg)
{
	CGameObject*	pInstance = new CSonia(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CSonia"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSonia::Free()
{
	__super::Free();

	for (auto iter = m_vNormalScript.begin(); iter != m_vNormalScript.end();)
		iter = m_vNormalScript.erase(iter);

	m_vNormalScript.clear();


	Safe_Release(m_pNavigationCom);;
	Safe_Release(m_pAABBCom);
	Safe_Release(m_pOBBCom);
	Safe_Release(m_pModelCom);
}
