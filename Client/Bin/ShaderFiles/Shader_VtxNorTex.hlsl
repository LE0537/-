
matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

vector			g_vCamPosition;

float4			g_vLightDiffuse = float4(1.f, 1.f, 1.f, 1.f); //ºûÀÇ»ö
float4			g_vLightAmbient = float4(0.3f, 0.3f, 0.3f, 1.f); //ºûÀÇ ÃÖ¼Ò ¹à±â
float4			g_vLightSpecular = float4(1.f, 1.f, 1.f, 1.f); //ºûÀÇ ÇÏÀÌ¶óÀÌÆ® (ºþµü°Å¸®´Â Èò»ö)

/* For.Directional */
float4			g_vLightDir = float4(1.f, -1.f, 1.f, 0.f); // ºûÀÇ¹æÇâ //¹æÇâ¼º±¤¿ø // ÇØ


/* For.Material */
texture2D		g_DiffuseTexture; 
float4			g_vMtrlAmbient = float4(1.f, 1.f, 1.f, 1.f); // ÀçÁúÀÇ °íÀ¯»ö
float4			g_vMtrlSpecular = float4(1.f, 1.f, 1.f, 1.f); // ÀçÁúÀÇ ÇÏÀÌ¶óÀÌÆ® (ºþµü°Å¸®´Â´À³¦)

sampler LinearSampler = sampler_state
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = wrap;
	AddressV = wrap;
};

sampler PointSampler = sampler_state
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = wrap;
	AddressV = wrap;
};


struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float3		vNormal : NORMAL;
	float		fShade : COLOR0;
	float		fSpecular : COLOR1;
	float2		vTexUV : TEXCOORD0;


};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vNormal = In.vNormal;


	vector		vWorldNormal = mul(vector(In.vNormal, 0.f), g_WorldMatrix);

	Out.fShade = max(dot(normalize(g_vLightDir) * -1.f, normalize(vWorldNormal)), 0.f);


	vector		vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);

	vector		vReflect = reflect(normalize(g_vLightDir), normalize(vWorldNormal));
	vector		vLook = vWorldPos - g_vCamPosition;

	Out.fSpecular = pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), 30);

	Out.vTexUV = In.vTexUV * 30.f;

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float3		vNormal : NORMAL;
	float		fShade : COLOR0;
	float		fSpecular : COLOR1;
	float2		vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	float4		vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector		vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);



	Out.vColor = (g_vLightDiffuse * vMtrlDiffuse) * saturate(In.fShade + g_vLightAmbient * g_vMtrlAmbient)
		+ (g_vLightSpecular * g_vMtrlSpecular) * In.fSpecular;

	return Out;
}


technique11 DefaultTechnique
{
	pass Default
	{
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

}