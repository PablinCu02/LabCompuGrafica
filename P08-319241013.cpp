/*
Práctica 8: Iluminación 2
*/
//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture carroceriaTexture;
Texture llantaTexture;
Texture rinTexture;
Texture ojosTexture;
Texture cofreTexture;
Texture FinnTexture;
Texture VitrineTexture;


Model Farola_M;

Model Carroceria_Bob;
Model Cofre_Bob;
Model Llanta_M1;
Model Llanta_M2;
Model Llanta_M3;
Model Llanta_M4;

Model Vitrine_M;
Model Finn_M;

Skybox skybox;

//materiales
Material Material_brillante;
Material Material_opaco;


//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// luz direccional
DirectionalLight mainLight;

//para declarar varias luces de tipo pointlight
PointLight pointLights1[MAX_POINT_LIGHTS];
PointLight pointLights2[MAX_POINT_LIGHTS];
PointLight pointLights3[MAX_POINT_LIGHTS];

SpotLight spotLights1[MAX_SPOT_LIGHTS]; // luz delantera
SpotLight spotLights2[MAX_SPOT_LIGHTS]; // luz trasera

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";


//función de calculo de normales por promedio de vértices 
void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}


void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};

	unsigned int vegetacionIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	   4,5,6,
	   4,6,7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,


	};

	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh* obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh* obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);

	Mesh* obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	calcAverageNormals(vegetacionIndices, 12, vegetacionVertices, 64, 8, 5);

}


void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.5f);

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();
	carroceriaTexture = Texture("Textures/bob_esponja.png");
	carroceriaTexture.LoadTextureA();
	ojosTexture = Texture("Textures/ojos_cars.png");
	ojosTexture.LoadTextureA();
	rinTexture = Texture("Textures/rin.png");
	rinTexture.LoadTextureA();
	llantaTexture = Texture("Textures/llanta.png");
	llantaTexture.LoadTextureA();
	cofreTexture = Texture("Textures/bob_esponja2.png");
	cofreTexture.LoadTextureA();
	FinnTexture = Texture("Textures/finn_color.png");
	FinnTexture.LoadTextureA();
	VitrineTexture = Texture("Textures/vitrine_t.png");
	VitrineTexture.LoadTextureA();



	Farola_M = Model();
	Farola_M.LoadModel("Models/farola.obj");

	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);

	Carroceria_Bob = Model();
	Carroceria_Bob.LoadModel("Models/carroceria.fbx");
	Cofre_Bob = Model();
	Cofre_Bob.LoadModel("Models/cofre.fbx");
	Llanta_M1 = Model();
	Llanta_M1.LoadModel("Models/Llanta1.fbx");
	Llanta_M2 = Model();
	Llanta_M2.LoadModel("Models/Llanta2.fbx");
	Llanta_M3 = Model();
	Llanta_M3.LoadModel("Models/Llanta3.fbx");
	Llanta_M4 = Model();
	Llanta_M4.LoadModel("Models/Llanta4.fbx");

	Vitrine_M = Model();
	Vitrine_M.LoadModel("Models/Vitrine.fbx");
	Finn_M = Model();
	Finn_M.LoadModel("Models/Finn.fbx");

	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.3f, 0.3f, // intensidad ambiental (radiacion de la luz), intensidad difusa
		0.0f, -1.0f, 0.0f);

	//contador de luces puntuales
	unsigned int pointLightCount = 0;

	//Declaración de primer luz puntual
	pointLights1[0] = PointLight(0.2f, 0.8f, 0.6f,
		0.0f, 1.0f,
		0.0f, 0.5f, 2.5f, 
		0.3f, 0.2f, 0.1f);
	pointLightCount++;

	pointLights2[0] = PointLight(0.8f, 0.95f, 0.75f,
		0.0f, 1.0f,
		10.0f, -0.1f, -4.0, 
		1.0f, 0.09f, 0.032f);
	pointLightCount++;

	pointLights3[0] = pointLights1[0];
	pointLights3[1] = pointLights2[0];


	unsigned int spotLightCount = 0; //contador de spotlights

	// linterna
	spotLights1[0] = SpotLight(1.0f, 0.6f, 1.0f, //color moradito
		1.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		15.0f);
	spotLightCount++;

	// luz delantera
	spotLights1[1] = SpotLight(0.0f, 0.3f, 1.0f,
		1.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		1.0f, 0.007f, 0.0002f,
		12.5f);
	spotLightCount++;

	// luz trasera
	spotLights1[2] = SpotLight(1.0f, 0.7f, 0.7f,
		1.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.007f, 0.0002f,
		12.5f);
	spotLightCount++;

	spotLights2[0] = spotLights1[0];
	spotLights2[1] = spotLights1[2];
	spotLights2[2] = spotLights1[1];

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);

	bool bandera_mov = true;
	bool bandera_luces[2] = { false, false };

	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		//Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();

		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);

		if (mainWindow.getTeclaPresionada() == GLFW_KEY_3) {
			bandera_luces[0] = true;
		}
		if (mainWindow.getTeclaPresionada() == GLFW_KEY_4) {
			bandera_luces[0] = false;
		}

		if (mainWindow.getTeclaPresionada() == GLFW_KEY_5) {
			bandera_luces[1] = true;
		}
		if (mainWindow.getTeclaPresionada() == GLFW_KEY_6) {
			bandera_luces[1] = false;
		}

		if (bandera_luces[0] && !bandera_luces[1]) {
			shaderList[0].SetPointLights(pointLights1, 1);
		}
		if (bandera_luces[1] && !bandera_luces[0]) {
			shaderList[0].SetPointLights(pointLights2, 1);
		}
		if (bandera_luces[0] && bandera_luces[1]) {
			shaderList[0].SetPointLights(pointLights3, pointLightCount);
		}
		if (!bandera_luces[0] && !bandera_luces[1]) {
			shaderList[0].SetPointLights(pointLights3, 0);
		}

		if (mainWindow.getTeclaPresionada() == GLFW_KEY_2) {
			shaderList[0].SetSpotLights(spotLights2, spotLightCount - 1);
			bandera_mov = false;
		}
		else {
			shaderList[0].SetSpotLights(spotLights1, spotLightCount - 1);
			bandera_mov = true;
		}

		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();


		//Pared
		model = glm::mat4(1.0);
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, glm::vec3(-30.0f, -30.0f, 0.0f));
		model = glm::scale(model, glm::vec3(10.0f, 1.0f, 10.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();

		//Carroceria
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(20.0f + mainWindow.getmovx(), -0.8f, 3.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		carroceriaTexture.UseTexture();
		Carroceria_Bob.RenderModel();

		// Cofre
		model = modelaux;
		model = glm::translate(model, glm::vec3(-2.8f, 2.9f, 0.0f));
		model = glm::scale(model, glm::vec3(1.8f, 1.8f, 1.8f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacionCofre()), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		cofreTexture.UseTexture();
		Cofre_Bob.RenderModel();

		// luz cofre
		glm::vec3 linternaCofrePos = glm::vec3(-5.0f + mainWindow.getmovx(), 2.0f, 0.0f);
		glm::vec3 linternaCofreDir = glm::normalize(glm::vec3(
			-1.0f + abs(mainWindow.getarticulacionCofre() / 90.0f),
			abs(mainWindow.getarticulacionCofre() / 90.0f),
			0.0f));
		if (bandera_mov) {
			spotLights1[0].SetFlash(linternaCofrePos, linternaCofreDir);
			// luz delantera
			spotLights1[1].SetFlash(glm::vec3(-5.0f + mainWindow.getmovx(), 1.5f, 0.0f)
				, glm::vec3(-1.0f, 0.0f, 0.0f));
		}
		else {
			spotLights2[0].SetFlash(linternaCofrePos, linternaCofreDir);
			// luz trasera
			spotLights2[1].SetFlash(glm::vec3(30.0f + mainWindow.getmovx(), 1.5f, 0.0f)
				, glm::vec3(1.0f, 0.0f, 0.0f));
		}

		//Llanta delantera derecha
		model = modelaux;
		model = glm::translate(model, glm::vec3(-3.8f, 1.0f, 2.6f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		llantaTexture.UseTexture();
		Llanta_M1.RenderModel();

		//Llanta trasera derecha
		model = modelaux;						//Y		//Z			//X
		model = glm::translate(model, glm::vec3(4.1f,	1.0f,		2.3f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		llantaTexture.UseTexture();
		Llanta_M2.RenderModel();

		////Llanta delantera izquierda
		model = modelaux;
		model = glm::translate(model, glm::vec3(-0.1f, -0.2f, -5.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		llantaTexture.UseTexture();
		Llanta_M3.RenderModel();

		//Llanta trasera izquierda
		model = modelaux;
		model = glm::translate(model, glm::vec3(3.7f, 1.0f, -2.6f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		llantaTexture.UseTexture();
		Llanta_M4.RenderModel();

		//lampara

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Farola_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(10.0f, 1.8f, -7.0));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		VitrineTexture.UseTexture();
		Vitrine_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(10.2f, -0.4f, -4.0));
		model = glm::scale(model, glm::vec3(0.5f,0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		FinnTexture.UseTexture();
		Finn_M.RenderModel();

		glUseProgram(0);
		mainWindow.swapBuffers();
	}

	return 0;
}