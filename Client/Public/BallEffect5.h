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

class CBallEffect5 : public CGameObj
{
private:
	CBallEffect5(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBallEffect5(const CBallEffect5& rhs);
	virtual ~CBallEffect5() = default;


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

private:

	_float		m_fFrameTime = 0.f;
	_int		m_iFrame = 0;
	_float					m_fSize = 0.f;

public:
	static CBallEffect5* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END
