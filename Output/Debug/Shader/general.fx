float4x4 modelMat;
float4x4 viewMat;
float4x4 projMat;

void MyVS(float3 pos:POSITION, float2 tc : TEXCOORD0, float3 normal : NORMAL, float4 col : COLOR,
		  out float4 opos:SV_POSITION,
		  out float2 otc:TEXCOORD0,
		  out float4 ocol : COLOR
		  )
{
	opos = mul(float4(pos,1),modelMat);
	opos = mul(opos,viewMat);
	opos = mul(opos,projMat);
	//opos = mul(projMat, mul(viewMat,mul(modelMat, float4(pos, 1))));
	
	otc = tc;
	ocol = col;
}


Texture2D diftex;

SamplerState defss
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

float4 MyPS(float4 pos:SV_POSITION, float2 tc : TEXCOORD0, float4 col : COLOR) :SV_Target
{
 
	return  diftex.Sample(defss,tc);// * col;
}



technique11 MyTechnique
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0,MyVS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0,MyPS()));
	}
}