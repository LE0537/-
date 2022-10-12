#include "stdafx.h"
#include "..\Public\GameObj.h"

#include "GameInstance.h"

CGameObj::CGameObj(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CGameObj::CGameObj(const CGameObj & rhs)
	: CGameObject(rhs)
{
}

HRESULT CGameObj::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGameObj::Initialize(void * pArg)
{
	return S_OK;
}

void CGameObj::Tick(_float fTimeDelta)
{
}

void CGameObj::Late_Tick(_float fTimeDelta)
{
}

HRESULT CGameObj::Render()
{
	return S_OK;
}



CGameObj * CGameObj::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CGameObj*	pInstance = new CGameObj(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CGameObj"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CGameObj::Clone(void * pArg)
{
	CGameObject*	pInstance = new CGameObj(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CGameObj"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGameObj::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
}
