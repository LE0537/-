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
public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

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

	_bool					m_bSkill = false;
	_int					m_iSelect = 0;

	CGameObject*			m_pPlayer;

	vector<CGameObject*>    m_vecPoke;

private:
	HRESULT Ready_Components();
	HRESULT SetUp_ShaderResources();
	HRESULT SetUp_ShaderSkill();
	HRESULT SetUp_ShaderPlayer();
	HRESULT SetUp_ShaderTarget();
	HRESULT SetUp_ShaderBall();
	HRESULT SetSelectButton(ButtonDir _eDir);
	void	Set_Pos();
	void	Set_PlayerPos();
	void	Set_TargetPos();
	void	Set_BallPos();
	void	Key_Input();
	void	Check_Selected();
	void	Set_CheckPos(CHECKPOS eType);
public:
	static CBattleUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END