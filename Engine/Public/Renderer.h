#pragma once

/* 화면에 그려져야할 객체들을 모아서 그리는 순서대로 보관한다. 순서대로 렌더함수를 호출해준다. */
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CRenderer final : public CComponent
{
public:
	enum RENDERGROUP {RENDER_PRIORITY,RENDER_SHADOWDEPTH, RENDER_NONALPHABLEND, RENDER_NONLIGHT, RENDER_ALPHABLEND, RENDER_UI,RENDER_UIPOKE ,RENDER_END };

private:
	CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);	
	virtual ~CRenderer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg)override;

public:
	HRESULT Add_RenderGroup(RENDERGROUP eRenderGroup, class CGameObject* pGameObject);
	HRESULT Add_RenderGroup_Front(RENDERGROUP eRenderGroup, class CGameObject* pGameObject);
	HRESULT Render_GameObjects(_bool _bDebug);

public:
	HRESULT Add_Debug(class CComponent* pDebugCom);

private:
	list<class CGameObject*>				m_GameObjects[RENDER_END];
	typedef list<class CGameObject*>		GAMEOBJECTS;

private:
	list<class CComponent*>					m_DebugComponents;

private:
	class CTarget_Manager*					m_pTarget_Manager = nullptr;
	class CLight_Manager*					m_pLight_Manager = nullptr;

	class CVIBuffer_Rect*					m_pVIBuffer = nullptr;
	class CShader*							m_pShader = nullptr;
	_float4x4								m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;

private:
	HRESULT Render_Priority();
	HRESULT Render_ShadowDepth();
	HRESULT Render_NonAlphaBlend();
	HRESULT Render_Lights();
	HRESULT Render_Blend();
	HRESULT Render_NonLight();
	HRESULT Render_AlphaBlend();
	HRESULT Render_UI();
	HRESULT Render_UIPOKE();

	HRESULT Render_Debug();
public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg = nullptr)override;
	virtual void Free();
};

END