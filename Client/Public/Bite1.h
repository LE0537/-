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

class CBite1 final : public CGameObj
{
private:
	CBite1(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBite1(const CBite1& rhs);
	virtual ~CBite1() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT Render_ShadowDepth();
private:
	CModel*					m_pModelCom = nullptr;

	_float4					m_vLook;
	_float					m_fScale = 0.f;
	_float					m_fDeadTime = 0.f;
private:
	HRESULT Ready_Components();
	HRESULT SetUp_ShaderResources(); /* 셰이더 전역변수에 값을 전달한다. */
	void	Set_Pos(_float fTimeDelta);

public:
	static CBite1* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObj* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END
