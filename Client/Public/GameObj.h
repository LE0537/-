#pragma once
#include "Client_Defines.h"
#include "GameObject.h"


BEGIN(Engine)

class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Terrain;

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
		_short		sPokeNum;
		_short		sMaxHp;
		_short		sHp;
		_short		sDmg;
		_short		sSDmg;
		_short		sDef;
		_short		sSDef;
		_short		sSpeed;
		_short		sLv;
		_short		sMaxExp;
		_short		sExp;
		_short		sItem;
		_short		sSex;
		_bool		bItem;
		POKETYPE	eType;
		POKETYPE	eType2;

	}POKEINFO;
	typedef struct tagInfo3 {
		wstring		strName;
		_short		sDmg;
		_short		sSDmg;
		POKETYPE	eType;
		
	}SKILLINFO;
protected:
	CShader*				m_pShaderCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CVIBuffer_Terrain*		m_pVIBufferCom = nullptr;

protected:
	PLAYERINFO				m_PlayerInfo;
	POKEINFO				m_PokemonInfo;
	SKILLINFO				m_SkillInfo;

public:
	static CGameObj* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END