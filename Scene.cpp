#include "Scene.h"
#include "GameObject.h"
#include "Camera.h"
#include "Shader.h"
#include "Object3D.h"
Scene::Scene(){
	prepareDefaultLight();
}
Scene::Scene::Scene(UltimateOpenGL_Context* _context_){
	context = _context_;
	prepareDefaultLight();
}
Scene::~Scene(){
	AllGamesObjects.Clear();
}

//Context
UltimateOpenGL_Context* Scene::GetContext(){
	return context;
}
void Scene::SetContext(UltimateOpenGL_Context* _context_){
	if(context)LOG("Warning : redefinition of context");
	context = _context_;
}
Upp::String Scene::GetName(){
	return name;
}
void Scene::SetName(const Upp::String& _name){
	name= _name;
}

//BackGround
void Scene::SetBackGroundColor(glm::vec3 color){
	BackGroundColor = color;
}
glm::vec3 Scene::GetBackGroundColor(){
	return BackGroundColor;
}


//GameObject
Upp::ArrayMap<Upp::String,GameObject>& Scene::GetAllGameObject(){
	return AllGamesObjects;
}
bool Scene::RemoveGameObject(const Upp::String& name){
	if(AllGamesObjects.Find(name)!=-1){
		AllGamesObjects.RemoveKey(name);
		return true;
	}
	return false;
}
 

//Camera 
Camera& Scene::AddCamera(const Upp::String& name){
	if(AllCameras.Find(name) ==-1){
		Camera& c=	AllCameras.Create<Camera>(name);
		c.SetScene(this);
		c.SetName(name);
		if(ActiveCamera == nullptr) ActiveCamera = &c;
		return c;
	}else{
		return AllCameras.Get(name);
	}
}
Camera& Scene::GetActiveCamera(){
	if(ActiveCamera == nullptr) throw UGLException(4,"Camera& Scene::GetActiveCamera() => No camera have been binded",1);
		return *ActiveCamera;
}
Camera& Scene::GetCamera(const Upp::String& name){
	if(AllCameras.Find(name) !=-1){
		return AllCameras.Get(name);
	}
	throw UGLException(8,"Camera& Scene::GetCamera(const Upp::String& name) => Camera named "+ name +" Do not exists",1);
}
Upp::ArrayMap<Upp::String,Camera>& Scene::GetAllCamera(){
	return AllCameras;
}
bool Scene::SetActiveCamera(const Upp::String& name){
	if(AllCameras.Find(name) !=-1){
		ActiveCamera = &AllCameras.Get(name);
		return true;
	}
	return false;
}
bool Scene::RemoveCamera(const Upp::String& name){
	if(AllCameras.Find(name) !=-1){
		if(ActiveCamera == &AllCameras.Get(name)) ActiveCamera = nullptr;
		AllCameras.Remove(AllCameras.Find(name));
		return true;
	}
	return false;
}

//Lights of scene
int Scene::GetNumberOfDirLight(){
	int number = 0;
	for(const Upp::String& objectName : AllGamesObjects.GetKeys()){
		GameObject& myObject = AllGamesObjects.Get(objectName);
		number +=myObject.GetDirLights().GetCount();
	}
	return number;
}
int Scene::GetNumberOfSpotLight(){
	int number = 0;
	for(const Upp::String& objectName : AllGamesObjects.GetKeys()){
		GameObject& myObject = AllGamesObjects.Get(objectName);
		number +=myObject.GetSpotLights().GetCount();
	}
	return number;
}
int Scene::GetNumberOfPointLight(){
	int number = 0;
	for(const Upp::String& objectName : AllGamesObjects.GetKeys()){
		GameObject& myObject = AllGamesObjects.Get(objectName);
		number +=myObject.GetPointLights().GetCount();
	}
	return number;
}
void Scene::prepareDefaultLight(){
	GameObject& go=	AllGamesObjects.Add("defaultDirLight");
	go.AddDirLight("defaultDirLight").SetDirection(glm::vec3(0.0f,-1.0f,0.0f)).SetAmbient(glm::vec3( 0.4f, 0.4f, 0.4f)).SetDiffuse(glm::vec3(0.9,0.9,0.9)).SetSpecular(glm::vec3( 0.04f, 0.04f, 0.04f));
}

//Logique
void Scene::Load(){
	for(const Upp::String& objectName : AllGamesObjects.GetKeys()){
		GameObject* myObject = &AllGamesObjects.Get(objectName);
		Object3D* myObject2 = dynamic_cast<Object3D*>(myObject);
		if( myObject2 ){
			myObject2->Load();
		}
	}
	loaded=true;
}
bool Scene::IsLoaded(){
	return loaded;
}
void  Scene::Draw(glm::mat4 model,glm::mat4 view,glm::mat4 projection,glm::mat4 transform){      
	glClearColor(BackGroundColor.x,BackGroundColor.y,BackGroundColor.z, 1.0f); //définie la couleur de fond dans la fenetre graphique 
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //nétoie la fenetre graphique et la regénère Scene::Draw(glm::mat4 model,glm::mat4 view,glm::mat4 projection,glm::mat4 transform){
	for(GameObject& object : AllGamesObjects){
		object.Draw(model,view,projection,transform,GetActiveCamera());
	}
}