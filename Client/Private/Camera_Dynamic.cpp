#include "stdafx.h"
#include "..\Public\Camera_Dynamic.h"
#include "GameInstance.h"
#include "GameObj.h"
#include "Level_GamePlay.h"

CCamera_Dynamic::CCamera_Dynamic(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera(pDevice, pContext)
{
}

CCamera_Dynamic::CCamera_Dynamic(const CCamera_Dynamic & rhs)
	: CCamera(rhs)
{
}

HRESULT CCamera_Dynamic::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCamera_Dynamic::Initialize(void* pArg)
{
	*((CGameObject**)&((CAMERADESC_DERIVED*)pArg)->CameraDesc.pCamera) = this;
	
	if (FAILED(__super::Initialize(&((CAMERADESC_DERIVED*)pArg)->CameraDesc)))
		return E_FAIL;
	
	m_FovAngle = XMConvertToRadians(60.f);
	Load();
	m_vTargetBattlePos = dynamic_cast<CGameObj*>(m_CameraDesc.pTarget)->Get_TargetBattlePos();
	camLookPos.y += 3.f;


	return S_OK;
}

void CCamera_Dynamic::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	if (!g_Battle)
	{
		if (!m_bEvent && !g_bBag && !g_bPokeDeck && !g_PokeInfo)
		{
			Key_Input(fTimeDelta);
		}
		if (m_bEvent)
		{
			EventCam(fTimeDelta);
		}
	}
	else
	{
		if (fSpeed < 1.4f)
			BattleEventcam(fTimeDelta);
		else
			Battlecam(fTimeDelta);
	
	}
	if (FAILED(Bind_OnPipeLine()))
		return;
}

void CCamera_Dynamic::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

}

HRESULT CCamera_Dynamic::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	
	return S_OK;
}
void CCamera_Dynamic::Key_Input(_float fTimeDelta)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_long MouseMove = 0;

	if (MouseMove = pGameInstance->Get_DIMMoveState(DIMM_WHEEL))
	{
		m_FovAngle += XMConvertToRadians(-MouseMove * fTimeDelta * 0.4f);
		if ((XMConvertToRadians(80.f) < m_CameraDesc.fFovy))
		{
			m_CameraDesc.fFovy = XMConvertToRadians(80.f);
			m_FovAngle = XMConvertToRadians(80.f);
		}
		else if ((XMConvertToRadians(35.f) > m_CameraDesc.fFovy))
		{
			m_CameraDesc.fFovy = XMConvertToRadians(35.f);
			m_FovAngle = XMConvertToRadians(35.f);
		}
	}
	m_CameraDesc.fFovy = m_FovAngle;

	static _float fAngleX = 0.f;
	static _float fAngleY = 0.f;

	_long         MouseMoveX = 0;
	_long         MouseMoveY = 0;

	CTransform* pTransform = dynamic_cast<CGameObj*>(m_CameraDesc.pTarget)->Get_Transfrom();


	if (MouseMoveY = pGameInstance->Get_DIMMoveState(DIMM_Y))
	{

		fAngleX += MouseMoveY * fTimeDelta * 8.f;

		if (40.f <= fAngleX)
			fAngleX = 40.f;
		else if (-40.f >= fAngleX)
			fAngleX = -40.f;

	}

	if (MouseMoveX = pGameInstance->Get_DIMMoveState(DIMM_X))
	{

		fAngleY += MouseMoveX * fTimeDelta * 8.f;

		if (360.f <= fAngleY)
			fAngleY = 0.f;
		else if (0.f >= fAngleY)
			fAngleY = 360.f;

	}

	_matrix matRotX = XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(fAngleX));
	_matrix matRotY = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(fAngleY));

	_vector vCamDir = XMVector3TransformNormal(XMVectorSet(0.f, 1.f, -1.f, 0.f), matRotX);
	vCamDir = XMVector3TransformNormal(vCamDir, matRotY);
	_vector vCamPos = vCamDir * 5.f;
	_vector vDestPos = pTransform->Get_State(CTransform::STATE_TRANSLATION) + vCamPos;

	m_pTransform->Set_State(CTransform::STATE_TRANSLATION, vDestPos);

	_vector vLookPos = XMVectorSetY(pTransform->Get_State(CTransform::STATE_TRANSLATION), XMVectorGetY(pTransform->Get_State(CTransform::STATE_TRANSLATION)) + 0.8f);
	vLookPos.m128_f32[1] += 1.f;
	m_pTransform->LookAt(vLookPos);

	RELEASE_INSTANCE(CGameInstance);
}

void CCamera_Dynamic::EventCam(_float fTimeDelta)
{
	if (!dynamic_cast<CGameObj*>(m_pTarget)->Get_PalyerInfo().bEvent)
		m_bEvent = false;

}

void CCamera_Dynamic::BattleEventcam(_float fTimeDelta)
{
	_vector TargetPos;
	fSpeed += 0.2f * fTimeDelta;

	_vector vPos = XMVectorCatmullRom(XMLoadFloat4(&camPos1), XMLoadFloat4(&camPos4)
		, XMLoadFloat4(&camPos3), XMLoadFloat4(&camPos2), fSpeed);

	if (fSpeed < 0.7f)
	{
		TargetPos = XMLoadFloat4(&m_vTargetBattlePos);
		m_CameraDesc.fFovy = XMConvertToRadians(50.f);
	}
	else
	{
		TargetPos = dynamic_cast<CGameObj*>(m_CameraDesc.pTarget)->Get_Transfrom()->Get_State(CTransform::STATE_TRANSLATION);
		m_CameraDesc.fFovy = XMConvertToRadians(90.f);
	}

	TargetPos.m128_f32[1] += 2.f;
	vPos.m128_f32[1] += 3.f;
	m_pTransform->Set_State(CTransform::STATE_TRANSLATION, vPos);
	m_pTransform->LookAt(TargetPos);
}

void CCamera_Dynamic::Battlecam(_float fTimeDelta)
{
	m_CameraDesc.fFovy = XMConvertToRadians(60.f);
	m_pTransform->Set_State(CTransform::STATE_TRANSLATION,XMLoadFloat4(&camLookPos));
	m_pTransform->LookAt(XMLoadFloat4(&camAt));
}

void CCamera_Dynamic::Set_Target(CGameObject * _pTarget)
{
	m_pTarget = _pTarget;
	m_bEvent = true;
	_vector vPos = dynamic_cast<CGameObj*>(m_pTarget)->Get_Transfrom()->Get_State(CTransform::STATE_TRANSLATION);
	_vector vTargetPos = dynamic_cast<CGameObj*>(m_pTarget)->Get_Transfrom()->Get_State(CTransform::STATE_TRANSLATION);
	_vector vLook = dynamic_cast<CGameObj*>(m_pTarget)->Get_Transfrom()->Get_State(CTransform::STATE_LOOK);
	_vector vUp = dynamic_cast<CGameObj*>(m_pTarget)->Get_Transfrom()->Get_State(CTransform::STATE_UP);
	XMStoreFloat4(&m_TargetPos, vPos);
	XMVector3Normalize(vLook);
	XMVector3Normalize(vUp);
	vPos += vLook * 100.f;
	vPos += vUp * 60.f;
	m_fOriginY = vPos.m128_f32[1];
	m_pTransform->Set_State(CTransform::STATE_TRANSLATION, vPos);
	vTargetPos += vUp * 10.f;
	m_pTransform->LookAt(vTargetPos);
}

void CCamera_Dynamic::Load()
{
	HANDLE		hFile = CreateFile(L"../Data/BattlePos.dat",			// ���� ��ο� �̸� ���
		GENERIC_READ,				// ���� ���� ��� (GENERIC_WRITE ���� ����, GENERIC_READ �б� ����)
		NULL,						// �������, ������ �����ִ� ���¿��� �ٸ� ���μ����� ������ �� ����� ���ΰ�, NULL�� ��� �������� �ʴ´�
		NULL,						// ���� �Ӽ�, �⺻��	
		OPEN_EXISTING,				// ���� ���, CREATE_ALWAYS�� ������ ���ٸ� ����, �ִٸ� ���� ����, OPEN_EXISTING ������ ���� ��쿡�� ����
		FILE_ATTRIBUTE_NORMAL,		// ���� �Ӽ�(�б� ����, ���� ��), FILE_ATTRIBUTE_NORMAL �ƹ��� �Ӽ��� ���� �Ϲ� ���� ����
		NULL);						// �������� ������ �Ӽ��� ������ ���ø� ����, �츮�� ������� �ʾƼ� NULL

	if (INVALID_HANDLE_VALUE == hFile)
	{
		// �˾� â�� ������ִ� ����� �Լ�
		// 1. �ڵ� 2. �˾� â�� �������ϴ� �޽��� 3. �˾� â �̸� 4. ��ư �Ӽ�
		MessageBox(g_hWnd, TEXT("Load File"), TEXT("Fail"), MB_OK);
		return;
	}

	// 2. ���� ����

	DWORD		dwByte = 0;
	CLevel_GamePlay::SaveInfo		tInfo;
	
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	while (true)
	{
		ReadFile(hFile, &tInfo, sizeof(CLevel_GamePlay::SaveInfo), &dwByte, nullptr);

		if (0 == dwByte)	// ���̻� ���� �����Ͱ� ���� ���
			break;
		
		
		if (tInfo.iType == 2)
		{
			if (tInfo.vScale.y == 0.01f)
				camPos1 = tInfo.vPos;
			else if (tInfo.vScale.y == 0.02f)
				camPos2 = tInfo.vPos;
			else if (tInfo.vScale.y == 0.03f)
				camPos3 = tInfo.vPos;
			else if (tInfo.vScale.y == 0.04f)
				camPos4 = tInfo.vPos;
		}
		else if (tInfo.iType == 4)
		{
			if (tInfo.vScale.y == 0.005f)
				camLookPos = tInfo.vPos;
			else if (tInfo.vScale.y == 0.01f)
				camAt = tInfo.vPos;
		}
	}

	Safe_Release(pGameInstance);
	// 3. ���� �Ҹ�
	CloseHandle(hFile);
}

CCamera_Dynamic * CCamera_Dynamic::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamera_Dynamic*	pInstance = new CCamera_Dynamic(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CCamera_Dynamic"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCamera_Dynamic::Clone(void* pArg)
{
	CCamera_Dynamic*	pInstance = new CCamera_Dynamic(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CCamera_Dynamic"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Dynamic::Free()
{
	__super::Free();

	
}
