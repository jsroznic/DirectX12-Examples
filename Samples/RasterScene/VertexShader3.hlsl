struct RootParams
{
	matrix MVP;
	matrix ModelView;
	matrix NormalMat;
};

ConstantBuffer<RootParams> ModelViewProjectionCB : register(b0);

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
	float4 VertexPos : POSITION;
	float4 Position : SV_Position;
};

VertexShaderOutput main(VertexPosColor IN)
{
	VertexShaderOutput OUT;

	OUT.Position = mul(ModelViewProjectionCB.MVP, float4(IN.Position, 1.0f));
	OUT.VertexPos = mul(ModelViewProjectionCB.ModelView, float4(IN.Position, 1.0f));
	
	OUT.Color = float4(IN.Color, 1.0f);
	OUT.Normal = mul((float3x3)ModelViewProjectionCB.NormalMat, IN.Normal);

	return OUT;
}