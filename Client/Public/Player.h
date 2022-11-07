#pragma once
#include "Client_Defines.h"
#include "GameObj.h"


BEGIN(Engine)

class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
class CModel;
class CCollider;
END


BEGIN(Client)

class CPlayer : public CGameObj
{
private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;
public:
	enum PLAYER_STATE{IDLE,CATCHBALL,BATTLEBALL,ATTACK,WALK,RUN,GET,GET_END,TAKEPOKE,
		RIDE_SNORLAX,RUN_SNORLAX,RIDE_KANGASKHAN,RUN_KANGASKHAN,PLAYER_END};
public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();
private:
	HRESULT SetUp_ShaderResources();
	HRESULT Ready_Components();
	void	CheckRideIDLE();
	void	Key_Input(_float fTimeDelta);
	void	Battle(_float fTimeDelta);
	void	BattleStart(_float fTimeDelta);
	void	Ready_Script();
public:
	void	Set_BattleStart() { m_bBattleStart = true; }
	void	Set_Bag(class CBag* _Bag) { m_pBag = _Bag; }
private:
	CModel*					m_pModelCom = nullptr;
	CCollider*				m_pAABBCom = nullptr;
	CCollider*				m_pOBBCom = nullptr;

	class CBag*				m_pBag = nullptr;

	vector<wstring>		    m_vBattleScript;

	_int					m_iAnim = 0;
	_bool					m_bBattleText = false;
	_bool					m_bBattleStart = false;
	_bool					m_bBattle = false;
	_float					m_fStartBattle = 0.f;
	_float					m_fBattleUITime = 0.f;
	_bool					m_bBattleUI = false;
public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END