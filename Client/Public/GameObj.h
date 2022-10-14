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


	}PLAYERINFO;
	typedef struct tagInfo2 {
		wstring		strName;
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
		_int		iItem;
		_int		iSex;
		_bool		bItem;
		POKETYPE	eType;
		POKETYPE	eType2;
		STATINFO    eStatInfo;
	}POKEINFO;
	typedef struct tagInfo3 {
		wstring		strName;
		_int		iDmg;
		_int		iSDmg;
		POKETYPE	eType;
		
	}SKILLINFO;
	typedef struct tagInfo4 {
		wstring		strName;
		wstring		strInfo;
		_int		iPrice;
		_int		iItemNum;
		_int		iNum;

	}ITEMINFO;
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

public:
	POKEINFO Get_PokeInfo() {return m_PokemonInfo;}
	PLAYERINFO Get_PalyerInfo() { return m_PlayerInfo; }
	SKILLINFO Get_SkillInfo() { return m_SkillInfo; }
	ITEMINFO Get_ItemInfo() { return m_ItemInfo; }

	
public:
	static CGameObj* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END