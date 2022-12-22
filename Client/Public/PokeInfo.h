#pragma once
#include "Client_Defines.h"
#include "GameObj.h"


BEGIN(Engine)

class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
class CVIBuffer_Hexagon;

END


BEGIN(Client)

class CPokeInfo : public CGameObj
{
private:
	CPokeInfo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPokeInfo(const CPokeInfo& rhs);
	virtual ~CPokeInfo() = default;
public:
	enum ButtonDir { DIR_UP, DIR_DOWN, DIR_END };

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT Render_ShadowDepth();
private:
	CVIBuffer_Hexagon*		m_pVIBufferHexagonCom = nullptr;
	CTransform*				m_pTransformCom2 = nullptr;
	CTransform*				m_pTransformCom3 = nullptr;
	CTransform*				m_pTransformCom4 = nullptr;
	CShader*				m_pShaderCom2 = nullptr;
	CShader*				m_pShaderCom3 = nullptr;
	CShader*				m_pShaderCom4 = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom2 = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom3 = nullptr;
	CTexture*				m_pTextureCom2 = nullptr; 
	CTexture*				m_pTextureCom3 = nullptr;
	CTexture*				m_pTextureCom4 = nullptr;
	CVIBuffer_Rect*			m_pVIBufferSkill[4] = { nullptr };
	CTransform*				m_pTransformSkill[4] = { nullptr };
	CShader*				m_pShaderSkill[4] = { nullptr };
	CVIBuffer_Rect*			m_pVIBufferInfo[3] = { nullptr };
	CTransform*				m_pTransformInfo[3] = { nullptr };
	CShader*				m_pShaderInfo[3] = { nullptr };
private:
	class CBag*				m_pBag = nullptr;
	_float					m_fX, m_fY, m_fSizeX, m_fSizeY;
	_float4x4				m_ViewMatrix, m_ProjMatrix;
	_float					m_fButtonX, m_fButtonY;

	_bool					m_bInven = false;
	_bool					m_bSelect = false;
	_int					m_iSelect = 0;
private:
	HRESULT Ready_Components();
	HRESULT SetUp_ShaderResources();
	HRESULT SetUp_SkillResources();
	HRESULT SetUp_InfoResources();
	HRESULT SetSelectButton(ButtonDir _eDir);
	void	Key_Input();
	void	Set_SkillPos();
	void	Set_InfoPos();
	void	RenderFonts();
	void	RenderFonts2();
	HRESULT	CheckSkillType(_int _iIndex);
public:
	static CPokeInfo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END