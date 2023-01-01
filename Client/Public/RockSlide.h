#pragma once
#include "Client_Defines.h"
#include "GameObj.h"


BEGIN(Engine)

class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;

END


BEGIN(Client)

class CRockSlide : public CGameObj
{
private:
	CRockSlide(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRockSlide(const CRockSlide& rhs);
	virtual ~CRockSlide() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT Render_ShadowDepth();
private:

	_float		m_fSkillTime = 0.f;
	_float		m_fDeadTime = 0.f;
	_bool		m_bSkill = false;
	_bool		m_bHitSkill = false;
	_bool		m_bSound = false;
private:

	void	Set_Pos(_float fTimeDelta);
public:
	static CRockSlide* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END
