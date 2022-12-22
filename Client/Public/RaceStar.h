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

class CRaceStar : public CGameObj
{
private:
	CRaceStar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRaceStar(const CRaceStar& rhs);
	virtual ~CRaceStar() = default;


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
private:


	_float		m_fOffSet = 0.f;


public:
	static CRaceStar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END
