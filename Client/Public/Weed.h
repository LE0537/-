#pragma once
#include "Client_Defines.h"
#include "GameObj.h"


BEGIN(Engine)

class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
class CVIBuffer_Point_Instance;

END


BEGIN(Client)

class CWeed : public CGameObj
{
private:
	CWeed(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWeed(const CWeed& rhs);
	virtual ~CWeed() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT Render_ShadowDepth();
public:
	void	Set_Vector(vector<_float4> _listPos, vector<_float3> _listScale, _int _iWeed) {
		m_listPos = _listPos;
		m_listScale = _listScale;
		m_iWeed = _iWeed;
	}
	vector<_float4> Get_PosVector() { return m_listPos; }
	vector<_float3> Get_ScaleVector() { return m_listScale; }
	_int			Get_iWeed() { return m_iWeed; }
private:
	HRESULT SetUp_ShaderResources();
	HRESULT Ready_Components();
	
private:
	CVIBuffer_Point_Instance*	m_pVIBufferInstanceCom = nullptr;


	vector<_float4>					m_listPos;
	vector<_float3>					m_listScale;
	_int							m_iWeed;

	_bool							m_bTrue = false;
public:
	static CWeed* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END
