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

class CMari : public CGameObj
{
private:
	CMari(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMari(const CMari& rhs);
	virtual ~CMari() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	HRESULT SetUp_ShaderResources();
	HRESULT Ready_Components();
	void	Move(_float fTimeDelta);
	void	Ready_Script();
	void	Check_Coll();
	void	Battle(_float fTimeDelta);
	void	BattleStart(_float fTimeDelta);
private:
	CModel*					m_pModelCom = nullptr;
	CCollider*				m_pAABBCom = nullptr;
	CCollider*				m_pOBBCom = nullptr;

	vector<wstring>		    m_vNormalScript;

	_int					m_iAnim = 0;
	_bool					m_bFindPlayer = false;
	_float					m_fEventTime = 0.f;
	_bool					m_bEvent = false;
	//Battle

	_bool					m_bBattle = false;
	_float					m_fStartBattle = 0.f;
public:
	static CMari* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END