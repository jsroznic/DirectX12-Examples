struct ModelViewProjection
{
	matrix MVP;
	matrix ModelView;
	matrix NormalMat;
	matrix ViewMat;
};

ConstantBuffer<ModelViewProjection> ModelViewProjectionCB : register(b0);

struct VertexPosColor
{
	float3 Position : POSITION;
	float3 Color    : COLOR;
	float3 Normal   : NORMAL;
};

struct VertexShaderOutput
{
	float4 Color    : COLOR;
	float3 Normal   : NORMAL;
	float4 WorldPos : POSITION;
	float4 LightPos : POSITION1;
	float4 Position : SV_Position;
};

VertexShaderOutput main(VertexPosColor IN)
{
	VertexShaderOutput OUT;

	// Static Light Position (World Coordinates)
	float3 light = float3(10, 10, -10);
	OUT.LightPos = mul(ModelViewProjectionCB.ViewMat, float4(light, 1.0f));

	OUT.Position = mul(ModelViewProjectionCB.MVP, float4(IN.Position, 1.0f));
	OUT.WorldPos = mul(ModelViewProjectionCB.ModelView, float4(IN.Position, 1.0f));
	
	OUT.Color = float4(IN.Color, 1.0f);
	OUT.Normal = mul((float3x3)ModelViewProjectionCB.NormalMat, IN.Normal);

	return OUT;
}