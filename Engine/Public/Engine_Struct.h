#pragma once

namespace Engine
{
	enum CHANNELID { SOUND_EFFECT, SOUND_BGM, MAXCHANNEL };

	typedef struct tagKeyframe
	{
		float				fTime;
		XMFLOAT3			vScale;
		XMFLOAT4			vRotation;
		XMFLOAT3			vPosition;
	}KEYFRAME;

	typedef struct tagModelMaterial
	{
		class CTexture*		pMaterials[AI_TEXTURE_TYPE_MAX];
	}MODELMATERIAL;

	typedef struct tagLightDesc
	{
		enum TYPE { TYPE_DIRECTIONAL, TYPE_POINT, TYPE_FIELDSHADOW, TYPE_BATTLESHADOW, TYPE_RACESHADOW, TYPE_END };

		TYPE		eType = TYPE_END;
		XMFLOAT4	vPosition;
		float		fRange;
		XMFLOAT4	vDirection;

		XMFLOAT4	vDiffuse;
		XMFLOAT4	vAmbient;
		XMFLOAT4	vSpecular;
	}LIGHTDESC;

	typedef struct tagLineIndices16
	{
		unsigned short		_0, _1;
	}LINEINDICES16;

	typedef struct tagLineIndices32
	{
		unsigned long		_0, _1;
	}LINEINDICES32;

	typedef struct tagFaceIndices16
	{
		unsigned short		_0, _1, _2;
	}FACEINDICES16;

	typedef struct tagFaceIndices32
	{
		unsigned long		_0, _1, _2;
	}FACEINDICES32;

	typedef struct tagVertexPosition
	{
		XMFLOAT3		vPosition;
	}VTXPOS;

	typedef struct ENGINE_DLL tagVertexPosition_Declaration
	{
		static const unsigned int iNumElements = 1;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	}VTXPOS_DECLARATION;

	typedef struct tagVertexTexture
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vTexture;
	}VTXTEX;

	typedef struct ENGINE_DLL tagVertexTexture_Declaration
	{
		static const unsigned int iNumElements = 2;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	}VTXTEX_DECLARATION;

	typedef struct tagVertexMatrix
	{
		XMFLOAT4		vRight;
		XMFLOAT4		vUp;
		XMFLOAT4		vLook;
		XMFLOAT4		vPosition;
	}VTXMATRIX;

	typedef struct ENGINE_DLL tagVertexRectInstance_Declaration
	{
		static const unsigned int iNumElements = 6;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	}VTXRECTINSTANCE_DECLARATION;

	typedef struct tagVertexPoint
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vPSize;
	}VTXPOINT;

	typedef struct ENGINE_DLL tagVertexPointInstance_Declaration
	{
		static const unsigned int iNumElements = 6;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	}VTXPOINTINSTANCE_DECLARATION;


	typedef struct tagVertexNormalTexture
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexture;
	}VTXNORTEX;

	typedef struct ENGINE_DLL tagVertexNormalTexture_Declaration
	{
		static const unsigned int iNumElements = 3;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	}VTXNORTEX_DECLARATION;

	typedef struct tagVertexCubeTexture
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vTexture;
	}VTXCUBETEX;

	typedef struct tagVertexColor
	{

	}VTXCOL;

	typedef struct tagVertexModel
	{
		XMFLOAT3			vPosition;
		XMFLOAT3			vNormal;
		XMFLOAT2			vTexUV;
		XMFLOAT3			vTangent;
	}VTXMODEL;

	typedef struct ENGINE_DLL tagVertexModel_Declaration
	{
		static const unsigned int iNumElements = 4;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	}VTXMODEL_DECLARATION;

	typedef struct tagVertexAnimModel
	{
		XMFLOAT3			vPosition;
		XMFLOAT3			vNormal;
		XMFLOAT2			vTexUV;
		XMFLOAT3			vTangent;
		XMUINT4				vBlendIndex;  /* ???????? ?????? ???? ??(???? ????)?? ??????*/
		XMFLOAT4			vBlendWeight; /* ???????? ?????? ???? ???????? ???? */
	}VTXANIMMODEL;

	typedef struct ENGINE_DLL tagVertexAnimModel_Declaration
	{
		static const unsigned int iNumElements = 6;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	}VTXANIMMODEL_DECLARATION;


	typedef struct tagGraphicDesc
	{
		enum WINMODE { MODE_FULL, MODE_WIN, MODE_END };

		unsigned int	iWinSizeX;
		unsigned int	iWinSizeY;
		HWND			hWnd;
		WINMODE			eWinMode;

	}GRAPHIC_DESC;

	// For. Data

	typedef struct tagBinHierarchyNode
	{

		char cName[MAX_PATH];
		char cParent[MAX_PATH];
		int  iDepth;
		XMFLOAT4X4 mTransform;

	}DATA_BINNODE;

	typedef struct tagBinMaterial
	{

		char cNames[AI_TEXTURE_TYPE_MAX][MAX_PATH];

	}DATA_BINMATERIAL;


	typedef struct tagBinBone
	{
		char		cNames[MAX_PATH];
		XMFLOAT4X4	mOffsetTransform;
	}DATA_BINBONE;

	typedef struct tagBinMesh
	{
		char				cName[MAX_PATH];
		int					iMaterialIndex;

		int					NumVertices;
		VTXMODEL*			pNonAnimVertices;
		VTXANIMMODEL*		pAnimVertices;

		int					iNumPrimitives;
		FACEINDICES32*		pIndices;

		int					iNumBones;
		DATA_BINBONE*		pBones;

	}DATA_BINMESH;


	typedef struct tagBinChannel
	{

		char				szName[MAX_PATH];
		int					iNumKeyFrames;
		KEYFRAME*			pKeyFrames;

	}DATA_BINCHANNEL;
	typedef struct tagBinAnim
	{

		bool				bLoop;
		char				szName[MAX_PATH];
		int					iNumChannels;
		float				fDuration;
		float				fTickPerSecond;
		DATA_BINCHANNEL*	pBinChannel;


	}DATA_BINANIM;

	typedef struct tagBinScene
	{

		int iNodeCount;
		DATA_BINNODE* pBinNodes;

		int iMaterialCount;
		DATA_BINMATERIAL* pBinMaterial;

		int iMeshCount;
		DATA_BINMESH* pBinMesh;

		int iNumAnimations;
		DATA_BINANIM* pBinAnim;

	}DATA_BINSCENE;
}
