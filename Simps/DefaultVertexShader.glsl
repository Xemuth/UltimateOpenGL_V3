#ifndef _UltimateOpenGL_V3_VERTEX_SHADER
#define _UltimateOpenGL_V3_VERTEX_SHADER
SHADER(330 core,
	layout (location = 0) in vec3 aPos;\n
	layout (location = 1) in vec3 aNormal;\n
	@LAYOUT_TEXTURE_COORD\n
	/*
		This Shader is used to Generate automatique Shader using UltimateOpenGL.
		\n It is not elegant but necessary because Annotation(@) will be at reading transform to
		Commentaire bracket(//) Allowing the compilation of annotation not used by GLSL
		compiler.
	*/
	
	@OUT_VECTOR_TEXCOORDS\n
	out vec3 Normal;\n
	out vec3 FragPos;\n	

	//uniform mat4 transform; Seems useless to the moment
	uniform mat4 model;\n
	uniform mat4 view;\n
	uniform mat4 projection;\n
	
	void main()\n
	{\n
		FragPos = vec3(model * vec4(aPos, 1.0));\n
		Normal = mat3(transpose(inverse(model))) * aNormal;\n
		gl_Position = projection * view * model * vec4(aPos, 1.0f);\n
		@TEXTCOORDS_CALCULATION\n
	}\n
)
#endif