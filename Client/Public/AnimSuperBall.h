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
class CCollider;
class CNavigation;
END


BEGIN(Client)

class CAnimSuperBall : public CGameObj
{
private:
	CAnimSuperBall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAnimSuperBall(const CAnimSuperBall& rhs);
	virtual ~CAnimSuperBall() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

private:

	HRESULT SetUp_ShaderResources();
	HRESULT Ready_Components();
	void	Move(_float fTimeDelta);
	void	Ready_SuccessScript();
	void	Ready_SuccessFailScript();
	void	Ready_FailScript();

	void	Capture_Success();
	_bool	Capture();

private:
	CModel*					m_pModelCom = nullptr;

	vector<wstring>		    m_vNormalScript;

	CGameObject*			m_pTargetPoke = nullptr;

	_bool					m_bCaptureStart = false;
	_bool					m_bCapture = false;
	_bool					m_bFail = false;
	_bool					m_bSuccessFail = false;
	_bool					m_bCaptureSuccesss = false;
	_float					m_fFailTime = 0.f;
	_float					m_fSuccessTime = 0.f;
public:
	static CAnimSuperBall* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END
