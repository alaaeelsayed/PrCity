uniform mat4 projection;
uniform mat4 view;
uniform mat4 world;

in vec4 a_position;
in vec2 a_uv1;

out vec2 v_uv1;

void main()
{
    vec4 pos = projection * view * world * a_position;
    // Reset the Z value to always be 1
    gl_Position = pos.xyww;
	v_uv1 = a_uv1;
}
