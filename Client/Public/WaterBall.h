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

class CWaterBall final : public CGameObj
{
private:
	CWaterBall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWaterBall(const CWaterBall& rhs);
	virtual ~CWaterBall() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

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
	static CWaterBall* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObj* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END