#include "..\Public\Light_Manager.h"
#include "Light.h"

IMPLEMENT_SINGLETON(CLight_Manager)

CLight_Manager::CLight_Manager()
{
}

const LIGHTDESC * CLight_Manager::Get_LightDesc(_uint iIndex)
{
	if (iIndex >= m_Lights.size())
		return nullptr;

	auto	iter = m_Lights.begin();

	for (_uint i = 0; i < iIndex; ++i)
		++iter;

	return (*iter)->Get_LightDesc();
}
const LIGHTDESC * CLight_Manager::Get_ShadowLightDesc(_uint iIndex)
{

	for (auto& iter : m_ShadowLights)
	{
		if(iter->Get_LightDesc()->eType == iIndex)
			return iter->Get_LightDesc();

	}
	return nullptr;
}
void CLight_Manager::Set_ShadowLightDesc(_uint iIndex ,_float4 vPos,_float4 vAt)
{

	for (auto& iter : m_ShadowLights)
	{
		if (iter->Get_LightDesc()->eType == iIndex)
		{
			iter->Set_ShadowLightDesc(vPos, vAt);
			return;
		}
			

	}
}
HRESULT CLight_Manager::Add_Light(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const LIGHTDESC & LightDesc)
{
	CLight*		pLight = CLight::Create(pDevice, pContext, LightDesc);
	if (nullptr == pLight)
		return E_FAIL;

	m_Lights.push_back(pLight);

	return S_OK;
}

HRESULT CLight_Manager::Add_ShadowLight(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const LIGHTDESC & LightDesc)
{
	CLight*		pLight = CLight::Create(pDevice, pContext, LightDesc);
	if (nullptr == pLight)
		return E_FAIL;

	m_ShadowLights.push_back(pLight);

	return S_OK;
}

HRESULT CLight_Manager::Render_Lights(CShader * pShader, CVIBuffer_Rect * pVIBuffer)
{
	for (auto& pLight : m_Lights)
	{
		if (nullptr != pLight)
			pLight->Render(pShader, pVIBuffer);
	}

	return S_OK;
}

void CLight_Manager::Free()
{
	for (auto& pLight : m_Lights)
		Safe_Release(pLight);

	for (auto& pLight : m_ShadowLights)
		Safe_Release(pLight);

	m_Lights.clear();
}
