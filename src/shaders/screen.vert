#version 430 core

uniform vec2 verts[6] = vec2[](vec2(-1.0, -1.0), vec2(1.0, -1.0), vec2(1.0, 1.0), 
								vec2(-1.0, -1.0), vec2(1.0, 1.0), vec2(-1.0, 1.0));

uniform vec2 texCoords[6] = vec2[](vec2(0.0, 0.0), vec2(1.0, 0.0), vec2(1.0, 1.0),
									vec2(0.0, 0.0), vec2(1.0, 1.0), vec2(0.0, 1.0));

out vec2 TexCoords;

void main()
{
	gl_Position = vec4(verts[gl_VertexID], 0.0, 1.0);
	TexCoords = texCoords[gl_VertexID];
}
