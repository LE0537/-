
matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix			g_ViewMatrixInv, g_ProjMatrixInv;

vector			g_vCamPosition;

vector			g_vLightDir;
vector			g_vLightPos;
float			g_fLightRange;
vector			g_vLightDiffuse;
vector			g_vLightAmbient;
vector			g_vLightSpecular;

vector			g_vMtrlAmbient = vector(1.f, 1.f, 1.f, 1.f);
vector			g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);

texture2D		g_DiffuseTexture;
texture2D		g_NormalTexture;
texture2D		g_DepthTexture;
texture2D		g_ShadeTexture;
texture2D		g_SpecularTexture;

sampler LinearSampler = sampler_state
{
	filter = min_mag_mip_Linear;
};


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

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	float4		vColor : SV_TARGET0;
};

/* 이렇게 만들어진 픽셀을 PS_MAIN함수의 인자로 던진다. */
/* 리턴하는 색은 Target0 == 장치에 0번째에 바인딩되어있는 렌더타겟(일반적으로 백버퍼)에 그린다. */
/* 그래서 백버퍼에 색이 그려진다. */
PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	
	return Out;
}

struct PS_OUT_LIGHT
{
	float4		vShade : SV_TARGET0;
	float4		vSpecular : SV_TARGET1;
};

PS_OUT_LIGHT PS_MAIN_LIGHT_DIRECTIONAL(PS_IN In)
{
	PS_OUT_LIGHT		Out = (PS_OUT_LIGHT)0;

	/* 0 ~ 1 => -1 ~ 1*/
	vector			vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	vector			vDepthDesc = g_DepthTexture.Sample(LinearSampler, In.vTexUV);

	float			fViewZ = vDepthDesc.y * 1300.f;

	vector			vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);

	Out.vShade = g_vLightDiffuse * (saturate(dot(normalize(g_vLightDir) * -1.f, normalize(vNormal))) + (g_vLightAmbient * g_vMtrlAmbient));
	
	Out.vShade.a = 1.f;

	vector			vWorldPos = (vector)0.f;

	/* 투영 공간상의 위치를 구했다. */
	/* 투영 공간 == 로컬점의위치 * 월드행렬 * 뷰행렬 * 투영행렬 / w */
	vWorldPos.x = In.vTexUV.x * 2.f - 1.f;
	vWorldPos.y = In.vTexUV.y * -2.f + 1.f;
	vWorldPos.z = vDepthDesc.r;
	vWorldPos.w = 1.0f;

	/* 로컬점의위치 * 월드행렬 * 뷰행렬 * 투영행렬 */
	vWorldPos *= fViewZ;

	/* 뷰 공간상의 위치르 ㄹ구한다. */
	/* 로컬점의위치 * 월드행렬 * 뷰행렬  */
	vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

	/* 로컬점의위치 * 월드행렬   */
	vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

	vector			vReflect = reflect(normalize(g_vLightDir), normalize(vNormal));
	vector			vLook = vWorldPos - g_vCamPosition;
	
	if (vNormalDesc.w == 1.f)
	{
		Out.vSpecular = 0.f;
	}
	else if (vNormalDesc.w == 2.f)
	{
		Out.vSpecular = 0.f;
	}
	else
	{
		Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * pow(saturate(dot(normalize(vReflect) * -1.f, normalize(vLook))), 256.f);
		Out.vSpecular.a = 0.f;
	}

	return Out;
}
PS_OUT_LIGHT PS_MAIN_LIGHT_POINT(PS_IN In)
{
	PS_OUT_LIGHT		Out = (PS_OUT_LIGHT)0;

	/* 0 ~ 1 => -1 ~ 1*/
	vector			vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	vector			vDepthDesc = g_DepthTexture.Sample(LinearSampler, In.vTexUV);

	float			fViewZ = vDepthDesc.y * 1300.f;

	vector			vWorldPos = (vector)0.f;

	/* 투영 공간상의 위치를 구했다. */
	/* 투영 공간 == 로컬점의위치 * 월드행렬 * 뷰행렬 * 투영행렬 / w */
	vWorldPos.x = In.vTexUV.x * 2.f - 1.f;
	vWorldPos.y = In.vTexUV.y * -2.f + 1.f;
	vWorldPos.z = vDepthDesc.r;
	vWorldPos.w = 1.0f;

	/* 로컬점의위치 * 월드행렬 * 뷰행렬 * 투영행렬 */
	vWorldPos *= fViewZ;

	/* 뷰 공간상의 위치르 ㄹ구한다. */
	/* 로컬점의위치 * 월드행렬 * 뷰행렬  */
	vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

	/* 로컬점의위치 * 월드행렬   */
	vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

	vector			vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);


	vector			vLightDir = vWorldPos - g_vLightPos;

	float			fDistance = length(vLightDir);
	float			fAtt = saturate((g_fLightRange - fDistance) / g_fLightRange);

	if (vNormalDesc.w == 2.f)
	{
		Out.vShade = g_vLightDiffuse * (saturate(dot(normalize(vLightDir) * -1.f, normalize(vNormal))) + 0.6f);

		Out.vShade *= fAtt;
	}
	else
	{
		fAtt = saturate((1.f - fDistance) / 1.f);
		Out.vShade *= fAtt;
	}
	Out.vShade.a = 1.f;


	vector			vReflect = reflect(normalize(vLightDir), normalize(vNormal));
	vector			vLook = vWorldPos - g_vCamPosition;


	if (vNormalDesc.w == 2.f)
	{
		Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * pow(saturate(dot(normalize(vReflect) * -1.f, normalize(vLook))), 20.f);
		Out.vSpecular *= fAtt;
		Out.vSpecular.a = 0.f;
	}
	else
		Out.vSpecular = 0.f;
	

	return Out;
}

PS_OUT PS_MAIN_BLEND(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector			vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);

	vector			vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector			vShade = g_ShadeTexture.Sample(LinearSampler, In.vTexUV);
	vector			vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexUV);
	vector			vDepth = g_DepthTexture.Sample(LinearSampler, In.vTexUV);

	vector		vFogColor = vector(1.f, 1.f, 1.f, 1.f);

	float		fFogPower = 0.0f;

	float		fFogDistance = 5.f;

	fFogPower = max((vDepth.y * 1300.f) - fFogDistance, 0.f) / 700.0f;

	Out.vColor = (vDiffuse * vShade + vSpecular) + vFogColor * fFogPower;


	if (Out.vColor.a == 0.f)
		discard;

	return Out;
}

RasterizerState RS_Default
{
	FillMode = solid;
	CullMode = back;
	FrontCounterClockwise = false;
};
BlendState BS_Default
{
	BlendEnable[0] = false;
};
BlendState BS_LightBlending
{
	BlendEnable[0] = true;
	BlendEnable[1] = true;

	SrcBlend = one;
	DestBlend = one;
	BlendOp = add;
};
DepthStencilState DSS_Default
{
	DepthEnable = true;
	DepthWriteMask = all;
	DepthFunc = less_equal;
};

DepthStencilState DSS_ZEnable_Disable_ZWrite_Disable
{
	DepthEnable = false;
	DepthWriteMask = zero;
};



technique11 DefaultTechnique
{
	pass DebugTarget
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_ZEnable_Disable_ZWrite_Disable, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass Light_Directional
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_LightBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_ZEnable_Disable_ZWrite_Disable, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_LIGHT_DIRECTIONAL();
	}

	pass Light_Point
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_LightBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_ZEnable_Disable_ZWrite_Disable, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_LIGHT_POINT();
	}

	pass Blend
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_ZEnable_Disable_ZWrite_Disable, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BLEND();
	}

}