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
	virtual HRESULT Render_ShadowDepth();
private:

	_float					m_fSkillTime = 0.f;
	_float					m_fDeadTime = 0.f;
	_bool					m_bSetPos = false;

	_bool		m_bSound = false;
private:

	void	Set_Pos(_float fTimeDelta);
	
public:
	static CWaterBeam* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObj* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END
