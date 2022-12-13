#include "stdafx.h"
#include "..\Public\Loader.h"

#include "GameInstance.h"
#include "Data_Manager.h"	// 추가

#include "Camera_Dynamic.h"
#include "BackGround.h"
#include "Terrain.h"
//UI 헤더
#include "UI.h" 
#include "Bag.h"
#include "PokeInfo.h"
#include "PokeDeck.h"
#include "TextBox.h"
#include "BattleUI.h"
#include "Lv_Up.h"
#include "Evolution.h"
#include "EndingErr.h"
//Map
#include "Field.h"
#include "BattleField.h"
#include "Tree.h"
#include "SkyBox.h"
#include "Weed.h"
//Player 헤더
#include "Player.h"
//Trainer
#include "Mari.h"
#include "Beat.h"
#include "Sonia.h"
#include "Dandel.h"
#include "Olive.h"
#include "Tr0.h"
#include "Tr1.h"
#include "Tr2.h"
#include "Tr3.h"
#include "Tr4.h"
#include "Tr5.h"
#include "Tr6.h"
#include "Tr7.h"
#include "Tr8.h"
#include "Tr9.h"
//Pokemon 헤더
#include "Charmander.h"
#include "Squirtle.h"
#include "Wartortle.h"
#include "Blastoise.h"
#include "Caterpie.h"
#include "Metapod.h"
#include "Butterfree.h"
#include "Rattata.h"
#include "Raticate.h"
#include "Pikachu.h"
#include "Jigglypuff.h"
#include "Wigglytuff.h"
#include "Meowth.h"
#include "Persian.h"
#include "Slowpoke.h"
#include "Slowbro.h"
#include "Onix.h"
#include "Lickitung.h"
#include "Chansey.h"
#include "Kangaskhan.h"
#include "Scyther.h"
#include "Magikarp.h"
#include "Gyarados.h"
#include "Snorlax.h"
#include "Dratini.h"
#include "Dragonair.h"
#include "Dragonite.h"
#include "Mewtwo.h"
#include "Mew.h"
#include "Garomakguri.h"
#include "NonePoke.h"
//Item 헤더
#include "MonsterBall.h"
#include "SuperBall.h"
#include "HyperBall.h"
#include "MasterBall.h"
#include "HpPotion.h"
#include "FullPotion.h"
#include "MaxPotion.h"
#include "Candy.h"
#include "ExpShare.h"
#include "None.h"
#include "Ball.h"
#include "GrassBadge.h"
#include "WaterBadge.h"
#include "FireBadge.h"
#include "FightBadge.h"
#include "GhostBadge.h"
#include "FairyBadge.h"
#include "TerrainBadge.h"
#include "IceBadge.h"
#include "EvilBadge.h"
#include "DragonBadge.h"
#include "AnimMonsterBall.h"
#include "AnimSuperBall.h"
#include "AnimHyperBall.h"
#include "AnimMasterBall.h"
//Skill 헤더
#include "Tackle.h"
#include "Tackle2.h"
#include "Tackle3.h"
#include "WaterGun.h"
#include "WaterGun1.h"
#include "WaterGun2.h"
#include "WaterGun3.h"
#include "HydroPump.h"
#include "HydroPump1.h"
#include "HydroPump2.h"
#include "HydroPump3.h"
#include "HydroPump4.h"
#include "BugBuzz.h"
#include "BugBuzz1.h"
#include "BugBuzz2.h"
#include "BugBuzz3.h"
#include "Bite.h"
#include "Bite1.h"
#include "Bite2.h"
#include "Thunder.h"
#include "Thunder1.h"
#include "Thunder2.h"
#include "Thunder3.h"
#include "BreakCar.h"
#include "NoneSkill.h"

//Effect 헤더
#include "BattleEvent.h"
#include "Screen.h"
#include "BattleIntro.h"
#include "WaterBeam.h"
#include "WaterBall.h"
#include "Land.h"
#include "EvolLight.h"
#include "EvolLight2.h"
#include "EvolLight3.h"
#include "EvolLight4.h"
#include "Leaf.h"
#include "BallEffect.h"
#include "BallEffect2.h"
#include "BallEffect3.h"
#include "BallEffect4.h"
#include "BallEffect5.h"
#include "BallEffect6.h"
#include "BallRed.h"
#include "BallRed2.h"
#include "CaptureEffect.h"
#include "CaptureStar.h"


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

	/* 텍스쳐 로딩 중. */
	lstrcpy(m_szLoadingText, TEXT("                       텍스쳐 로딩 중."));

	/* UI 텍스쳐 */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_EndingErr"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Ending/%d.png"), 1))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Bag/%d.dds"), 12))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Item"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Item/%d.dds"), 51))))
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
	//Effect 텍스쳐
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Land"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Land/%d.png"), 16))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_EffectLight"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Light/%d.png"), 8))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Leaf"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Leaf/%d.png"), 1))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_BallEffect"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/BallEffect/%d.png"), 4))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_BallLand"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/BallLand/%d.png"), 4))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_BallRed"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/BallRed/%d.png"), 1))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Star"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Star/%d.png"), 1))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Tackle"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Skill/Tackle/%d.png"), 1))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Tackle2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Skill/Tackle/Tackle2/%d.png"), 8))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_WaterGun1"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Skill/WaterGun/WaterGun1/%d.png"), 8))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_WaterGun2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Skill/WaterGun/WaterGun2/%d.png"), 4))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_WaterGun3"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Skill/WaterGun/WaterGun3/%d.png"), 8))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_HydroPump1"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Skill/HydroPump/HydroPump1/%d.png"), 3))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_HydroPump2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Skill/HydroPump/HydroPump2/%d.png"), 8))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_BugBuzz1"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Skill/BugBuzz/BugBuzz1/%d.png"), 1))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_BugBuzz2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Skill/BugBuzz/BugBuzz2/%d.png"), 2))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_BugBuzz3"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Skill/BugBuzz/BugBuzz3/%d.png"), 4))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Bite2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Skill/Bite/Bite2/%d.png"), 4))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Thunder1"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Skill/Thunder/Thunder1/%d.png"), 8))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Thunder2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Skill/Thunder/Thunder2/%d.png"), 16))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Thunder3"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Skill/Thunder/Thunder3/%d.png"), 4))))
		return E_FAIL;
	/* 터레인 텍스쳐 */
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Terrain"),
	//	CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Grass_%d.dds"), 2))))
	//	return E_FAIL;



	/* 모델 로딩 중. */
	lstrcpy(m_szLoadingText, TEXT("                     모델 로딩 중."));
	
	/* 셰이더 로딩 중. */
	lstrcpy(m_szLoadingText, TEXT("                     셰이더 로딩 중."));
	
	lstrcpy(m_szLoadingText, TEXT("                     객체 생성 중."));

	/* 터레인 객체 */
	/*if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;*/

	/* 카메라 객체 */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Dynamic"),
		CCamera_Dynamic::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* UI 객체 */
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
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EndingErr"),
		CEndingErr::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* 아이템 객체 */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_None"),
		CNone::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MonsterBall"),
		CMonsterBall::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SuperBall"),
		CSuperBall::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_HyperBall"),
		CHyperBall::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MasterBall"),
		CMasterBall::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_HpPotion"),
		CHpPotion::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FullPotion"),
		CFullPotion::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MaxPotion"),
		CMaxPotion::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Candy"),
		CCandy::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ExpShare"),
		CExpShare::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Ball"),
		CBall::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_GrassBadge"),
		CGrassBadge::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_WaterBadge"),
		CWaterBadge::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FireBadge"),
		CFireBadge::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FightBadge"),
		CFightBadge::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_GhostBadge"),
		CGhostBadge::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FairyBadge"),
		CFairyBadge::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TerrainBadge"),
		CTerrainBadge::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_IceBadge"),
		CIceBadge::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EvilBadge"),
		CEvilBadge::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_DragonBadge"),
		CDragonBadge::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	_matrix			PivotMatrix = XMMatrixIdentity();

	PivotMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	CData_Manager::Get_Instance()->Create_Try_BinModel(TEXT("AnimMonsterBall"), LEVEL_STATIC, CData_Manager::DATA_ANIM);
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_AnimMonsterBall"),
	CAnimMonsterBall::Create(m_pDevice, m_pContext))))
	return E_FAIL;

	CData_Manager::Get_Instance()->Create_Try_BinModel(TEXT("AnimSuperBall"), LEVEL_STATIC, CData_Manager::DATA_ANIM);
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_AnimSuperBall"),
	CAnimSuperBall::Create(m_pDevice, m_pContext))))
	return E_FAIL;

	CData_Manager::Get_Instance()->Create_Try_BinModel(TEXT("AnimHyperBall"), LEVEL_STATIC, CData_Manager::DATA_ANIM);
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_AnimHyperBall"),
	CAnimHyperBall::Create(m_pDevice, m_pContext))))
	return E_FAIL;

	CData_Manager::Get_Instance()->Create_Try_BinModel(TEXT("AnimMasterBall"), LEVEL_STATIC, CData_Manager::DATA_ANIM);
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_AnimMasterBall"),
	CAnimMasterBall::Create(m_pDevice, m_pContext))))
	return E_FAIL;

	/* 포켓몬 객체 */

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

	CData_Manager::Get_Instance()->Create_Try_BinModel(TEXT("Wartortle"), LEVEL_STATIC, CData_Manager::DATA_ANIM);
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Wartortle"),
	CWartortle::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	CData_Manager::Get_Instance()->Create_Try_BinModel(TEXT("Blastoise"), LEVEL_STATIC, CData_Manager::DATA_ANIM);
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Blastoise"),
	CBlastoise::Create(m_pDevice, m_pContext))))
	return E_FAIL;

	CData_Manager::Get_Instance()->Create_Try_BinModel(TEXT("Caterpie"), LEVEL_STATIC, CData_Manager::DATA_ANIM);
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Caterpie"),
	CCaterpie::Create(m_pDevice, m_pContext))))
	return E_FAIL;

	CData_Manager::Get_Instance()->Create_Try_BinModel(TEXT("Metapod"), LEVEL_STATIC, CData_Manager::DATA_ANIM);
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Metapod"),
	CMetapod::Create(m_pDevice, m_pContext))))
	return E_FAIL;

	CData_Manager::Get_Instance()->Create_Try_BinModel(TEXT("Butterfree"), LEVEL_STATIC, CData_Manager::DATA_ANIM);
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Butterfree"),
	CButterfree::Create(m_pDevice, m_pContext))))
	return E_FAIL;

	CData_Manager::Get_Instance()->Create_Try_BinModel(TEXT("Rattata"), LEVEL_STATIC, CData_Manager::DATA_ANIM);
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Rattata"),
	CRattata::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	CData_Manager::Get_Instance()->Create_Try_BinModel(TEXT("Raticate"), LEVEL_STATIC, CData_Manager::DATA_ANIM);
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Raticate"),
	CRaticate::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	CData_Manager::Get_Instance()->Create_Try_BinModel(TEXT("Pikachu"), LEVEL_STATIC, CData_Manager::DATA_ANIM);
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Pikachu"),
		CPikachu::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	CData_Manager::Get_Instance()->Create_Try_BinModel(TEXT("Jigglypuff"), LEVEL_STATIC, CData_Manager::DATA_ANIM);
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Jigglypuff"),
	CJigglypuff::Create(m_pDevice, m_pContext))))
	return E_FAIL;

	CData_Manager::Get_Instance()->Create_Try_BinModel(TEXT("Wigglytuff"), LEVEL_STATIC, CData_Manager::DATA_ANIM);
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Wigglytuff"),
		CWigglytuff::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	CData_Manager::Get_Instance()->Create_Try_BinModel(TEXT("Meowth"), LEVEL_STATIC, CData_Manager::DATA_ANIM);
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Meowth"),
		CMeowth::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	CData_Manager::Get_Instance()->Create_Try_BinModel(TEXT("Persian"), LEVEL_STATIC, CData_Manager::DATA_ANIM);
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Persian"),
	CPersian::Create(m_pDevice, m_pContext))))
	return E_FAIL;

	CData_Manager::Get_Instance()->Create_Try_BinModel(TEXT("Slowpoke"), LEVEL_STATIC, CData_Manager::DATA_ANIM);
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Slowpoke"),
	CSlowpoke::Create(m_pDevice, m_pContext))))
	return E_FAIL;

	CData_Manager::Get_Instance()->Create_Try_BinModel(TEXT("Slowbro"), LEVEL_STATIC, CData_Manager::DATA_ANIM);
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Slowbro"),
		CSlowbro::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	CData_Manager::Get_Instance()->Create_Try_BinModel(TEXT("Onix"), LEVEL_STATIC, CData_Manager::DATA_ANIM);
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Onix"),
	COnix::Create(m_pDevice, m_pContext))))
	return E_FAIL;

	CData_Manager::Get_Instance()->Create_Try_BinModel(TEXT("Lickitung"), LEVEL_STATIC, CData_Manager::DATA_ANIM);
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Lickitung"),
	CLickitung::Create(m_pDevice, m_pContext))))
	return E_FAIL;

	CData_Manager::Get_Instance()->Create_Try_BinModel(TEXT("Chansey"), LEVEL_STATIC, CData_Manager::DATA_ANIM);
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Chansey"),
	CChansey::Create(m_pDevice, m_pContext))))
	return E_FAIL;

	CData_Manager::Get_Instance()->Create_Try_BinModel(TEXT("Kangaskhan"), LEVEL_STATIC, CData_Manager::DATA_ANIM);
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Kangaskhan"),
	CKangaskhan::Create(m_pDevice, m_pContext))))
	return E_FAIL;
	
	CData_Manager::Get_Instance()->Create_Try_BinModel(TEXT("Scyther"), LEVEL_STATIC, CData_Manager::DATA_ANIM);
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Scyther"),
	CScyther::Create(m_pDevice, m_pContext))))
	return E_FAIL;

	CData_Manager::Get_Instance()->Create_Try_BinModel(TEXT("Magikarp"), LEVEL_STATIC, CData_Manager::DATA_ANIM);
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Magikarp"),
	CMagikarp::Create(m_pDevice, m_pContext))))
	return E_FAIL;
	
	CData_Manager::Get_Instance()->Create_Try_BinModel(TEXT("Gyarados"), LEVEL_STATIC, CData_Manager::DATA_ANIM);
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Gyarados"),
		CGyarados::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	CData_Manager::Get_Instance()->Create_Try_BinModel(TEXT("Snorlax"), LEVEL_STATIC, CData_Manager::DATA_ANIM);
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Snorlax"),
		CSnorlax::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	CData_Manager::Get_Instance()->Create_Try_BinModel(TEXT("Dratini"), LEVEL_STATIC, CData_Manager::DATA_ANIM);
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Dratini"),
	CDratini::Create(m_pDevice, m_pContext))))
	return E_FAIL;

	CData_Manager::Get_Instance()->Create_Try_BinModel(TEXT("Dragonair"), LEVEL_STATIC, CData_Manager::DATA_ANIM);
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Dragonair"),
	CDragonair::Create(m_pDevice, m_pContext))))
	return E_FAIL;

	CData_Manager::Get_Instance()->Create_Try_BinModel(TEXT("Dragonite"), LEVEL_STATIC, CData_Manager::DATA_ANIM);
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Dragonite"),
	CDragonite::Create(m_pDevice, m_pContext))))
	return E_FAIL;

	CData_Manager::Get_Instance()->Create_Try_BinModel(TEXT("Mewtwo"), LEVEL_STATIC, CData_Manager::DATA_ANIM);
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Mewtwo"),
	CMewtwo::Create(m_pDevice, m_pContext))))
	return E_FAIL;

	CData_Manager::Get_Instance()->Create_Try_BinModel(TEXT("Mew"), LEVEL_STATIC, CData_Manager::DATA_ANIM);
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Mew"),
	CMew::Create(m_pDevice, m_pContext))))
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

	CData_Manager::Get_Instance()->Create_Try_BinModel(TEXT("Beat"), LEVEL_STATIC, CData_Manager::DATA_ANIM);
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Beat"),
	CBeat::Create(m_pDevice, m_pContext))))
	return E_FAIL;

	CData_Manager::Get_Instance()->Create_Try_BinModel(TEXT("Sonia"), LEVEL_STATIC, CData_Manager::DATA_ANIM);
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sonia"),
		CSonia::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	CData_Manager::Get_Instance()->Create_Try_BinModel(TEXT("Dandel"), LEVEL_STATIC, CData_Manager::DATA_ANIM);
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Dandel"),
		CDandel::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	CData_Manager::Get_Instance()->Create_Try_BinModel(TEXT("Olive"), LEVEL_STATIC, CData_Manager::DATA_ANIM);
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Olive"),
		COlive::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	CData_Manager::Get_Instance()->Create_Try_BinModel(TEXT("Tr0"), LEVEL_STATIC, CData_Manager::DATA_ANIM);
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Tr0"),
		CTr0::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	CData_Manager::Get_Instance()->Create_Try_BinModel(TEXT("Tr1"), LEVEL_STATIC, CData_Manager::DATA_ANIM);
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Tr1"),
		CTr1::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	CData_Manager::Get_Instance()->Create_Try_BinModel(TEXT("Tr2"), LEVEL_STATIC, CData_Manager::DATA_ANIM);
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Tr2"),
		CTr2::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	CData_Manager::Get_Instance()->Create_Try_BinModel(TEXT("Tr3"), LEVEL_STATIC, CData_Manager::DATA_ANIM);
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Tr3"),
		CTr3::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	CData_Manager::Get_Instance()->Create_Try_BinModel(TEXT("Tr4"), LEVEL_STATIC, CData_Manager::DATA_ANIM);
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Tr4"),
		CTr4::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	CData_Manager::Get_Instance()->Create_Try_BinModel(TEXT("Tr5"), LEVEL_STATIC, CData_Manager::DATA_ANIM);
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Tr5"),
		CTr5::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	CData_Manager::Get_Instance()->Create_Try_BinModel(TEXT("Tr6"), LEVEL_STATIC, CData_Manager::DATA_ANIM);
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Tr6"),
		CTr6::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	CData_Manager::Get_Instance()->Create_Try_BinModel(TEXT("Tr7"), LEVEL_STATIC, CData_Manager::DATA_ANIM);
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Tr7"),
		CTr7::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	CData_Manager::Get_Instance()->Create_Try_BinModel(TEXT("Tr8"), LEVEL_STATIC, CData_Manager::DATA_ANIM);
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Tr8"),
		CTr8::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	CData_Manager::Get_Instance()->Create_Try_BinModel(TEXT("Tr9"), LEVEL_STATIC, CData_Manager::DATA_ANIM);
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Tr9"),
		CTr9::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	CData_Manager::Get_Instance()->Create_Try_BinModel(TEXT("MonsterBall"), LEVEL_STATIC, CData_Manager::DATA_PARTS);
	CData_Manager::Get_Instance()->Create_Try_BinModel(TEXT("SuperBall"), LEVEL_STATIC, CData_Manager::DATA_PARTS);
	CData_Manager::Get_Instance()->Create_Try_BinModel(TEXT("HyperBall"), LEVEL_STATIC, CData_Manager::DATA_PARTS);
	CData_Manager::Get_Instance()->Create_Try_BinModel(TEXT("MasterBall"), LEVEL_STATIC, CData_Manager::DATA_PARTS);

	//Map


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
	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/fbx/SkyBox/SkyBox.fbx"))))
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

	/* 스킬 객체 */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Tackle"),
		CTackle::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Tackle2"),
		CTackle2::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Tackle3"),
		CTackle3::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_WaterGun"),
		CWaterGun::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_WaterGun1"),
		CWaterGun1::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_WaterGun2"),
		CWaterGun2::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_WaterGun3"),
		CWaterGun3::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_HydroPump"),
		CHydroPump::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_HydroPump1"),
		CHydroPump1::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_HydroPump2"),
		CHydroPump2::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_HydroPump3"),
		CHydroPump3::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_HydroPump4"),
		CHydroPump4::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BugBuzz"),
		CBugBuzz::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BugBuzz1"),
		CBugBuzz1::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BugBuzz2"),
		CBugBuzz2::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BugBuzz3"),
		CBugBuzz3::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Bite"),
		CBite::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	CData_Manager::Get_Instance()->Create_Try_BinModel(TEXT("Bite"), LEVEL_STATIC, CData_Manager::DATA_ANIM);
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Bite1"),
		CBite1::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Bite2"),
		CBite2::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Thunder"),
		CThunder::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Thunder1"),
		CThunder1::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Thunder2"),
		CThunder2::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Thunder3"),
		CThunder3::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BreakCar"),
		CBreakCar::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_NoneSkill"),
		CNoneSkill::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* Effect 객체 */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BattleEvent"),
		CBattleEvent::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Screen"),
		CScreen::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BattleIntro"),
		CBattleIntro::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Land"),
		CLand::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EvolLight"),
		CEvolLight::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Cross"),
	//CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/fbx/Cross/Cross.fbx", PivotMatrix))))
	//return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EvolLight2"),
		CEvolLight2::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EvolLight3"),
		CEvolLight3::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EvolLight4"),
		CEvolLight4::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Leaf"),
		CLeaf::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BallEffect"),
		CBallEffect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BallEffect2"),
		CBallEffect2::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BallEffect3"),
		CBallEffect3::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BallEffect4"),
		CBallEffect4::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BallEffect5"),
		CBallEffect5::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BallEffect6"),
		CBallEffect6::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BallRed"),
		CBallRed::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BallRed2"),
		CBallRed2::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CaptureEffect"),
		CCaptureEffect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CaptureStar"),
		CCaptureStar::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Cylinder"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/fbx/Cylinder/Cylinder.fbx", PivotMatrix))))
	//	return E_FAIL;
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Cone"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/fbx/Cone/Cone.fbx", PivotMatrix))))
	//	return E_FAIL;
	CData_Manager::Get_Instance()->Create_Try_BinModel(TEXT("WaterBall"), LEVEL_STATIC, CData_Manager::DATA_NONANIM);
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_WaterBeam"),
		CWaterBeam::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_WaterBall"),
		CWaterBall::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	lstrcpy(m_szLoadingText, TEXT("                        로딩이 완료되었습니다."));

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
