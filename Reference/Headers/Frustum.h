#pragma once

#include "Base.h"

BEGIN(Engine)

class CFrustum final : public CBase
{
	DECLARE_SINGLETON(CFrustum)
private:
	CFrustum();
	virtual ~CFrustum() = default;
public:
	HRESULT Initialize();
	void Tick();
	_bool IsinFrustum(_vector vPos, _float fLength);
private:
	_vector m_vProj[8];
	_vector m_vProj2[6][3];

public:
	virtual void Free() override;
};

END
