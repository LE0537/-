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

private:
	CModel*					m_pModelCom = nullptr;
	CNavigation*			m_pNavigationCom = nullptr;



public:
	static CField* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END