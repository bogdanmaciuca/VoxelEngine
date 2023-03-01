#version 430 core
out vec4 FragColor;

in vec2 TexCoord;

// texture samplers
uniform sampler2D tex;

void main() {
	FragColor = vec4(texture(tex, TexCoord).rgb, 1);
	//FragColor = vec4(1, 0, 0, 1);
}
