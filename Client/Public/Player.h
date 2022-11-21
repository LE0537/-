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
class CNavigation;
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
	void	OnNavi();
	HRESULT SetUp_ShaderResources();
	HRESULT Ready_Components();
	void	CheckRideIDLE();
	void	Key_Input(_float fTimeDelta);
	void	Battle(_float fTimeDelta);
	void	BattleStart(_float fTimeDelta);
	void	Ready_Script();
public:
	_bool	Get_Return() { return m_bReturn; }
	void	Set_BattleStart() { m_bBattleStart = true; }
	void	Set_Bag(class CBag* _Bag) { m_pBag = _Bag; }
	class CBag* Get_Bag() { return m_pBag; }
	void	Set_TargetPoke(vector<CGameObject*>* _pvecPoke) { m_pvecTargetPoke = _pvecPoke; }
	void	Set_BattleTarget(CGameObject* _pTarget, BATTLETYPE eBattle_Type) { m_pBattleTarget = _pTarget; m_eBattleType = eBattle_Type; }
	void	Battle_Win();
	void	Check_Anim(_float fTimeDelta);
private:
	CModel*					m_pModelCom = nullptr;
	CCollider*				m_pAABBCom = nullptr;
	CCollider*				m_pOBBCom = nullptr;
	CNavigation*			m_pNavigationCom = nullptr;

	class CBag*				m_pBag = nullptr;
	
	CGameObject*			m_pBattleTarget = nullptr;
	vector<wstring>		    m_vBattleScript;
	vector<CGameObject*>*    m_pvecTargetPoke;

	wstring					m_szTargetName;
	_int					m_iAnim = 0;
	_bool					m_bBattleText = false;
	_bool					m_bBattleStart = false;
	_bool					m_bBattle = false;
	_float					m_fStartBattle = 0.f;
	_float					m_fBattleUITime = 0.f;
	_bool					m_bBattleUI = false;

	_bool					m_bPrevPos = false;
	_float4					m_vPrevPos;
	//Battle
	_bool					m_ChangePoke = false;
	_bool					m_bChangeAnim = false;
	_bool					m_bReturnPoke = false;
	_bool					m_bReturn = false;
	BATTLETYPE				m_eBattleType = BATTLE_END;
public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END