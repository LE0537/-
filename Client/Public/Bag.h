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
	virtual HRESULT Render_ShadowDepth();
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

	_int					m_iHealHP = 0;
	_int					m_iHealDot = 0;
	_int					m_iHealPokeIndex = 0;
	_bool					m_bHeal = false;
	_float					m_fHealTime = 0.f;

	_int					m_iLvPokeIndex = 0;
	_bool					m_iLvUp = false;

	CGameObject*			m_pPlayer;
	vector<ITEMINFO*>    m_vecItem;
	vector<CGameObject*>    m_vecPoke;

	//BattlePoke
	_bool					m_bStart = false;
	_int					m_iChangePoke = 0;
	_bool					m_bBattlePokeDead = false;
	_bool					m_bBattleChangePoke = false;
	_bool					m_bBattleUseItem = false;
	_bool					m_bClose = false;
public:
	void		 Set_vecPokeExpShare(_int _iExp);
	_bool		 Get_Close() { return m_bClose; }
	void		 Set_Close() { m_bClose = false; }
	_int		 Get_iChangePoke() { return m_iChangePoke; }
	CGameObject* Get_SelectPoke() { return m_vecPoke[m_iPokeSelect]; }
	CGameObject* Get_vecPoke(_int _iIndex) { return m_vecPoke[_iIndex]; }
	void		 Set_vecPoke(_int _iIndex, CGameObject* pPoke) { m_vecPoke[_iIndex] = pPoke; }
	_int		 Get_vecItemNum(_int _iIndex) { return m_vecItem[_iIndex]->iItemNum; }
	void		 Set_vecItem(_int _iIndex, ITEMINFO* pItem) { m_vecItem[_iIndex] = pItem; }
	_bool		 Get_BattlePokeDead() { return m_bBattlePokeDead; }
	void		 Set_BattlePokeDead() { m_bBattlePokeDead = true; }
	_bool		 Get_BattleChangePoke() { return m_bBattleChangePoke; }
	void		 Set_BattleChangePoke() { m_bBattleChangePoke = true; }
	_bool		 Get_BattleUseItem() { return m_bBattleUseItem; }
	void		 Set_BattleUseItem() { m_bBattleUseItem = true; }
	_int		 Get_EvolIndex();
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
	void	HealPoke(_float fTimeDelta);
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
	_bool	CheckGiveItem();
	void	ClearBag();
	void	RidePoke();
	void	CheckLv();
	void	BattlePokeKey();
	void	BattleChangePokeKey();
	void	BattleUseItemKey();
public:
	static CBag* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END