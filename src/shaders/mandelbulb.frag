#version 430 core

out vec3 fColor;

uniform ivec2 offscreen_size;
uniform mat3 cam_basis;
uniform vec3 cam_pos;
uniform float epsilon;
uniform float fov;
uniform float mandelbulb_power;
uniform float view_radius;
uniform int mandelbulb_iter_num;

float mandelbulb_sdf(vec3 pos) {
	vec3 z = pos;
	float dr = 1.0;
	float r = 0.0;
	for (int i = 0; i < mandelbulb_iter_num ; i++)
	{
		r = length(z);
		if (r>2) break;
		
		// convert to polar coordinates
		float theta = acos(z.z / r);
		float phi = atan(z.y, z.x);

		dr =  pow( r, mandelbulb_power-1.0)*mandelbulb_power*dr + 1.0;
		
		// scale and rotate the point
		float zr = pow( r,mandelbulb_power);
		theta = theta*mandelbulb_power;
		phi = phi*mandelbulb_power;
		
		// convert back to cartesian coordinates
		z = zr*vec3(sin(theta)*cos(phi), sin(phi)*sin(theta), cos(theta));
		z+=pos;
	}
	return 0.5*log(r)*r/dr;
}

float scene_sdf(vec3 p)
{
	return mandelbulb_sdf(p);
}

vec3 ray_marching(const vec3 eye, const vec3 ray, out float depth, out int steps)
{
	depth = 0;
	steps = 0;
	float dist;
	vec3 intersection_point;

	do
	{
		intersection_point = eye + depth*ray;
		dist = scene_sdf(intersection_point);
		depth += dist;
		steps++;
	}
	while(depth < view_radius && dist > epsilon);

	return intersection_point;
}

vec3 estimate_normal(const vec3 p, const float delta)
{
    return normalize(vec3(
        scene_sdf(vec3(p.x + delta, p.y, p.z)) - scene_sdf(vec3(p.x - delta, p.y, p.z)),
        scene_sdf(vec3(p.x, p.y + delta, p.z)) - scene_sdf(vec3(p.x, p.y - delta, p.z)),
        scene_sdf(vec3(p.x, p.y, p.z  + delta)) - scene_sdf(vec3(p.x, p.y, p.z - delta))
    ));
}


vec2 transformed_coordinates()
{
	vec2 coord = (gl_FragCoord.xy / offscreen_size)*2 - 1;
	coord.y *= offscreen_size.y / float(offscreen_size.x);
	return coord;
}

float contrast(float val, float contrast_offset, float contrast_mid_level)
{
	return clamp((val - contrast_mid_level) * (1 + contrast_offset) + contrast_mid_level, 0, 1);
}

vec3 contrast(vec3 val, float contrast_offset, float contrast_mid_level)
{
	return clamp((val - contrast_mid_level) * (1 + contrast_offset) + contrast_mid_level, 0, 1);
}


void main()
{
	const float ambient_light = 0.2;

	vec2 coord = transformed_coordinates();

	vec3 ray = normalize(vec3(coord*tan(fov), 1));

	ray = cam_basis*ray;

	float depth = 0;
	int steps = 0;
	vec3 intersection_point = ray_marching(cam_pos + epsilon*ray, ray, depth, steps);

//	if(depth >= view_radius) // background
//	{
//		fColor = vec3(1);
//		return;
//	}

	//AO

	float ao = 1 - float(steps) / (steps + 100); // reinhard
	//ao = pow(ao, 2); // gamma

	const float contrast_offset = 0.4;
	const float contrast_mid_level = 0.65;
	ao = contrast(ao, contrast_offset, contrast_mid_level);

	//Sun

	vec3 light_dir = cam_basis*normalize(vec3(-0.5, -1, 1));

	vec3 refl_intersection_point = ray_marching(intersection_point - light_dir*epsilon*2, -light_dir, depth, steps);

	vec3 normal = estimate_normal(intersection_point, epsilon*0.5);

	float shadow = 0;
	if(depth >= view_radius) shadow = 1;

	float luminance = clamp(dot(normal, -light_dir), 0, 1)*(1-ambient_light)*shadow + ambient_light;
	luminance = pow(luminance, 1/2.2); // gamma correction

	fColor = vec3(luminance*ao*contrast(normal*0.5 + 0.5, -0.2, 1));
}
