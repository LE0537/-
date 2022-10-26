#pragma once
#include "Client_Defines.h"
#include "GameObj.h"


BEGIN(Engine)

class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
class CModel;
END


BEGIN(Client)

class CMeowth : public CGameObj
{
private:
	CMeowth(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMeowth(const CMeowth& rhs);
	virtual ~CMeowth() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	HRESULT SetUp_ShaderResources();
	HRESULT Ready_Components();
	void	Set_DeckPos();
	void	Key_Input(_float fTimeDelta);
private:
	CModel*					m_pModelCom = nullptr;

	_int					m_iAnim = 0;
	_bool					m_bSetPos = false;

	_float					m_fX, m_fY, m_fSizeX, m_fSizeY;
	_float4x4				m_ViewMatrix, m_ProjMatrix;



public:
	static CMeowth* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END
