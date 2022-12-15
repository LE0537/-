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

class CRockSlide1 : public CGameObj
{
private:
	CRockSlide1(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRockSlide1(const CRockSlide1& rhs);
	virtual ~CRockSlide1() = default;


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

	_float					m_fFrame = 0.f;
	_int					m_iFrame = 0;

	_float					m_fSize = 0.f;

	_float					m_fDeadTime = 0.f;
	_bool					m_bEffect = false;
	_bool					m_bDeadEffect = false;
public:
	static CRockSlide1* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END
