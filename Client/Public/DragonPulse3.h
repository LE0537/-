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

class CDragonPulse3 : public CGameObj
{
private:
	CDragonPulse3(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDragonPulse3(const CDragonPulse3& rhs);
	virtual ~CDragonPulse3() = default;


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
	void	Set_Pos(_float fTimeDelta);
private:

	_float					m_fSize = 0.f;

	_float					m_fDeadTime = 0.f;

	_float					m_fFrameTime = 0.f;
	_int					m_iFrame = 0;

	_bool					m_bUp = false;
	_bool					m_bSkill = false;
public:
	static CDragonPulse3* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END
