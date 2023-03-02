#version 430 core

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
layout(rgba32f, binding = 0) uniform image2D imgOutput;

struct Voxel {
	int type;
};
layout(std430, binding = 1 ) readonly buffer bufferData
{
    Voxel data[];
};

uniform float theta;
uniform float alpha;
uniform vec3 cam_pos;

int wnd_width = 800, wnd_height = 600;
const int world_width = 6;

struct Ray {
	vec3 origin;
	vec3 direction;
};


mat3 rotationMatrix(vec3 axis, float angle) {
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;
    
    return mat3(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,
				oc * axis.z * axis.x - axis.y * s, oc * axis.y * axis.z + axis.x * s, oc * axis.z * axis.z + c);
}

Ray GetRay() {
	float viewport_height = 2.0;
    float viewport_width = 4.0/3.0 * viewport_height;
    float focal_length = 1.0;

	vec3 origin = cam_pos;
	vec3 horizontal = vec3(viewport_width, 0, 0);
    vec3 vertical = vec3(0, viewport_height, 0);
    vec3 lower_left_corner = origin - horizontal/2 - vertical/2 - vec3(0, 0, focal_length);
	
	ivec2 pixel_pos = ivec2(gl_GlobalInvocationID.xy);
	float u = float(pixel_pos.x) / (wnd_width-1);
	float v = float(pixel_pos.y) / (wnd_height-1);
	
	Ray ray;
	ray.origin = origin;
	ray.direction = rotationMatrix(vec3(0, 1, 0), theta.x) * normalize(lower_left_corner + u*horizontal + v*vertical - origin);
	return ray;
}

bool TraverseVoxels(vec3 origin, vec3 ray) {
	vec3 ray_start = origin;
	vec3 ray_dir = ray/ sqrt(ray.x * ray.x + ray.y * ray.y + ray.z * ray.z);

	vec3 step_size = { abs(1.0f / ray_dir.x), abs(1.0f / ray_dir.y), abs(1.0f / ray_dir.z)};
	ivec3 tile_index = ivec3(floor(ray_start.x), floor(ray_start.y), floor(ray_start.z));
	vec3 ray_length;
	ivec3 step;

	// Establish Starting Conditions
	if (ray_dir.x < 0) {
		step.x = -1;
		ray_length.x = (ray_start.x - float(tile_index.x)) * step_size.x;
	}
	else {
		step.x = 1;
		ray_length.x = (float(tile_index.x + 1) - ray_start.x) * step_size.x;
	}

	if (ray_dir.y < 0) {
		step.y = -1;
		ray_length.y = (ray_start.y - float(tile_index.y)) * step_size.y;
	}
	else {
		step.y = 1;
		ray_length.y = (float(tile_index.y + 1) - ray_start.y) * step_size.y;
	}

	if (ray_dir.z < 0) {
		step.z = -1;
		ray_length.z= (ray_start.z - float(tile_index.z)) * step_size.z;
	}
	else {
		step.z = 1;
		ray_length.z = (float(tile_index.z + 1) - ray_start.z) * step_size.z;
	}

	// Perform "Walk" until collision or range check
	bool bTileFound = false;
	float fMaxDistance = 100.0f;
	float fDistance = 0.0f;
	while (!bTileFound) {
		// Walk along shortest path
		if (ray_length.x < ray_length.y) {
			if (ray_length.x < ray_length.z) {
				tile_index.x += step.x;
				ray_length.x += step_size.x;
			}
			else {
				tile_index.z += step.z;
				ray_length.z += step_size.z;
			}
		}
		else {
			if (ray_length.y < ray_length.z) {
				tile_index.y += step.y;
				ray_length.y += step_size.y;
			}
			else {
				tile_index.z += step.z;
				ray_length.z += step_size.z;
			}
		}

		if (tile_index.x < 0 || tile_index.x >= world_width || tile_index.y < 0 || tile_index.y >= world_width || tile_index.z < 0 || tile_index.z >= world_width )
			return false;
		

		if (data[tile_index.z * world_width * world_width + tile_index.y * world_width + tile_index.x].type == 1)
			return true;
	}

	//// Calculate intersection location
	//sf::Vector2fvIntersection;
	//if (bTileFound)
	//{
	//	vIntersection = ray_start + ray_dir * fDistance;
	//}
	return bTileFound;
}


void main() {
	Ray ray = GetRay();
	//float fov = radians(80);
	//float x =  (2*(gl_GlobalInvocationID.x + 0.5)/float(wnd_width)  - 1)*tan(fov/2.)*wnd_width/float(wnd_height);
    //float y = (2*(gl_GlobalInvocationID.y + 0.5)/float(wnd_height) - 1)*tan(fov/2.);
	//
	////vec3 dir = normalize(vec3(x, y, -1));
	//vec3 dir = rotationMatrix(vec3(0, 1, 0), theta.x) * normalize(vec3(x, y, -1));
	//vec3 pos = vec3(2, 2, 3);
	vec4 color = vec4(0);
	
	if (TraverseVoxels(ray.origin, ray.direction)) color = vec4(1, 0, 0, 1);

	//if (cast_ray(ray.origin, ray.direction)) {
	//	color = vec3(1, 1, 0);
	//}
	//else { // does a nice sky effect
	//	vec3 unit_direction = normalize(ray.direction);
	//	float t = 0.5*(unit_direction.y + 1.0);
	//	color = vec4((1.0-t)*vec3(1.0, 1.0, 1.0) + t*vec3(0.5, 0.7, 1.0), 1);
	//}

	imageStore(imgOutput, ivec2(gl_GlobalInvocationID.xy), color);
}
