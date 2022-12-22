#pragma once
#include "Client_Defines.h"
#include "GameObj.h"
#include "Camera_Dynamic.h"

BEGIN(Engine)

class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
class CModel;
END


BEGIN(Client)

class CRace : public CGameObj
{
private:
	CRace(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRace(const CRace& rhs);
	virtual ~CRace() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT Render_ShadowDepth();
public:
	void	Set_RaceInfo(_float4  vPos, _int iType);
private:
	HRESULT SetUp_ShaderResources();
	HRESULT Ready_Components();
	HRESULT Create_RacePoke();
	void	Set_RacePokePos();
	void	Ready_Script();
	void	Ready_CheckRacePoke();
	void	Ready_ChoiceRacePoke();
	void	Ready_StartRace();
	void	Ready_EndRace();
	void	Check_RacePoke(_float fTimeDelta);
	void	Run_RacePoke(_float fTimeDelta);
	void	RaceEnd();
	HRESULT	RandPoke();
private:
	CModel*					m_pModelCom = nullptr;

	vector<wstring>		    m_vBattleScript;

	vector<CCamera_Dynamic::RaceInfo>	m_vecRace;
	list<CGameObject*>					m_listRacePoke;

	_bool								m_bRaceStart = false;
	_float4				m_vPos1, m_vPos2, m_vPos3, m_vPos4;
	_float4				m_vLook1, m_vLook2, m_vLook3, m_vLook4;
	_float4				m_vGoal1, m_vGoal2, m_vGoal3, m_vGoal4;
	_float4				m_vRaceEndPos, m_vRaceEndLook;

	_float				m_fTextTime = 0.f;
	_bool				m_bCheckRaceText = false;
	_bool				m_bChoiceRacePoke = false;
	_bool				m_bStartText = false;
	_bool				m_bCount = false;
	_bool				m_bSlowMotion = false;
	_bool				m_bGoal = false;
	_bool				m_bRaceEnd = false;
	_bool				m_bCheckWin = false;

	_float				m_fRand1 = 0.f;
	_float				m_fRand2 = 0.f;
	_float				m_fRand3 = 0.f;
	_float				m_fRand4 = 0.f;
	_float				m_fRandTime = 0.f;

	_float				fDist1 = 9999.f;
	_float				fDist2 = 9999.f;;
	_float				fDist3 = 9999.f;;
	_float				fDist4 = 9999.f;;

	_int				m_iWinNum = 0;

	_bool				m_bReset = false;

	_float				m_fLeafTime = 0.f;
public:
	static CRace* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END