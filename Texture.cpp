#include "Texture.h"
Texture::Texture(){}
Texture::Texture(Upp::String _path){
	if(FileExists(_path)){
		pathTexture = Upp::Replace(_path,Upp::Vector<Upp::String>{"\\"},Upp::Vector<Upp::String>{"/"});
		LoadDefaultname();
	}else{
		LOG("Error : Texture::Texture(...) FilePath : \""+ _path +"\" is incorrect !");
	}
}
Texture::Texture(Upp::String _path,Upp::String _Name){
	if(FileExists(_path)){
		pathTexture = Replace(_path,Upp::Vector<Upp::String>{"\\"},Upp::Vector<Upp::String>{"/"});
	}else{
		LOG("Error : Texture::Texture(...) FilePath : \""+ _path +"\" is incorrect !");
	}
	textureName = _Name;
}
Texture::Texture(const Texture& texture){
	pathTexture = texture.GetPath();
	textureName = texture.GetName();
	width = texture.GetWidth();
	height = texture.GetHeight();
	nrChannels = texture.GetNrChannels();
	GenerateMipMap = texture.IsMipMap();
	Loaded = texture.Loaded;
	if(Loaded)ID = texture.GetId();
	TextureIterator = texture.GetTextureIterator();
	textureParameters.Append(texture.GetTextureParameters());
}
Texture& Texture::operator=(const Texture& texture){
	pathTexture = texture.GetPath();
	textureName = texture.GetName();
	width = texture.GetWidth();
	height = texture.GetHeight();
	nrChannels = texture.GetNrChannels();
	GenerateMipMap = texture.IsMipMap();
	Loaded = texture.Loaded;
	if(Loaded)ID = texture.GetId();
	TextureIterator = texture.GetTextureIterator();
	textureParameters.Append(texture.GetTextureParameters());
	return *this;
}
Texture::~Texture(){
	//This part is commented since Texture is working on Copying paradigme
	//So Getting a texture to retrieve ID would make it unload when the object is deleted
	//To Unload texture from graphic card, you must use Unload Function
	/*if(loaded){
		glDeleteTextures(1, &ID);
	}*/
}
Texture& Texture::LoadDefaultTextureParameter(){
	textureParameters.Clear();
	textureParameters = Upp::Vector<TextureParameter>{
		TextureParameter(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,(int)GL_REPEAT),
		TextureParameter(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (int)GL_REPEAT),
		TextureParameter(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,(int) GL_LINEAR_MIPMAP_LINEAR),
		TextureParameter(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,(int) GL_NEAREST),
	};
	LOG("Info : Texture::LoadDefaultTextureParameter() Default Texture parameter have been loaded for texture named : \""+ Name +"\"");
}
TextureParameter& Texture::AddTextureParameter(const TextureParameter& parameter){
	return textureParameters.Add(parameter);
}
Upp::Array<TextureParameter>& Texture::GetTextureParameters(){
	return textureParameters;
}
unsigned int Texture::GetId(){
	return ID;
}
Texture& Texture::LoadDefaultname(){
	textureName = pathTexture.Right(pathTexture.GetCount()- pathTexture.ReverseFind("/") -1);
	textureName = textureName.Left(textureName.Find("."));
	LOG("Info : Texture::LoadDefaultname() default name have been setted : \""+ Name +"\"");
	return *this;
}
Texture&  Texture::SetPath(Upp::String _path){
	if(FileExists(_path)){
		Path = _path;
		return *this;
	}else{
		LOG("Warning : Texture::SetPath(...) Texture file path is incorrect !");
	}
	return *this;
}
Texture& Texture::SetPath(Upp::String _path){
	Path = _path;
	return *this;
}
Upp::String Texture::GetPath(){
	return Path;
}
Texture& Texture::SetName(Upp::String _name){
	Name = _name;
	return *this;
}
Upp::String Texture::GetName(){
	return Name;
}
Texture& Texture::SetType(TextureType _type){
	type = _type;
	return *this;
}
TextureType Texture::GetType(){
	return type;
}
bool Texture::IsMipMap(){
	return MipMap;
}
Texture& Texture::EnableMipMap(){
	MipMap = true;
	return *this;
}
Texture& Texture::DisableMipMap(){
	MipMap = false;
	return *this;
}
bool Texture::IsFlippedOnLoad(){
	return FlipOnLoad;
}
Texture& Texture::EnableFlipOnLoad(){
	FlipOnLoad = true;
	return *this;
}
Texture& Texture::DisableFlipOnLoad(){
	FlipOnLoad = false;
	return *this;
}
int Texture::GetTextureIterator(){
	return TextureIterator;
}
int Texture::GetWidth(){
	return width;
}
int Texture::GetHeight(){
	return height;
}
int Texture::GetNrChannels(){
	return nrChannels;
}
bool Texture::IsLoaded(){
	return Loaded;
}
void Texture::LoadTextureParameter(){
	for(TextureParameter& param :textureParameters){
		glTexParameteri(param.target,param.pname,param.params);
	}
}
bool Texture::Reload(unsigned int ActiveIterator){
	Unload();
	return Load(ActiveIterator);
}
bool Texture::Load(unsigned int ActiveIterator){
	if(FileExists(pathTexture)){
		if(!Loaded){
			TextureIterator = ActiveIterator;
			ID=0;
			glGenTextures(1, &ID);
			glActiveTexture(GL_TEXTURE0 + TextureIterator);
			glBindTexture(GL_TEXTURE_2D, ID);
			
			if(textureparameters.GetCount() ==0)LoadDefaultTextureParameter();
			LoadTextureParameter();
		
			stbi_set_flip_vertically_on_load(FlipOnLoad);
			unsigned char *data=nullptr; //Comment this and un comment Class attribute Data* to load it in our unsigned char* buffer
			data = stbi_load(pathTexture.ToStd().c_str(), &width, &height, &nrChannels, 0);
			if(data){
		        if (nrChannels == 1)
		            format = GL_RED;
		        else if (nrChannels == 3)
		            format = GL_RGB;
		        else if (nrChannels == 4)
		            format = GL_RGBA;
		        if(format ==GL_RGBA){
					glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
					glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		        }
				glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
				if(GenerateMipMap)glGenerateMipmap(GL_TEXTURE_2D);
					Loaded =true;
			}else{
				LOG("Error : Texture::Load(...) Error on loading texture named : \""+ Name +"\" !");
				Loaded=false;
			}
			stbi_image_free(data); //If we manage to use class attribute, we must dont forget to clean it in class destructor ! it's very important to prevent memory leak
		}else{
			LOG("Error : Texture::Load(...) Texture named : \""+ Name +"\" is already loaded ! Use Unload() Function to Unload or Reload() to reload the texture");
			return false;
		}
	}else{
		LOG("Error : Texture::Load(...) FilePath : \""+ Path +"\" of texture named : \""+ Name +"\" is incorrect !");
		Loaded =false;
	}
	return Loaded;
}
bool Texture::Unload(){
	if(Loaded){
		glDeleteTextures(1, &ID);
		Loaded = false;
		return true;
	}else{
		LOG("Warning : Texture::Unload() Nothing to remove !");
	}
	return false;
}
bool Texture::Use(){
	if(Loaded){
		//Upp::Cout() << "Using texture ID " << Upp::AsString(ID) << " to the iterator number " <<Upp::AsString(TextureIterator) << "\n";
		glActiveTexture(GL_TEXTURE0 +TextureIterator);
		glBindTexture(GL_TEXTURE_2D, ID);
		return true;
	}else{
		Upp::Cout() << "Warning Use(): Can't use an Unloaded texture !\n";
	}
	return false;
}