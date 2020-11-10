/***************************************************************
 **	Name:        PixelShader.hlsl                             **
 **	Description: Passthrough PS                               **
 **	Author:      Geoffrey Douglas, geoffrey.douglas@intel.com **
 **	Published:   <insert date>                                **
 **************************************************************/

struct VertexOut
{
	float4 PosH : SV_POSITION;
	float2 Tex : TEXCOORD;
};

SamplerState samLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

Texture2D gTexture : register(t0);

float4 PS(VertexOut pin) : SV_Target
{
	return gTexture.Sample(samLinear, pin.Tex);
}