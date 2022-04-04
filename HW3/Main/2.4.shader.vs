#version 330 core
layout (location = 0) in vec3 aPos;
uniform vec2 center;

void main()
{
    gl_Position = vec4(aPos[0] + center[0], aPos[1] + center[1], aPos[2], 1.0);
}

