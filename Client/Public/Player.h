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
private:
	CModel*					m_pModelCom = nullptr;
	CCollider*				m_pAABBCom = nullptr;
	CCollider*				m_pOBBCom = nullptr;
	CCollider*				m_pSPHERECom = nullptr;

	_int					m_iAnim = 0;

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END