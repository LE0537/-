#pragma once
#include "Client_Defines.h"
#include "GameObj.h"
#include "Bag.h"

BEGIN(Engine)

class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;

END


BEGIN(Client)

class CPokeDeck : public CGameObj
{
private:
	CPokeDeck(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPokeDeck(const CPokeDeck& rhs);
	virtual ~CPokeDeck() = default;
public:


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	CTexture*				m_pTextureCom2 = nullptr;
	CTexture*				m_pTextureCom3 = nullptr;
	CTexture*				m_pTextureCom4 = nullptr;
	CTransform*				m_pTransformCom2 = nullptr;
	CShader*				m_pShaderCom2 = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom2 = nullptr;
	CTransform*				m_pTransformCom3 = nullptr;
	CShader*				m_pShaderCom3 = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom3 = nullptr;
	CTransform*				m_pTransformPoke[6] = { nullptr };
	CShader*				m_pShaderPoke[6] = { nullptr };
	CVIBuffer_Rect*			m_pVIBufferPoke[6] = { nullptr };
	CTransform*				m_pTransformType[3] = { nullptr };
	CShader*				m_pShaderType[3] = { nullptr };
	CVIBuffer_Rect*			m_pVIBufferType[3] = { nullptr };
private:
	_float					m_fX, m_fY, m_fSizeX, m_fSizeY;
	_float4x4				m_ViewMatrix, m_ProjMatrix;

	vector<CGameObject*>	m_vecPoke;

	_bool					m_bSelect = false;
	_bool					m_bUseKey = false;

	_int					m_iPokeScroll = 0;
	_int					m_iSelect = 0;
	_int					m_iPokePos = 0;
private:
	HRESULT Ready_Components();
	HRESULT SetUp_ShaderResources(); /* 셰이더 전역변수에 값을 전달한다. */
	HRESULT SetSelectButton(CBag::ButtonDir _eDir);
	void	Key_Input();
	void	Set_PokePos();
	void	Set_TypePos();
	void	RenderFonts();
	void	RenderFonts2();
public:
	static CPokeDeck* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END