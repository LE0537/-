#include "Client_Shader_Defines.hpp"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D		g_DiffuseTexture;	
texture2D		g_NormalTexture;

vector			g_vCamPosition;

float g_fStatHp;
float g_fStatDmg;
float g_fStatSDmg;
float g_fStatDef;
float g_fStatSDef;
float g_fStatSpeed;

float4			g_vLightDiffuse = float4(1.f, 1.f, 1.f, 1.f); //빛의색
float4			g_vLightAmbient = float4(0.5f, 0.5f, 0.5f, 1.f); //빛의 최소 밝기
float4			g_vLightSpecular = float4(1.f, 1.f, 1.f, 1.f); //빛의 하이라이트 (빤딱거리는 흰색)

float4			g_vLightDir = float4(3.f, -3.f, -3.f, 0.f); // 빛의방향 //방향성광원 // 해

float4			g_vMtrlAmbient = float4(1.f, 1.f, 1.f, 1.f);  // 재질의 고유색
float4			g_vMtrlSpecular = float4(0.1f, 0.1f, 0.1f, 1.f);  // 재질의 하이라이트 (빤딱거리는느낌)


float g_fHP;

struct VS_IN
{
	float3		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
};
struct VS_OUTrefract
{
	float4		vPosition : SV_POSITION;
//	float3		vNormal : NORMAL;
//	float		fShade : COLOR0;
//	float		fSpecular : COLOR0;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
};
/* DrawIndexed함수를 호출하면. */
/* 인덱스버퍼에 담겨있는 인덱스번째의 정점을 VS_MAIN함수에 인자로 던진다. VS_IN에 저장된다. */
/* 일반적으로 TriangleList로 그릴경우, 정점 세개를 각각 VS_MAIN함수의 인자로 던진다. */
VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	/* 정점의 위치에 월드 뷰 투영행렬을 곱한다. 현재 정점은 ViewSpace에 존재하낟. */
	/* 투영행렬까지 곱하면 정점위치의 w에 뷰스페이스 상의 z를 보관한다. == Out.vPosition이 반드시 float4이어야하는 이유. */
	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	return Out;
}

VS_OUT VS_HEXAGON(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	/* 정점의 위치에 월드 뷰 투영행렬을 곱한다. 현재 정점은 ViewSpace에 존재하낟. */
	/* 투영행렬까지 곱하면 정점위치의 w에 뷰스페이스 상의 z를 보관한다. == Out.vPosition이 반드시 float4이어야하는 이유. */
//	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	if (In.vTexUV.x == 0.f)
		Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	if (In.vTexUV.x == 1.f)
		Out.vPosition = mul(vector(In.vPosition * min(g_fStatHp,1.f), 1.f), matWVP);
	if (In.vTexUV.x == 2.f)
		Out.vPosition = mul(vector(In.vPosition * min(g_fStatDmg,1.f), 1.f), matWVP);
	if (In.vTexUV.x == 3.f)
		Out.vPosition = mul(vector(In.vPosition * min(g_fStatDef,1.f), 1.f), matWVP);
	if (In.vTexUV.x == 4.f)
		Out.vPosition = mul(vector(In.vPosition * min(g_fStatSpeed,1.f), 1.f), matWVP);
	if (In.vTexUV.x == 5.f)
		Out.vPosition = mul(vector(In.vPosition * min(g_fStatSDef,1.f), 1.f), matWVP);
	if (In.vTexUV.x == 6.f)
		Out.vPosition = mul(vector(In.vPosition * min(g_fStatSDmg,1.f), 1.f), matWVP);

	return Out;
}

VS_OUTrefract VS_Refract(VS_IN In)
{
	VS_OUTrefract		Out = (VS_OUTrefract)0;

	matrix		matWV, matWVP;
	
	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);
	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);

	Out.vTexUV = In.vTexUV;

	vector		vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
	Out.vWorldPos = vWorldPos;

	return Out;
}
/* TriangleList로 그렸다면 정점 세개가 VS_MAIN의 수행을 모두 마친 이후. */

/* w나누기 연산을 수행하여 투영스페이스로 간다. */

/* !!!!!!!!!!!!!!!!잠깐. 투영영역을 벗어난 정점에 대해서는 이후 과정을 생략한다. */

/* 뷰포트스페이스(클라이언트영역 == 윈도우좌표)로의 변환을 수행한다. */
/* 래스터라이즈를 수행한다. == 정점의 정보를 기반으로하여 픽셀을 생성한다. VS_OUT == PS_IN이 같은 이유. */

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct PS_INrefract
{
	float4		vPosition : SV_POSITION;
//	float3		vNormal : NORMAL;
//	float		fShade : COLOR0;
//	float		fSpecular : COLOR0;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
};

struct PS_OUT
{
	float4		vColor : SV_TARGET0;
};

/* 이렇게 만들어진 픽셀을 PS_MAIN함수의 인자로 던진다. */
/* 리턴하는 색은 Target0 == 장치에 0번째에 바인딩되어있는 렌더타겟(일반적으로 백버퍼)에 그린다. */
/* 그래서 백버퍼에 색이 그려진다. */
PS_OUT PS_Refract(PS_INrefract In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	vector      normal = g_NormalTexture.Sample(LinearSampler, In.vTexUV);

	vector		vWorldNormal = mul(normal, g_WorldMatrix);

	vector      fShade = max(dot(normalize(g_vLightDir) * -1.f, normalize(vWorldNormal)), 0.f);

	vector		vReflect = reflect(normalize(g_vLightDir), normalize(vWorldNormal));
	vector		vLook = In.vWorldPos - g_vCamPosition;

	vector      fSpecular = pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), 20);

	Out.vColor = (g_vLightDiffuse * vDiffuse) *saturate(fShade + g_vLightAmbient * g_vMtrlAmbient) + (g_vLightSpecular * g_vMtrlSpecular) * fSpecular;

	return Out;
}
PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	return Out;
}
PS_OUT PS_Screen(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	if (Out.vColor.a < 0.3f)
	{
		discard;
		return Out;
	}
	Out.vColor.a = 0.4f;

	return Out;
}
PS_OUT PS_BattleIntro(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	Out.vColor.a = 0.6f;

	return Out;
}
PS_OUT PS_BattleLight(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = 1.f;

	return Out;
}
PS_OUT PS_HP(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;
	
	Out.vColor.g = min(g_fHP * 1.3f,1.f);

	Out.vColor.r = min(1.5f - g_fHP, 1.f);

	return Out;
}
PS_OUT PS_EXP(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor.g = 0.5f;
	Out.vColor.b = 1.f;
	Out.vColor.r = 0.f;

	return Out;
}
PS_OUT PS_Hexagon(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor.a = 0.6f;
	Out.vColor.r = 0.f;
	Out.vColor.g = 0.3f;
	Out.vColor.b = 1.f;

	return Out;
}

PS_OUT PS_TextBox(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	if (Out.vColor.a < 0.1f)
		discard;

	return Out;
}

PS_OUT PS_BattleInfo(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	if (Out.vColor.a < 0.1f)
		discard;

	Out.vColor.a = 0.85f;

	return Out;
}
PS_OUT PS_Land(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	if (Out.vColor.a < 0.3f)
	{
		discard;
	}

	return Out;
}
PS_OUT PS_EvolLight(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	if (Out.vColor.a < 0.1f)
	{
		discard;
	}

	return Out;
}
PS_OUT PS_EvolLight2(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	if (Out.vColor.a < 0.3f)
	{
		discard;
	}

	return Out;
}
PS_OUT PS_BallRed(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	if (Out.vColor.a < 0.3f)
	{
		discard;
	}

	return Out;
}
PS_OUT PS_Trail(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = 1.f;
	Out.vColor.a = 0.75f;

	return Out;
}
technique11 DefaultTechnique
{
	pass Default //0
	{ 
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass PokeHp //1
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_HP();
	}

	pass Hexagon //2
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.1f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_HEXAGON();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_Hexagon();
	}

	pass Screen //3
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.1f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_Screen();
	}
	pass BattleIntro //4
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.1f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_BattleIntro();
	}
	pass BattleLight //5
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 0.1f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_BattleLight();
	}

	pass TextBox //6
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_TextBox();
	}

	pass BattleInfo //7
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_BattleInfo();
	}

	pass PokeEXP //8
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_EXP();
	}

	pass Land //9
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_Land();
	}

	pass EvolLight //10
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_EvolLight();
	}
	pass EvolLight2 //11
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_EvolLight2();
	}

	pass BallEffect //12
	{
		SetRasterizerState(RS_Effect);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_EvolLight2();
	}
	pass BallRed //13
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_BallRed();
	}
	pass BallRed2 //14
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Priority, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
	pass Skill //15
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Priority, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_BallRed();
	}
	pass Skill2 //16
	{
		SetRasterizerState(RS_Effect);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Priority, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_BallRed();
	}
	pass Refract //17
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_Refract();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_Refract();
	}
	pass Trail //18
	{
		SetRasterizerState(RS_Effect);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_Trail();
	}
}