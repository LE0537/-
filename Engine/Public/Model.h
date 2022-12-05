#pragma once
#include "Component.h"
#include "HierarchyNode.h"

BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
public:
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END };
private:
	CModel(ID3D11Device* pDevice ,ID3D11DeviceContext* pContext);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	_uint	Get_NumMeshContainers() const {
		return m_iNumMeshes;
	}

	_float4x4 Get_PivotFloat4x4() const {
		return m_PivotMatrix;
	}

	class CHierarchyNode* Get_BonePtr(const char* pBoneName) const;


public:
	virtual HRESULT Initialize_Prototype(TYPE eModelType, const char* pModelFilePath, _fmatrix PivotMatrix);
	virtual HRESULT Bin_Initialize_Prototype(DATA_BINSCENE* pScene, TYPE eType, const char* pModelFilePath, _fmatrix PivotMatrix);	// 추가
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Bin_Initialize(void* pArg); // 추가
public:
	HRESULT SetUp_Material(class CShader* pShader, const char* pConstantName, _uint iMeshIndex, aiTextureType eType);
	HRESULT Play_Animation(_float fTimeDelta);
	HRESULT Render(class CShader* pShader, _uint iMeshIndex, _uint iPassIndex = 0);

public: // For. Data 추가
	HRESULT Get_HierarchyNodeData(DATA_BINSCENE* pBinScene);
	HRESULT Get_MaterialData(DATA_BINSCENE* pBinScene);
	HRESULT Get_MeshData(DATA_BINSCENE* pBinScene);
	HRESULT Get_AnimData(DATA_BINSCENE* pBinScene);

	virtual _bool Picking(class CTransform* pTransform, _float3* pOut);

private: // 추가
	vector<DATA_BINMATERIAL>				m_DataMaterials;

private:
	const aiScene*				m_pAIScene = nullptr;

	DATA_BINSCENE*				m_pBin_AIScene = nullptr; // 추가
	_bool						m_bIsProto = false; // 추가
	_bool						m_bIsBin = false; // 추가

	Assimp::Importer			m_Importer;

private:
	TYPE								m_eModelType = TYPE_END;

	_uint								m_iNumMeshes = 0;
	vector<class CMeshContainer*>		m_Meshes;

private:
	_uint								m_iNumMaterials = 0;
	vector<MODELMATERIAL>				m_Materials;

private:
	/* 내 모델 전체 뼈의 갯수. */
	_uint								m_iNumBones;
	vector<class CHierarchyNode*>		m_Bones;

private:
	_uint								m_iNumAnimations = 0;
	vector<class CAnimation*>			m_Animations;

	_uint								m_iCurrentAnimIndex = 0;
	_uint								m_iPrevAnimIndex = 0;
private:
	_float4x4				m_PivotMatrix;
	_bool					m_bAnimReset = false;

	_float3*					vPos;
private: // 추가
	HRESULT Bin_Ready_MeshContainers(_fmatrix PivotMatrix);
	HRESULT Bin_Ready_Materials(const char* pModelFilePath);
	HRESULT Bin_Ready_HierarchyNodes();
	HRESULT Bin_Ready_Animations(CModel* pModel);
	HRESULT Safe_Release_Scene();

public:
	void Set_CurrentAnimIndex(_uint iAnimIndex){
		if (m_iPrevAnimIndex != iAnimIndex) { m_bAnimReset = true; }
		m_iPrevAnimIndex = iAnimIndex; }
	_bool	Get_End(_int iAnimIndex);
	void	Set_End(_int iAnimIndex);
	void	Set_Loop(_uint iAnimIndex);
private:
	HRESULT Create_MeshContainer();
	HRESULT Create_Materials(const char* pModelFilePath);
	HRESULT Create_HierarchyNodes(const aiNode* pNode, CHierarchyNode* pParent = nullptr);
	HRESULT Create_Animations();
	
public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eModelType, const char* pModelFilePath, _fmatrix PivotMatrix = XMMatrixIdentity());
	static CModel* Bin_Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, DATA_BINSCENE* pScene, TYPE eType, const char* pModelFilePath, _fmatrix PivotMatrix = XMMatrixIdentity()); // 추가
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END