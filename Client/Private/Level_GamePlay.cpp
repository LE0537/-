#include "stdafx.h"
#include "..\Public\Level_GamePlay.h"

#include "GameInstance.h"
#include "Camera_Dynamic.h"
#include "SoundMgr.h"


CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_GamePlay::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	ZeroMemory(&m_LoadFile, sizeof(LOADFILE));
	Load();
	LoadBattle();
	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;

	//if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
	//	return E_FAIL;	

	if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
		return E_FAIL;

	
	CSoundMgr::Get_Instance()->PlayBGM(TEXT("hov.wav"),0.75f);

	return S_OK;
}

void CLevel_GamePlay::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);	

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Key_Down(DIK_F1))
		g_CollBox = !g_CollBox;

	RELEASE_INSTANCE(CGameInstance);
}

void CLevel_GamePlay::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	//SetWindowText(g_hWnd, TEXT("게임플레이레벨입니다."));
}

HRESULT CLevel_GamePlay::Ready_Lights()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	LIGHTDESC			LightDesc;

	/* For.Directional*/
	ZeroMemory(&LightDesc, sizeof(LIGHTDESC));

	LightDesc.eType = LIGHTDESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.4f, 0.4f, 0.4f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc)))
		return E_FAIL;

	/* For.Point */
	ZeroMemory(&LightDesc, sizeof(LIGHTDESC));

	
	for (auto& iter : m_vecSave)
	{
		if (iter.iType == 10)
		{
			LightDesc.eType = LIGHTDESC::TYPE_POINT;
			iter.vPos.y += 10.f;
			LightDesc.vPosition = iter.vPos;
			LightDesc.fRange = 300.f;
			LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
			LightDesc.vAmbient = _float4(0.4f, 0.4f, 0.4f, 1.f);
			LightDesc.vSpecular = LightDesc.vDiffuse;
			if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc)))
				return E_FAIL;
		}
	}
	

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player(const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	for (auto& iter : m_vecSaveBattle)
	{
		if (iter.iType == 3)
			m_LoadFile.vMyPos = iter.vPos;
		
		if (iter.iType == 5)
			m_LoadFile.vTargetPos = iter.vPos;
	}
	for (auto& iter : m_vecSave)
	{
		if (iter.iType == 3)
		{
			m_LoadFile.vPos = iter.vPos;
			m_LoadFile.vScale = iter.vScale;
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Player"), LEVEL_GAMEPLAY, pLayerTag, &m_LoadFile)))
				return E_FAIL;
		}
	}
	Safe_Release(pGameInstance);

	
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_BackGround(const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_PointInstance"),
		CVIBuffer_Point_Instance::Create(m_pDevice, m_pContext, m_iWeed[0]))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_PointInstance2"),
		CVIBuffer_Point_Instance::Create(m_pDevice, m_pContext, m_iWeed[1]))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_PointInstance3"),
		CVIBuffer_Point_Instance::Create(m_pDevice, m_pContext, m_iWeed[2]))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Weed"), LEVEL_GAMEPLAY, pLayerTag, &tInsInfo)))
		return E_FAIL;
	dynamic_cast<CWeed*>(tInsInfo)->Set_Vector(m_listPos1, m_listScale1, 0);

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Weed"), LEVEL_GAMEPLAY, pLayerTag, &tInsInfo)))
		return E_FAIL;
	dynamic_cast<CWeed*>(tInsInfo)->Set_Vector(m_listPos2, m_listScale2, 1);

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Weed"), LEVEL_GAMEPLAY, pLayerTag, &tInsInfo)))
		return E_FAIL;
	dynamic_cast<CWeed*>(tInsInfo)->Set_Vector(m_listPos3, m_listScale3, 2);

	for (auto& iter : m_vecSave)
	{
		if (iter.iType == 1)
		{
			m_LoadFile.vPos = iter.vPos;
			m_LoadFile.vScale = iter.vScale;
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Field"), LEVEL_GAMEPLAY, TEXT("Layer_Field"), &m_LoadFile)))
				return E_FAIL;
		}
		if (iter.iType == 4)
		{
			m_LoadFile.vPos = iter.vPos;
			m_LoadFile.vScale = iter.vScale;
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Tree"), LEVEL_GAMEPLAY, pLayerTag, &m_LoadFile)))
				return E_FAIL;
		}
		if (iter.iType == 6)
		{
			_float3 vScale = { 0.013f, 0.013f, 0.013f};
			m_LoadFile.vPos = iter.vPos;
			m_LoadFile.vScale = vScale;
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_SkyBox"), LEVEL_GAMEPLAY, pLayerTag, &m_LoadFile)))
				return E_FAIL;
		}
	}
	for (auto& iter : m_vecSaveBattle)
	{
		if (iter.iType == 0)
		{
			m_LoadFile.vPos = iter.vPos;
			m_LoadFile.vScale = iter.vScale;
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_BattleField"), LEVEL_GAMEPLAY, pLayerTag, &m_LoadFile)))
				return E_FAIL;
		}
		if (iter.iType == 6)
		{
			_float3 vScale = { 0.008f, 0.008f, 0.008f };
			m_LoadFile.vPos = iter.vPos;
			m_LoadFile.vScale = vScale;
			m_LoadFile.bBattleMap = true;
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_SkyBox"), LEVEL_GAMEPLAY, pLayerTag, &m_LoadFile)))
				return E_FAIL;
		}
	}
	Safe_Release(pGameInstance);



	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Effect(const _tchar * pLayerTag)
{
	
	
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera(const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	ZeroMemory(&CameraDesc, sizeof(CCamera_Dynamic::CAMERADESC_DERIVED));

	CameraDesc.CameraDesc.pTarget = m_LoadFile.pTarget;
	
	CameraDesc.CameraDesc.vEye = _float4(0.f, 10.0f, -10.f, 1.f);
	CameraDesc.CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);

	CameraDesc.CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraDesc.CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	CameraDesc.CameraDesc.fNear = 0.2f;
	CameraDesc.CameraDesc.fFar = 1300.f;

	CameraDesc.CameraDesc.TransformDesc.fSpeedPerSec = 10.f;
	CameraDesc.CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Camera_Dynamic"), LEVEL_GAMEPLAY, pLayerTag, &CameraDesc)))
		return E_FAIL;
	m_LoadFile.pCamera = CameraDesc.CameraDesc.pCamera;
	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Monster(const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	//Prototype_GameObject_Squirtle
	//Prototype_GameObject_Pikachu
	//Prototype_GameObject_Wigglytuff
	//Prototype_GameObject_Meowth
	//Prototype_GameObject_Slowbro
	//Prototype_GameObject_Snorlax
	//Prototype_GameObject_Garomakguri
	for (auto& iter : m_vecSave)
	{
	
		if (iter.iType == 12)
		{
			CGameObject* tInfo = nullptr;

			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Squirtle"), LEVEL_GAMEPLAY, pLayerTag, &tInfo)))
				return E_FAIL;
		
			dynamic_cast<CGameObj*>(tInfo)->Set_WildPoke();
			dynamic_cast<CGameObj*>(tInfo)->Set_Target(m_LoadFile.pTarget);
			dynamic_cast<CGameObj*>(tInfo)->Set_Camera(m_LoadFile.pCamera);
			dynamic_cast<CGameObj*>(tInfo)->Set_MyBattlePos(m_LoadFile.vTargetPos);
			dynamic_cast<CGameObj*>(tInfo)->Get_Transfrom()->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&iter.vPos));
		}
		if (iter.iType == 13)
		{
			CGameObject* tInfo = nullptr;

			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Wartortle"), LEVEL_GAMEPLAY, pLayerTag, &tInfo)))
				return E_FAIL;

			dynamic_cast<CGameObj*>(tInfo)->Set_WildPoke();
			dynamic_cast<CGameObj*>(tInfo)->Set_Target(m_LoadFile.pTarget);
			dynamic_cast<CGameObj*>(tInfo)->Set_Camera(m_LoadFile.pCamera);
			dynamic_cast<CGameObj*>(tInfo)->Set_MyBattlePos(m_LoadFile.vTargetPos);
			dynamic_cast<CGameObj*>(tInfo)->Get_Transfrom()->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&iter.vPos));
		}
		if (iter.iType == 14)
		{
			CGameObject* tInfo = nullptr;

			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Blastoise"), LEVEL_GAMEPLAY, pLayerTag, &tInfo)))
				return E_FAIL;

			dynamic_cast<CGameObj*>(tInfo)->Set_WildPoke();
			dynamic_cast<CGameObj*>(tInfo)->Set_Target(m_LoadFile.pTarget);
			dynamic_cast<CGameObj*>(tInfo)->Set_Camera(m_LoadFile.pCamera);
			dynamic_cast<CGameObj*>(tInfo)->Set_MyBattlePos(m_LoadFile.vTargetPos);
			dynamic_cast<CGameObj*>(tInfo)->Get_Transfrom()->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&iter.vPos));
		}
		if (iter.iType == 15)
		{
			CGameObject* tInfo = nullptr;

			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Caterpie"), LEVEL_GAMEPLAY, pLayerTag, &tInfo)))
				return E_FAIL;

			dynamic_cast<CGameObj*>(tInfo)->Set_WildPoke();
			dynamic_cast<CGameObj*>(tInfo)->Set_Target(m_LoadFile.pTarget);
			dynamic_cast<CGameObj*>(tInfo)->Set_Camera(m_LoadFile.pCamera);
			dynamic_cast<CGameObj*>(tInfo)->Set_MyBattlePos(m_LoadFile.vTargetPos);
			dynamic_cast<CGameObj*>(tInfo)->Get_Transfrom()->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&iter.vPos));
		}
		if (iter.iType == 16)
		{
			CGameObject* tInfo = nullptr;

			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Metapod"), LEVEL_GAMEPLAY, pLayerTag, &tInfo)))
				return E_FAIL;

			dynamic_cast<CGameObj*>(tInfo)->Set_WildPoke();
			dynamic_cast<CGameObj*>(tInfo)->Set_Target(m_LoadFile.pTarget);
			dynamic_cast<CGameObj*>(tInfo)->Set_Camera(m_LoadFile.pCamera);
			dynamic_cast<CGameObj*>(tInfo)->Set_MyBattlePos(m_LoadFile.vTargetPos);
			dynamic_cast<CGameObj*>(tInfo)->Get_Transfrom()->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&iter.vPos));
		}
		if (iter.iType == 17)
		{
			CGameObject* tInfo = nullptr;

			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Butterfree"), LEVEL_GAMEPLAY, pLayerTag, &tInfo)))
				return E_FAIL;

			dynamic_cast<CGameObj*>(tInfo)->Set_WildPoke();
			dynamic_cast<CGameObj*>(tInfo)->Set_Target(m_LoadFile.pTarget);
			dynamic_cast<CGameObj*>(tInfo)->Set_Camera(m_LoadFile.pCamera);
			dynamic_cast<CGameObj*>(tInfo)->Set_MyBattlePos(m_LoadFile.vTargetPos);
			dynamic_cast<CGameObj*>(tInfo)->Get_Transfrom()->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&iter.vPos));
		}
		if (iter.iType == 18)
		{
			CGameObject* tInfo = nullptr;

			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Rattata"), LEVEL_GAMEPLAY, pLayerTag, &tInfo)))
				return E_FAIL;

			dynamic_cast<CGameObj*>(tInfo)->Set_WildPoke();
			dynamic_cast<CGameObj*>(tInfo)->Set_Target(m_LoadFile.pTarget);
			dynamic_cast<CGameObj*>(tInfo)->Set_Camera(m_LoadFile.pCamera);
			dynamic_cast<CGameObj*>(tInfo)->Set_MyBattlePos(m_LoadFile.vTargetPos);
			dynamic_cast<CGameObj*>(tInfo)->Get_Transfrom()->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&iter.vPos));
		}
		if (iter.iType == 19)
		{
			CGameObject* tInfo = nullptr;

			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Raticate"), LEVEL_GAMEPLAY, pLayerTag, &tInfo)))
				return E_FAIL;

			dynamic_cast<CGameObj*>(tInfo)->Set_WildPoke();
			dynamic_cast<CGameObj*>(tInfo)->Set_Target(m_LoadFile.pTarget);
			dynamic_cast<CGameObj*>(tInfo)->Set_Camera(m_LoadFile.pCamera);
			dynamic_cast<CGameObj*>(tInfo)->Set_MyBattlePos(m_LoadFile.vTargetPos);
			dynamic_cast<CGameObj*>(tInfo)->Get_Transfrom()->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&iter.vPos));
		}
		if (iter.iType == 20)
		{
			CGameObject* tInfo = nullptr;

			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Pikachu"), LEVEL_GAMEPLAY, pLayerTag, &tInfo)))
				return E_FAIL;

			dynamic_cast<CGameObj*>(tInfo)->Set_WildPoke();
			dynamic_cast<CGameObj*>(tInfo)->Set_Target(m_LoadFile.pTarget);
			dynamic_cast<CGameObj*>(tInfo)->Set_Camera(m_LoadFile.pCamera);
			dynamic_cast<CGameObj*>(tInfo)->Set_MyBattlePos(m_LoadFile.vTargetPos);
			dynamic_cast<CGameObj*>(tInfo)->Get_Transfrom()->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&iter.vPos));
		}
		if (iter.iType == 21)
		{
			CGameObject* tInfo = nullptr;

			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Jigglypuff"), LEVEL_GAMEPLAY, pLayerTag, &tInfo)))
				return E_FAIL;

			dynamic_cast<CGameObj*>(tInfo)->Set_WildPoke();
			dynamic_cast<CGameObj*>(tInfo)->Set_Target(m_LoadFile.pTarget);
			dynamic_cast<CGameObj*>(tInfo)->Set_Camera(m_LoadFile.pCamera);
			dynamic_cast<CGameObj*>(tInfo)->Set_MyBattlePos(m_LoadFile.vTargetPos);
			dynamic_cast<CGameObj*>(tInfo)->Get_Transfrom()->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&iter.vPos));
		}
		if (iter.iType == 22)
		{
			CGameObject* tInfo = nullptr;

			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Wigglytuff"), LEVEL_GAMEPLAY, pLayerTag, &tInfo)))
				return E_FAIL;

			dynamic_cast<CGameObj*>(tInfo)->Set_WildPoke();
			dynamic_cast<CGameObj*>(tInfo)->Set_Target(m_LoadFile.pTarget);
			dynamic_cast<CGameObj*>(tInfo)->Set_Camera(m_LoadFile.pCamera);
			dynamic_cast<CGameObj*>(tInfo)->Set_MyBattlePos(m_LoadFile.vTargetPos);
			dynamic_cast<CGameObj*>(tInfo)->Get_Transfrom()->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&iter.vPos));
		}
		if (iter.iType == 23)
		{
			CGameObject* tInfo = nullptr;

			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Meowth"), LEVEL_GAMEPLAY, pLayerTag, &tInfo)))
				return E_FAIL;

			dynamic_cast<CGameObj*>(tInfo)->Set_WildPoke();
			dynamic_cast<CGameObj*>(tInfo)->Set_Target(m_LoadFile.pTarget);
			dynamic_cast<CGameObj*>(tInfo)->Set_Camera(m_LoadFile.pCamera);
			dynamic_cast<CGameObj*>(tInfo)->Set_MyBattlePos(m_LoadFile.vTargetPos);
			dynamic_cast<CGameObj*>(tInfo)->Get_Transfrom()->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&iter.vPos));
		}
		if (iter.iType == 24)
		{
			CGameObject* tInfo = nullptr;

			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Persian"), LEVEL_GAMEPLAY, pLayerTag, &tInfo)))
				return E_FAIL;

			dynamic_cast<CGameObj*>(tInfo)->Set_WildPoke();
			dynamic_cast<CGameObj*>(tInfo)->Set_Target(m_LoadFile.pTarget);
			dynamic_cast<CGameObj*>(tInfo)->Set_Camera(m_LoadFile.pCamera);
			dynamic_cast<CGameObj*>(tInfo)->Set_MyBattlePos(m_LoadFile.vTargetPos);
			dynamic_cast<CGameObj*>(tInfo)->Get_Transfrom()->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&iter.vPos));
		}
		if (iter.iType == 25)
		{
			CGameObject* tInfo = nullptr;

			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Slowpoke"), LEVEL_GAMEPLAY, pLayerTag, &tInfo)))
				return E_FAIL;

			dynamic_cast<CGameObj*>(tInfo)->Set_WildPoke();
			dynamic_cast<CGameObj*>(tInfo)->Set_Target(m_LoadFile.pTarget);
			dynamic_cast<CGameObj*>(tInfo)->Set_Camera(m_LoadFile.pCamera);
			dynamic_cast<CGameObj*>(tInfo)->Set_MyBattlePos(m_LoadFile.vTargetPos);
			dynamic_cast<CGameObj*>(tInfo)->Get_Transfrom()->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&iter.vPos));
		}
		if (iter.iType == 26)
		{
			CGameObject* tInfo = nullptr;

			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Slowbro"), LEVEL_GAMEPLAY, pLayerTag, &tInfo)))
				return E_FAIL;

			dynamic_cast<CGameObj*>(tInfo)->Set_WildPoke();
			dynamic_cast<CGameObj*>(tInfo)->Set_Target(m_LoadFile.pTarget);
			dynamic_cast<CGameObj*>(tInfo)->Set_Camera(m_LoadFile.pCamera);
			dynamic_cast<CGameObj*>(tInfo)->Set_MyBattlePos(m_LoadFile.vTargetPos);
			dynamic_cast<CGameObj*>(tInfo)->Get_Transfrom()->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&iter.vPos));
		}
		if (iter.iType == 27)
		{
			CGameObject* tInfo = nullptr;

			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Onix"), LEVEL_GAMEPLAY, pLayerTag, &tInfo)))
				return E_FAIL;

			dynamic_cast<CGameObj*>(tInfo)->Set_WildPoke();
			dynamic_cast<CGameObj*>(tInfo)->Set_Target(m_LoadFile.pTarget);
			dynamic_cast<CGameObj*>(tInfo)->Set_Camera(m_LoadFile.pCamera);
			dynamic_cast<CGameObj*>(tInfo)->Set_MyBattlePos(m_LoadFile.vTargetPos);
			dynamic_cast<CGameObj*>(tInfo)->Get_Transfrom()->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&iter.vPos));
		}
		if (iter.iType == 28)
		{
			CGameObject* tInfo = nullptr;

			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Lickitung"), LEVEL_GAMEPLAY, pLayerTag, &tInfo)))
				return E_FAIL;

			dynamic_cast<CGameObj*>(tInfo)->Set_WildPoke();
			dynamic_cast<CGameObj*>(tInfo)->Set_Target(m_LoadFile.pTarget);
			dynamic_cast<CGameObj*>(tInfo)->Set_Camera(m_LoadFile.pCamera);
			dynamic_cast<CGameObj*>(tInfo)->Set_MyBattlePos(m_LoadFile.vTargetPos);
			dynamic_cast<CGameObj*>(tInfo)->Get_Transfrom()->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&iter.vPos));
		}
		if (iter.iType == 29)
		{
			CGameObject* tInfo = nullptr;

			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Chansey"), LEVEL_GAMEPLAY, pLayerTag, &tInfo)))
				return E_FAIL;

			dynamic_cast<CGameObj*>(tInfo)->Set_WildPoke();
			dynamic_cast<CGameObj*>(tInfo)->Set_Target(m_LoadFile.pTarget);
			dynamic_cast<CGameObj*>(tInfo)->Set_Camera(m_LoadFile.pCamera);
			dynamic_cast<CGameObj*>(tInfo)->Set_MyBattlePos(m_LoadFile.vTargetPos);
			dynamic_cast<CGameObj*>(tInfo)->Get_Transfrom()->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&iter.vPos));
		}
		if (iter.iType == 30)
		{
			CGameObject* tInfo = nullptr;

			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Kangaskhan"), LEVEL_GAMEPLAY, pLayerTag, &tInfo)))
				return E_FAIL;

			dynamic_cast<CGameObj*>(tInfo)->Set_WildPoke();
			dynamic_cast<CGameObj*>(tInfo)->Set_Target(m_LoadFile.pTarget);
			dynamic_cast<CGameObj*>(tInfo)->Set_Camera(m_LoadFile.pCamera);
			dynamic_cast<CGameObj*>(tInfo)->Set_MyBattlePos(m_LoadFile.vTargetPos);
			dynamic_cast<CGameObj*>(tInfo)->Get_Transfrom()->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&iter.vPos));
		}
		if (iter.iType == 31)
		{
			CGameObject* tInfo = nullptr;

			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Scyther"), LEVEL_GAMEPLAY, pLayerTag, &tInfo)))
				return E_FAIL;

			dynamic_cast<CGameObj*>(tInfo)->Set_WildPoke();
			dynamic_cast<CGameObj*>(tInfo)->Set_Target(m_LoadFile.pTarget);
			dynamic_cast<CGameObj*>(tInfo)->Set_Camera(m_LoadFile.pCamera);
			dynamic_cast<CGameObj*>(tInfo)->Set_MyBattlePos(m_LoadFile.vTargetPos);
			dynamic_cast<CGameObj*>(tInfo)->Get_Transfrom()->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&iter.vPos));
		}
		if (iter.iType == 32)
		{
			CGameObject* tInfo = nullptr;

			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Magikarp"), LEVEL_GAMEPLAY, pLayerTag, &tInfo)))
				return E_FAIL;

			dynamic_cast<CGameObj*>(tInfo)->Set_WildPoke();
			dynamic_cast<CGameObj*>(tInfo)->Set_Target(m_LoadFile.pTarget);
			dynamic_cast<CGameObj*>(tInfo)->Set_Camera(m_LoadFile.pCamera);
			dynamic_cast<CGameObj*>(tInfo)->Set_MyBattlePos(m_LoadFile.vTargetPos);
			dynamic_cast<CGameObj*>(tInfo)->Get_Transfrom()->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&iter.vPos));
		}
		if (iter.iType == 33)
		{
			CGameObject* tInfo = nullptr;

			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Gyarados"), LEVEL_GAMEPLAY, pLayerTag, &tInfo)))
				return E_FAIL;

			dynamic_cast<CGameObj*>(tInfo)->Set_WildPoke();
			dynamic_cast<CGameObj*>(tInfo)->Set_Target(m_LoadFile.pTarget);
			dynamic_cast<CGameObj*>(tInfo)->Set_Camera(m_LoadFile.pCamera);
			dynamic_cast<CGameObj*>(tInfo)->Set_MyBattlePos(m_LoadFile.vTargetPos);
			dynamic_cast<CGameObj*>(tInfo)->Get_Transfrom()->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&iter.vPos));
		}
		if (iter.iType == 34)
		{
			CGameObject* tInfo = nullptr;

			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Snorlax"), LEVEL_GAMEPLAY, pLayerTag, &tInfo)))
				return E_FAIL;

			dynamic_cast<CGameObj*>(tInfo)->Set_WildPoke();
			dynamic_cast<CGameObj*>(tInfo)->Set_Target(m_LoadFile.pTarget);
			dynamic_cast<CGameObj*>(tInfo)->Set_Camera(m_LoadFile.pCamera);
			dynamic_cast<CGameObj*>(tInfo)->Set_MyBattlePos(m_LoadFile.vTargetPos);
			dynamic_cast<CGameObj*>(tInfo)->Get_Transfrom()->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&iter.vPos));
		}
		if (iter.iType == 35)
		{
			CGameObject* tInfo = nullptr;

			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Dratini"), LEVEL_GAMEPLAY, pLayerTag, &tInfo)))
				return E_FAIL;

			dynamic_cast<CGameObj*>(tInfo)->Set_WildPoke();
			dynamic_cast<CGameObj*>(tInfo)->Set_Target(m_LoadFile.pTarget);
			dynamic_cast<CGameObj*>(tInfo)->Set_Camera(m_LoadFile.pCamera);
			dynamic_cast<CGameObj*>(tInfo)->Set_MyBattlePos(m_LoadFile.vTargetPos);
			dynamic_cast<CGameObj*>(tInfo)->Get_Transfrom()->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&iter.vPos));
		}
		if (iter.iType == 36)
		{
			CGameObject* tInfo = nullptr;

			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Dragonair"), LEVEL_GAMEPLAY, pLayerTag, &tInfo)))
				return E_FAIL;

			dynamic_cast<CGameObj*>(tInfo)->Set_WildPoke();
			dynamic_cast<CGameObj*>(tInfo)->Set_Target(m_LoadFile.pTarget);
			dynamic_cast<CGameObj*>(tInfo)->Set_Camera(m_LoadFile.pCamera);
			dynamic_cast<CGameObj*>(tInfo)->Set_MyBattlePos(m_LoadFile.vTargetPos);
			dynamic_cast<CGameObj*>(tInfo)->Get_Transfrom()->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&iter.vPos));
		}
		if (iter.iType == 37)
		{
			CGameObject* tInfo = nullptr;

			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Dragonite"), LEVEL_GAMEPLAY, pLayerTag, &tInfo)))
				return E_FAIL;

			dynamic_cast<CGameObj*>(tInfo)->Set_WildPoke();
			dynamic_cast<CGameObj*>(tInfo)->Set_Target(m_LoadFile.pTarget);
			dynamic_cast<CGameObj*>(tInfo)->Set_Camera(m_LoadFile.pCamera);
			dynamic_cast<CGameObj*>(tInfo)->Set_MyBattlePos(m_LoadFile.vTargetPos);
			dynamic_cast<CGameObj*>(tInfo)->Get_Transfrom()->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&iter.vPos));
		}
		if (iter.iType == 38)
		{
			CGameObject* tInfo = nullptr;

			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Mewtwo"), LEVEL_GAMEPLAY, pLayerTag, &tInfo)))
				return E_FAIL;

			dynamic_cast<CGameObj*>(tInfo)->Set_WildPoke();
			dynamic_cast<CGameObj*>(tInfo)->Set_Target(m_LoadFile.pTarget);
			dynamic_cast<CGameObj*>(tInfo)->Set_Camera(m_LoadFile.pCamera);
			dynamic_cast<CGameObj*>(tInfo)->Set_MyBattlePos(m_LoadFile.vTargetPos);
			dynamic_cast<CGameObj*>(tInfo)->Get_Transfrom()->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&iter.vPos));
		}
		if (iter.iType == 39)
		{
			CGameObject* tInfo = nullptr;

			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Mew"), LEVEL_GAMEPLAY, pLayerTag, &tInfo)))
				return E_FAIL;

			dynamic_cast<CGameObj*>(tInfo)->Set_WildPoke();
			dynamic_cast<CGameObj*>(tInfo)->Set_Target(m_LoadFile.pTarget);
			dynamic_cast<CGameObj*>(tInfo)->Set_Camera(m_LoadFile.pCamera);
			dynamic_cast<CGameObj*>(tInfo)->Set_MyBattlePos(m_LoadFile.vTargetPos);
			dynamic_cast<CGameObj*>(tInfo)->Get_Transfrom()->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&iter.vPos));
		}
	/*	if (iter.iType == 5)
		{
			m_LoadFile.vPos = iter.vPos;
			m_LoadFile.vScale = iter.vScale;

			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Olive"), LEVEL_GAMEPLAY, pLayerTag, &m_LoadFile)))
				return E_FAIL;
		}*/
	}
	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_UI(const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);


	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_UI"), LEVEL_STATIC, pLayerTag)))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Bag"), LEVEL_STATIC, pLayerTag, &m_LoadFile.pTarget)))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_PokeDeck"), LEVEL_STATIC, pLayerTag)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

void CLevel_GamePlay::Load()
{
	HANDLE		hFile = CreateFile(L"../Data/Pos.dat",			// 파일 경로와 이름 명시
		GENERIC_READ,				// 파일 접근 모드 (GENERIC_WRITE 쓰기 전용, GENERIC_READ 읽기 전용)
		NULL,						// 공유방식, 파일이 열려있는 상태에서 다른 프로세스가 오픈할 때 허용할 것인가, NULL인 경우 공유하지 않는다
		NULL,						// 보안 속성, 기본값	
		OPEN_EXISTING,				// 생성 방식, CREATE_ALWAYS는 파일이 없다면 생성, 있다면 덮어 쓰기, OPEN_EXISTING 파일이 있을 경우에면 열기
		FILE_ATTRIBUTE_NORMAL,		// 파일 속성(읽기 전용, 숨기 등), FILE_ATTRIBUTE_NORMAL 아무런 속성이 없는 일반 파일 생성
		NULL);						// 생성도리 파일의 속성을 제공할 템플릿 파일, 우리는 사용하지 않아서 NULL

	if (INVALID_HANDLE_VALUE == hFile)
	{
		// 팝업 창을 출력해주는 기능의 함수
		// 1. 핸들 2. 팝업 창에 띄우고자하는 메시지 3. 팝업 창 이름 4. 버튼 속성
		MessageBox(g_hWnd, TEXT("Load File"), TEXT("Fail"), MB_OK);
		return;
	}

	// 2. 파일 쓰기

	DWORD		dwByte = 0;
	SaveInfo		tInfo;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	while (true)
	{
		ReadFile(hFile, &tInfo, sizeof(SaveInfo), &dwByte, nullptr);

		if (0 == dwByte)	// 더이상 읽을 데이터가 없을 경우
			break;
		m_vecSave.push_back(tInfo);
		if (tInfo.iType == 7)
		{
			m_listPos1.push_back(tInfo.vPos);
			m_listScale1.push_back(tInfo.vScale);
			++m_iWeed[0];
		}
		if (tInfo.iType == 8)
		{
			m_listPos2.push_back(tInfo.vPos);
			m_listScale2.push_back(tInfo.vScale);
			++m_iWeed[1];
		}
		if (tInfo.iType == 9)
		{
			m_listPos3.push_back(tInfo.vPos);
			m_listScale3.push_back(tInfo.vScale);
			++m_iWeed[2];
		}
	}
	
	Safe_Release(pGameInstance);
	// 3. 파일 소멸
	CloseHandle(hFile);
}

void CLevel_GamePlay::LoadBattle()
{
	HANDLE		hFile = CreateFile(L"../Data/BattlePos.dat",			// 파일 경로와 이름 명시
		GENERIC_READ,				// 파일 접근 모드 (GENERIC_WRITE 쓰기 전용, GENERIC_READ 읽기 전용)
		NULL,						// 공유방식, 파일이 열려있는 상태에서 다른 프로세스가 오픈할 때 허용할 것인가, NULL인 경우 공유하지 않는다
		NULL,						// 보안 속성, 기본값	
		OPEN_EXISTING,				// 생성 방식, CREATE_ALWAYS는 파일이 없다면 생성, 있다면 덮어 쓰기, OPEN_EXISTING 파일이 있을 경우에면 열기
		FILE_ATTRIBUTE_NORMAL,		// 파일 속성(읽기 전용, 숨기 등), FILE_ATTRIBUTE_NORMAL 아무런 속성이 없는 일반 파일 생성
		NULL);						// 생성도리 파일의 속성을 제공할 템플릿 파일, 우리는 사용하지 않아서 NULL

	if (INVALID_HANDLE_VALUE == hFile)
	{
		// 팝업 창을 출력해주는 기능의 함수
		// 1. 핸들 2. 팝업 창에 띄우고자하는 메시지 3. 팝업 창 이름 4. 버튼 속성
		MessageBox(g_hWnd, TEXT("Load File"), TEXT("Fail"), MB_OK);
		return;
	}

	// 2. 파일 쓰기

	DWORD		dwByte = 0;
	SaveInfo		tInfo;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	while (true)
	{
		ReadFile(hFile, &tInfo, sizeof(SaveInfo), &dwByte, nullptr);

		if (0 == dwByte)	// 더이상 읽을 데이터가 없을 경우
			break;
		m_vecSaveBattle.push_back(tInfo);
	}

	Safe_Release(pGameInstance);
	// 3. 파일 소멸
	CloseHandle(hFile);
}

CLevel_GamePlay * CLevel_GamePlay::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_GamePlay*	pInstance = new CLevel_GamePlay(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		ERR_MSG(TEXT("Failed to Created : CLevel_GamePlay"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_GamePlay::Free()
{
	__super::Free();

	m_listPos1.clear();
	m_listScale1.clear();
	m_listPos2;
	m_listScale2;
	m_listPos3;
	m_listScale3;


}
