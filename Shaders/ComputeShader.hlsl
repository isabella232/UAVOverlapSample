/******************************************************************************************************
 **	Name:        ComputeShader.hlsl                                                                  **
 **	Description: Sample Compute Shader - By design, each dispatch is guaranteed to write to a unique **
 **              location within the bound UAV, so it is safe to disable UAV syncs between them.     **
 **	Author:      Geoffrey Douglas, geoffrey.douglas@intel.com                                        **
 **	Published:   <insert date>                                                                       **
 *****************************************************************************************************/

RWTexture2D<float4> gOutput : register(u0);

cbuffer cbuff : register(b0)
{
	uint dispatchX;
	uint dispatchY;
	uint windowWidth;
	uint windowHeight;
};

[numthreads(16, 16, 1)]
void CS(uint3 mGroupThreadID : SV_GroupThreadID)
{
	// Compute screen coordinates for the current thread
	uint xcoord = dispatchX * 16 + mGroupThreadID.x;
	uint ycoord = dispatchY * 16 + mGroupThreadID.y;
	uint2 coord = uint2(xcoord, ycoord);

	// Write out a color to the bound UAV at this thread's screen coordinate
	gOutput[coord] = float4((float)xcoord / windowWidth, (float)ycoord / windowHeight, 0.5, 1.0);
}