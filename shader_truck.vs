#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aOffset;
layout (location = 3) in float angle;

out vec3 FragPos;
out vec3 Normal;
out vec3 index;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    mat4 m4 = mat4(
                cos(angle), -sin(angle), 0.0, 0.0,
                sin(angle), cos(angle), 0.0, 0.0,
                0.0,        0.0,         1.0, 0.0,
                0.0,        0.0,         0.0, 1.0
                );
    vec4 pos = m4 * vec4(aPos,1.0);

    vec3 pos2 = pos.xyz;
    FragPos = vec3(model * vec4(pos2+aOffset*1500, 1.0));
    Normal = mat3(transpose(inverse(model*m4))) * aNormal;
    gl_Position = projection * view * vec4(FragPos, 1.0);
    index = aOffset;
}
