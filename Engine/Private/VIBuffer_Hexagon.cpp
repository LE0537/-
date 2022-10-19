#include "..\Public\VIBuffer_Hexagon.h"

CVIBuffer_Hexagon::CVIBuffer_Hexagon(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Hexagon::CVIBuffer_Hexagon(const CVIBuffer_Hexagon & rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CVIBuffer_Hexagon::Initialize_Prototype()
{
#pragma region VERTICES
	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));

	m_iStride = sizeof(VTXTEX);
	m_iNumVertices = 7;
	m_iNumVertexBuffers = 1;
	m_eFormat = DXGI_FORMAT_R16_UINT;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* 정적버퍼를 생성한다. */
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStride;

	VTXTEX*			pVertices = new VTXTEX[m_iNumVertices];

	pVertices[0].vPosition = _float3(0.f, 0.f, 0.f);
	pVertices[0].vTexture = { 0.f,0.f};
	pVertices[1].vPosition = _float3(0.f, 1.f, 0.f);
	pVertices[1].vTexture = { 1.f,1.f};
	pVertices[2].vPosition = _float3(sqrtf(3.f) / 2.f, 0.5f, 0.f);
	pVertices[2].vTexture = { 2.f,2.f};
	pVertices[3].vPosition = _float3(sqrtf(3.f) / 2.f, -0.5f, 0.f);
	pVertices[3].vTexture = { 3.f,3.f };
	pVertices[4].vPosition = _float3(0.f, -1.f, 0.f);
	pVertices[4].vTexture = { 4.f,4.f };
	pVertices[5].vPosition = _float3(-(sqrtf(3.f) / 2.f), -0.5f, 0.f);
	pVertices[5].vTexture = { 5.f,5.f };
	pVertices[6].vPosition = _float3(-(sqrtf(3.f) / 2.f), 0.5f, 0.f);
	pVertices[6].vTexture = { 6.f,6.f };
	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pVertices;

	/* 정점을 담기 위한 공간을 할당하고, 내가 전달해준 배열의 값들을 멤카피한다. */
	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pVertices);
#pragma endregion


#pragma region Indices
	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));

	m_iIndicesByte = sizeof(FACEINDICES16);
	m_iNumPrimitive = 6;
	m_iNumIndicesPerPrimitive = 3;

	m_BufferDesc.ByteWidth = m_iIndicesByte * m_iNumPrimitive;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* 정적버퍼를 생성한다. */
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = sizeof(_ushort);

	FACEINDICES16*			pIndices = new FACEINDICES16[m_iNumPrimitive];

	pIndices[0]._0 = 0;
	pIndices[0]._1 = 1;
	pIndices[0]._2 = 2;

	pIndices[1]._0 = 0;
	pIndices[1]._1 = 2;
	pIndices[1]._2 = 3;

	pIndices[2]._0 = 0;
	pIndices[2]._1 = 3;
	pIndices[2]._2 = 4;

	pIndices[3]._0 = 0;
	pIndices[3]._1 = 4;
	pIndices[3]._2 = 5;

	pIndices[4]._0 = 0;
	pIndices[4]._1 = 5;
	pIndices[4]._2 = 6;

	pIndices[5]._0 = 0;
	pIndices[5]._1 = 6;
	pIndices[5]._2 = 1;

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pIndices;

	/* 정점을 담기 위한 공간을 할당하고, 내가 전달해준 배열의 값들을 멤카피한다. */
	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pIndices);
#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Hexagon::Initialize(void * pArg)
{
	return S_OK;
}

CVIBuffer_Hexagon * CVIBuffer_Hexagon::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CVIBuffer_Hexagon*	pInstance = new CVIBuffer_Hexagon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CVIBuffer_Hexagon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CComponent * CVIBuffer_Hexagon::Clone(void * pArg)
{
	CVIBuffer_Hexagon*	pInstance = new CVIBuffer_Hexagon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CVIBuffer_Hexagon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Hexagon::Free()
{
	__super::Free();
}
