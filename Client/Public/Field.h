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
class CNavigation;
END


BEGIN(Client)

class CField : public CGameObj
{
private:
	CField(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CField(const CField& rhs);
	virtual ~CField() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	HRESULT SetUp_ShaderResources();
	HRESULT Ready_Components();

	HRESULT	Create_Cell();
	void	Check_Range();
	void	Sort_Cell();

	HRESULT	Ending(_float fTimeDelta);
private:
	CModel*					m_pModelCom = nullptr;
	CNavigation*			m_pNavigationCom = nullptr;


	_bool					m_bPicking = false;
	_float3							m_vMousePos;
	_float3			vPoints[3];
	_int			m_iCheckPoints = 0;
	_int			m_iEndinCell = 0;
	_bool			m_bEnding = false;
	_float			m_fDeadtime = 0.f;
	_bool			m_bDeadSound = false;
public:
	static CField* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END