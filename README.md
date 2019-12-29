# UltimateOpenGL 

UltimateOpenGL is a 3D motor used to make 3D games made by a beginner. It's designed to be "code only". It means you don't have (at this time) any GUI to help you create scene with your mouse.
Furthermore, it needs Ultimate++ Framework (A powerful C++ Framework) and the IDE used by it : [TheIDE](https://www.ultimatepp.org/index.html)

Here it is the Ultimate++ post : https://www.ultimatepp.org/forums/index.php?t=msg&goto=52755&#msg_52755

At this time, UOGL allow Scene creation, insertion of multiple 'GameObject'(Shapes, Lights), insertion of multiple camera per scene. 

Things are coming : 
	Support of animation and FBX file
	Gestion of physics
	Sound

## Prerequisites 

First be sure to have Ultimate++ Framework and TheIDE.
 
You also need some extern libraries : 
GLM (OpenGL Mathematique) https://glm.g-truc.net/0.9.9/index.html

Glad (Multi-Language GL/GLES/EGL/GLX/WGL Loader-Generator based on the official specs) used to properly load OpenGL
http://glad.dav1d.de/#profile=core&specification=gl&api=gl%3D4.6&api=gles1%3Dnone&api=gles2%3Dnone&api=glsc2%3Dnone&language=c&loader=on

Assimp (The Open-Asset-Importer-Lib) Used to load 3D Models
http://www.assimp.org  (You must compile it by yourself on your computer, However if you have some trouble doing it, then ask me I will do a quick tutorial)

Glfw used to generate the screen and render the openGL Scene https://www.glfw.org

You can find all libs compiled by myself for W10 Mingw here (it's working with V13684 of Upp) : https://we.tl/t-Q5vDvyhBz3

## Getting Started and Showing a 3D model 

Here you can find a look of what it should look in your Builder setting (based on TheIDE)
![how my lib is setting](https://i.imgur.com/KfHdK6N.png)

First thing to do is create a new Project using Ultimate++ Core then add UltimateOpenGl to it :

![project starting](https://i.imgur.com/J2KJRjo.png)

Second thing is importing UltimateOpenGl and GLFW and create UltimateOpenGL context object as follow :
```
#include <Core/Core.h>
#include <UltimateOpenGL_V2/UltimateOpenGL.h>
#include <GLFW/glfw3.h>

using namespace Upp;

UltimateOpenGL_Context context; //Context carrying all scene and object (Basicly context is a game)

CONSOLE_APP_MAIN
{
}
```
You also need to add GLFW lib to linker setting of the project. To do this, right click your package and goto Package organiser, then right click the package option field and select "new Librairie" then type in ":" fields : 'GLFW3' as follow :
![Adding Glfw3](https://i.imgur.com/P9eo1Al.png)


here we go, we are now ready to do some code, first of all we need to set up all the glfw window setup. (I won't dig the process, if you want to know more about glfw window and is initialisation, you must go here :https://learnopengl.com/Getting-started/Hello-Window )
```
#include <Core/Core.h>
#include <UltimateOpenGL_V3/UltimateOpenGL.h>
#include <GLFW/glfw3.h>

using namespace Upp;

const float screenWidth = 800.0f;
const float screenHeight = 600.0f;


UltimateOpenGL_Context context; //Context carrying all scene and object (Basicly context is a game)

CONSOLE_APP_MAIN
{
	/*****GLFW INITIALISATION AND OPENGL INITIALISATION USING GLAD ***/
	StdLogSetup(LOG_COUT|LOG_FILE);
	glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4); // Anti alliasing 
    context.SetScreenSize(screenHeight,screenWidth);
    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight,"Hello UltimateOpenGL V3", NULL, NULL);
    
    if (window == nullptr)
    {
        Cout() << "Failed to create GLFW window" << "\n";
        glfwTerminate();
        Exit(-1);
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        Cout()  << "Failed to initialize GLAD" << "\n";;
        Exit(-1);
    }
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);   // Anti alliasing 

        while(!glfwWindowShouldClose(window)) {
   
	    glfwSwapBuffers(window);
	    glfwPollEvents(); 
	}
	glfwTerminate();
```

If you now try the code, you should see a black window? Let's now add A simple model to this window and change the background color :

```
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        Cout()  << "Failed to initialize GLAD" << "\n";;
        Exit(-1);
    }
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);   // Anti alliasing 
	
  //***All basic code to set Up glwf is coming before see my exemple or Readme of my GITHUB**/// 
    Scene& myScene = context.AddScene("main");
    myScene.AddCamera("main");
    myScene.SetBackGroundColor(context.TransformRGBToFloatColor(40,180,200));
    
	Object3D& modele =  myScene.CreateGameObject<Object3D>("modele");
	modele.LoadModel("C:\\Upp\\myapps\\ExempleUltimateOpenGL_V3\\obj upp\\upp.obj"); //Loading of model
	
	modele.GetTransform().SetNewPosition(glm::vec3(0,0,-5)); //Set new position 
	modele.GetTransform().ScaleNewValue(glm::vec3(0.05f,0.05f,0.05f)); //Scale the model
	
	myScene.Load();
    while(!glfwWindowShouldClose(window)) {
        
		glfwSetWindowTitle(window, "UltimateOpenGL V3 - " +AsString(context.GetFPS()) +" FPS");
		try{
			context.Draw();  //Draw the context
		}catch(UGLException& e){
			LOG(e.what());	
		}
	
	    glfwSwapBuffers(window);
	    glfwPollEvents(); 
	}
	glfwTerminate();
```

Well, it's not that's graphic :
![modeleLoaded](https://i.imgur.com/KqJDOvU.png)
Let's make it rotate every frame !

```
	modele.GetTransform().SetNewPosition(glm::vec3(0,0,-5)); //Set new position 
	modele.GetTransform().ScaleNewValue(glm::vec3(0.05f,0.05f,0.05f)); //Scale the model
	
	//NEW CODE : 
	modele.SetOnDrawFunction([](GameObject& gm){ //Bind event on draw
		double rotation = glm::cos(context.GetEllapsedTime())/100;
		gm.GetTransform().RotateFromEulerAngles(context.GetDeltaTime() * 2,glm::vec3(0,1,0) );
	});
	
	myScene.Load();
    
```

now, you should see a rotating the model on the screen !

![Rotating](https://i.imgur.com/jhoMNRe.png)

This exemple is available here : https://github.com/Xemuth/SimpleExampleUltimateOpenGL

You can find another exemple of working code here : https://github.com/Xemuth/ExempleUltimateOpenGL_V3

![Ex1](https://i.imgur.com/bI2kepS.png)
![Ex2](https://i.imgur.com/KL2ixM6.png)
![Ex3](https://i.imgur.com/BxMIgiL.png)

## Documentation and deep example 

Incoming

## Authors

Clément Hamon


## License

Read Licenses.txt 

## Acknowledgments

Thanks to https://learnopengl.com
Thanks to All external librairies owner 
Thanks to the Upp Team


