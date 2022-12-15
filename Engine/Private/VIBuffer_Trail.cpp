#include "..\Public\VIBuffer_Trail.h"


CVIBuffer_Trail::CVIBuffer_Trail(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Trail::CVIBuffer_Trail(const CVIBuffer_Trail & rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CVIBuffer_Trail::Initialize_Prototype()
{
#pragma region VERTICES
	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));

	m_iStride = sizeof(VTXTEX);
	m_iNumVertices = 30;
	m_iNumVertexBuffers = 1;
	m_eFormat = DXGI_FORMAT_R16_UINT;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC; 
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStride;

	VTXTEX*			pVertices = new VTXTEX[m_iNumVertices];

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		pVertices[i].vPosition = _float3(-50000.f, -50000.f, -50000.f);
	}
	
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
	m_iNumPrimitive = 28;
	m_iNumIndicesPerPrimitive = 3;

	m_BufferDesc.ByteWidth = m_iIndicesByte * m_iNumPrimitive;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* 정적버퍼를 생성한다. */
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = sizeof(_ushort);

	FACEINDICES16*			pIndices = new FACEINDICES16[m_iNumPrimitive];


	for (_uint i = 0; i < m_iNumPrimitive; i += 2)
	{
		pIndices[i]._0 = i + 3;
		pIndices[i]._1 = i + 1;
		pIndices[i]._2 = i;
				 
		pIndices[i + 1]._0 = i + 2;
		pIndices[i + 1]._1 = i + 3;
		pIndices[i + 1]._2 = i;
	}
	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pIndices;

	/* 정점을 담기 위한 공간을 할당하고, 내가 전달해준 배열의 값들을 멤카피한다. */
	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pIndices);
#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Trail::Initialize(void * pArg)
{



	return S_OK;
}
void CVIBuffer_Trail::Set_VBTrail(BUFFERDESC BufferDesc)
{
	D3D11_MAPPED_SUBRESOURCE		MappedSubResource;
	ZeroMemory(&MappedSubResource, sizeof MappedSubResource);

	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &MappedSubResource);
	
	VTXTEX* pVertices = (VTXTEX*)MappedSubResource.pData;

	listTrail.push_back(BufferDesc);
	if (listTrail.size() > m_iNumVertices / 2)
	{
		listTrail.pop_front();
	}
	_uint i2 = 0;
	for (auto& iter : listTrail)
	{
	//	XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&iter.vLowPos), XMLoadFloat4x4(&iter.matCurWorld)));
	//	XMStoreFloat3(&pVertices[i + 1].vPosition, XMVector3TransformCoord(XMLoadFloat3(&iter.vHighPos), XMLoadFloat4x4(&iter.matCurWorld)));
		
		pVertices[i2].vPosition = iter.vLowPos;
		pVertices[i2 + 1].vPosition = iter.vHighPos;

		i2 += 2;
	}
	for (_uint i = 0; i < listTrail.size(); i += 2)
	{
		XMStoreFloat2(&pVertices[i].vTexture , XMLoadFloat2(&_float2((_float)i / ((_float)listTrail.size() - 2),1.f)));
		XMStoreFloat2(&pVertices[i + 1].vTexture, XMLoadFloat2(&_float2((_float)i / ((_float)listTrail.size() - 2), 0.f)));
	}
	//////여기서부터 테스트
	//if (listTrail.size() > 14)
	//{
	//	m_iEndIndex = m_iCatRomCnt * 2 + m_iNumVertices;
	//	m_iCatRomIndex[2] = m_iEndIndex - 2;
	//	m_iCatRomIndex[3] = m_iEndIndex;

	//	pVertices[m_iEndIndex - 2].vPosition = pVertices[m_iNumVertices - 2].vPosition;
	//	pVertices[m_iEndIndex - 1].vPosition = pVertices[m_iNumVertices - 1].vPosition;

	//	pVertices[m_iEndIndex].vPosition = BufferDesc.vLowPos;
	//	pVertices[m_iEndIndex + 1].vPosition = BufferDesc.vHighPos;

	//	for (_uint j = 0; j < m_iCatRomCnt; ++j)
	//	{
	//		_uint index = j * 2 + m_iNumVertices - 2;
	//		_float fWeight = _float(j + 1) / (m_iCatRomCnt + 1);
	//		XMStoreFloat3(&pVertices[index].vPosition, XMVectorCatmullRom(
	//			XMLoadFloat3(&pVertices[m_iCatRomIndex[0]].vPosition),
	//			XMLoadFloat3(&pVertices[m_iCatRomIndex[1]].vPosition),
	//			XMLoadFloat3(&pVertices[m_iCatRomIndex[2]].vPosition),
	//			XMLoadFloat3(&pVertices[m_iCatRomIndex[3]].vPosition), fWeight
	//		));

	//		XMStoreFloat3(&pVertices[index + 1].vPosition, XMVectorCatmullRom(
	//			XMLoadFloat3(&pVertices[m_iCatRomIndex[0] + 1].vPosition),
	//			XMLoadFloat3(&pVertices[m_iCatRomIndex[1] + 1].vPosition),
	//			XMLoadFloat3(&pVertices[m_iCatRomIndex[2] + 1].vPosition),
	//			XMLoadFloat3(&pVertices[m_iCatRomIndex[3] + 1].vPosition), fWeight
	//		));
	//	}

	//	m_iNumVertices = m_iEndIndex + 2;
	//	m_iCatRomIndex[0] = m_iCatRomIndex[1];
	//	m_iCatRomIndex[1] = m_iCatRomIndex[2];
	//}


	m_pContext->Unmap(m_pVB, 0);

}
void CVIBuffer_Trail::Reset_VBTrail()
{
	listTrail.clear();
	D3D11_MAPPED_SUBRESOURCE		MappedSubResource;
	ZeroMemory(&MappedSubResource, sizeof MappedSubResource);

	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &MappedSubResource);

	VTXTEX* pVertices = (VTXTEX*)MappedSubResource.pData;

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		pVertices[i].vPosition = _float3(-50000.f, -50000.f, -50000.f);
		
	}

	m_pContext->Unmap(m_pVB, 0);
}
CVIBuffer_Trail * CVIBuffer_Trail::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CVIBuffer_Trail*	pInstance = new CVIBuffer_Trail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CVIBuffer_Trail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CComponent * CVIBuffer_Trail::Clone(void * pArg)
{
	CVIBuffer_Trail*	pInstance = new CVIBuffer_Trail(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CVIBuffer_Trail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Trail::Free()
{
	__super::Free();
}
