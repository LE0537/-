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

class CScreen : public CGameObj
{
private:
	CScreen(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CScreen(const CScreen& rhs);
	virtual ~CScreen() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	CTransform*				m_pTransformCom2 = nullptr;
	CShader*				m_pShaderCom2 = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom2 = nullptr;
private:
	_float					m_fX, m_fX2, m_fY, m_fSizeX, m_fSizeY;
	_float4x4				m_ViewMatrix, m_ProjMatrix;

private:
	HRESULT Ready_Components();
	HRESULT SetUp_ShaderResources(); /* 셰이더 전역변수에 값을 전달한다. */
	void	Set_Pos();
	void	Set_Pos2();
public:
	static CScreen* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END