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

class CDandel : public CGameObj
{
private:
	CDandel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDandel(const CDandel& rhs);
	virtual ~CDandel() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT Render_ShadowDepth();
private:
	HRESULT Ready_Parts();
	HRESULT SetUp_ShaderResources();
	HRESULT Ready_Components();
	void	Move(_float fTimeDelta);
	void	Ready_Script();
	void	Ready_LoseText();
	void	Ready_EndingText();
	void	Check_Coll();
	void	Battle(_float fTimeDelta);
	void	BattleStart(_float fTimeDelta);
	void	Check_Anim(_float fTimeDelta);
	void	OnNavi();
	void	Ckeck_Dist();
private:
	CModel*					m_pModelCom = nullptr;
	CCollider*				m_pAABBCom = nullptr;
	CCollider*				m_pOBBCom = nullptr;
	CNavigation*			m_pNavigationCom = nullptr;

	vector<wstring>		    m_vNormalScript;
	vector<wstring>		    m_vBattleScript;

	vector<CGameObject*>    m_vecPoke;

	CGameObject*			m_pBall = nullptr;

	_float					m_fDist = 0.f;
	_int					m_iAnim = 0;
	_bool					m_bFindPlayer = false;
	_float					m_fEventTime = 0.f;
	_bool					m_bEvent = false;

	_bool					m_bEffect = false;
	_bool					m_bEffectEnd = false;
	_bool					m_bMotionEnd = false;

	//Battle
	_bool					m_bBattleText = false;
	_bool					m_bBattle = false;
	_bool					m_bChangeAnim = false;
	_float					m_fStartBattle = 0.f;
	_bool					m_ChangePoke = false;
	_bool					m_bLose = false;
	_bool					m_OnOff = false;
	//BattleEnd
	_float4					m_vPrevPos;
	_bool					m_bPrevPos = false;
	_bool					m_bBattleLose = false;
	_bool					m_bEndingText = false;
public:
	static CDandel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END
