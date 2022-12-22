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

class CBattleIntro : public CGameObj
{
private:
	CBattleIntro(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBattleIntro(const CBattleIntro& rhs);
	virtual ~CBattleIntro() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT Render_ShadowDepth();
private:
	CTransform*				m_pTransform[5] = { nullptr };
	CShader*				m_pShader[5] = {nullptr};
	CVIBuffer_Rect*			m_pVIBuffer[5] = { nullptr };
	CTransform*				m_pTransformCom2 = nullptr;
	CTransform*				m_pTransformCom3 = nullptr;
	CShader*				m_pShaderCom2 = nullptr;
	CShader*				m_pShaderCom3 = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom2 = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom3 = nullptr;
private:
	_float					m_fY, m_fY2,m_fSizeX, m_fSizeY;
	_float					m_fBallX,m_fBallY, m_fBallSizeX, m_fBallSizeY;
	_float4x4				m_ViewMatrix, m_ProjMatrix;
	_float					m_fX[3];

	_float					m_fDist = 0.f;
	_float					m_fDeadtime = 0.f;
	_bool					m_bEnd = false;
private:
	HRESULT Ready_Components();
	HRESULT SetUp_ShaderResources(); /* 셰이더 전역변수에 값을 전달한다. */
	void	Set_Pos();
	void	Set_Pos2();
	void	RatBall(_float fTimeDelta);
public:
	static CBattleIntro* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END