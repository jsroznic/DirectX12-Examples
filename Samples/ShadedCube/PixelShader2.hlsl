struct PixelShaderInput
{
	float4 Color    : COLOR;
	float3 Normal   : NORMAL;
	float4 WorldPos : POSITION;
	float4 LightPos : POSITION1;
};

/** Calculate diffuse shading. Normal and light direction do not need
 * to be normalized. */
float diffuseScalar(float3 normal, float3 lightDir, bool frontBackSame)
{
	/* Basic equation for diffuse shading */
	float diffuse = dot(normalize(lightDir), normalize(normal));

	/* The diffuse value will be negative if the normal is pointing in
	 * the opposite direction of the light. Set diffuse to 0 in this
	 * case. Alternatively, we could take the absolute value to light
	 * the front and back the same way. Either way, diffuse should now
	 * be a value from 0 to 1. */
	if (frontBackSame)
		diffuse = abs(diffuse);
	else
		diffuse = clamp(diffuse, 0, 1);

	/* Keep diffuse value in range from .5 to 1 to prevent any object
	 * from appearing too dark. Not technically part of diffuse
	 * shading---however, you may like the appearance of this. */
	diffuse = diffuse / 2 + .5;

	return diffuse;
}

float specularScalar(float3 normal, float3 lightDir, float3 cameraDir, float power) {
	/* Calculate Half-Angle Vector */
	float3 halfVector = 0.5*(normalize(lightDir) + normalize(cameraDir));
	halfVector = normalize(halfVector);

	/* Basic Specular Shading */
	float specular = dot(normalize(normal), halfVector);
	if (specular < 0)
		specular = 0;
	specular = pow(specular, power);
	return specular;
}

float4 main(PixelShaderInput IN) : SV_Target
{
	float4 color = IN.Color;

	float3 lightDir = IN.LightPos.xyz - IN.WorldPos.xyz;
	float diffuse = diffuseScalar(IN.Normal, lightDir, false);
	float specular = specularScalar(IN.Normal, lightDir, - IN.WorldPos.xyz, 10);

	// Color the Cube Red
	color = float4(1.0, 0.0, 0.0, 1.0f);

	return (specular + diffuse)*color;
}