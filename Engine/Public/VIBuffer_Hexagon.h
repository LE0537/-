#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Hexagon final : public CVIBuffer
{
protected:
	CVIBuffer_Hexagon(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Hexagon(const CVIBuffer_Hexagon& rhs);
	virtual ~CVIBuffer_Hexagon() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

public:
	static CVIBuffer_Hexagon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END