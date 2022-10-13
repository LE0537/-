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

class CNone : public CGameObj
{
private:
	CNone(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNone(const CNone& rhs);
	virtual ~CNone() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

private:

private:
	_float					m_fX, m_fY, m_fSizeX, m_fSizeY;
	_float4x4				m_ViewMatrix, m_ProjMatrix;

	_bool					m_bInven = false;
	_bool					m_bSelect = false;
private:
	HRESULT Ready_Components();
	HRESULT SetUp_ShaderResources(); /* 셰이더 전역변수에 값을 전달한다. */

public:
	static CNone* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END
