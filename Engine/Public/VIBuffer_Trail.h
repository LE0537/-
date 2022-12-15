#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Trail final : public CVIBuffer
{
public:
	typedef struct tagBufferDesc
	{
		_float4x4		matCurWorld;
		_float3			vHighPos;
		_float3			vLowPos;
	}BUFFERDESC;
protected:
	CVIBuffer_Trail(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Trail(const CVIBuffer_Trail& rhs);
	virtual ~CVIBuffer_Trail() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
public:
	void	Set_VBTrail(BUFFERDESC BufferDesc);
	void	Reset_VBTrail();
private:
	list<BUFFERDESC>		listTrail;
	_float3					m_vHighPos;
	_float3					m_vLowPos;

	_int					m_iEndIndex = 0;
	_int					m_iCatRomCnt = 0;
	_int					m_iCatRomIndex[4] = { 0,0,0,0 };

public:
	static CVIBuffer_Trail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END
