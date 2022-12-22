#include "stdafx.h"
#include "..\Public\AnimMasterBall.h"

#include "GameInstance.h"
#include "SoundMgr.h"
#include "Level_GamePlay.h"
#include "Player.h"
#include "Bag.h"
#include "TextBox.h"
#include "Data_Manager.h"	// 추가

CAnimMasterBall::CAnimMasterBall(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObj(pDevice, pContext)
{
}

CAnimMasterBall::CAnimMasterBall(const CAnimMasterBall & rhs)
	: CGameObj(rhs)
{
}

HRESULT CAnimMasterBall::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAnimMasterBall::Initialize(void * pArg)
{

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTarget = *(&((CLevel_GamePlay::LOADFILE*)pArg)->pTarget);
	m_pTargetPoke = *(&((CLevel_GamePlay::LOADFILE*)pArg)->pCamera);

	_vector vLookAt = dynamic_cast<CGameObj*>(m_pTarget)->Get_Transfrom()->Get_State(CTransform::STATE_TRANSLATION);

	_vector MyPos = XMLoadFloat4((&((CLevel_GamePlay::LOADFILE*)pArg)->vPos));

	MyPos.m128_f32[1] += 0.2f;
	vLookAt.m128_f32[1] += 0.2f;

	m_pModelCom->Set_CurrentAnimIndex(3);
	//	m_pTransformCom->Turn2(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f));
	//m_pTransformCom->Set_Scale(XMLoadFloat3((&((CLevel_GamePlay::LOADFILE*)pArg)->vScale)));
	m_pTransformCom->Set_Scale(XMVectorSet(0.15f, 0.15f, 0.15f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, MyPos);
	m_pTransformCom->LookAt(vLookAt);


	return S_OK;
}

void CAnimMasterBall::Tick(_float fTimeDelta)
{
	Move(fTimeDelta);

	if (m_bFail)
	{
		m_fFailTime += fTimeDelta;
		if (m_fFailTime > 0.7f)
			g_bCaptureRender = false;
		if (m_fFailTime > 1.5f)
		{
			g_bCapture = false;
			Set_Dead();
		}
	}
	if (m_bCaptureSuccesss)
	{
		m_fSuccessTime += fTimeDelta;
		if (m_fSuccessTime > 1.5f)
		{
			dynamic_cast<CPlayer*>(m_pTarget)->Set_Captrue(true);
			Set_Dead();
		}
	}


}

void CAnimMasterBall::Late_Tick(_float fTimeDelta)
{

	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	//	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOWDEPTH, this);
	}

}

HRESULT CAnimMasterBall::Render()
{

	if (nullptr == m_pShaderCom ||
		nullptr == m_pModelCom)
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshContainers();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 2)))
			return E_FAIL;

	}

	RELEASE_INSTANCE(CGameInstance);


	return S_OK;
}
HRESULT CAnimMasterBall::Render_ShadowDepth()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pModelCom)
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_World4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;


	_vector			vLightEye = XMLoadFloat4(&pGameInstance->Get_ShadowLightDesc(LIGHTDESC::TYPE_FIELDSHADOW)->vDirection);
	_vector			vLightAt = XMLoadFloat4(&pGameInstance->Get_ShadowLightDesc(LIGHTDESC::TYPE_FIELDSHADOW)->vDiffuse);
	_vector			vLightUp = { 0.f, 1.f, 0.f ,0.f };
	_matrix			matLightView = XMMatrixLookAtLH(vLightEye, vLightAt, vLightUp);

	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixTranspose(matLightView), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;



	_uint		iNumMeshes = m_pModelCom->Get_NumMeshContainers();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 3)))
			return E_FAIL;

	}

	RELEASE_INSTANCE(CGameInstance);



	return S_OK;
}
HRESULT CAnimMasterBall::Ready_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model*/
	if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_STATIC, TEXT("AnimMasterBall"), (CComponent**)&m_pModelCom)))
		return E_FAIL;




	return S_OK;
}

void CAnimMasterBall::Move(_float fTimeDelta)
{

	if (m_bCaptureStart && m_pModelCom->Get_End(m_iAnimIndex))
	{
		m_pModelCom->Set_End(m_iAnimIndex);
		m_iAnimIndex = 1;
		m_pModelCom->Set_CurrentAnimIndex(m_iAnimIndex);

		m_bCaptureStart = false;
	}
	if (!m_bCaptureStart && m_iAnimIndex == 0)
	{
		m_pModelCom->Set_Loop(m_iAnimIndex);
		m_pModelCom->Set_CurrentAnimIndex(m_iAnimIndex);
		m_bCaptureStart = true;
	}
	if (m_bCapture && m_pModelCom->Get_End(m_iAnimIndex))
	{
		m_pModelCom->Set_End(m_iAnimIndex);
		if (Capture())
		{
			m_iAnimIndex = 3;
			Capture_Success();
			if (m_bSuccessFail)
			{
				m_iAnimIndex = 2;
				m_bFail = true;
			}
			g_bCaptureRender = false;
		}
		else
		{
			m_iAnimIndex = 2;
			m_pModelCom->Set_Loop(m_iAnimIndex);
	
			m_bFail = true;
			Ready_FailScript();
			CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
			CTextBox::TINFO tTInfo;

			tTInfo.iScriptSize = (_int)m_vNormalScript.size();
			tTInfo.pTarget = m_pTarget;
			tTInfo.pScript = new wstring[m_vNormalScript.size()];
			tTInfo.iType = 3;
			for (_int i = 0; i < m_vNormalScript.size(); ++i)
				tTInfo.pScript[i] = m_vNormalScript[i];

			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_TextBox"), LEVEL_GAMEPLAY, TEXT("Layer_UI"), &tTInfo)))
				return;
			RELEASE_INSTANCE(CGameInstance);
		}
		m_pModelCom->Set_CurrentAnimIndex(m_iAnimIndex);

		m_bCapture = false;
	}
	if (!m_bCapture && m_iAnimIndex == 1)
	{
		m_pModelCom->Set_Loop(m_iAnimIndex);
		m_pModelCom->Set_CurrentAnimIndex(m_iAnimIndex);
		m_bCapture = true;
	}


	m_pModelCom->Play_Animation(fTimeDelta);


}

void CAnimMasterBall::Ready_SuccessScript()
{
	for (auto iter = m_vNormalScript.begin(); iter != m_vNormalScript.end();)
		iter = m_vNormalScript.erase(iter);


	wstring strTextBegin = TEXT("'");
	wstring strTextEnd = TEXT("'    넌  내꺼야 !!!!");

	strTextBegin += dynamic_cast<CGameObj*>(m_pTargetPoke)->Get_PokeInfo().strName;
	strTextBegin += strTextEnd;

	m_vNormalScript.push_back(strTextBegin);
}

void CAnimMasterBall::Ready_SuccessFailScript()
{
	for (auto iter = m_vNormalScript.begin(); iter != m_vNormalScript.end();)
		iter = m_vNormalScript.erase(iter);

	m_vNormalScript.push_back(TEXT("포켓몬이 꽉 차서 더 이상 데리고 다닐 수 가 없어..."));
}

void CAnimMasterBall::Ready_FailScript()
{
	for (auto iter = m_vNormalScript.begin(); iter != m_vNormalScript.end();)
		iter = m_vNormalScript.erase(iter);

	m_vNormalScript.push_back(TEXT("아쉽다! 조금만 더 하면 잡을 수 있었는데..."));

}

void CAnimMasterBall::Capture_Success()
{
	for (_int i = 0; i < 6; ++i)
	{
		if (dynamic_cast<CGameObj*>(dynamic_cast<CPlayer*>(m_pTarget)->Get_Bag()->Get_vecPoke(i))->Get_PokeInfo().iPokeNum == 999)
		{
			dynamic_cast<CGameObj*>(m_pTargetPoke)->Set_CapturePoke();
			dynamic_cast<CGameObj*>(m_pTargetPoke)->Get_Transfrom()->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(-50000.f, -50000.f, -50000.f, 1.f));
			dynamic_cast<CGameObj*>(m_pTargetPoke)->Set_BallType(0);
			dynamic_cast<CPlayer*>(m_pTarget)->Get_Bag()->Set_vecPoke(i, m_pTargetPoke);
			m_bCaptureSuccesss = true;
			Ready_SuccessScript();
			CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
			CTextBox::TINFO tTInfo;

			tTInfo.iScriptSize = (_int)m_vNormalScript.size();
			tTInfo.pTarget = m_pTarget;
			tTInfo.pScript = new wstring[m_vNormalScript.size()];
			tTInfo.iType = 3;
			for (_int i = 0; i < m_vNormalScript.size(); ++i)
				tTInfo.pScript[i] = m_vNormalScript[i];

			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_TextBox"), LEVEL_GAMEPLAY, TEXT("Layer_UI"), &tTInfo)))
				return;

			CLevel_GamePlay::LOADFILE tInfo;

			_vector vInfoPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

			vInfoPos.m128_f32[1] -= 0.8f;
			XMStoreFloat4(&tInfo.vPos, vInfoPos);

			for (_int i = 0; i < 100; ++i)
			{
				if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_BallEffect4"), LEVEL_GAMEPLAY, TEXT("Layer_Effect"), &tInfo)))
					return;
			}
			vInfoPos.m128_f32[1] += 0.3f;
			for (_int i = 0; i < 3; ++i)
			{
				tInfo.vScale = { (_float)i,0.f,0.f};
				if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_CaptureStar"), LEVEL_GAMEPLAY, TEXT("Layer_Effect"), &tInfo)))
					return;
			}
			RELEASE_INSTANCE(CGameInstance);
			return;
		}
	}


	m_bSuccessFail = true;
	Ready_SuccessFailScript();
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	CTextBox::TINFO tTInfo;

	tTInfo.iScriptSize = (_int)m_vNormalScript.size();
	tTInfo.pTarget = m_pTarget;
	tTInfo.pScript = new wstring[m_vNormalScript.size()];
	tTInfo.iType = 3;
	for (_int i = 0; i < m_vNormalScript.size(); ++i)
		tTInfo.pScript[i] = m_vNormalScript[i];

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_TextBox"), LEVEL_GAMEPLAY, TEXT("Layer_UI"), &tTInfo)))
		return;
	RELEASE_INSTANCE(CGameInstance);
}

_bool CAnimMasterBall::Capture()
{
	_int iDest = rand() % 100 + 1;

	if (iDest <= 100)
		return true;
	else
		return false;
}


HRESULT CAnimMasterBall::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_World4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;



	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CAnimMasterBall * CAnimMasterBall::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CAnimMasterBall*	pInstance = new CAnimMasterBall(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CAnimMasterBall"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CAnimMasterBall::Clone(void * pArg)
{
	CGameObject*	pInstance = new CAnimMasterBall(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CAnimMasterBall"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAnimMasterBall::Free()
{
	__super::Free();

	for (auto iter = m_vNormalScript.begin(); iter != m_vNormalScript.end();)
		iter = m_vNormalScript.erase(iter);

	m_vNormalScript.clear();

	Safe_Release(m_pModelCom);
}
