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

class CBreakCar1 final : public CGameObj
{
private:
	CBreakCar1(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBreakCar1(const CBreakCar1& rhs);
	virtual ~CBreakCar1() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	CModel*					m_pModelCom = nullptr;

	_float					m_fSkillTime = 0.f;
	_float4					m_vLook;
	_float					m_fScale = 0.f;
	_float					m_fDeadTime = 0.f;

	_float					m_fSize = 0.f;
private:
	HRESULT Ready_Components();
	HRESULT SetUp_ShaderResources(); /* 셰이더 전역변수에 값을 전달한다. */
	void	Set_Pos(_float fTimeDelta);

public:
	static CBreakCar1* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObj* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END
