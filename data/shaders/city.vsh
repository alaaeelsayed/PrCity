uniform mat4 projection;
uniform mat4 view;
uniform mat4 world;
uniform mat4 worldIT;

in vec4 a_position;
in vec2 a_uv1;
in vec3 a_normal;
in vec3 a_tangent;

out vec4 v_normal;
out vec2 v_uv1;
out vec4 v_tangent;

void main()
{
    gl_Position = projection * view * world * a_position;
	v_uv1 = a_uv1;
    v_normal = worldIT * vec4(a_normal, 0.0);
    v_tangent = worldIT * vec4(a_tangent, 0.0);
}
