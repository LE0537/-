#include "stdafx.h"
#include "..\Public\Race.h"

#include "GameInstance.h"
#include "Level_GamePlay.h"
#include "Data_Manager.h"	// 추가
#include "TextBox.h"
#include "SoundMgr.h"

CRace::CRace(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
}

CRace::CRace(const CRace & rhs)
	: CGameObj(rhs)
{
}

HRESULT CRace::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CRace::Initialize(void * pArg)
{
	*(CGameObject**)(&((CLevel_GamePlay::LOADFILE*)pArg)->pRaceMap) = this;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_PlayerInfo.strName = L"병규";

	m_pTarget = *(&((CLevel_GamePlay::LOADFILE*)pArg)->pTarget);

	m_pTransformCom->Set_Scale(XMLoadFloat3((&((CLevel_GamePlay::LOADFILE*)pArg)->vScale)));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4((&((CLevel_GamePlay::LOADFILE*)pArg)->vPos)));

	return S_OK;
}

void CRace::Tick(_float fTimeDelta)
{
	if (g_bRace)
	{
		if (!m_bRaceStart)
		{
			Create_RacePoke();
		}
		else if (m_bRaceStart)
		{
			if (!m_bRaceEnd)
				Check_RacePoke(fTimeDelta);
			else
			{
				m_fLeafTime += fTimeDelta;
				if (m_fLeafTime > 0.01f)
				{
					CLevel_GamePlay::LOADFILE tInfo;

					_float4 vPos = m_vRaceEndPos;
					vPos.x += rand() % 100 - 50.f;
					vPos.y += 10.f;
					vPos.z += rand() % 100 - 50.f;

					tInfo.vPos = vPos;

					m_fLeafTime = 0.f;
					CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

					if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_RaceStar"), LEVEL_GAMEPLAY, TEXT("Effect"), &tInfo)))
						return;

					RELEASE_INSTANCE(CGameInstance);
				}
			}
		}
	}
	else if(!g_bRace)
		RaceEnd();
}

void CRace::Late_Tick(_float fTimeDelta)
{
	if (g_bRace)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
		Compute_CamDistance(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	}
}

HRESULT CRace::Render()
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

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 1)))
			return E_FAIL;

	}

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CRace::Render_ShadowDepth()
{
	return S_OK;
}

void CRace::Set_RaceInfo(_float4 vPos, _int iType)
{
	CCamera_Dynamic::RaceInfo tInfo;
	tInfo.iType = iType;
	tInfo.vPos = vPos;
	m_vecRace.push_back(tInfo);
}
void CRace::Ready_Script()
{
	for (auto iter = m_vBattleScript.begin(); iter != m_vBattleScript.end();)
		iter = m_vBattleScript.erase(iter);

	m_vBattleScript.clear();

	wstring strTextBegin = TEXT("신사숙녀 여러분~ 반갑습니다!\n지금부터 포켓몬 경마를");
	


	m_vBattleScript.push_back(strTextBegin);
	m_vBattleScript.push_back(TEXT("시~~~~~이~~~~~작!!하겠습니다!!!"));
	m_vBattleScript.push_back(TEXT("그럼! 오늘 출전하는 포켓몬 소개가 있겠습니다!"));
}
void CRace::Ready_CheckRacePoke()
{
	for (auto iter = m_vBattleScript.begin(); iter != m_vBattleScript.end();)
		iter = m_vBattleScript.erase(iter);

	m_vBattleScript.clear();

	wstring strRacePoke1 = TEXT("");
	wstring strRacePoke2 = TEXT("");
	wstring strRacePoke3 = TEXT("");
	wstring strRacePoke4 = TEXT("");

	auto& iter = m_listRacePoke.begin();

	for (_int i = 0 ; i < m_listRacePoke.size();++i,++iter)
	{
		if (i == 0)
			strRacePoke1 = dynamic_cast<CGameObj*>(*iter)->Get_PokeInfo().strName;
		if (i == 1)
			strRacePoke2 = dynamic_cast<CGameObj*>(*iter)->Get_PokeInfo().strName;
		if (i == 2)
			strRacePoke3 = dynamic_cast<CGameObj*>(*iter)->Get_PokeInfo().strName;
		if (i == 3)
			strRacePoke4 = dynamic_cast<CGameObj*>(*iter)->Get_PokeInfo().strName;

	}
	wstring strRace1 = TEXT("오늘의 1번 포켓몬!! \n '");
	wstring strRace2 = TEXT("' 입니다!!");

	strRace1 += strRacePoke1;
	strRace1 += strRace2;
	
	m_vBattleScript.push_back(strRace1);

	strRace1 = TEXT("자~ 이어서 2번 포켓몬!! \n '");
	strRace2 = TEXT("' 입니다!!");

	strRace1 += strRacePoke2;
	strRace1 += strRace2;

	m_vBattleScript.push_back(strRace1);

	strRace1 = TEXT("3번 포켓몬!! \n '");
	strRace2 = TEXT("' 입니다!!");

	strRace1 += strRacePoke3;
	strRace1 += strRace2;

	m_vBattleScript.push_back(strRace1);

	strRace1 = TEXT("라스트 4번 포켓몬!! \n '");
	strRace2 = TEXT("' 입니다!!");

	strRace1 += strRacePoke4;
	strRace1 += strRace2;

	m_vBattleScript.push_back(strRace1);
}
void CRace::Ready_ChoiceRacePoke()
{
	for (auto iter = m_vBattleScript.begin(); iter != m_vBattleScript.end();)
		iter = m_vBattleScript.erase(iter);

	m_vBattleScript.clear();

	wstring strRacePoke1 = TEXT("");
	wstring strRacePoke2 = TEXT("");
	wstring strRacePoke3 = TEXT("");
	wstring strRacePoke4 = TEXT("");

	auto& iter = m_listRacePoke.begin();

	for (_int i = 0; i < m_listRacePoke.size(); ++i,++iter)
	{
		if (i == 0)
			strRacePoke1 = dynamic_cast<CGameObj*>(*iter)->Get_PokeInfo().strName;
		if (i == 1)
			strRacePoke2 = dynamic_cast<CGameObj*>(*iter)->Get_PokeInfo().strName;
		if (i == 2)
			strRacePoke3 = dynamic_cast<CGameObj*>(*iter)->Get_PokeInfo().strName;
		if (i == 3)
			strRacePoke4 = dynamic_cast<CGameObj*>(*iter)->Get_PokeInfo().strName;

	}

	m_vBattleScript.push_back(TEXT("자!! 지금부터 배팅 시작하겠습니다!"));

	wstring strRace1 = TEXT("1. '");
	wstring strRace2 = TEXT("'       2. '");
	wstring strRace3 = TEXT("'\n3. '");
	wstring strRace4 = TEXT("'       4. '");
	wstring strRace5 = TEXT("'");

	strRace1 += strRacePoke1;
	strRace1 += strRace2;
	strRace1 += strRacePoke2;
	strRace1 += strRace3;
	strRace1 += strRacePoke3;
	strRace1 += strRace4;
	strRace1 += strRacePoke4;
	strRace1 += strRace5;

	m_vBattleScript.push_back(strRace1);
}
void CRace::Ready_StartRace()
{
	for (auto iter = m_vBattleScript.begin(); iter != m_vBattleScript.end();)
		iter = m_vBattleScript.erase(iter);

	m_vBattleScript.clear();

	m_vBattleScript.push_back(TEXT("배팅 종료!!!\n레이스 시작 합니다!!"));
	
}
void CRace::Ready_EndRace()
{
	for (auto iter = m_vBattleScript.begin(); iter != m_vBattleScript.end();)
		iter = m_vBattleScript.erase(iter);

	m_vBattleScript.clear();

	wstring strRacePoke = TEXT("");


	auto& iter = m_listRacePoke.begin();

	for (_int i = 0; i < m_listRacePoke.size(); ++i,++iter)
	{
		if (i == m_iWinNum - 1)
			strRacePoke = dynamic_cast<CGameObj*>(*iter)->Get_PokeInfo().strName;
		
	}

	wstring strbegin = TEXT("오늘 포켓몬경마 우승은 바로바로~~\n '");
	wstring strEnd = TEXT("'  입니다!! 축하합니다!");

	strbegin += strRacePoke;
	strbegin += strEnd;

	m_vBattleScript.push_back(TEXT("경기 종료!!!"));
	m_vBattleScript.push_back(strbegin);
	m_vBattleScript.push_back(TEXT("배팅 성공 하신 분들은 모두모두 축하드립니다.\n안되신 분들은 다음 경기 때 또 봐요!!"));
}
void CRace::Check_RacePoke(_float fTimeDelta)
{
	m_fTextTime += fTimeDelta;
	if (!m_bCheckRaceText)
	{
		if (m_fTextTime > 8.f)
		{
			CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
			Ready_CheckRacePoke();
			CTextBox::TINFO tTInfo;

			tTInfo.iScriptSize = (_int)m_vBattleScript.size();
			tTInfo.pTarget = this;
			tTInfo.pScript = new wstring[m_vBattleScript.size()];
			tTInfo.iType = 10;
			for (_int i = 0; i < m_vBattleScript.size(); ++i)
				tTInfo.pScript[i] = m_vBattleScript[i];

			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_TextBox"), LEVEL_GAMEPLAY, TEXT("Layer_UI"), &tTInfo)))
				return;

			RELEASE_INSTANCE(CGameInstance);

			m_bCheckRaceText = true;
			m_fTextTime = 0.f;
		}
	}
	else if (m_bCheckRaceText)
	{
		if (!m_bChoiceRacePoke && m_fTextTime > 12.f)
		{
			CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
			Ready_ChoiceRacePoke();
			CTextBox::TINFO tTInfo;

			tTInfo.iScriptSize = (_int)m_vBattleScript.size();
			tTInfo.pTarget = this;
			tTInfo.pScript = new wstring[m_vBattleScript.size()];
			tTInfo.iType = 11;
			for (_int i = 0; i < m_vBattleScript.size(); ++i)
				tTInfo.pScript[i] = m_vBattleScript[i];

			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_TextBox"), LEVEL_GAMEPLAY, TEXT("Layer_UI"), &tTInfo)))
				return;

			RELEASE_INSTANCE(CGameInstance);
			m_bChoiceRacePoke = true;
		}
	}
	if (g_iRaceNum != 99 && !m_bStartText)
	{
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
		Ready_StartRace();
		CTextBox::TINFO tTInfo;

		tTInfo.iScriptSize = (_int)m_vBattleScript.size();
		tTInfo.pTarget = this;
		tTInfo.pScript = new wstring[m_vBattleScript.size()];
		tTInfo.iType = 10;
		for (_int i = 0; i < m_vBattleScript.size(); ++i)
			tTInfo.pScript[i] = m_vBattleScript[i];

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_TextBox"), LEVEL_GAMEPLAY, TEXT("Layer_UI"), &tTInfo)))
			return;

		RELEASE_INSTANCE(CGameInstance);
		m_bStartText = true;
		m_fTextTime = 0.f;
	}
	else if (!m_bCount && m_bStartText && m_fTextTime > 3.5f)
	{
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_CountDown"), LEVEL_GAMEPLAY, TEXT("Layer_UI"))))
			return;

		RELEASE_INSTANCE(CGameInstance);

		CSoundMgr::Get_Instance()->PlayEffect(TEXT("CountDown.mp3"), 1.f);
		m_bCount = true;
		m_fTextTime = 0.f;
		m_fRandTime = 3.f;
	}
	else if (m_bCount && m_fTextTime > 4.f)
	{
		Run_RacePoke(fTimeDelta);
	}
}
void CRace::Run_RacePoke(_float fTimeDelta)
{
	m_fRandTime += fTimeDelta;
	if (m_fRandTime > 3.f)
	{
		m_fRand1 = _float(rand() % 80 + 1) * 0.1f;
		m_fRand2 = _float(rand() % 80 + 1) * 0.1f;
		m_fRand3 = _float(rand() % 80 + 1) * 0.1f;
		m_fRand4 = _float(rand() % 80 + 1) * 0.1f;
		m_fRandTime = 0.f;
	}
	if (fDist1 < 5.f || fDist2 < 5.f || fDist3 < 5.f || fDist4 < 5.f)
	{
		m_fRand1 = 6.f;
		m_fRand2 = 6.f;
		m_fRand3 = 6.f;
		m_fRand4 = 6.f;

		if (!m_bCheckWin && fDist1 < 5.f)
		{
			m_iWinNum = 1;
			m_bCheckWin = true;
		}
		else if (!m_bCheckWin && fDist2 < 5.f)
		{
			m_iWinNum = 2;
			m_bCheckWin = true;
		}
		else if (!m_bCheckWin && fDist3 < 5.f)
		{
			m_iWinNum = 3;
			m_bCheckWin = true;
		}
		else if (!m_bCheckWin && fDist4 < 5.f)
		{
			m_iWinNum = 4;
			m_bCheckWin = true;
		}
	}
	if (fDist1 < 0.3f && fDist2 < 0.3f && fDist3 < 0.3f && fDist4 < 0.3f)
	{
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
		Ready_EndRace();
		CTextBox::TINFO tTInfo;

		tTInfo.iScriptSize = (_int)m_vBattleScript.size();
		tTInfo.pTarget = this;
		tTInfo.pScript = new wstring[m_vBattleScript.size()];
		tTInfo.iType = 12;
		for (_int i = 0; i < m_vBattleScript.size(); ++i)
			tTInfo.pScript[i] = m_vBattleScript[i];

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_TextBox"), LEVEL_GAMEPLAY, TEXT("Layer_UI"), &tTInfo)))
			return;

		RELEASE_INSTANCE(CGameInstance);

		auto& iter = m_listRacePoke.begin();

		for (_int i = 0; i < m_listRacePoke.size(); ++i,++iter)
		{
			if (i == m_iWinNum - 1)
			{
				dynamic_cast<CGameObj*>(*iter)->Get_Transfrom()->Set_State(CTransform::STATE_TRANSLATION,XMLoadFloat4(&m_vRaceEndPos));
				dynamic_cast<CGameObj*>(*iter)->Get_Transfrom()->LookAt(XMLoadFloat4(&m_vRaceEndLook));
				dynamic_cast<CGameObj*>(*iter)->Set_AnimIndex(9);
			}
			else
			{
				dynamic_cast<CGameObj*>(*iter)->Get_Transfrom()->LookAt(XMLoadFloat4(&m_vRaceEndLook));
				dynamic_cast<CGameObj*>(*iter)->Set_AnimIndex(2);
			}
		}

		if (m_iWinNum == g_iRaceNum)
			dynamic_cast<CGameObj*>(m_pTarget)->Set_Money(2000);

		g_bRaceEnd = true;
		m_bRaceEnd = true;
		return;
	}

	auto& iter = m_listRacePoke.begin();

	_vector vPos,vLook;

	if (fDist1 > 0.3f)
	{
		if (m_fRand1 <= 4.f)
			dynamic_cast<CGameObj*>(*iter)->Set_AnimIndex(7);
		else
			dynamic_cast<CGameObj*>(*iter)->Set_AnimIndex(8);

		vPos = dynamic_cast<CGameObj*>(*iter)->Get_Transfrom()->Get_State(CTransform::STATE_TRANSLATION);
		vLook = XMLoadFloat4(&m_vGoal1) - vPos;
		fDist1 = XMVectorGetX(XMVector3Length(vLook));
		if (!m_bSlowMotion)
		{
			vPos += XMVector3Normalize(vLook) * m_fRand1 * fTimeDelta;
			dynamic_cast<CGameObj*>(*iter)->Set_SlowMotionClear();
		}
		else
		{
			vPos += XMVector3Normalize(vLook) * m_fRand1 * fTimeDelta * 0.3f;
			dynamic_cast<CGameObj*>(*iter)->Set_SlowMotion();
		}
		dynamic_cast<CGameObj*>(*iter)->Get_Transfrom()->Set_State(CTransform::STATE_TRANSLATION, vPos);
	}
	else
	{
		dynamic_cast<CGameObj*>(*iter)->Set_AnimIndex(2);
	}

	++iter;

	if (fDist2 > 0.3f)
	{
		if (m_fRand2 <= 4.f)
			dynamic_cast<CGameObj*>(*iter)->Set_AnimIndex(7);
		else
			dynamic_cast<CGameObj*>(*iter)->Set_AnimIndex(8);

		vPos = dynamic_cast<CGameObj*>(*iter)->Get_Transfrom()->Get_State(CTransform::STATE_TRANSLATION);
		vLook = XMLoadFloat4(&m_vGoal2) - vPos;
		fDist2 = XMVectorGetX(XMVector3Length(vLook));
		if (!m_bSlowMotion)
		{
			vPos += XMVector3Normalize(vLook) * m_fRand2 * fTimeDelta;
			dynamic_cast<CGameObj*>(*iter)->Set_SlowMotionClear();
		}
		else
		{
			vPos += XMVector3Normalize(vLook) * m_fRand2 * fTimeDelta * 0.3f;
			dynamic_cast<CGameObj*>(*iter)->Set_SlowMotion();
		}
		dynamic_cast<CGameObj*>(*iter)->Get_Transfrom()->Set_State(CTransform::STATE_TRANSLATION, vPos);
	}
	else
	{
		dynamic_cast<CGameObj*>(*iter)->Set_AnimIndex(2);
	}

	++iter;

	if (fDist3 > 0.3f)
	{
		if (m_fRand3 <= 4.f)
			dynamic_cast<CGameObj*>(*iter)->Set_AnimIndex(7);
		else
			dynamic_cast<CGameObj*>(*iter)->Set_AnimIndex(8);

		vPos = dynamic_cast<CGameObj*>(*iter)->Get_Transfrom()->Get_State(CTransform::STATE_TRANSLATION);
		vLook = XMLoadFloat4(&m_vGoal3) - vPos;
		fDist3 = XMVectorGetX(XMVector3Length(vLook));
		if (!m_bSlowMotion)
		{
			vPos += XMVector3Normalize(vLook) * m_fRand3 * fTimeDelta;
			dynamic_cast<CGameObj*>(*iter)->Set_SlowMotionClear();
		}
		else
		{
			vPos += XMVector3Normalize(vLook) * m_fRand3 * fTimeDelta * 0.3f;
			dynamic_cast<CGameObj*>(*iter)->Set_SlowMotion();
		}
		dynamic_cast<CGameObj*>(*iter)->Get_Transfrom()->Set_State(CTransform::STATE_TRANSLATION, vPos);
	}
	else
	{
		dynamic_cast<CGameObj*>(*iter)->Set_AnimIndex(2);
	}

	++iter;

	if (fDist4 > 0.3f)
	{
		if (m_fRand4 <= 4.f)
			dynamic_cast<CGameObj*>(*iter)->Set_AnimIndex(7);
		else
			dynamic_cast<CGameObj*>(*iter)->Set_AnimIndex(8);

		vPos = dynamic_cast<CGameObj*>(*iter)->Get_Transfrom()->Get_State(CTransform::STATE_TRANSLATION);
		vLook = XMLoadFloat4(&m_vGoal4) - vPos;
		fDist4 = XMVectorGetX(XMVector3Length(vLook));
		if (!m_bSlowMotion)
		{
			vPos += XMVector3Normalize(vLook) * m_fRand4 * fTimeDelta;
			dynamic_cast<CGameObj*>(*iter)->Set_SlowMotionClear();
		}
		else
		{
			vPos += XMVector3Normalize(vLook) * m_fRand4 * fTimeDelta * 0.3f;
			dynamic_cast<CGameObj*>(*iter)->Set_SlowMotion();
		}
		dynamic_cast<CGameObj*>(*iter)->Get_Transfrom()->Set_State(CTransform::STATE_TRANSLATION, vPos);
	}
	else
	{
		dynamic_cast<CGameObj*>(*iter)->Set_AnimIndex(2);
	}

	if (!m_bGoal && !m_bSlowMotion)
	{
		if(fDist1 < 10.5f || fDist2 < 10.5f || fDist3 < 10.5f || fDist4 < 10.5f)
			m_bSlowMotion = true;
	}
	if (m_bSlowMotion)
	{
		if (fDist1 < 5.f || fDist2 < 5.f || fDist3 < 5.f || fDist4 < 5.f)
			m_bSlowMotion = false;

		m_bGoal = true;
	}

}

void CRace::RaceEnd()
{
	if (!m_bReset)
	{
		for (auto& iter : m_listRacePoke)
		{
			iter->Set_Dead();
		}
		m_listRacePoke.clear();

	    m_bRaceStart = false;
		m_fTextTime = 0.f;
		m_bCheckRaceText = false;
		m_bChoiceRacePoke = false;
		m_bStartText = false;
		m_bCount = false;
		m_bSlowMotion = false;
		m_bGoal = false;
		m_bRaceEnd = false;
		m_bCheckWin = false;

		m_fRand1 = 0.f;
		m_fRand2 = 0.f;
		m_fRand3 = 0.f;
		m_fRand4 = 0.f;
		m_fRandTime = 0.f;

		fDist1 = 9999.f;
		fDist2 = 9999.f;;
		fDist3 = 9999.f;;
		fDist4 = 9999.f;;

		m_iWinNum = 0;

		g_iRaceNum = 99;
		g_bRaceEnd = false;

		m_bReset = true;
	}
}

HRESULT CRace::RandPoke()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_int iDest[4];

	for (_int i = 0; i < 4; ++i) 
	{	
		iDest[i] = rand() % 27;
		for (_int j = 0; j < i; ++j)
		{
			if (iDest[i] == iDest[j]) 
				--i;
		}
	}

	CGameObject* tInfo;
	for (_int i = 0; i < 4; ++i)
	{
		switch (iDest[i])
		{
		case 0:
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Squirtle"), LEVEL_STATIC, TEXT("Layer_Pokemon"), &tInfo)))
				return E_FAIL;
			break;
		case 1:
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Wartortle"), LEVEL_STATIC, TEXT("Layer_Pokemon"), &tInfo)))
				return E_FAIL;
			break;
		case 2:
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Blastoise"), LEVEL_STATIC, TEXT("Layer_Pokemon"), &tInfo)))
				return E_FAIL;
			break;
		case 3:
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Caterpie"), LEVEL_STATIC, TEXT("Layer_Pokemon"), &tInfo)))
				return E_FAIL;
			break;
		case 4:
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Metapod"), LEVEL_STATIC, TEXT("Layer_Pokemon"), &tInfo)))
				return E_FAIL;
			break;
		case 5:
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Butterfree"), LEVEL_STATIC, TEXT("Layer_Pokemon"), &tInfo)))
				return E_FAIL;
			break;
		case 6:
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Rattata"), LEVEL_STATIC, TEXT("Layer_Pokemon"), &tInfo)))
				return E_FAIL;
			break;
		case 7:
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Raticate"), LEVEL_STATIC, TEXT("Layer_Pokemon"), &tInfo)))
				return E_FAIL;
			break;
		case 8:
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Pikachu"), LEVEL_STATIC, TEXT("Layer_Pokemon"), &tInfo)))
				return E_FAIL;
			break;
		case 9:
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Jigglypuff"), LEVEL_STATIC, TEXT("Layer_Pokemon"), &tInfo)))
				return E_FAIL;
			break;
		case 10:
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Wigglytuff"), LEVEL_STATIC, TEXT("Layer_Pokemon"), &tInfo)))
				return E_FAIL;
			break;
		case 11:
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Meowth"), LEVEL_STATIC, TEXT("Layer_Pokemon"), &tInfo)))
				return E_FAIL;
			break;
		case 12:
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Persian"), LEVEL_STATIC, TEXT("Layer_Pokemon"), &tInfo)))
				return E_FAIL;
			break;
		case 13:
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Slowpoke"), LEVEL_STATIC, TEXT("Layer_Pokemon"), &tInfo)))
				return E_FAIL;
			break;
		case 14:
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Slowbro"), LEVEL_STATIC, TEXT("Layer_Pokemon"), &tInfo)))
				return E_FAIL;
			break;
		case 15:
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Onix"), LEVEL_STATIC, TEXT("Layer_Pokemon"), &tInfo)))
				return E_FAIL;
			break;
		case 16:
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Lickitung"), LEVEL_STATIC, TEXT("Layer_Pokemon"), &tInfo)))
				return E_FAIL;
			break;
		case 17:
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Chansey"), LEVEL_STATIC, TEXT("Layer_Pokemon"), &tInfo)))
				return E_FAIL;
			break;
		case 18:
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Kangaskhan"), LEVEL_STATIC, TEXT("Layer_Pokemon"), &tInfo)))
				return E_FAIL;
			break;
		case 19:
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Scyther"), LEVEL_STATIC, TEXT("Layer_Pokemon"), &tInfo)))
				return E_FAIL;
			break;
		case 20:
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Magikarp"), LEVEL_STATIC, TEXT("Layer_Pokemon"), &tInfo)))
				return E_FAIL;
			break;
		case 21:
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Gyarados"), LEVEL_STATIC, TEXT("Layer_Pokemon"), &tInfo)))
				return E_FAIL;
			break;
		case 22:
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Snorlax"), LEVEL_STATIC, TEXT("Layer_Pokemon"), &tInfo)))
				return E_FAIL;
			break;
		case 23:
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Dratini"), LEVEL_STATIC, TEXT("Layer_Pokemon"), &tInfo)))
				return E_FAIL;
			break;
		case 24:
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Dragonair"), LEVEL_STATIC, TEXT("Layer_Pokemon"), &tInfo)))
				return E_FAIL;
			break;
		case 25:
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Dragonite"), LEVEL_STATIC, TEXT("Layer_Pokemon"), &tInfo)))
				return E_FAIL;
			break;
		case 26:
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Garomakguri"), LEVEL_STATIC, TEXT("Layer_Pokemon"), &tInfo)))
				return E_FAIL;
			break;
		default:
			break;
		}
		if (i == 0)
		{
			dynamic_cast<CGameObj*>(tInfo)->Set_RacePoke();
			dynamic_cast<CGameObj*>(tInfo)->Set_RaceNum(1);
			m_listRacePoke.push_back(tInfo);
		}
		else if (i == 1)
		{
			dynamic_cast<CGameObj*>(tInfo)->Set_RacePoke();
			dynamic_cast<CGameObj*>(tInfo)->Set_RaceNum(2);
			m_listRacePoke.push_back(tInfo);
		}
		else if (i == 2)
		{
			dynamic_cast<CGameObj*>(tInfo)->Set_RacePoke();
			dynamic_cast<CGameObj*>(tInfo)->Set_RaceNum(3);
			m_listRacePoke.push_back(tInfo);
		}
		else if (i == 3)
		{
			dynamic_cast<CGameObj*>(tInfo)->Set_RacePoke();
			dynamic_cast<CGameObj*>(tInfo)->Set_RaceNum(4);
			m_listRacePoke.push_back(tInfo);
		}
	}
	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CRace::Ready_Components()
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
	if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_STATIC, TEXT("Race"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRace::Create_RacePoke()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	RandPoke();

	_float4 vPos, vAt;

	for (auto& iter : m_vecRace)
	{
		if (iter.iType == 12)
			m_vPos1 = iter.vPos;
		if (iter.iType == 13)
			m_vPos2 = iter.vPos;
		if (iter.iType == 14)
			m_vPos3 = iter.vPos;
		if (iter.iType == 15)
		{
			m_vPos4 = iter.vPos;
			vAt = iter.vPos;
		}
		if (iter.iType == 16)
			m_vLook1 = iter.vPos;
		if (iter.iType == 17)
			m_vLook2 = iter.vPos;
		if (iter.iType == 18)
			m_vLook3 = iter.vPos;
		if (iter.iType == 19)
			m_vLook4 = iter.vPos;

		if (iter.iType == 34)
		{
			m_vGoal1 = iter.vPos;
			vPos = iter.vPos;
		}
		if (iter.iType == 35)
			m_vGoal2 = iter.vPos;
		if (iter.iType == 36)
			m_vGoal3 = iter.vPos;
		if (iter.iType == 37)
			m_vGoal4 = iter.vPos;
		if (iter.iType == 20)
			m_vRaceEndPos = iter.vPos;
		if (iter.iType == 21)
			m_vRaceEndLook = iter.vPos;
	}

	Set_RacePokePos();


	vPos.x += 30.f;
	vPos.y += 30.f;
	vPos.z -= 30.f;

	pGameInstance->Set_ShadowLightDesc(LIGHTDESC::TYPE_FIELDSHADOW, vPos, vAt);

	
	CTextBox::TINFO tTInfo;
	Ready_Script();
	tTInfo.iScriptSize = (_int)m_vBattleScript.size();
	tTInfo.pTarget = this;
	tTInfo.pScript = new wstring[m_vBattleScript.size()];
	tTInfo.iType = 7;
	for (_int i = 0; i < m_vBattleScript.size(); ++i)
		tTInfo.pScript[i] = m_vBattleScript[i];

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_TextBox"), LEVEL_GAMEPLAY, TEXT("Layer_UI"), &tTInfo)))
		return E_FAIL;


	m_bReset = false;
	m_bRaceStart = true;
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CRace::Set_RacePokePos()
{
	auto& iter = m_listRacePoke.begin();

	dynamic_cast<CGameObj*>(*iter)->Get_Transfrom()->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&m_vPos1));
	dynamic_cast<CGameObj*>(*iter)->Get_Transfrom()->LookAt(XMLoadFloat4(&m_vLook1));

	++iter;

	dynamic_cast<CGameObj*>(*iter)->Get_Transfrom()->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&m_vPos2));
	dynamic_cast<CGameObj*>(*iter)->Get_Transfrom()->LookAt(XMLoadFloat4(&m_vLook2));

	++iter;

	dynamic_cast<CGameObj*>(*iter)->Get_Transfrom()->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&m_vPos3));
	dynamic_cast<CGameObj*>(*iter)->Get_Transfrom()->LookAt(XMLoadFloat4(&m_vLook3));

	++iter;

	dynamic_cast<CGameObj*>(*iter)->Get_Transfrom()->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&m_vPos4));
	dynamic_cast<CGameObj*>(*iter)->Get_Transfrom()->LookAt(XMLoadFloat4(&m_vLook4));
}


HRESULT CRace::SetUp_ShaderResources()
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

CRace * CRace::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CRace*	pInstance = new CRace(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CRace"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CRace::Clone(void * pArg)
{
	CGameObject*	pInstance = new CRace(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CRace"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRace::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);

	for (auto iter = m_vBattleScript.begin(); iter != m_vBattleScript.end();)
		iter = m_vBattleScript.erase(iter);

	m_vBattleScript.clear();

	m_vecRace.clear();

	if (!m_listRacePoke.empty())
	{
		for (auto& iter : m_listRacePoke)
		{
			Safe_Release(iter);
		}
		m_listRacePoke.clear();
	}
}
