#include "..\Public\Frustum.h"
#include "PipeLine.h"

IMPLEMENT_SINGLETON(CFrustum)

CFrustum::CFrustum()
{

}

HRESULT CFrustum::Initialize()
{
	m_vProj[0] = { -1.f, 1.f, 0.f,1.f };
	m_vProj[1] = { 1.f, 1.f, 0.f ,1.f };
	m_vProj[2] = { 1.f, -1.f, 0.f,1.f };
	m_vProj[3] = { -1.f, -1.f, 0.f,1.f };
	m_vProj[4] = {-1.f, 1.f, 1.f, 1.f };
	m_vProj[5] = { 1.f, 1.f, 1.f,1.f };
	m_vProj[6] = { 1.f, -1.f, 1.f,1.f };
	m_vProj[7] = { -1.f, -1.f, 1.f,1.f };

	return S_OK;
}

void CFrustum::Tick()
{
	_vector vProj[8];

	for (_uint i = 0; i < 8; ++i)
		vProj[i] = m_vProj[i];

	_matrix matProj, matView, matProjInv, matViewInv;
	matProj = CPipeLine::Get_Instance()->Get_TransformMatrix(CPipeLine::D3DTS_PROJ);
	matProjInv = XMMatrixInverse(nullptr, matProj);
	matView = CPipeLine::Get_Instance()->Get_TransformMatrix(CPipeLine::D3DTS_VIEW);
	matViewInv = XMMatrixInverse(nullptr, matView);
	
	for (_uint i = 0; i < 8; ++i)
	{
		vProj[i] = XMVector3TransformCoord(vProj[i], matProjInv);
		vProj[i] = XMVector3TransformCoord(vProj[i], matViewInv);
	}
	// +z
	m_vProj2[0][0] = vProj[5];
	m_vProj2[0][1] = vProj[4];
	m_vProj2[0][2] = vProj[7];
	// -z
	m_vProj2[1][0] = vProj[0];
	m_vProj2[1][1] = vProj[1];
	m_vProj2[1][2] = vProj[2];

	// -x
	m_vProj2[2][0] = vProj[4];
	m_vProj2[2][1] = vProj[0];
	m_vProj2[2][2] = vProj[3];

	// +x
	m_vProj2[3][0] = vProj[1];
	m_vProj2[3][1] = vProj[5];
	m_vProj2[3][2] = vProj[6];

	// +y
	m_vProj2[4][0] = vProj[4];
	m_vProj2[4][1] = vProj[5];
	m_vProj2[4][2] = vProj[1];


	//-y
	m_vProj2[5][0] = vProj[3];
	m_vProj2[5][1] = vProj[2];
	m_vProj2[5][2] = vProj[6];
}

_bool CFrustum::IsinFrustum(_vector vPos, _float fLength)
{

	for (_uint i = 0; i < 6; ++i)
	{
		
		_vector	Plane = XMPlaneFromPoints(m_vProj2[i][0], m_vProj2[i][1], m_vProj2[i][2]);

		_float		fHeight = XMVectorGetX(XMPlaneDotCoord(Plane, vPos));
		//(Plane.m128_f32[0] * vPos.m128_f32[0]) + (Plane.m128_f32[1] * vPos.m128_f32[1]) + (Plane.m128_f32[2] * vPos.m128_f32[2]) + Plane.m128_f32[3];
		if (fHeight > 0)
		{
			if (fHeight > fLength)
				return false;
		}
	}

	return true;
}

void CFrustum::Free()
{
	
}
