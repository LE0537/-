
#include "Client_Shader_Defines.hpp"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D		g_DiffuseTexture;

vector			g_vCamPosition;

float4			g_vLightDiffuse = float4(1.f, 1.f, 1.f, 1.f); //���ǻ�
float4			g_vLightAmbient = float4(0.5f, 0.5f, 0.5f, 1.f); //���� �ּ� ���
float4			g_vLightSpecular = float4(1.f, 1.f, 1.f, 1.f); //���� ���̶���Ʈ (�����Ÿ��� ���)

float4			g_vLightDir = float4(3.f, -3.f, -3.f, 0.f); // ���ǹ��� //���⼺���� // ��

float4			g_vMtrlAmbient = float4(1.f, 1.f, 1.f, 1.f);  // ������ ������
float4			g_vMtrlSpecular = float4(0.1f, 0.1f, 0.1f, 1.f);  // ������ ���̶���Ʈ (�����Ÿ��´���)

struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float3		vTangent : TANGENT;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float3		vNormal : NORMAL;
	float		fShade : COLOR0;
	float		fSpecular : COLOR1;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
};

/* DrawIndexed�Լ��� ȣ���ϸ�. */
/* �ε������ۿ� ����ִ� �ε�����°�� ������ VS_MAIN�Լ��� ���ڷ� ������. VS_IN�� ����ȴ�. */
/* �Ϲ������� TriangleList�� �׸����, ���� ������ ���� VS_MAIN�Լ��� ���ڷ� ������. */
VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	/* ������ ��ġ�� ���� �� ��������� ���Ѵ�. ���� ������ ViewSpace�� �����ϳ�. */
	/* ������ı��� ���ϸ� ������ġ�� w�� �佺���̽� ���� z�� �����Ѵ�. == Out.vPosition�� �ݵ�� float4�̾���ϴ� ����. */
	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	Out.vNormal = In.vNormal;

	vector		vWorldNormal = mul(vector(In.vNormal, 0.f), g_WorldMatrix);

	Out.fShade = max(dot(normalize(g_vLightDir) * -1.f, normalize(vWorldNormal)), 0.f);


	vector		vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);

	vector		vReflect = reflect(normalize(g_vLightDir), normalize(vWorldNormal));
	vector		vLook = vWorldPos - g_vCamPosition;

	Out.fSpecular = pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), 20);

	Out.vWorldPos = vWorldPos;

	return Out;
}
VS_OUT VS_Sky(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	/* ������ ��ġ�� ���� �� ��������� ���Ѵ�. ���� ������ ViewSpace�� �����ϳ�. */
	/* ������ı��� ���ϸ� ������ġ�� w�� �佺���̽� ���� z�� �����Ѵ�. == Out.vPosition�� �ݵ�� float4�̾���ϴ� ����. */
	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float3		vNormal : NORMAL;
	float		fShade : COLOR0;
	float		fSpecular : COLOR1;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
};

struct PS_OUT
{
	float4		vColor : SV_TARGET0;
};

/* �̷��� ������� �ȼ��� PS_MAIN�Լ��� ���ڷ� ������. */
/* �����ϴ� ���� Target0 == ��ġ�� 0��°�� ���ε��Ǿ��ִ� ����Ÿ��(�Ϲ������� �����)�� �׸���. */
/* �׷��� ����ۿ� ���� �׷�����. */
PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);


	Out.vColor = (g_vLightDiffuse * vDiffuse) *saturate(In.fShade + g_vLightAmbient * g_vMtrlAmbient)
		+ (g_vLightSpecular * g_vMtrlSpecular) * In.fSpecular;

	if (Out.vColor.a <= 0.3f)
		discard;

	return Out;
}
PS_OUT PS_Tree(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);


	Out.vColor = (g_vLightDiffuse * vDiffuse) *saturate(In.fShade + g_vLightAmbient * g_vMtrlAmbient)
		+ (g_vLightSpecular * g_vMtrlSpecular) * In.fSpecular;

	Out.vColor.a = Out.vColor.g;

	if (Out.vColor.a <= 0.15f)
		discard;

	return Out;
}
PS_OUT PS_Map(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);


	Out.vColor = (g_vLightDiffuse * vDiffuse) *saturate(In.fShade + g_vLightAmbient * g_vMtrlAmbient)
		+ (g_vLightSpecular * g_vMtrlSpecular) * In.fSpecular;


	if (Out.vColor.a <= 0.6f)
		discard;


	return Out;
}

PS_OUT PS_Sky(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	if (Out.vColor.a <= 0.6f)
		discard;

	return Out;
}

technique11 DefaultTechnique
{
	pass Default
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass Map
	{
		SetRasterizerState(RS_Map);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_Map();
	}
	
	pass Tree
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_Tree();
	}

	pass Sky
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_Sky();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_Sky();
	}
}