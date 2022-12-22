#pragma once
#include "Client_Defines.h"
#include "GameObj.h"
#include "VIBuffer_Trail.h"

BEGIN(Engine)

class CShader;
class CTexture;
class CRenderer;
class CTransform;
END


BEGIN(Client)

class CTrail : public CGameObj
{
private:
	CTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTrail(const CTrail& rhs);
	virtual ~CTrail() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT Render_ShadowDepth();
public:
	void	Set_Trail(CVIBuffer_Trail::BUFFERDESC _BUFFERDESC);
	void	Reset_Trail();
private:
	HRESULT SetUp_ShaderResources();
	HRESULT Ready_Components();

private:
	CVIBuffer_Trail*		m_pVIBufferTrailCom = nullptr;

	_float		m_fFrameTime = 0.f;
	_int		m_iFrame = 0;

public:
	static CTrail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END
