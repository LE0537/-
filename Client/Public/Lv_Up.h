#pragma once
#include "Client_Defines.h"
#include "GameObj.h"


BEGIN(Engine)

class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
END


BEGIN(Client)

class CLv_Up : public CGameObj
{
private:
	CLv_Up(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLv_Up(const CLv_Up& rhs);
	virtual ~CLv_Up() = default;
public:
	typedef struct tagLvUpInfo {
		_int iPrevMaxHp;
		_int iPrevDmg;
		_int iPrevDef;
		_int iPrevSDmg;
		_int iPrevSDef;
		_int iPrevSpeed;
		_int iMaxHp;
		_int iDmg;
		_int iDef;
		_int iSDmg;
		_int iSDef;
		_int iSpeed;
		_int iPokeNum;

	}LVUPINFO;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT Render_ShadowDepth();
private:
	HRESULT SetUp_ShaderResources();
	HRESULT Ready_Components();
	void	Render_Fonts();
private:
	CVIBuffer_Rect*			m_pVIBufferCom1 = nullptr;
	CTexture*				m_pTextureCom1 = nullptr;
	CTransform*				m_pTransformCom1 = nullptr;
	CShader*				m_pShaderCom1 = nullptr;

	_float					m_fX, m_fY, m_fSizeX, m_fSizeY;
	_float4x4				m_ViewMatrix, m_ProjMatrix;

	_float		m_fDeadTime = 0.f;
	LVUPINFO    m_LvInfo;

public:
	static CLv_Up* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END