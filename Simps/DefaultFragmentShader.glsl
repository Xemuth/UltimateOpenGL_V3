#ifndef _UltimateOpenGL_V3_FRAGMENT_SHADER
#define _UltimateOpenGL_V3_FRAGMENT_SHADER
SHADER(330 core,
	@STRUCT_MATERIAL_TEXTURE\n
	@STRUCT_MATERIAL_COLOR\n
	@STRUCT_POINT_LIGHT\n
	@STRUCT_DIR_LIGHT\n
	@STRUCT_SPOT_LIGHT\n

	/*
		This Shader is used to Generate automatique Shader using UltimateOpenGL.
		\n It is not elegant but necessary because Annotation(@) will be at reading transform to
		Commentaire bracket(//) Allowing the compilation of annotation not used by GLSL
		compiler.
	*/

	
	@OUT_FRAG_COLOR\n
	
	@IN_VECTOR_TEXTURE_COORD\n

	
	in vec3 Normal;\n
 	in vec3 FragPos;\n 

	uniform vec3 viewPos;\n

	@UNIFORM_MATERIAL_TEXTURE_NAME\n
	@UNIFORM_MATERIAL_COLOR_NAME\n

	@POINT_LIGHTS_ARRAY\n


	@DIR_LIGHTS_ARRAY\n


	@SPOT_LIGHTS_ARRAY\n

	

	@IMPORT_POINT_LIGHT_TEXTURE_PROTOTYPE\n
	@IMPORT_DIR_LIGHT_TEXTURE_PROTOTYPE\n
	@IMPORT_SPOT_LIGHT_TEXTURE_PROTOTYPE\n
	@IMPORT_POINT_LIGHT_COLOR_PROTOTYPE\n
	@IMPORT_DIR_LIGHT_COLOR_PROTOTYPE\n
	@IMPORT_SPOT_LIGHT_COLOR_PROTOTYPE\n


	void main()\n
	{\n
		
		@LIGHT_STARTING_DATA\n
	    // properties
	  /*  vec3 norm = normalize(Normal);
	    vec3 viewDir = normalize(viewPos - FragPos);
		vec3 result;*/

		@DIRECTIONAL_LIGHTS\n
	    // phase 1: directional lighting
		/*for(int i = 0; i < NR_DIR_LIGHTS; i++)
	    	 result += CalcDirLight(pyramide,dirLights[i], norm, viewDir);*/

		@POINT_LIGHTS\n
	    // phase 2: point lights
	    /*for(int i = 0; i < NR_POINT_LIGHTS; i++)
	        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);    */

		@SPOT_LIGHTS\n
		// phase 3: Spot lights
		/*for(int i = 0; i < NR_SPOT_LIGHTS; i++)
	    	result += CalcSpotLight(pyramide,spotLights[i], norm, FragPos, viewDir);  */

		@FRAG_COLOR_CALCULATION\n 
	}\n
	

	@POINT_LIGHT_TEXTURE_FUNCTION\n
	@DIR_LIGHT_TEXTURE_FUNCTION\n
	@SPOT_LIGHT_TEXTURE_FUNCTION\n
	
	@POINT_LIGHT_COLOR_FUNCTION\n
	@DIR_LIGHT_COLOR_FUNCTION\n
	@SPOT_LIGHT_COLOR_FUNCTION\n

//	LIGHT_POINT_TEXTURE_FUNCTION()
//	LIGHT_DIR_TEXTURE_FUNCTION()
//	LIGHT_SPOT_TEXTURE_FUNCTION()
//	LIGHT_POINT_COLOR_FUNCTION()
//	LIGHT_DIR_COLOR_FUNCTION()
//	LIGHT_SPOT_COLOR_FUNCTION()
)

#endif