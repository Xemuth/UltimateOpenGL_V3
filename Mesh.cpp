#include "UltimateOpenGL.h"
Mesh::Mesh(const Upp::Vector<Vertex>& vertices, Upp::Vector<unsigned int>& indices, Upp::Vector<Texture>& textures)
{
    this->vertices.Append(vertices);
    this->indices.Append(indices);
    
    // this->textures.Append(textures);
	
	Texture lastDiffuse;
	Texture lastHeight;
	Texture lastNormal;
	Texture lastAmbient;
	Texture lastSpecular;
	
	for(Texture& t :   textures){
		if(t.GetType() == DIFFUSE && lastDiffuse.IsLoaded()){
			MaterialTexture& m =  materialsTexture.Add(lastDiffuse.GetName());
			m.SetDiffuse(lastDiffuse.GetTextureIterator());
			if(lastSpecular.IsLoaded())
				; // Iñaki: Is this the intent?
			m.SetSpecular(lastSpecular.GetId());
			if(lastNormal.IsLoaded())
				; // Iñaki: Is this the intent?
			m.SetNormal(lastNormal.GetId());
			
			lastNormal =Texture();
			lastSpecular =Texture();
			lastDiffuse = Texture();
		}else{
			if(t.GetType() == DIFFUSE){
				lastDiffuse = t;
			}else if(t.GetType() == NORMAL){
				lastNormal = t;
			}else if(t.GetType() == HEIGHT){
				lastHeight = t;
			}else if(t.GetType() == AMBIENT){
				lastAmbient = t;
			}else if(t.GetType() == SPECULAR){
				lastSpecular = t;
			}
		}
	}
	if(lastDiffuse.IsLoaded()){
		MaterialTexture& m =  materialsTexture.Add(lastDiffuse.GetName());
		m.SetDiffuse(lastDiffuse.GetTextureIterator());
		if(lastSpecular.IsLoaded())
			;	// Iñaki: Was this the intent?
		m.SetSpecular(lastSpecular.GetId());
		if(lastNormal.IsLoaded())
			;	// Iñaki: Was this the intent?
		m.SetNormal(lastNormal.GetId());
	}

	for(MaterialTexture& mt : materialsTexture){
		mt.SetMix((float) (1.0f / materialsTexture.GetCount()));
	}

	
   //Load();
}

Mesh::Mesh(Mesh& _mesh){
	object3D = _mesh.GetObject3D();
	VAO = _mesh.GetVAO();
	VBO = _mesh.GetVBO();
	EBO = _mesh.GetEBO();
        
    vertices.Append(_mesh.GetVertices());
    indices.Append(_mesh.GetIndices());
 //   textures.Append(_mesh.GetTextures());
    
    shader = _mesh.GetShader();
    
    transform = _mesh.GetTransform();
    
    Upp::VectorMap<Upp::String,MaterialColor>& mc = _mesh.GetMaterialColor();
    for(const Upp::String& str : mc.GetKeys()){
        materialsColor.Add(str, mc.Get(str));
    }
    Upp::VectorMap<Upp::String,MaterialTexture>& mt = _mesh.GetMaterialTextures();
    for(const Upp::String& str : mt.GetKeys()){
        materialsTexture.Add(str, mt.Get(str));
    }

	LightAffected = _mesh.IsLightAffected();
}
void Mesh::LoadDefaultIndices(){
    int cpt= vertices.GetCount();
    for(int r = 0; r < cpt; r++){
        indices.Add(r);
    }
}

void Mesh::Load(){
	if(indices.size() != 0 || vertices.size() != 0){
		/*if(textures.GetCount() > 0 && materialsTexture.GetCount() == 0){
			for(Texture& t : textures){
			//	Upp::Cout() << "Texture of mesh is " + t.GetName() + " with ID of " +  Upp::AsString(t.GetId()) + "\n";
				MaterialTexture& m =  materialsTexture.Add(t.GetName());
				m.SetDiffuse(t.GetTextureIterator());
			}
		}*/
		
		//Juste un test,ne doit pas rester en létat
		for(MaterialTexture& mt : materialsTexture){
			mt.SetMix((float) (1.0f / materialsTexture.GetCount()));
		}
		
		if(!shader.IsCompiled() && object3D != nullptr && object3D->GetScene() != nullptr){
			LOG("Shader not compiled !, Default shaders loaded !\n");
			GenerateAutoShader(object3D->GetScene() ->GetNumberOfDirLight(),  object3D->GetScene() ->GetNumberOfPointLight(),object3D->GetScene()->GetNumberOfSpotLight());
		}
		if(indices.size() == 0 && vertices.size() > 0){
            LOG("No indice defined ! Auto generation of incices !");
            LoadDefaultIndices();
        }


		// create buffers/arrays
	    glGenVertexArrays(1, &VAO);
	    glGenBuffers(1, &VBO);
	    glGenBuffers(1, &EBO);
	
	    glBindVertexArray(VAO);
	    // load data into vertex buffers
	    glBindBuffer(GL_ARRAY_BUFFER, VBO);
	    // A great thing about structs is that their memory layout is sequential for all its items.
	    // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
	    // again translates to 3/2 floats which translates to a byte array.
	    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);  
	
	    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
	
	
	/*	Upp::Cout() << "SizeOFVertex : " << sizeof(Vertex) << "\n";
		float* buffer = nullptr;
		for(Vertex& v : vertices){
			buffer = (float*)&v;
			
			for (int e =0 ; e < 13 ; e++){
				Upp::Cout() <<Upp::AsString(*buffer) <<",";	
				buffer++;
			}
			
			Upp::Cout() <<  Upp::EOL;
		}*/
	
	    // set the vertex attribute pointers
	    // vertex Positions
	    glEnableVertexAttribArray(0);	
	    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	    // vertex normals
	    glEnableVertexAttribArray(1);	
	    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	    // vertex texture coords
	    glEnableVertexAttribArray(2);	
	    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
	    // vertex tangent
	    glEnableVertexAttribArray(3);
	    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
	    // vertex bitangent
	    glEnableVertexAttribArray(4);
	    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
	
	    glBindVertexArray(0);
	   // shader.Use();
	}else{
	    LOG("Mesh of object named " + object3D->GetName() + " are error ! or undefined ! nothing to load !");
	}
}



void Mesh::Draw(glm::mat4 model,glm::mat4 view,glm::mat4 projection,glm::mat4 transform,Camera& camera){

	shader.Use();
	//Cout() << "x: " << Position.x << ", y: " << Position.y << ", z: " << Position.z << "\n";
	model = glm::translate(model,GetTransform().GetPosition())*glm::mat4_cast(GetTransform().GetQuaterion())*GetTransform().GetModelMatrixScaller();

	//if(ClearOnDraw)transformations.Clear();
    shader.SetMat4("model",model);
    shader.SetMat4("transform", transform);
    shader.SetMat4("view",view);
    shader.SetMat4("projection", projection);
    shader.SetVec3("viewPos",GetTransform().GetPosition());
    int cptColor= 0;
    for(const Upp::String &mcStr : materialsColor.GetKeys()){
        
		shader.SetMaterialColor("color"+ Upp::AsString(cptColor),materialsColor.Get(mcStr));
		cptColor++;
	}
	int cptTexture =0;
	for(const Upp::String &mtStr : materialsTexture.GetKeys()){
				materialsTexture.Get(mtStr).diffuse = object3D->GetScene()->GetContext()->GetTextures().Get(mtStr).GetTextureIterator(); 
		materialsTexture.Get(mtStr).specular= object3D->GetScene()->GetContext()->GetTextures().Get(mtStr).GetTextureIterator();
		object3D->GetScene()->GetContext()->GetTextures().Get(mtStr).Use();
		shader.SetMaterialTexture("texture"+ Upp::AsString(cptTexture),materialsTexture.Get(mtStr));
		cptTexture++;
	}
    for(const Upp::String& str : object3D->GetScene()->GetAllGameObject().GetKeys()){
		int cptObject = 0;
    	GameObject& obj =  object3D->GetScene()->GetAllGameObject().Get(str);
    	if(!obj.IsDrawableDuringThisFrame()){
			for(const Upp::String& dlStr : obj.GetDirLights().GetKeys()){
				shader.SetDirLight(dlStr,obj.GetDirLights().Get(dlStr),cptObject);
				cptObject++;
			}
			cptObject=0;
			for(const Upp::String& spStr : obj.GetSpotLights().GetKeys()){
				shader.SetSpotLight(spStr,obj.GetSpotLights().Get(spStr),cptObject);
				cptObject++;
			}
			cptObject=0;
			for(const Upp::String& poStr : obj.GetPointLights().GetKeys()){
				shader.SetPointLight(poStr,obj.GetPointLights().Get(poStr),cptObject);
				cptObject++;
			}
    	}
    }
  
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.GetCount(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    // always good practice to set everything back to defaults once configured.
	glActiveTexture(GL_TEXTURE0);
    shader.Unbind();
	/*
	// bind appropriate textures
    unsigned int diffuseNr  = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr   = 1;
    unsigned int heightNr   = 1;
    for(unsigned int i = 0; i < textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
        // retrieve texture number (the N in diffuse_textureN)
        Upp::String number;
        TextureType name = textures[i].GetType();
        if(name == DIFFUSE)
			number = std::to_string(diffuseNr++);
		else if(name == SPECULAR)
			number = std::to_string(specularNr++); // transfer unsigned int to stream
        else if(name == NORMAL)
			number = std::to_string(normalNr++); // transfer unsigned int to stream
         else if(name == HEIGHT)
		    number = std::to_string(heightNr++); // transfer unsigned int to stream

												 // now set the sampler to the correct texture unit
       // glUniform1i(glGetUniformLocation(shader.GetId(), (name + number).c_str()), i);
        // and finally bind the texture
        glBindTexture(GL_TEXTURE_2D, textures[i].GetId());
    }
    
    // draw mesh
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // always good practice to set everything back to defaults once configured.
    glActiveTexture(GL_TEXTURE0);
    */
}

/*
bool Mesh::BindTexture(Upp::String textureName,float textureShininess,Upp::String TextureSpecularName){
	if(object3D != nullptr && object3D->GetScene() != nullptr &&  textureName.GetCount() > 0 && object3D->GetScene()->GetContext()->GetTextures().Find(textureName) !=-1){
		if(TextureSpecularName.GetCount() > 0 && object3D->GetScene()->GetContext()->GetTextures().Find(TextureSpecularName)!=-1){
			materialsTexture.Add(textureName, MaterialTexture(textureShininess,true,object3D->GetScene()->GetContext()->GetTextures().Get(textureName).GetTextureIterator(),object3D->GetScene()->GetContext()->GetTextures().Get(TextureSpecularName).GetTextureIterator()));
		}else{
			materialsTexture.Add(textureName, MaterialTexture(textureShininess,false,object3D->GetScene()->GetContext()->GetTextures().Get(textureName).GetTextureIterator(),0 ));
		}
		return true;
	}else{
		LOG("Error : BindTexture(String,float,String) Invalide texture name ! he can't be empty or undefined !\n");
		return false;
	}
}*/

void Mesh::SetShader(Shader& _shader){
	shader = _shader;
}
Shader& Mesh::GetShader(){
	return shader;
}


unsigned int Mesh::GetVAO(){
	return VAO;
}
unsigned int Mesh::GetVBO(){
	return VBO;
}
unsigned int Mesh::GetEBO(){
	return EBO;
}

Upp::Vector<Vertex>& Mesh::GetVertices(){
	return vertices;
}
Upp::Vector<unsigned int>& Mesh::GetIndices(){
	return indices;
}

bool Mesh::ReadData(Upp::Vector<float>& data,ReaderParameters readerParameter){
	//I thing this code is really bad.
	if(readerParameter.verticesPosition> -1){
		enum AllDataType{VERTICES,NORMAL,TEXTURE,TANGANT,BITANGANT};
		Upp::ArrayMap<int,AllDataType> map;
		int* iterateur =static_cast<int*>( &readerParameter.verticesPosition);
		int cpt  = 0;
		for(int e=0;e< 5;iterateur++,e++){
			if(*iterateur != -1){
				for(int r = 0;r < 5; r++){
					if(*iterateur == r){
						switch(cpt){
							case 0:
								map.Add(*iterateur,VERTICES);
							break;
							case 1:
								map.Add(*iterateur,NORMAL);
							break;
							case 2:
								map.Add(*iterateur,TEXTURE);
							break;
							case 3:
								map.Add(*iterateur,TANGANT);
							break;
							case 4:
								map.Add(*iterateur,BITANGANT);
							break;
							default:
								LOG("Class Mesh:(Warning) bool Mesh::ReadData() strange iterator is passed over the map !");
						}
						break;
					}
				}
			}
			cpt++;
		}
		Upp::SortByKey(map,[](const int& a, const int& b) { return a<b; });
		float* fIterator =static_cast<float*>( &(data[0]));
		int iteratorSize = 0;
		while(iteratorSize < data.GetCount()){
			Vertex& v =  vertices.Add();
			for(const int& iterator  : map.GetKeys()){
				switch( map.Get(iterator)){ 
					case VERTICES:
						v.Position = glm::vec3(*fIterator,*(fIterator++),*(fIterator++));
						iteratorSize+=3;
						fIterator++;
					break;
					case NORMAL:
						v.Normal = glm::vec3(*fIterator,*(fIterator++),*(fIterator++));
						iteratorSize+=3;
						fIterator++;
					break;
					case TEXTURE:
						v.TexCoords = glm::vec2(*fIterator,*(fIterator++));
						iteratorSize+=2;
						fIterator++;
					break;
					case TANGANT:
						v.Tangent = glm::vec3(*fIterator,*(fIterator++),*(fIterator++));
						iteratorSize+=3;
						fIterator++;
					break;
					case BITANGANT:
						v.Bitangent = glm::vec3(*fIterator,*(fIterator++),*(fIterator++));
						iteratorSize+=3;
						fIterator++;
					break;
					default:
						LOG("Class Mesh:(Warning) bool Mesh::ReadData() strange value spotted on the map used to readData !");
				}
			}
		}
		return true;
	}else{
		LOG("Class Mesh:(Warning) bool Mesh::ReadData() no position of vertices set in readerParameters !");
		return false;
	}
}

/* Texture Gestion */
Mesh& Mesh::BindTexture(const Upp::String& TextureName,float mixValue, float textureShininess,const Upp::String& TextureSpecularName, const Upp::String& NormalMappingTextureName){
	if(object3D != nullptr && object3D->GetScene() != nullptr && object3D->GetScene()->GetContext() !=nullptr){	
		Texture t =object3D->GetScene()->GetContext()->GetTexture(TextureName);
		if(t.IsLoaded()){
			MaterialTexture& m =  materialsTexture.Add(t.GetName());
			m.SetDiffuse(t.GetTextureIterator()).SetMix(mixValue).SetShininess(textureShininess);
			if(TextureSpecularName.GetCount() > 0){
				Texture tSpeculare =object3D->GetScene()->GetContext()->GetTexture(TextureName);
				if(tSpeculare.IsLoaded()){
					/*** Here I add the texture to material ***/
					m.SetSpecular(tSpeculare.GetId());
					if(tSpeculare.GetType() != SPECULAR)
						LOG("Class Mesh:(Warning) Mesh& Mesh::BindTexture(...) You are binding as speculare a texture wich is not a speculare type !");
				}else{
					LOG("Error : Mesh& Mesh::BindTexture(...) Specular texture of " + object3D->GetName() +" named " + tSpeculare.GetName() +" is not loaded !" );
				}
			}
			if(NormalMappingTextureName.GetCount() > 0){
				Texture tNormal = object3D->GetScene()->GetContext()->GetTexture(TextureName);
				if(tNormal.IsLoaded()){
					/**Here I add the texture to material ***/
					m.SetNormal(tNormal.GetId());
					if(tNormal.GetType() != NORMAL)
						LOG("Class Mesh:(Warning) Mesh& Mesh::BindTexture(...) You are binding as Normal a texture wich is not a Normal type !");
				}else{
					LOG("Class Mesh:(Error) Mesh& Mesh::BindTexture(...) Normal texture of " + object3D->GetName() +" named " + tNormal.GetName() +" is not loaded !" );
				}
			}
			if(t.GetType() != DIFFUSE)
				LOG("Class Mesh:(Warning) Mesh& Mesh::BindTexture(...) You are binding as Diffuse a texture wich is not a Diffuse type !");
			
			return *this;
		}
		else{
			LOG("Class Mesh:(Error) Mesh& Mesh::BindTexture(...) " + TextureName + " texture don't existe in context, you must add it before getting it !");
			return *this;
		}
	}
	LOG("Class Mesh:(Error) Mesh& Mesh::BindTexture(...) Mesh is not bind to GameObject or GameObject is not bind to Scene or Scene is not bind to Context wich is carrying texture");
	return *this;
}

/* Transform */
Transform& Mesh::GetTransform(){
	return transform;
}
void Mesh::SetTransform(const Transform& _transform){
	transform = _transform;
}

/* Material Manager */
Upp::VectorMap<Upp::String,MaterialTexture>& Mesh::GetMaterialTextures(){
	return materialsTexture;
}
Upp::VectorMap<Upp::String,MaterialColor>& Mesh::GetMaterialColor(){
	return materialsColor;
}

MaterialTexture& Mesh::CreateMaterialTexture(const Upp::String& _name){
	if( materialsTexture.Find(_name) ==-1){
		return materialsTexture.Add(_name);
	}else{
		return materialsTexture.Get(_name);
	}
}
MaterialColor& Mesh::CreateMaterialColor(const Upp::String& _name){
	if( materialsColor.Find(_name) ==-1){
		return materialsColor.Add(_name);
	}else{
		return materialsColor.Get(_name);
	}
}
//Miscelnious
void Mesh::SetObject3D(Object3D* _object3D){
	object3D = _object3D;
}
Object3D* Mesh::GetObject3D(){
	return object3D;
}

void Mesh::SetLightAffected(bool _light){
	LightAffected = _light;
}
bool Mesh::IsLightAffected(){
	return LightAffected;
}

void Mesh::GenerateAutoShader(int NbLightDir,int NbLightPoint,int NbLightSpot){
	Upp::String vertexShader = BasicShaders.Get("Default_Vertex_Shader");
	Upp::String fragmentShader = BasicShaders.Get("Default_Fragment_Shader");
		
	Upp::String all_uniform="";
	Upp::String all_VecMulti="FragColor = ";
	
	Upp::String AllDirLights="";
	Upp::String AllPointLights="";
	Upp::String AllSpotLights="";	
	if(materialsTexture.GetCount()== 0 && materialsColor.GetCount() == 0){
		float r = float(Upp::Randomf());	float g = float(Upp::Randomf());	float b = float(Upp::Randomf());
		MaterialColor& m = CreateMaterialColor("defaultColor");
		m.SetDiffuse(glm::vec3( r,g,b));
		LOG("Class Mesh:(INFO) void Mesh::GenerateAutoShader(int,int,int) -> static color set for " + object3D->GetName() + " object !");	
	}
	if(materialsTexture.GetCount() > 0){
		//First we must had in Vertex Shader  all this thing : 
		vertexShader.Replace("//LAYOUT_TEXTURE_COORD","layout (location = 2) in vec2 aTexCoord;\nlayout (location =3) in vec3 aTangent;\nlayout (location =4) in vec3 aBiTangent;");
		vertexShader.Replace("//OUT_VECTOR_TEXCOORDS","out vec2 TexCoords;");
		vertexShader.Replace("//TEXTCOORDS_CALCULATION","TexCoords = aTexCoord;");
		
		//Then for fragment Shader :
		fragmentShader.Replace("//STRUCT_MATERIAL_TEXTURE","MATERIAL_TEXTURE_STRUCT()");
		fragmentShader.Replace("//OUT_FRAG_COLOR","out vec4 FragColor;");
		fragmentShader.Replace("//IN_VECTOR_TEXTURE_COORD","in vec2 TexCoords;");
		int cpt = 0;
		for(const Upp::String& key : materialsTexture.GetKeys()){
			Upp::String textureNameIdentifier = "texture" + Upp::AsString(cpt) ;
			all_uniform << "uniform MaterialTexture " << textureNameIdentifier << ";\n";
			all_VecMulti << ((cpt > 0)?"*":"") <<  "texture(" + textureNameIdentifier + ".diffuse,TexCoords)"; 
			cpt++;
		}
		if(all_uniform.GetCount()>0) fragmentShader.Replace("//UNIFORM_MATERIAL_TEXTURE_NAME",all_uniform);

		if( NbLightDir > 0 && LightAffected){
			cpt = 0;
			for(const Upp::String& key : materialsTexture.GetKeys()){
				Upp::String textureNameIdentifier = "texture" + Upp::AsString(cpt) ;
				if(cpt == 0){
					AllDirLights << "for(int i = 0; i < " + Upp::AsString(NbLightDir) +"; i++){\n";
					AllDirLights << "\tresult += CalcTextureDirLight("+ textureNameIdentifier +",dirLights[i], norm, viewDir);\n";
				}else{
					AllDirLights << "\tresult += CalcTextureDirLight("+ textureNameIdentifier +",dirLights[i], norm, viewDir);\n";
				}
				cpt++;
			}
			AllDirLights << "}";	
			fragmentShader.Replace("//STRUCT_DIR_LIGHT","LIGHT_DIR_STRUCT()");
			fragmentShader.Replace("//IMPORT_DIR_LIGHT_TEXTURE_PROTOTYPE","LIGHT_DIR_TEXTURE_PROTOTYPE()");
			fragmentShader.Replace("//DIR_LIGHT_TEXTURE_FUNCTION","LIGHT_DIR_TEXTURE_FUNCTION()");	
			fragmentShader.Replace("//DIR_LIGHTS_ARRAY","uniform DirLight dirLights["+ Upp::AsString(NbLightDir) +"];");
			fragmentShader.Replace("//DIRECTIONAL_LIGHTS",AllDirLights);
		}
		if( NbLightPoint > 0 && LightAffected){
			cpt = 0;
			for(const Upp::String& key : materialsTexture.GetKeys()){
				Upp::String textureNameIdentifier = "texture" + Upp::AsString(cpt) ;
				if(cpt == 0){
					AllPointLights << "for(int i = 0; i < " + Upp::AsString(NbLightPoint) +"; i++){\n";
					AllPointLights << "\tresult += CalcTexturePointLight("+ textureNameIdentifier +",pointLights[i], norm, FragPos , viewDir);\n";
				}else{
					AllPointLights << "\tresult += CalcTexturePointLight("+ textureNameIdentifier +",pointLights[i], norm, FragPos , viewDir);\n";
				}
				cpt++;
			}
			AllPointLights << "}";	
			fragmentShader.Replace("//STRUCT_POINT_LIGHT","LIGHT_POINT_STRUCT()");
			fragmentShader.Replace("//IMPORT_POINT_LIGHT_TEXTURE_PROTOTYPE","LIGHT_POINT_TEXTURE_PROTOTYPE()");
			fragmentShader.Replace("//POINT_LIGHT_TEXTURE_FUNCTION","LIGHT_POINT_TEXTURE_FUNCTION()");	
			fragmentShader.Replace("//POINT_LIGHTS_ARRAY","uniform PointLight pointLights["+ Upp::AsString(NbLightPoint) +"];");
			fragmentShader.Replace("//POINT_LIGHTS",AllPointLights);
		}
		if( NbLightSpot > 0 && LightAffected){
			cpt = 0;
			for(const Upp::String& key : materialsTexture.GetKeys()){
				Upp::String textureNameIdentifier = "texture" + Upp::AsString(cpt) ;
				if(cpt == 0){
					AllSpotLights << "for(int i = 0; i < " + Upp::AsString(NbLightSpot) +"; i++){\n";
					AllSpotLights << "\tresult += CalcTextureSpotLight("+ textureNameIdentifier +",spotLights[i], norm, FragPos , viewDir);\n";
				}else{
					AllSpotLights << "\tresult += CalcTextureSpotLight("+ textureNameIdentifier +",spotLights[i], norm, FragPos , viewDir);\n";
				}
				cpt++;
			}
			AllSpotLights << "}";
			fragmentShader.Replace("//STRUCT_SPOT_LIGHT","LIGHT_SPOT_STRUCT()");
			fragmentShader.Replace("//IMPORT_SPOT_LIGHT_TEXTURE_PROTOTYPE","LIGHT_SPOT_TEXTURE_PROTOTYPE()");
			fragmentShader.Replace("//SPOT_LIGHT_TEXTURE_FUNCTION","LIGHT_SPOT_TEXTURE_FUNCTION()");	
			fragmentShader.Replace("//SPOT_LIGHTS_ARRAY","uniform SpotLight spotLights["+ Upp::AsString(NbLightSpot) +"];");
			fragmentShader.Replace("//SPOT_LIGHTS",AllSpotLights);
		}
		
		if(all_VecMulti.GetCount()>12){
			if((NbLightDir > 0 || NbLightPoint > 0 || NbLightSpot > 0) && LightAffected){
			//	fragmentShader.Replace("//FRAG_COLOR_CALCULATION","FragColor = vec4(result,1.0);");
				fragmentShader.Replace("//FRAG_COLOR_CALCULATION","FragColor = texture(texture0.diffuse,TexCoords);");
			}else{
				all_VecMulti << ";\n";
				fragmentShader.Replace("//FRAG_COLOR_CALCULATION",all_VecMulti);
			//	fragmentShader.Replace("//FRAG_COLOR_CALCULATION","FragColor = texture(texture0.diffuse,TexCoords);\n");
			}
		}
	}
	if(materialsColor.GetCount() > 0){
		fragmentShader.Replace("//STRUCT_MATERIAL_COLOR","MATERIAL_COLOR_STRUCT()");
		fragmentShader.Replace("//OUT_FRAG_COLOR","out vec4 FragColor;");
		
		int cpt = 0;
		for(const Upp::String& key : materialsColor.GetKeys()){
			Upp::String colorIdentifier = "color" + Upp::AsString(cpt) ;
			all_uniform << "uniform MaterialColor " <<  colorIdentifier << ";\n";
			all_VecMulti << ((cpt > 0)?"*":"") <<  "vec4(" + colorIdentifier + ".diffuse,1.0)"; 
			cpt++;
		}
		if(all_uniform.GetCount()>0) fragmentShader.Replace("//UNIFORM_MATERIAL_COLOR_NAME",all_uniform);
		
		if( NbLightDir > 0 && LightAffected){
			cpt = 0;
			for(const Upp::String& key : materialsColor.GetKeys()){
				Upp::String colorIdentifier = "color" + Upp::AsString(cpt) ;
				if(cpt == 0){
					AllDirLights << "for(int i = 0; i < " + Upp::AsString(NbLightDir) +"; i++){\n";
					AllDirLights << "\tresult += CalcColorDirLight("+ colorIdentifier +",dirLights[i], norm, viewDir);\n";
				}else{
					AllDirLights << "\tresult += CalcColorDirLight("+ colorIdentifier +",dirLights[i], norm, viewDir);\n";
				}
			}
			AllDirLights << "}";
			fragmentShader.Replace("//STRUCT_DIR_LIGHT","LIGHT_DIR_STRUCT()");
			fragmentShader.Replace("//IMPORT_DIR_LIGHT_COLOR_PROTOTYPE","LIGHT_DIR_COLOR_PROTOTYPE()");
			fragmentShader.Replace("//DIR_LIGHT_COLOR_FUNCTION","LIGHT_DIR_COLOR_FUNCTION()");	
			fragmentShader.Replace("//DIR_LIGHTS_ARRAY","uniform DirLight dirLights["+ Upp::AsString(NbLightDir) +"];");
			fragmentShader.Replace("//DIRECTIONAL_LIGHTS",AllDirLights);
		}
		if( NbLightPoint > 0&& LightAffected){
			cpt = 0;
			for(const Upp::String& key : materialsColor.GetKeys()){
				Upp::String colorIdentifier = "color" + Upp::AsString(cpt) ;
				if(cpt == 0){
					AllPointLights << "for(int i = 0; i < " + Upp::AsString(NbLightPoint) +"; i++){\n";
					AllPointLights << "\tresult += CalcColorPointLight("+ colorIdentifier +",pointLights[i], norm, FragPos ,viewDir);\n";
				}else{
					AllPointLights << "\tresult += CalcColorPointLight("+ colorIdentifier +",pointLights[i], norm, FragPos ,viewDir);\n";
				}
			}
			AllPointLights << "}";
			fragmentShader.Replace("//STRUCT_POINT_LIGHT","LIGHT_POINT_STRUCT()");
			fragmentShader.Replace("//IMPORT_POINT_LIGHT_COLOR_PROTOTYPE","LIGHT_POINT_COLOR_PROTOTYPE()");
			fragmentShader.Replace("//POINT_LIGHT_COLOR_FUNCTION","LIGHT_POINT_COLOR_FUNCTION()");	
			fragmentShader.Replace("//POINT_LIGHTS_ARRAY","uniform PointLight pointLights["+ Upp::AsString(NbLightPoint) +"];");
			fragmentShader.Replace("//POINT_LIGHTS",AllPointLights);
		}
		if( NbLightSpot > 0&& LightAffected){
			cpt = 0;
			for(const Upp::String& key : materialsColor.GetKeys()){
				Upp::String colorIdentifier = "color" + Upp::AsString(cpt) ;
				if(cpt == 0){
					AllSpotLights << "for(int i = 0; i < " + Upp::AsString(NbLightSpot) +"; i++){\n";
					AllSpotLights << "\tresult += CalcColorSpotLight("+ colorIdentifier +",spotLights[i], norm, FragPos , viewDir);\n";
				}else{
					AllSpotLights << "\tresult += CalcColorSpotLight("+ colorIdentifier +",spotLights[i], norm, FragPos , viewDir);\n";
				}
			}
			AllSpotLights << "}";
			fragmentShader.Replace("//STRUCT_SPOT_LIGHT","LIGHT_SPOT_STRUCT()");
			fragmentShader.Replace("//IMPORT_SPOT_LIGHT_COLOR_PROTOTYPE","LIGHT_SPOT_COLOR_PROTOTYPE()");
			fragmentShader.Replace("//SPOT_LIGHT_COLOR_FUNCTION","LIGHT_SPOT_COLOR_FUNCTION()");	
			fragmentShader.Replace("//SPOT_LIGHTS_ARRAY","uniform SpotLight spotLights["+ Upp::AsString(NbLightSpot) +"];");
			fragmentShader.Replace("//SPOT_LIGHTS",AllSpotLights);
		}
		
		if(all_VecMulti.GetCount()>12){
			if((NbLightDir > 0 || NbLightPoint > 0 || NbLightSpot > 0) && LightAffected){
				fragmentShader.Replace("//FRAG_COLOR_CALCULATION","FragColor = vec4(result,1.0);");
			}else{
				all_VecMulti << ";\n";
				fragmentShader.Replace("//FRAG_COLOR_CALCULATION",all_VecMulti);
			}
		}
	}	
	if( (NbLightDir > 0 || NbLightPoint > 0 || NbLightSpot > 0) && LightAffected){
		//HEre we ensure to put everything to start lightcalculation
		fragmentShader.Replace("//LIGHT_STARTING_DATA","vec3 norm = normalize(Normal);\nvec3 viewDir = normalize(viewPos - FragPos);\nvec3 result = vec3(0.0,0.0,0.0);\n");
	}
	if(!GetShader().AddShader(  "Vertex",VERTEX,vertexShader).AddShader("Fragment",FRAGMENT,fragmentShader).CompileShader(true)){
		LOG("Class Mesh:(ERROR) void Mesh::GenerateAutoShader(int,int,int) ->Shader failled to compilate !");	
	}

}
