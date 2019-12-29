#include "UltimateOpenGL.h"

#include "Simps/MaterialTexture.glsl"
#include "Simps/MaterialColor.glsl"
#include "Simps/PointLight.glsl"
#include "Simps/SpotLight.glsl"
#include "Simps/DirLight.glsl"



Upp::String& IncludeShader(Upp::String& shader){
	shader.Replace("MATERIAL_TEXTURE_STRUCT()",MATERIAL_TEXTURE_STRUCT());
	shader.Replace("MATERIAL_COLOR_STRUCT()",MATERIAL_COLOR_STRUCT());
	
	shader.Replace("LIGHT_POINT_STRUCT()",LIGHT_POINT_STRUCT());
	shader.Replace("LIGHT_POINT_TEXTURE_PROTOTYPE()",LIGHT_POINT_TEXTURE_PROTOTYPE());
	shader.Replace("LIGHT_POINT_TEXTURE_FUNCTION()",LIGHT_POINT_TEXTURE_FUNCTION());
	shader.Replace("LIGHT_POINT_COLOR_PROTOTYPE()",LIGHT_POINT_COLOR_PROTOTYPE());
	shader.Replace("LIGHT_POINT_COLOR_FUNCTION()",LIGHT_POINT_COLOR_FUNCTION());
	
	shader.Replace("LIGHT_SPOT_STRUCT()",LIGHT_SPOT_STRUCT());
	shader.Replace("LIGHT_SPOT_TEXTURE_PROTOTYPE()",LIGHT_SPOT_TEXTURE_PROTOTYPE());
	shader.Replace("LIGHT_SPOT_TEXTURE_FUNCTION()",LIGHT_SPOT_TEXTURE_FUNCTION());
	shader.Replace("LIGHT_SPOT_COLOR_PROTOTYPE()",LIGHT_SPOT_COLOR_PROTOTYPE());
	shader.Replace("LIGHT_SPOT_COLOR_FUNCTION()",LIGHT_SPOT_COLOR_FUNCTION());
	
	shader.Replace("LIGHT_DIR_STRUCT()",LIGHT_DIR_STRUCT());
	shader.Replace("LIGHT_DIR_TEXTURE_PROTOTYPE()",LIGHT_DIR_TEXTURE_PROTOTYPE());
	shader.Replace("LIGHT_DIR_TEXTURE_FUNCTION()",LIGHT_DIR_TEXTURE_FUNCTION());
	shader.Replace("LIGHT_DIR_COLOR_PROTOTYPE()",LIGHT_DIR_COLOR_PROTOTYPE());
	shader.Replace("LIGHT_DIR_COLOR_FUNCTION()",LIGHT_DIR_COLOR_FUNCTION());
	shader = Replace(shader,Upp::Vector<Upp::String>{"\""},Upp::Vector<Upp::String>{""});
	return shader;
}

UltimateOpenGL_Context::UltimateOpenGL_Context(){
	ScreenSize.cx = 600;  //Default Size
	ScreenSize.cy = 600;
	StartTimer();
}
UltimateOpenGL_Context::~UltimateOpenGL_Context(){
	for(Texture& t : textures){
		t.Remove();
	}
}


//****************Static part **************************
Upp::Vector<int> UltimateOpenGL_Context::TransformGlmColorToRGB(glm::vec3 FloatColor){
	return Upp::Vector<int>{ ColorUniformisation((int)(FloatColor.x*255)),ColorUniformisation((int)(FloatColor.y*255)), ColorUniformisation((int)(FloatColor.z*255)) };
}
Upp::Vector<int> UltimateOpenGL_Context::TransformFloatColorToRGB(float RedFloat,float GreenFloat,float BlueFloat){
	return Upp::Vector<int>{ ColorUniformisation((int)(RedFloat*255)),ColorUniformisation((int)(GreenFloat*255)), ColorUniformisation((int)(BlueFloat*255))};
}
glm::vec3 UltimateOpenGL_Context::TransformRGBToFloatColor(int Red,int Green,int Blue){
	return  glm::vec3( ((float)ColorUniformisation(Red))/255.0f ,((float)ColorUniformisation(Green))/255.0f,((float)ColorUniformisation(Blue))/255.0f);
}
glm::vec3 UltimateOpenGL_Context::TransformVectorToFloatColor(Upp::Vector<int> rgb){
	if(rgb.GetCount() < 3){
		LOG("Erreur TransformRGBToFloatColor(Vector<int>) : Vector incorrect !\n");
		return  glm::vec3(1.0);
	}
	return glm::vec3( ((float)ColorUniformisation(rgb[0]))/255.0f ,((float)ColorUniformisation(rgb[1]))/255.0f,((float)ColorUniformisation(rgb[2]))/255.0f);
}
int UltimateOpenGL_Context::ColorUniformisation(int ColorRgb){ //Fonction très utile et très complexe
	if(ColorRgb> 255) ColorRgb=255;
	if(ColorRgb< 0) ColorRgb=0;
	return ColorRgb;
}
float UltimateOpenGL_Context::ColorUniformisation(float ColorFloat){
	if(ColorFloat> 1.0f) ColorFloat=1.0f;
	if(ColorFloat< 0.0f) ColorFloat=0.0f;
	return ColorFloat;
}

Upp::String UltimateOpenGL_Context::TransformFilePath(const Upp::String& FilePath){
	Upp::String FilePathBuffer =FilePath;

	FilePathBuffer = Replace(FilePathBuffer,Upp::Vector<Upp::String>{"\\"},Upp::Vector<Upp::String>{"/"});

	return FilePathBuffer;
}

void GLAPIENTRY MessageCallback( GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar* message, void* userParam ){
	LOG("GL CALLBACK: " + Upp::String( (type == GL_DEBUG_TYPE_ERROR) ? "** GL ERROR **" : "" ) +" type = 0x" + type +", severity = 0x"+ severity +", message = " + message);
 // fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ), type, severity, message );
}

void UltimateOpenGL_Context::Trace(bool b){
	if(b){
		glEnable( GL_DEBUG_OUTPUT );
		glDebugMessageCallback(static_cast<GLDEBUGPROC>(&MessageCallback), 0 );		
	}else{
		glDisable( GL_DEBUG_OUTPUT );
	}
}

//time management
void UltimateOpenGL_Context::StartTimer(){
	start= std::chrono::high_resolution_clock::now();
}
double UltimateOpenGL_Context::GetTime(){
	end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> diff = end-start;
	Timer =diff.count();
	return Timer;
}
int UltimateOpenGL_Context::GetFPS(){
	return frameCount;
}
double UltimateOpenGL_Context::GetEllapsedTime(){
	return Timer;
}
double UltimateOpenGL_Context::GetDeltaTime(){
	return DeltaTime;
}


//Screen size
void UltimateOpenGL_Context::SetScreenSize(float ScreenHeight,float ScreenWidth){
	ScreenSize.cx = ScreenWidth;
	ScreenSize.cy = ScreenHeight;
}
Upp::Sizef UltimateOpenGL_Context::GetScreenSize(){
	return ScreenSize;
}

//scene
Scene& UltimateOpenGL_Context::AddScene(const Upp::String& name){
	if(AllScenes.Find(name) ==-1){
		Scene& s= AllScenes.Create<Scene>(name);
		s.SetName(name);
		s.SetContext(this);
		if(ActiveScene == nullptr) ActiveScene = &s;
		return s;
	}else{
		return AllScenes.Get(name);
	}
}
Scene& UltimateOpenGL_Context::GetActiveScene(){
	if(ActiveScene == nullptr) throw UGLException(4,"Scene& UltimateOpenGL_Context::GetActiveScene() => No scene have been binded",1);
		return *ActiveScene;
}
Scene& UltimateOpenGL_Context::GetScene(const Upp::String& name){
	if(AllScenes.Find(name) !=-1){
		return AllScenes.Get(name);
	}
	throw UGLException(8,"Scene& UltimateOpenGL_Context::GetScene(const Upp::String& name) => Scene named "+ name +" Do not exists",1);
}
Upp::ArrayMap<Upp::String,Scene>& UltimateOpenGL_Context::GetAllScene(){
	return AllScenes;
}
bool UltimateOpenGL_Context::SetActiveScene(const Upp::String& name){
	if(AllScenes.Find(name) !=-1){
		ActiveScene = &AllScenes.Get(name);
		return true;
	}
	return false;
}
bool UltimateOpenGL_Context::RemoveScene(const Upp::String& nameOfScene){
	if(AllScenes.Find(nameOfScene) !=-1){
		if(ActiveScene == &AllScenes.Get(nameOfScene)) ActiveScene = nullptr;
		AllScenes.Remove(AllScenes.Find(nameOfScene));
		return true;
	}
	return false;
}

//Textures
Texture UltimateOpenGL_Context::AddTexture(const Upp::String& TextureName,const Upp::String& textureFilePath,TextureType _type,bool loadDefault, bool flipLoad){ //Add and load Texture
	if(textures.Find(TextureName) == -1){
		Texture& texture =textures.Add(TextureName);
		texture.SetPath(textureFilePath);
		texture.SetName(TextureName);
		if(!texture.Load(TextureCompteur,loadDefault,flipLoad)){
			textures.RemoveKey(TextureName);
			LOG("Error : AddTextures(String,String,bool,bool) Loading error !\n");
			return Texture();
		}else{
			LOG("Info : Texture " + TextureName +"  number " + Upp::AsString( TextureCompteur) +" Loaded with success with ID of " + Upp::AsString(texture.GetId()) +"!\n");
			TextureCompteur++;
			return texture;
		}
		return Texture();
	}
	return Texture();
}
Texture UltimateOpenGL_Context::GetTexture(const Upp::String& TextureName){
	if(textures.Find(TextureName)	!=-1)
		return textures.Get(TextureName);
	return Texture();
}

Upp::VectorMap<Upp::String,Texture>& UltimateOpenGL_Context::GetTextures(){
	return textures;
}

//Logique
void UltimateOpenGL_Context::Draw(){
	try{
		//HEre I must do DeltaTime Calculation
		double currentFrame = GetTime(); //Calcules le nombre de frames par seconde
		DeltaTime = currentFrame - lastFrame;//Calcules le nombre de frames par seconde
		lastFrame = currentFrame;//Calcules le nombre de frames par seconde
		
		//Fps counter
		bufferFrame++;
		if( currentFrame -  LastTime >= 1.0){
			frameCount =bufferFrame;
			bufferFrame=0;
			LastTime = currentFrame;
		}
		
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);
		glm::mat4 transform = glm::mat4(1.0f);
		view = GetActiveScene().GetActiveCamera().GetViewMatrix();
		projection = glm::perspective(glm::radians(90.0f),(float)( ScreenSize.cx / ScreenSize.cy), 0.1f, 100.0f);
		GetActiveScene().Draw(model,view,projection,transform);
	}catch(UGLException exception){
		LOG("Error, context don't have anyScene to Draw ! " + Upp::AsString(exception.what()));
		exit(1);
	}
}

