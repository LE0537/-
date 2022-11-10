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
END


BEGIN(Client)

class CSnorlax : public CGameObj
{
private:
	CSnorlax(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSnorlax(const CSnorlax& rhs);
	virtual ~CSnorlax() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	HRESULT SetUp_ShaderResources();
	HRESULT Ready_Components();
	void	Set_DeckPos();
	void	Key_Input(_float fTimeDelta);
	void	Set_RidePos();
	void	CheckRide();
	void	Battle(_float fTimeDelta);
	void	Set_Stats();
private:
	CModel*					m_pModelCom = nullptr;

	_int					m_iAnim = 0;
	_bool					m_bSetPos = false;
	_bool					m_bBrath = false;
	_bool					m_bBattle = false;
	_float					m_fStartBattle = 0.f;
	_bool					m_bAttack = false;
	_bool					m_bHit = false;
	_bool					m_bDown = false;
	_bool					m_bStopAnim = false;

	_float					m_fX, m_fY, m_fSizeX, m_fSizeY;
	_float4x4				m_ViewMatrix, m_ProjMatrix;

public:
	static CSnorlax* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END
