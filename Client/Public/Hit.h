#pragma once
#include "Client_Defines.h"
#include "GameObj.h"


BEGIN(Engine)

class CShader;
class CCollider;
class CRenderer;
class CTransform;
class CModel;
class CHierarchyNode;

END


BEGIN(Client)

class CHit final : public CGameObj
{
private:
	CHit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHit(const CHit& rhs);
	virtual ~CHit() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT Render_ShadowDepth();
private:
	CModel*					m_pModelCom = nullptr;


private:


private:
	HRESULT Ready_Components();
	HRESULT SetUp_ShaderResources(); /* 셰이더 전역변수에 값을 전달한다. */


public:
	static CHit* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObj* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END
