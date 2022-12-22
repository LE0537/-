#pragma once

#include "Client_Defines.h"
#include "Level.h"
#include "GameObject.h"
#include "Camera_Dynamic.h"
#include "Weed.h"

BEGIN(Client)

class CLevel_GamePlay final : public CLevel
{
private:
	CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_GamePlay() = default;
public:
	struct SaveInfo {
		_float4  vPos;
		_float3	 vScale;
		_int	 iType;
	};
	typedef struct tagLoad
	{
		_float4  vPos;
		_float4  vMyPos;
		_float4  vTargetPos;
		_float3	 vScale;
		CGameObject* pTarget;
		CGameObject* pCamera;
		CGameObject* pRaceMap;
		_bool	 bBattleMap;
		_bool	 bRaceMap;
	}LOADFILE;

public:
	virtual HRESULT Initialize();
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);

public:
	HRESULT Ready_Lights();
	HRESULT Ready_Layer_Player(const _tchar* pLayerTag);
	HRESULT Ready_Layer_BackGround(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Effect(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Camera(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Monster(const _tchar* pLayerTag);
	HRESULT Ready_Layer_UI(const _tchar* pLayerTag);
private:
	void Load();
	void LoadCamPos();
	void LoadBattle();
	void LoadRace();
	void Create_Leaf(_float fTimeDelta);
private:
	LOADFILE			m_LoadFile;
	vector<SaveInfo>	m_vecSave;
	vector<SaveInfo>	m_vecCamPos;
	vector<SaveInfo>	m_vecSaveLookAt;
	vector<SaveInfo>	m_vecSaveBattle;
	vector<SaveInfo>	m_vecSaveRace;
	CCamera_Dynamic::CAMERADESC_DERIVED			CameraDesc;

	vector<_float4>					m_listPos1;
	vector<_float3>					m_listScale1;
	vector<_float4>					m_listPos2;
	vector<_float3>					m_listScale2;
	vector<_float4>					m_listPos3;
	vector<_float3>					m_listScale3;
	_int					m_iWeed[3] = { 0,0,0 };

	_float					m_fLeafTime = 0.f;

	CGameObject* tInsInfo = nullptr;

public:
	static CLevel_GamePlay* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END

