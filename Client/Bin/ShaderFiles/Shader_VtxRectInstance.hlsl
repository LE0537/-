
#include "Client_Shader_Defines.hpp"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D		g_DiffuseTexture;
//vector			g_vCamPosition;

struct VS_IN
{
	float3		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;

	float4		vRight : TEXCOORD1;
	float4		vUp : TEXCOORD2;
	float4		vLook : TEXCOORD3;
	float4		vTranslation : TEXCOORD4;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
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

	float4x4	TransformMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);

	vector		vPosition = mul(vector(In.vPosition, 1.f), TransformMatrix);
	
	Out.vPosition = mul(vPosition, matWVP);
	Out.vTexUV = In.vTexUV;

	return Out;
}

/* TriangleList�� �׷ȴٸ� ���� ������ VS_MAIN�� ������ ��� ��ģ ����. */

/* w������ ������ �����Ͽ� ���������̽��� ����. */

/* !!!!!!!!!!!!!!!!���. ���������� ��� ������ ���ؼ��� ���� ������ �����Ѵ�. */

/* ����Ʈ�����̽�(Ŭ���̾�Ʈ���� == ��������ǥ)���� ��ȯ�� �����Ѵ�. */
/* �����Ͷ���� �����Ѵ�. == ������ ������ ��������Ͽ� �ȼ��� �����Ѵ�. VS_OUT == PS_IN�� ���� ����. */

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
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

	Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
		
	if (Out.vColor.a < 0.5f)
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

}