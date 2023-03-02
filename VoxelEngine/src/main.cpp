/*
* - try bigger world
* - normals
*/
#pragma comment(lib, "glfw3_mt")
#pragma comment(lib, "glew32s")
#pragma comment(lib, "opengl32")

#define GLEW_STATIC
#include <GL/glew.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <chrono>
#include <Windows.h>
#include "vendor/shader.h"

#define WND_WIDTH 800
#define WND_HEIGHT 600

typedef unsigned char Byte;
struct Voxel {
	int solid;
};

void PrintLimitations();

namespace Time {
	std::chrono::high_resolution_clock::time_point frame_begin, frame_end;
	float delta_time = 0;
	void BeginFrame() {
		frame_begin = std::chrono::high_resolution_clock::now();
	}
	void EndFrame() {
		frame_end = std::chrono::high_resolution_clock::now();
		delta_time = std::chrono::duration<float, std::milli>(frame_end - frame_begin).count() / 5.0;
	}
};

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	GLFWwindow* window = glfwCreateWindow(WND_WIDTH, WND_HEIGHT, "Voxel raytracer", 0, 0);
	glfwMakeContextCurrent(window);
	glewInit();
	
	Shader shader("res/shaders/vertex.glsl", "res/shaders/fragment.glsl");
	shader.Use();
	shader.SetInt("tex", 0);
	
	PrintLimitations();
	ComputeShader compute_shader("res/shaders/compute.glsl");

	float vertices[] = {
		 // positions         // texture coords
		 1.0f,  1.0f, 0.0f,   1.0f, 1.0f,
		 1.0f, -1.0f, 0.0f,   1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,   0.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,   0.0f, 1.0f
	};
	char indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};
	
	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	
	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	
	// Create texture for opengl operation
	// -----------------------------------
	unsigned int texture;

	glGenTextures(1, &texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, WND_WIDTH, WND_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);

	glBindImageTexture(0, texture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	std::vector<Voxel> data = { 
		{0}, {0}, {0}, {0}, {0}, {0},
		{0}, {0}, {0}, {0}, {0}, {0},
		{0}, {0}, {0}, {0}, {0}, {0},
		{0}, {0}, {0}, {0}, {0}, {0},
		{0}, {0}, {0}, {0}, {0}, {0},
		{0}, {0}, {0}, {0}, {0}, {0},
		
		{0}, {0}, {0}, {0}, {0}, {0},
		{0}, {0}, {0}, {0}, {0}, {0},
		{0}, {1}, {1}, {1}, {1}, {0},		
		{0}, {0}, {0}, {0}, {0}, {0},
		{0}, {0}, {0}, {0}, {0}, {0},
		{0}, {0}, {0}, {0}, {0}, {0},
		
		{0}, {0}, {0}, {0}, {0}, {0},
		{0}, {0}, {0}, {0}, {0}, {0},
		{0}, {0}, {0}, {0}, {0}, {0},
		{0}, {0}, {0}, {0}, {0}, {0},
		{0}, {0}, {0}, {0}, {0}, {0},
		{0}, {0}, {0}, {0}, {0}, {0},

		{0}, {0}, {0}, {0}, {0}, {0},
		{0}, {0}, {0}, {0}, {0}, {0},
		{0}, {0}, {0}, {0}, {0}, {0},
		{0}, {0}, {0}, {0}, {0}, {0},
		{0}, {0}, {0}, {0}, {0}, {0},
		{0}, {0}, {0}, {0}, {0}, {0},

		{0}, {0}, {0}, {0}, {0}, {0},
		{0}, {0}, {0}, {0}, {0}, {0},
		{0}, {1}, {1}, {1}, {1}, {0},
		{0}, {0}, {0}, {0}, {0}, {0},
		{0}, {0}, {0}, {0}, {0}, {0},
		{0}, {0}, {0}, {0}, {0}, {0},

		{0}, {0}, {0}, {0}, {0}, {0},
		{0}, {0}, {0}, {0}, {0}, {0},
		{0}, {0}, {0}, {0}, {0}, {0},
		{0}, {0}, {0}, {0}, {0}, {0},
		{0}, {0}, {0}, {0}, {0}, {0},
		{0}, {0}, {0}, {0}, {0}, {0}
	};
	GLuint SSBO;
	glGenBuffers(1, &SSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, data.size() * sizeof(Voxel), &data[0], GL_DYNAMIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	compute_shader.Use();
	int ssbo_binding = 1;
	int block_index = glGetProgramResourceIndex(compute_shader.ID, GL_SHADER_STORAGE_BLOCK, "bufferData");
	glShaderStorageBlockBinding(compute_shader.ID, block_index, ssbo_binding);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, ssbo_binding, SSBO);

	float theta = 0.0f, alpha = 0.0f;
	glm::vec3 cam_pos(0, 1, 0);
	while (!glfwWindowShouldClose(window)) {
		Time::BeginFrame();

		if (GetAsyncKeyState(VK_LEFT)) theta -= 0.01f * Time::delta_time;
		else if (GetAsyncKeyState(VK_RIGHT)) theta += 0.01f * Time::delta_time;
		if (GetAsyncKeyState(VK_UP)) alpha += 0.01f * Time::delta_time;
		else if (GetAsyncKeyState(VK_DOWN)) alpha -= 0.01f * Time::delta_time;
		if (GetAsyncKeyState('W')) cam_pos.z -= 0.01f * Time::delta_time;
		else if (GetAsyncKeyState('S')) cam_pos.z += 0.01f * Time::delta_time;
		if (GetAsyncKeyState('A')) cam_pos.x -= 0.01f * Time::delta_time;
		else if (GetAsyncKeyState('D')) cam_pos.x += 0.01f * Time::delta_time;

		//std::cout << cam_pos.z << "\n";

		compute_shader.Use();
		compute_shader.SetFloat("theta", theta);
		compute_shader.SetVec3("cam_pos", cam_pos);
		
		glDispatchCompute(800, 600, 1);
		
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		shader.Use();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, 0);
		glfwSwapBuffers(window);
		glfwPollEvents();

		Time::EndFrame();
	}
	return 0;
}

void PrintLimitations() {
	int max_compute_work_group_count[3];
	int max_compute_work_group_size[3];
	int max_compute_work_group_invocations;

	for (int idx = 0; idx < 3; idx++) {
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, idx, &max_compute_work_group_count[idx]);
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, idx, &max_compute_work_group_size[idx]);
	}
	glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &max_compute_work_group_invocations);

	std::cout << "OpenGL Limitations: " << std::endl;
	std::cout << "maximum number of work groups in X dimension " << max_compute_work_group_count[0] << std::endl;
	std::cout << "maximum number of work groups in Y dimension " << max_compute_work_group_count[1] << std::endl;
	std::cout << "maximum number of work groups in Z dimension " << max_compute_work_group_count[2] << std::endl;

	std::cout << "maximum size of a work group in X dimension " << max_compute_work_group_size[0] << std::endl;
	std::cout << "maximum size of a work group in Y dimension " << max_compute_work_group_size[1] << std::endl;
	std::cout << "maximum size of a work group in Z dimension " << max_compute_work_group_size[2] << std::endl;

	std::cout << "Number of invocations in a single local work group that may be dispatched to a compute shader " << max_compute_work_group_invocations << std::endl;
}