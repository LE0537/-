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

class CTextBox : public CGameObj
{
private:
	CTextBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTextBox(const CTextBox& rhs);
	virtual ~CTextBox() = default;

public:
	typedef struct tagTextInfo
	{
		wstring*		pScript;
		_int			iScriptSize;
		CGameObject*	pTarget;
		_int			iType;
	}TINFO;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	_float					m_fX, m_fX2, m_fY, m_fSizeX, m_fSizeY;
	_float4x4				m_ViewMatrix, m_ProjMatrix;


	wstring					m_wstr;
	_int					m_iIndex = 0;
	_int					m_iScriptIndex = 0;
	_float					m_fTimeDelta = 0.f;
	_bool					m_bFontRender = false;
	wstring*				m_pScript = nullptr;


	_float					m_fDeadTime = 0.f;

	TINFO					m_tTInfo;
private:
	HRESULT Ready_Components();
	HRESULT SetUp_ShaderResources(); /* 셰이더 전역변수에 값을 전달한다. */
	void	Print_Text();
	void	Running_TextBox();
	void	RenderFonts();
public:
	static CTextBox* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END