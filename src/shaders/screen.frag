#version 430 core
out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D screen_texture;
uniform ivec2 offscreen_size;
uniform ivec2 screen_size;

void main()
{
	float offscreen_ratio = float(offscreen_size.x) / offscreen_size.y;
	float screen_ratio = float(screen_size.x) / screen_size.y;

	vec2 coord;

	if(screen_ratio > offscreen_ratio) // align Y
	{
		coord.y = gl_FragCoord.y / screen_size.y;
		float a = screen_ratio / offscreen_ratio - 1;
		coord.x = (gl_FragCoord.x) / screen_size.y / offscreen_ratio - a*0.5;
	}
	else // align X
	{
		float a = offscreen_ratio / screen_ratio - 1;
		coord.y = gl_FragCoord.y / screen_size.x * offscreen_ratio - a*0.5;
		coord.x = gl_FragCoord.x / screen_size.x;
	}
	FragColor = vec4(texture(screen_texture, coord).rgb, 1);
}