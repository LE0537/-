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

class CWaterBeam final : public CGameObj
{
private:
	CWaterBeam(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWaterBeam(const CWaterBeam& rhs);
	virtual ~CWaterBeam() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	CTransform*				m_pTransformCom2 = nullptr;
	CModel*					m_pModelCom = nullptr;
	CModel*					m_pModelCom2 = nullptr;

	_float					m_fDeadTime = 0.f;
	_bool					m_bSetPos = false;
private:
	HRESULT Ready_Components();
	HRESULT SetUp_ShaderResources(); /* 셰이더 전역변수에 값을 전달한다. */
	void	Set_Pos(_float fTimeDelta);
	
public:
	static CWaterBeam* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObj* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END