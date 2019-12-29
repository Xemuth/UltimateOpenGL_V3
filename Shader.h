#ifndef _UltimateOpenGl_V3_Shader_h_
#define _UltimateOpenGl_V3_Shader_h_
#include "Definition.h"

Upp::String& IncludeShader(Upp::String& shader);
//*****************Material***********//
struct Material {
	float shininess=32.0f;
	float mix = 1.0f; //Used to mix texture or color if needed , Determine the % of this color or texture in the result can't be higher than 1 and lower than 0
	
	Material& SetMix(float _mix){if(_mix > 1) mix =1; else if(_mix <0) mix = 0; else mix = _mix; return *this;}
	Material& SetShininess(float _shininess){shininess = _shininess; return *this;}
};
struct MaterialTexture: public Material,Upp::Moveable<MaterialTexture> {
	MaterialTexture(){}
	MaterialTexture(float _shininess, bool _useSpecular,int _diffuse,int _specular){
		shininess = _shininess;
		useSpecular = (_useSpecular)?1:-1;
		diffuse = _diffuse;
		specular = _specular;
	}
	MaterialTexture& operator=(const MaterialTexture& _materialTexture){
		diffuse = _materialTexture.diffuse;
		shininess = _materialTexture.shininess;
		specular = _materialTexture.specular;
		return *this;
	}
	MaterialTexture& UseSpecular(bool _useSpecular){useSpecular = (_useSpecular)?1:-1; return *this;}
	MaterialTexture& SetDiffuse(int _diffuse){diffuse = _diffuse; return *this;}
	MaterialTexture& SetSpecular(int _specular){if(useSpecular== -1)useSpecular =1; specular = _specular; return *this;}
	MaterialTexture& SetShininess(float _shininess){shininess = _shininess; return *this;}
	MaterialTexture& UseNormal(bool _useNormal){useNormal = (_useNormal)?1:-1; return *this;}
	MaterialTexture& SetNormal(int _Normal){if(useNormal== -1)useNormal =1; normal = _Normal; return *this;}
	int diffuse = 0.64f;//You can prevent this automatique value change // Iñaki: Is this the intent?
	
	int useSpecular=-1; //If this one is set to One then we use Shininess, UltimateOpenGL will set it to one before sending material to the shader if Specular is different of -1
	int specular = -1; // Id of speculare used for normal Mapping
	
	int useNormal=-1; //If this use normal mapping it have to be set to one 
	int normal =-1; // Id of texture used for normal Mapping
	
};
struct MaterialColor: public Material,Upp::Moveable<MaterialColor> {
	MaterialColor(){}
	MaterialColor(float _shininess, glm::vec3 _ambient,glm::vec3 _diffuse, glm::vec3 _specular){
		ambient = _ambient;
		diffuse = _diffuse;
		shininess = _shininess;
		specular = _specular;
	}
	MaterialColor& operator=(const MaterialColor& _materialColor){
		ambient = _materialColor.ambient;
		diffuse = _materialColor.diffuse;
		shininess = _materialColor.shininess;
		specular = _materialColor.specular;
		return *this;
	}
	MaterialColor& SetAmbient(glm::vec3 _ambient){ambient = _ambient; return *this;}
	MaterialColor& SetDiffuse(glm::vec3 _diffuse){diffuse = _diffuse; return *this;}
	MaterialColor& SetSpecular(glm::vec3 _specular){specular = _specular; return *this;}
	MaterialColor& SetShininess(float _shininess){shininess = _shininess; return *this;}
    glm::vec3 ambient=glm::vec3(1.0f, 0.5f, 0.31f);
    glm::vec3 diffuse=glm::vec3( 1.0f, 0.5f, 0.31f);
    glm::vec3 specular=glm::vec3(0.5f, 0.5f, 0.5f);
    
};
//****************Light************//
struct Light{
	glm::vec3 ambient=glm::vec3( 0.05f, 0.05f, 0.05f);
    glm::vec3 diffuse=glm::vec3(0.8f, 0.8f, 0.8f);
    glm::vec3 specular=glm::vec3(1.0f,1.0f,1.0f);
    
	Light& SetAmbient(glm::vec3 _ambient){ambient = _ambient;return *this;}
	Light& SetDiffuse(glm::vec3 _diffuse){diffuse = _diffuse;return *this;}
	Light& SetSpecular(glm::vec3 _specular){specular = _specular;return *this;}
};
struct DirLight : public Light ,Upp::Moveable<DirLight> {
	DirLight(){}
	DirLight(glm::vec3 _direction, glm::vec3 _ambient, glm::vec3 _diffuse , glm::vec3 _specular){
		direction = _direction;
		ambient = _ambient;
		diffuse = _diffuse;
		specular = _specular;
	}
	DirLight& operator=(const DirLight& _dirLight){
		direction = _dirLight.direction;
		ambient = _dirLight.ambient;
		diffuse = _dirLight.diffuse;
		specular = _dirLight.specular;
		return *this;
	}
	DirLight& SetDirection(glm::vec3 _direction){direction = _direction;return *this;}
	DirLight& SetAmbient(glm::vec3 _ambient){ambient = _ambient;return *this;}
	DirLight& SetDiffuse(glm::vec3 _diffuse){diffuse = _diffuse;return *this;}
	DirLight& SetSpecular(glm::vec3 _specular){specular = _specular;return *this;}
	
    glm::vec3 direction=glm::vec3(0.0f);
};
struct PointLight : public Light,Upp::Moveable<PointLight> {
	PointLight(){}
	PointLight(glm::vec3 _position, float _constant,float _linear, float _quadratic,glm::vec3 _ambient, glm::vec3 _diffuse , glm::vec3 _specular){
		ambient = _ambient;
		diffuse = _diffuse;
		position = _position;
		specular = _specular;
	    constant = _constant;
		linear = _linear;
		quadratic = _quadratic;
	}
	PointLight& operator=(const PointLight& _pointLight){
		ambient = _pointLight.ambient;
		diffuse = _pointLight.diffuse;
		position = _pointLight.position;
		specular = _pointLight.specular;
	    constant = _pointLight.constant;
		linear = _pointLight.linear;
		quadratic = _pointLight.quadratic;
		return *this;
	}
	PointLight& SetPosition(glm::vec3 _position){ position = _position;return *this;}
	PointLight& SetConstant(float _constant){constant = _constant;return *this;}
	PointLight& SetLinear(float _linear){linear = _linear;return *this;}
	PointLight& SetQuadratic(float _quadratic){quadratic = _quadratic;return *this;}
	PointLight& SetAmbient(glm::vec3 _ambient){ambient = _ambient;return *this;}
	PointLight& SetDiffuse(glm::vec3 _diffuse){diffuse = _diffuse;return *this;}
	PointLight& SetSpecular(glm::vec3 _specular){specular = _specular;return *this;}
    glm::vec3 position=glm::vec3(0.0f,0.0f,0.0f);
    float constant=1.0f;
    float linear=0.09f;
    float quadratic=0.032f;
};
struct SpotLight : public Light,Upp::Moveable<SpotLight> {
	SpotLight(){}
	SpotLight(glm::vec3 _position,glm::vec3 _direction, float _cutOff,float _outerCutOff, float _constant,float _linear,float _quadratic, glm::vec3 _ambient, glm::vec3 _diffuse , glm::vec3 _specular){
		ambient = _ambient;
		diffuse = _diffuse;
		position = _position;
		direction = _direction;
		cutOff = _cutOff;
		outerCutOff = _outerCutOff;
		specular = _specular;
	    constant = _constant;
		linear = _linear;
		quadratic = _quadratic;
	}
	SpotLight& operator=(const SpotLight& _spotLight){
		ambient = _spotLight.ambient;
		diffuse = _spotLight.diffuse;
		position = _spotLight.position;
		direction = _spotLight.direction;
		cutOff = _spotLight.cutOff;
		outerCutOff = _spotLight.outerCutOff;
		specular = _spotLight.specular;
	    constant = _spotLight.constant;
		linear = _spotLight.linear;
		quadratic = _spotLight.quadratic;
		return *this;
	}
	SpotLight& SetAmbient(glm::vec3 _ambient){ambient = _ambient;return *this;}
	SpotLight& SetDiffuse(glm::vec3 _diffuse){diffuse = _diffuse;return *this;}
	SpotLight& SetSpecular(glm::vec3 _specular){specular = _specular;return *this;}
	SpotLight& SetPosition(glm::vec3 _position){position = _position;return *this;}
	SpotLight& SetDirection(glm::vec3 _direction){direction = _direction;return *this;}
	SpotLight& SetConstant(float _constant){constant = _constant;return *this;}
	SpotLight& SetLinear(float _linear){linear = _linear;return *this;}
	SpotLight& SetQuadratic(float _quadratic){quadratic = _quadratic;return *this;}
	SpotLight& SetCutOff(float _cutOff){cutOff = _cutOff;return *this;}
	SpotLight& SetOuterCutOff(float _outerCutOff){outerCutOff = _outerCutOff;return *this;}
	
    glm::vec3 position=glm::vec3(0.0f);
    glm::vec3 direction=glm::vec3(0.0f,-1.0f,0.0f);
    float cutOff=glm::cos(glm::radians(12.5f));
    float outerCutOff=glm::cos(glm::radians(15.0f));
    float constant=1.0f;
    float linear=0.09f;
    float quadratic=0.032f;
};
//*****************Material***********//

class Shader {
private:
	class shader_Data : Upp::Moveable<shader_Data> {
		public:
			shader_Data();
			shader_Data(Upp::String _name,ShaderType _type,Upp::String _shader);
			shader_Data& operator=(const shader_Data& data);
			
			Upp::String name;
			ShaderType type;
			Upp::String shader;
	};
	
// utility function for checking shader compilation/linking errors.
	Upp::VectorMap<Upp::String,shader_Data> shaders;

	unsigned int ID;
	bool compiled = false;
	bool autoGenerated = false;
	bool CheckCompileErrors(unsigned int shader, ShaderType type);
public:
    Shader();
    ~Shader();
    Shader(Upp::String _name,ShaderType _type,Upp::String _shader);
    Shader(const Shader& _shader);
    Shader& operator=(const Shader& data);
	
    Shader& AddShader(Upp::String _name,ShaderType _type,Upp::String _shader);
    const Upp::VectorMap<Upp::String,Shader::shader_Data>& GetShaders() const;
    
    bool CompileShader(bool autoGenerated =false);
    bool IsCompiled() const;
    bool IsAutoGenerated() const;
    unsigned int GetId() const;
    
    void SetBool(Upp::String name, bool value) const;
    void SetInt(Upp::String name, int value) const;
    void SetFloat(Upp::String name, float value) const;
    
	void SetVec2(Upp::String name, const glm::vec2 &value) const;
	void SetVec2(Upp::String name, float x, float y) const;
	void SetVec3(Upp::String name, const glm::vec3 &value) const;
	void SetVec3(Upp::String name, float x, float y, float z) const;
	void SetVec4(Upp::String name, const glm::vec4 &value) const;
	void SetVec4(Upp::String name, float x, float y, float z, float w);
	void SetMat2(Upp::String name, const glm::mat2 &mat) const;
	void SetMat3(Upp::String name, const glm::mat3 &mat) const;
	void SetMat4(Upp::String name, const glm::mat4 &mat) const;
	
	void SetPointLight(Upp::String name, const PointLight &pointLight,int num =-1) const;
	void SetDirLight(Upp::String name, const DirLight &dirLight,int num =-1) const;
	void SetSpotLight(Upp::String name, const SpotLight &spotLight,int num =-1) const;
	
	void SetMaterialColor(Upp::String name, const MaterialColor &materialColor) const;
	void SetMaterialTexture(Upp::String name, MaterialTexture &materialTexture,bool CheckSpecular=true) const;
	
	void Use();
	void Unbind();
};
#endif
