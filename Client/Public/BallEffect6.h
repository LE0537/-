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

class CBallEffect6 : public CGameObj
{
private:
	CBallEffect6(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBallEffect6(const CBallEffect6& rhs);
	virtual ~CBallEffect6() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT Render_ShadowDepth();
private:
	HRESULT SetUp_ShaderResources();
	HRESULT Ready_Components();
	void	OnBillboard();

private:

	_float		m_fFrameTime = 0.f;
	_int		m_iFrame = 0;
	_float					m_fSize = 0.f;
	_float4		m_vPos;
public:
	static CBallEffect6* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END
