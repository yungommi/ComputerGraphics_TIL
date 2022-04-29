#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec4 aColor;
layout (location = 3) in vec2 aTexCoord;
//pos랑 color만 사용

out vec4 toColor;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0);
    toColor = aColor;
}

//position(local) +model => world
//world + view => camera
//camera + projection, ndc => gl_Position

