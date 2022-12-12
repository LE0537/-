#pragma once
#include "Client_Defines.h"
#include "GameObj.h"


BEGIN(Engine)

class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
END


BEGIN(Client)

class CCaptureStar : public CGameObj
{
private:
	CCaptureStar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCaptureStar(const CCaptureStar& rhs);
	virtual ~CCaptureStar() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	HRESULT SetUp_ShaderResources();
	HRESULT Ready_Components();
	void	OnBillboard();
	void	Set_Pos(_float fTimeDelta);
	void	Check_Pos(_int _iIndex);
private:

	_float					m_fSize = 0.f;

	_float					m_fDeadTime = 0.f;
	_bool					m_bEffect = false;
	_bool					m_bDeadEffect = false;

	_float4					m_vPos1, m_vPos2, m_vPos3, m_vPos4;
	_float					m_fSpeed = 0.f;

public:
	static CCaptureStar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END
