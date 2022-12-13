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

class CThunder2 : public CGameObj
{
private:
	CThunder2(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CThunder2(const CThunder2& rhs);
	virtual ~CThunder2() = default;


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
private:

	_float					m_fSize = 0.f;

	_float					m_fDeadTime = 0.f;

	_float					m_fFrameTime = 0.f;
	_int					m_iFrame = 0;
public:
	static CThunder2* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END
