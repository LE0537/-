#include "stdafx.h"
#include "..\Public\Loader.h"

#include "GameInstance.h"
#include "Data_Manager.h"	// �߰�

#include "Camera_Dynamic.h"
#include "BackGround.h"
#include "Terrain.h"
//UI ���
#include "UI.h" 
#include "Bag.h"
#include "PokeInfo.h"
#include "PokeDeck.h"
#include "TextBox.h"
#include "BattleUI.h"
#include "Lv_Up.h"
#include "Evolution.h"
//Map
#include "Field.h"
#include "BattleField.h"
#include "Tree.h"
#include "SkyBox.h"
#include "Weed.h"
//Player ���
#include "Player.h"
//Trainer
#include "Mari.h"
//Pokemon ���
#include "Charmander.h"
#include "Squirtle.h"
#include "Wartortle.h"
#include "Pikachu.h"
#include "Wigglytuff.h"
#include "Meowth.h"
#include "Slowbro.h"
#include "Snorlax.h"
#include "Garomakguri.h"
#include "NonePoke.h"
//Item ���
#include "MonsterBall.h"
#include "HpPotion.h"
#include "ExpShare.h"
#include "None.h"
#include "Ball.h"
//Skill ���
#include "Tackle.h"
#include "BreakCar.h"
#include "NoneSkill.h"
//Effect ���
#include "BattleEvent.h"
#include "Screen.h"
#include "BattleIntro.h"
#include "WaterBeam.h"
#include "WaterBall.h"

CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

unsigned int APIENTRY Thread_Main(void* pArg)
{
	CLoader*		pLoader = (CLoader*)pArg;

	EnterCriticalSection(&pLoader->Get_CriticalSection());

	switch (pLoader->Get_NextLevelID())
	{
	case LEVEL_LOGO:
		pLoader->Loading_ForLogoLevel();
		break;
	case LEVEL_GAMEPLAY:
		pLoader->Loading_ForGamePlayLevel();
		break;
	}

	LeaveCriticalSection(&pLoader->Get_CriticalSection());

	return 0;
}

HRESULT CLoader::Initialize(LEVEL eNextLevel)
{
	CoInitializeEx(nullptr, 0);

	m_eNextLevel = eNextLevel;

	InitializeCriticalSection(&m_CriticalSection);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, Thread_Main, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;
		
	return S_OK;
}

HRESULT CLoader::Loading_ForLogoLevel()
{
	CData_Manager* pData_Manager = GET_INSTANCE(CData_Manager);
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	Safe_AddRef(pGameInstance);

	/* �ؽ��� �ε� ��. */
	lstrcpy(m_szLoadingText, TEXT("                       �ؽ��� �ε� ��."));
	/* ��� �ؽ��� */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_BackGround"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Default%d.jpg"), 2))))
		return E_FAIL;
	/* UI �ؽ��� */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Bag/%d.dds"), 12))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Item"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Item/%d.dds"), 41))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_PokeNum"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/PokeNum/%d.dds"), 153))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_StatInfo"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/StatInfo/%d.dds"), 6))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Battle"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Battle/%d.dds"), 22))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Type"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Type/%d.dds"), 18))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_TypeSmall"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/TypeSmall/%d.dds"), 18))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_PokeDeck"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/PokeDeck/%d.dds"), 2))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Weed"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Weed/%d.dds"), 3))))
		return E_FAIL;
	/* �ͷ��� �ؽ��� */
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Terrain"),
	//	CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Grass_%d.dds"), 2))))
	//	return E_FAIL;



	/* �� �ε� ��. */
	lstrcpy(m_szLoadingText, TEXT("                     �� �ε� ��."));
	
	/* ���̴� �ε� ��. */
	lstrcpy(m_szLoadingText, TEXT("                     ���̴� �ε� ��."));
	
	lstrcpy(m_szLoadingText, TEXT("                     ��ü ���� ��."));

	/* ��� ��ü */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround"),
		CBackGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* �ͷ��� ��ü */
	/*if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;*/

	/* ī�޶� ��ü */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Dynamic"),
		CCamera_Dynamic::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* UI ��ü */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI"),
		CUI::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Bag"),
		CBag::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PokeInfo"),
		CPokeInfo::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PokeDeck"),
		CPokeDeck::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TextBox"),
		CTextBox::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BattleUI"),
		CBattleUI::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Lv_Up"),
		CLv_Up::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Evolution"),
		CEvolution::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* ������ ��ü */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_None"),
		CNone::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MonsterBall"),
		CMonsterBall::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_HpPotion"),
		CHpPotion::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ExpShare"),
		CExpShare::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Ball"),
		CBall::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	/* ���ϸ� ��ü */


	/*if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Charmander"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/fbx/Charmander/Charmander.fbx", PivotMatrix))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Charmander"),
		CCharmander::Create(m_pDevice, m_pContext))))
		return E_FAIL;*/

	CData_Manager::Get_Instance()->Create_Try_BinModel(TEXT("Squirtle"), LEVEL_STATIC, CData_Manager::DATA_ANIM);
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Squirtle"),
		CSquirtle::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Wartortle"),
	//CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/fbx/Wartortle/Wartortle.fbx", PivotMatrix))))
	//	return E_FAIL;
	CData_Manager::Get_Instance()->Create_Try_BinModel(TEXT("Wartortle"), LEVEL_STATIC, CData_Manager::DATA_ANIM);
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Wartortle"),
	CWartortle::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	CData_Manager::Get_Instance()->Create_Try_BinModel(TEXT("Pikachu"), LEVEL_STATIC, CData_Manager::DATA_ANIM);
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Pikachu"),
		CPikachu::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	CData_Manager::Get_Instance()->Create_Try_BinModel(TEXT("Wigglytuff"), LEVEL_STATIC, CData_Manager::DATA_ANIM);
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Wigglytuff"),
		CWigglytuff::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	CData_Manager::Get_Instance()->Create_Try_BinModel(TEXT("Meowth"), LEVEL_STATIC, CData_Manager::DATA_ANIM);
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Meowth"),
		CMeowth::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	CData_Manager::Get_Instance()->Create_Try_BinModel(TEXT("Slowbro"), LEVEL_STATIC, CData_Manager::DATA_ANIM);
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Slowbro"),
		CSlowbro::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	CData_Manager::Get_Instance()->Create_Try_BinModel(TEXT("Snorlax"), LEVEL_STATIC, CData_Manager::DATA_ANIM);
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Snorlax"),
		CSnorlax::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	CData_Manager::Get_Instance()->Create_Try_BinModel(TEXT("Garomakguri"), LEVEL_STATIC, CData_Manager::DATA_ANIM);
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Garomakguri"),
		CGaromakguri::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_NonePoke"),
		CNonePoke::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	//Player
	CData_Manager::Get_Instance()->Create_Try_BinModel(TEXT("Player"), LEVEL_STATIC, CData_Manager::DATA_ANIM);
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player"),
		CPlayer::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	//Trainer
	CData_Manager::Get_Instance()->Create_Try_BinModel(TEXT("Mari"), LEVEL_STATIC, CData_Manager::DATA_ANIM);
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Mari"),
		CMari::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	CData_Manager::Get_Instance()->Create_Try_BinModel(TEXT("MonsterBall"), LEVEL_STATIC, CData_Manager::DATA_PARTS);
	CData_Manager::Get_Instance()->Create_Try_BinModel(TEXT("SuperBall"), LEVEL_STATIC, CData_Manager::DATA_PARTS);
	CData_Manager::Get_Instance()->Create_Try_BinModel(TEXT("HyperBall"), LEVEL_STATIC, CData_Manager::DATA_PARTS);
	CData_Manager::Get_Instance()->Create_Try_BinModel(TEXT("MasterBall"), LEVEL_STATIC, CData_Manager::DATA_PARTS);

	//Map
	_matrix			PivotMatrix = XMMatrixIdentity();

	PivotMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	CData_Manager::Get_Instance()->Create_Try_BinModel(TEXT("Field"), LEVEL_STATIC, CData_Manager::DATA_NONANIM);
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Field"),
		CField::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	CData_Manager::Get_Instance()->Create_Try_BinModel(TEXT("BattleField"), LEVEL_STATIC, CData_Manager::DATA_NONANIM);
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BattleField"),
		CBattleField::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	CData_Manager::Get_Instance()->Create_Try_BinModel(TEXT("Tree"), LEVEL_STATIC, CData_Manager::DATA_NONANIM);
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Tree"),
		CTree::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_SkyBox"),
	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/fbx/SkyBox/SkyBox.fbx", PivotMatrix))))
	return E_FAIL;
	/*CData_Manager::Get_Instance()->Create_Try_BinModel(TEXT("SkyBox"), LEVEL_STATIC, CData_Manager::DATA_NONANIM);*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SkyBox"),
	CSkyBox::Create(m_pDevice, m_pContext))))
	return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weed"),
		CWeed::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// Navi
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Data/NaviPos.dat")))))
		return E_FAIL;

	/* ��ų ��ü */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Tackle"),
		CTackle::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BreakCar"),
		CBreakCar::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_NoneSkill"),
		CNoneSkill::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* Effect ��ü */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BattleEvent"),
		CBattleEvent::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Screen"),
		CScreen::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BattleIntro"),
		CBattleIntro::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Cylinder"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/fbx/Cylinder/Cylinder.fbx", PivotMatrix))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Cone"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/fbx/Cone/Cone.fbx", PivotMatrix))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_WaterBall"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/fbx/WaterBall/WaterBall.fbx", PivotMatrix))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_WaterBeam"),
		CWaterBeam::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_WaterBall"),
		CWaterBall::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	lstrcpy(m_szLoadingText, TEXT("                        �ε��� �Ϸ�Ǿ����ϴ�."));

	m_isFinished = true;
	RELEASE_INSTANCE(CData_Manager);
	Safe_Release(pGameInstance);
	

	return S_OK;
}

HRESULT CLoader::Loading_ForGamePlayLevel()
{
	m_isFinished = true;

	return S_OK;
}

CLoader * CLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevel)
{
	CLoader*	pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevel)))
	{
		ERR_MSG(TEXT("Failed to Created : CLoader"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoader::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);

	CloseHandle(m_hThread);

	DeleteCriticalSection(&m_CriticalSection);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
