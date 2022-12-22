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

class CBattleUI : public CGameObj
{
private:
	CBattleUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBattleUI(const CBattleUI& rhs);
	virtual ~CBattleUI() = default;
public:
	enum ButtonDir { DIR_UP, DIR_DOWN, DIR_LEFT, DIR_END };
	enum CHECKPOS { CHECKPOS_INFO,CHECKPOS_SKILL, CHECKPOS_END };	

	typedef struct tagBattleInfo
	{
		CGameObject* pPlayer_Orgin;
		CGameObject* pBattleTarget;
		class CBag* pPlayer;
		BATTLETYPE	eBattleType;
		vector<CGameObject*>* pvecTargetPoke;
	}BATTLEINFO;
public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT Render_ShadowDepth();
private:
	CTexture*				m_pTextureCom2 = nullptr; //타입스몰 텍스쳐
	CTexture*				m_pTextureCom3 = nullptr; // 백 텍스쳐
	CTexture*				m_pTextureCom4 = nullptr; // 상태이상
	CVIBuffer_Rect*			m_pVIBufferPlayer[6] = { nullptr };
	CTransform*				m_pTransformPlayer[6] = { nullptr };
	CShader*				m_pShaderPlayer[6] = { nullptr };
	CVIBuffer_Rect*			m_pVIBufferTarget[5] = { nullptr };
	CTransform*				m_pTransformTarget[5] = { nullptr };
	CShader*				m_pShaderTarget[5] = { nullptr };
	CVIBuffer_Rect*			m_pVIBufferInfo[4] = { nullptr };
	CTransform*				m_pTransformInfo[4] = { nullptr };
	CShader*				m_pShaderInfo[4] = { nullptr };
	CVIBuffer_Rect*			m_pVIBufferSkill[8] = { nullptr };
	CTransform*				m_pTransformSkill[8] = { nullptr };
	CShader*				m_pShaderSkill[8] = { nullptr };
	CVIBuffer_Rect*			m_pVIBufferBall[12] = { nullptr };
	CTransform*				m_pTransformBall[12] = { nullptr };
	CShader*				m_pShaderBall[12] = { nullptr };
private:
	_float					m_fX, m_fY, m_fSizeX, m_fSizeY;
	_float					m_PokefX, m_PokefY, m_PokefSizeX, m_PokefSizeY;
	_float4x4				m_ViewMatrix, m_ProjMatrix;

	wstring					m_szPlayerName;
	_bool					m_bSkill = false;
	_int					m_iSelect = 0;

	BATTLEINFO				m_tInfo;
	_int					m_iPlayerIndex = 0;
	_int					m_iPlayerHPIndex = 0;
	_int					m_iTargetIndex = 0;
	
	//Battle
	_int					m_iPlayerFinalDmg = 0;
	_bool					m_bPlayerHit = false;
	_bool					m_bPlayerCri = false;
	_int					m_bPlayerSkillIndex = 0;
	_float					m_fPlayerType = 0.f;

	_int					m_iTargetFinalDmg = 0;
	_bool					m_bTargetHit = false;
	_bool					m_bTargetCri = false;
	_int					m_bTargetSkillIndex = 0;
	_float					m_fTargetType = 0.f;

	_bool					m_bPlayerAttack = false;
	_bool					m_bCheckAttack = false;
	_bool					m_bCheckAttack2 = false;
	_bool					m_bCheckAttack3 = false;

	_bool					m_bCreateTextBox = false;
	_bool					m_bCreateTextBox2 = false;

	_int					m_fDotDeal = 0;
	_float					m_fHPTime = 0.f;
	_float					m_fEXPTime = 0.f;
	_bool					m_bGet_EXP = false;
	_int					m_iDotEXP = 0;
	_int					m_iTotalEXP = 0;
	_float					m_fDelayTime = 0.f;
	_bool					m_bPokeDead = false;

	_bool					m_bChangePokeStart = false;
	_bool					m_bChangeAttack = false;


	_bool					m_bBattleBagPoke = false;
	_bool					m_bBattleChangePoke = false;
	_bool					m_bBattleUseItem = false;

	_float					m_fTextSizeTime = 0.f;
	vector<wstring>		    m_vBattleScript;

	_bool					m_bWildWin = false;
	_float					m_WinTime = 0.f;

	_float4					m_MySKillPos;
	_float4					m_TargetSkillPos;
private:	
	HRESULT Ready_Components();
	HRESULT SetUp_ShaderResources();
	HRESULT SetUp_ShaderSkill();
	HRESULT SetUp_ShaderPlayer();
	HRESULT SetUp_ShaderTarget();
	HRESULT SetUp_ShaderBall();
	HRESULT SetSelectButton(ButtonDir _eDir);
	void	Wild_Win(_float fTimeDelta);
	void	Ready_Run();
	void	Ready_RunFAILScript();
	void	Ready_ChangePokeScript();
	void	Ready_PlayerScript();
	void	Ready_TargetScript();
	void	Ready_PlayerChange_Poke();
	void	Ready_TargetChange_Poke();
	void	Render_Fonts();
	void	Change_Poke(_float fTimeDelta);
	void	Use_Item(_float fTimeDelta);
	void	Set_Pos();
	void	Set_PlayerPos();
	void	Set_TargetPos();
	void	Set_BallPos();
	void	Key_Input();
	void	BattleFrame();
	void	BattleDelay(_float fTimeDelta);
	void	Check_Selected();
	_int	Check_TargetSkill();
	void	Set_CheckPos(CHECKPOS eType);
	void	Use_PlayerSkill(_int _iSkillIndex);
	void	Use_TargetSkill(_int _iSkillIndex);
	_float	Check_Type(POKETYPE _eType, POKETYPE _eTargetType);
	void	Change_PlayerPoke(_float fTimeDelta);
	void	Change_TargetPoke(_float fTimeDelta);
	void	Use_SkillEffect();
	void	Use_TargetSkillEffect();
public:
	static CBattleUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END