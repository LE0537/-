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

class CBag : public CGameObj
{
private:
	CBag(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBag(const CBag& rhs);
	virtual ~CBag() = default;
public:
	enum ButtonDir {DIR_UP,DIR_DOWN,DIR_END};

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	CTransform*				m_pTransformCom2 = nullptr;
	CTransform*				m_pTransformCom3 = nullptr;
	CShader*				m_pShaderCom2 = nullptr;
	CShader*				m_pShaderCom3 = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom2 = nullptr; // 아이템 셀렉
	CVIBuffer_Rect*			m_pVIBufferCom3 = nullptr; // 포켓몬 셀렉
	CTexture*				m_pTextureCom2 = nullptr; //아이템 텍스쳐
	CTexture*				m_pTextureCom3 = nullptr; // 포켓 텍스쳐
	CVIBuffer_Rect*			m_pVIBufferPoke[18];
	CVIBuffer_Rect*			m_pVIBufferItem[8];
	CTransform*				m_pTransformPoke[18];
	CTransform*				m_pTransformItem[8];
	CShader*				m_pShaderPoke[18];
	CShader*				m_pShaderItem[8];
private:
	_float					m_fX, m_fY, m_fSizeX, m_fSizeY;
	_float4x4				m_ViewMatrix, m_ProjMatrix;

	_bool					m_bItemSelect = true;
	_bool					m_bPokeSelect = false;
	_int					m_iSelect = 0;
	_int					m_iItemScoll = 0;
	_int					m_iItemPos = 0;
	vector<CGameObject*>    m_vecItem;
	vector<CGameObject*>    m_vecPoke;
private:
	HRESULT Ready_Components();
	HRESULT SetUp_ShaderResources(); /* 셰이더 전역변수에 값을 전달한다. */
	HRESULT SetUp_ItemResources();
	HRESULT SetSelectButton(_int iIndex, ButtonDir _eDir);
	void	Key_Input();
	void	Set_ItemPos();
public:
	static CBag* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END