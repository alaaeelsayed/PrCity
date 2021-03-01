in vec4 v_normal;
in vec2 v_uv1;
in vec4 v_tangent;

uniform sampler2D tex;
uniform sampler2D u_normalMap;

uniform mat4 scale;
uniform vec3 lightDirection;
uniform vec3 lightColor;

out vec4 PixelColor;

vec3 tangent_to_world_space(vec3 normalMapSample, vec3 normal, vec3 tangent)
{
	vec3 nT = 2.0 * normalMapSample - 1.0;

	vec3 N = normal;
	vec3 T = normalize( tangent - dot(tangent, N) * N);
	vec3 B = cross(N, T);

	mat3 TBN = mat3(T,B,N);

	vec3 bumpNormal = TBN * nT;
	return bumpNormal;
}

void main()
{
    vec4 uv = scale * vec4(v_uv1, 0, 1);

	vec3 normalMapSample = texture(u_normalMap, uv.xy).rgb;
    vec3 normal = tangent_to_world_space( normalMapSample, v_normal.xyz, v_tangent.xyz );

    vec3 diffuseLight = lightColor * max(0, dot(normal, lightDirection));
    vec3 diffuseMaterial = texture(tex, uv.xy).rgb;
    PixelColor.rgb = diffuseMaterial * (vec3(0.3, 0.3, 0.3) + diffuseLight);
    PixelColor.a = 1.0;
}


