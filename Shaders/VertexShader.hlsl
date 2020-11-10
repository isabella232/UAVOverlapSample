/***************************************************************
 **	Name:        VertexShader.hlsl                            **
 **	Description: Passthrough VS                               **
 **	Author:      Geoffrey Douglas, geoffrey.douglas@intel.com **
 **	Published:   <insert date>                                **
 **************************************************************/

struct VertexIn
{
	float3 PosL : POSITION;
	float2 Tex : TEXCOORD;
};

struct VertexOut
{
	float4 PosH : SV_POSITION;
	float2 Tex : TEXCOORD;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	vout.PosH = float4(vin.PosL, 1.0f);
	vout.Tex = vin.Tex;

	return vout;
}