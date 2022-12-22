#pragma once
#include "Base.h"

BEGIN(Engine)

class CLight_Manager final : public CBase
{
	DECLARE_SINGLETON(CLight_Manager)
private:
	CLight_Manager();
	virtual ~CLight_Manager() = default;

public:
	const LIGHTDESC* Get_LightDesc(_uint iIndex);
	const LIGHTDESC* Get_ShadowLightDesc(_uint iIndex);
	void Set_ShadowLightDesc(_uint iIndex, _float4 vPos, _float4 vAt);
public:
	HRESULT Add_Light(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const LIGHTDESC& LightDesc);
	HRESULT Add_ShadowLight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const LIGHTDESC& LightDesc);
	HRESULT Render_Lights(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
private:
	list<class CLight*>				m_Lights;
	list<class CLight*>				m_ShadowLights;
public:
	virtual void Free() override;
};

END