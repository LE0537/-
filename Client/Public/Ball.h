#pragma once
#include "Client_Defines.h"
#include "GameObject.h"


BEGIN(Engine)

class CShader;
class CCollider;
class CRenderer;
class CTransform;
class CModel;
class CHierarchyNode;

END


BEGIN(Client)

class CBall final : public CGameObject
{
public:
	typedef struct tagWeaponDesc
	{
		CHierarchyNode*		pSocket = nullptr;
		_float4x4			SocketPivotMatrix;
		const _float4x4*	pParentWorldMatrix;

	}WEAPONDESC;
private:
	CBall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBall(const CBall& rhs);
	virtual ~CBall() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom[4] = { nullptr };


private:
	WEAPONDESC				m_WeaponDesc;
	_float4x4				m_CombinedWorldMatrix;


	_bool					m_bRender = false;
	_int					m_iBallIndex = 0;

	_bool					m_bTakeBall = false;
	_float					m_fTakeBall = 0.f;
	_float4					m_vTargetPos;


public:
	void Set_Render(_bool _bRender, _int _iBallIndex) { m_bRender = _bRender; m_iBallIndex = _iBallIndex; }
	void Set_Render2(_bool _bRender, _int _iBallIndex,_bool _bTakeBall , _float4 _vTargetPos) {
		m_bRender = _bRender; 
		m_iBallIndex = _iBallIndex; 
		m_bTakeBall = _bTakeBall;
		m_vTargetPos = _vTargetPos;
	}
private:
	HRESULT Ready_Components();
	HRESULT SetUp_ShaderResources(); /* 셰이더 전역변수에 값을 전달한다. */


public:
	static CBall* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END