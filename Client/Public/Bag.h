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
	enum ButtonDir {DIR_UP,DIR_DOWN,DIR_LEFT,DIR_END};
	enum UseButton { ITEM_BUTTON, POKE_BUTTON, BUTTON_END };
public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	CTransform*				m_pTransformCom2 = nullptr;
	CTransform*				m_pTransformCom3 = nullptr;
	CTransform*				m_pTransformCom4 = nullptr;
	CTransform*				m_pTransformCom5 = nullptr;
	CShader*				m_pShaderCom2 = nullptr;
	CShader*				m_pShaderCom3 = nullptr;
	CShader*				m_pShaderCom4 = nullptr;
	CShader*				m_pShaderCom5 = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom2 = nullptr; // 아이템 셀렉
	CVIBuffer_Rect*			m_pVIBufferCom3 = nullptr; // 포켓몬 셀렉
	CVIBuffer_Rect*			m_pVIBufferCom4 = nullptr; // 사용UI
	CVIBuffer_Rect*			m_pVIBufferCom5 = nullptr; // 사용UI
	CTexture*				m_pTextureCom2 = nullptr; //아이템 텍스쳐
	CTexture*				m_pTextureCom3 = nullptr; // 포켓 텍스쳐
	CTexture*				m_pTextureCom4 = nullptr; // 상태이상 텍스쳐
	CVIBuffer_Rect*			m_pVIBufferPoke[24] = { nullptr };
	CVIBuffer_Rect*			m_pVIBufferItem[8] = { nullptr };
	CVIBuffer_Rect*			m_pVIBufferPokeItem[6] = { nullptr };
	CTransform*				m_pTransformPoke[24] = { nullptr };
	CTransform*				m_pTransformItem[8] = { nullptr };
	CTransform*				m_pTransformPokeItem[6] = { nullptr };
	CShader*				m_pShaderPoke[24] = { nullptr };
	CShader*				m_pShaderItem[8] = { nullptr };
	CShader*				m_pShaderPokeItem[6] = { nullptr };
private:
	_float					m_fX, m_fY, m_fSizeX, m_fSizeY;
	_float					m_PokefX, m_PokefY, m_PokefSizeX, m_PokefSizeY;
	_float4x4				m_ViewMatrix, m_ProjMatrix;

	_bool					m_bItemSelect = true;
	_int					m_iPokeSelect = 0;
	_int					m_iSelect = 0;
	_int					m_iItemScoll = 0;
	_int					m_iItemPos = 0;

	_bool					m_bUseKey = false;
	_bool					m_bUseItem = false;
	_bool					m_bUsePoke = false;
	_int					m_UsePos = 0;

	_bool					m_bUse = false;

	_bool					m_bItem = false;
	_bool					m_bGiveItem = false;
	_bool					m_bSwap = false;
	_int					m_iSwapPoke = 99;


	vector<ITEMINFO*>    m_vecItem;
	vector<CGameObject*>    m_vecPoke;
public:
	CGameObject* Get_SelectPoke() { return m_vecPoke[m_iPokeSelect]; }
private:
	HRESULT Ready_Components();
	HRESULT SetUp_ShaderResources(); /* 셰이더 전역변수에 값을 전달한다. */
	HRESULT SetUp_ItemResources();
	HRESULT SetUp_PokeResources();
	HRESULT SetUp_PokeItemResources();
	HRESULT SetUp_UseResources();
	HRESULT SetSelectButton(_int iIndex, ButtonDir _eDir);
	HRESULT SetSelectButtonPoke(_int iIndex, ButtonDir _eDir);
	HRESULT SetSelectButtonUse(_int iIndex, ButtonDir _eDir , UseButton _eButton);
	void	Key_Input();
	void	Key_UseInput();
	void	Key_PokeInput();
	void	Set_ItemPos();
	void	Set_PokePos();
	void	Set_PokeItemPos();
	void	Set_UseItemPos(_int _iIndex);
	void	Set_UsePokePos(_int _iIndex);
	void	UseItem();
	void	GiveItem();
	void	ClearItem();
	void	SwapPoke(_int _iSwapPoke);
	void	RenderFonts();
	_bool	CheckUseItem();
	_bool	CheckGiveItem();
public:
	static CBag* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END