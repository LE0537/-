#pragma once
#include "Client_Defines.h"
#include "GameObject.h"


BEGIN(Engine)

class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;

END


BEGIN(Client)

class CGameObj : public CGameObject
{
protected:
	CGameObj(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObj(const CGameObj& rhs);
	virtual ~CGameObj() = default;
public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	typedef struct tagInfo {
		wstring		strName;
		_int		iMoney;
		_int		iLevelIndex;
		_bool		bRide;
		_int		iRideNum;
		_bool		bEvent;
		_bool		bBattle;
	}PLAYERINFO;
	typedef struct tagInfo2 {
		wstring		strName;
		wstring		strInfo;
		_int		iSkillNum;
		_int		iDmg;
		_int		iSDmg;
		_float		fHit;
		_int		iMaxPoint;
		_int		iPoint;
		POKETYPE	eType;
		
	}SKILLINFO;
	typedef struct tagInfo3 {
		wstring		strName;
		wstring		strInfo;
		_int		iPrice;
		_int		iItemNum;
		_int		iNum;

	}ITEMINFO;
	typedef struct tagInfo4 {
		wstring		strName;
		wstring		strInfo;
		wstring		strChar;
		_int		iPokeNum;
		_int		iMaxHp;
		_int		iHp;
		_int		iDmg;
		_int		iSDmg;
		_int		iDef;
		_int		iSDef;
		_int		iSpeed;
		_int		iLv;
		_int		iMaxExp;
		_int		iExp;
		_int		iSex;
		_int		iBallNum;
		_bool		bRide;
		_bool		bEvolution;
		_bool		bLvUp;
		ITEMINFO*		eItem;
		SKILLINFO*		eSkillNum1;
		SKILLINFO*		eSkillNum2;
		SKILLINFO*		eSkillNum3;
		SKILLINFO*		eSkillNum4;
		POKETYPE	eType;
		POKETYPE	eType2;
		STATINFO    eStatInfo;
	}POKEINFO;

protected:
	CShader*				m_pShaderCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;

protected:
	PLAYERINFO				m_PlayerInfo;
	POKEINFO				m_PokemonInfo;
	SKILLINFO				m_SkillInfo;
	ITEMINFO				m_ItemInfo;
	_bool					m_bOnOff = false;
	_bool					m_bDeckInfo = false;
	_bool					m_bDeckPoke = false;
	CGameObject*			m_pTarget = nullptr;
	CGameObject*			m_pCamera = nullptr;
	_float4					m_vMyBattlePos;
	_float4					m_vTargetBattlePos;
	_int					m_iAnimIndex = 0;
	_bool					m_bBattleMap = false;
	_float					m_fBattleMapTime = 0.f;
	_int					m_iPokeSize = 0;
	_int					m_iPokeMaxSize = 0;
	_bool					m_bAnimReset = false;
public:
	POKEINFO Get_PokeInfo() {return m_PokemonInfo;}
	PLAYERINFO Get_PalyerInfo() { return m_PlayerInfo; }
	SKILLINFO Get_SkillInfo() { return m_SkillInfo; }
	ITEMINFO Get_ItemInfo() { return m_ItemInfo; }

	CGameObject* Get_Target() {	return m_pTarget;}
	void Set_Target(CGameObject* pTarget) { m_pTarget = pTarget; }
	CTransform* Get_Transfrom() { return m_pTransformCom; }
	_float4 Get_MyBattlePos() { return m_vMyBattlePos; }
	_float4 Get_TargetBattlePos() { return m_vTargetBattlePos; }
	void	Reset_Anim() { m_bAnimReset = true; }
	void	Set_AnimIndex(_int _iAnimIndex) { m_iAnimIndex = _iAnimIndex; }
	void	Set_BattleMap(_bool _bBattle, _float _fTime) { m_bBattleMap = _bBattle; m_fBattleMapTime = _fTime; }
	void	Set_PokeSize() { ++m_iPokeSize; }
	_int	Get_PokeMaxSize() { return m_iPokeMaxSize; }
	//PlayerInfo
	_bool Get_Ride() { return m_PlayerInfo.bRide; }
	void OnOffRide() { m_PlayerInfo.bRide = !m_PlayerInfo.bRide; }
	void Set_RideIndex(_int iIndex) { m_PlayerInfo.iRideNum = iIndex; }

	_bool Get_Event() { return m_PlayerInfo.bEvent; }
	void OnOffEvent() { m_PlayerInfo.bEvent = !m_PlayerInfo.bEvent; }
	_bool Get_bBattle() { return m_PlayerInfo.bBattle; }
	void OnOffbBattle() { m_PlayerInfo.bBattle = !m_PlayerInfo.bBattle; }
	


	//PokeInfo
	void	Set_PokeHp(_int _iDmg) {
		m_PokemonInfo.iHp += _iDmg;
		if (m_PokemonInfo.iHp > m_PokemonInfo.iMaxHp) { m_PokemonInfo.iHp = m_PokemonInfo.iMaxHp; }
		else if (m_PokemonInfo.iHp < 0) { m_PokemonInfo.iHp = 0; }
	}
	void	Set_PokeEXP(_int _iEXP) { m_PokemonInfo.iExp += _iEXP;
		if (m_PokemonInfo.iExp >= m_PokemonInfo.iMaxExp) { m_PokemonInfo.iExp = 0;
		++m_PokemonInfo.iLv; m_PokemonInfo.iMaxExp += 5; m_PokemonInfo.bLvUp = true;
		}
	}
	void	Set_PokePP1(_int _iPP) {
		m_PokemonInfo.eSkillNum1->iPoint += _iPP;
		if (m_PokemonInfo.eSkillNum1->iPoint > m_PokemonInfo.eSkillNum1->iMaxPoint) { m_PokemonInfo.eSkillNum1->iPoint = m_PokemonInfo.eSkillNum1->iMaxPoint; }
		else if (m_PokemonInfo.eSkillNum1->iPoint < 0) { m_PokemonInfo.eSkillNum1->iPoint = 0; }
	}
	void	Set_PokePP2(_int _iPP) {
		m_PokemonInfo.eSkillNum2->iPoint += _iPP;
		if (m_PokemonInfo.eSkillNum2->iPoint > m_PokemonInfo.eSkillNum2->iMaxPoint) { m_PokemonInfo.eSkillNum2->iPoint = m_PokemonInfo.eSkillNum2->iMaxPoint; }
		else if (m_PokemonInfo.eSkillNum2->iPoint < 0) { m_PokemonInfo.eSkillNum2->iPoint = 0; }
	}
	void	Set_PokePP3(_int _iPP) {
		m_PokemonInfo.eSkillNum3->iPoint += _iPP;
		if (m_PokemonInfo.eSkillNum3->iPoint > m_PokemonInfo.eSkillNum3->iMaxPoint) { m_PokemonInfo.eSkillNum3->iPoint = m_PokemonInfo.eSkillNum3->iMaxPoint; }
		else if (m_PokemonInfo.eSkillNum3->iPoint < 0) { m_PokemonInfo.eSkillNum3->iPoint = 0; }
	}
	void	Set_PokePP4(_int _iPP) {
		m_PokemonInfo.eSkillNum4->iPoint += _iPP;
		if (m_PokemonInfo.eSkillNum4->iPoint > m_PokemonInfo.eSkillNum4->iMaxPoint) { m_PokemonInfo.eSkillNum4->iPoint = m_PokemonInfo.eSkillNum4->iMaxPoint; }
		else if (m_PokemonInfo.eSkillNum4->iPoint < 0) { m_PokemonInfo.eSkillNum4->iPoint = 0; }
	}
	void	Set_PokeItem(ITEMINFO* _eItem) { m_PokemonInfo.eItem = _eItem; }
	void	Set_StatInfo(STATINFO eStatInfo) { m_PokemonInfo.eStatInfo = eStatInfo; }



	void	Set_DeckPoke() { m_bDeckPoke = true; }
	void	Set_PokeUIOnOff(){ m_bOnOff = !m_bOnOff; }
	void	Set_DeckInfoOn() { m_bDeckInfo = false; }
	void	Set_DeckInfoOff() { m_bDeckInfo = true; }


public:
	static CGameObj* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END