
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

#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include "Model.h"
#include "Skybox.h"

// Headers de Iluminación 
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"

// Headers para Keyframes 
#include <fstream>
#include <iostream>
#include <string>

const float toRadians = 3.14159265f / 180.0f;


// Sistema Principal 
Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;
Camera camera;
Skybox skybox;
Skybox skyboxNoche; 

// Modelos 
Model Globo_M;
Model Piedra_M; 

// Texturas 
Texture pisoTexture;

// Materiales 
Material Material_brillante;
Material Material_opaco;

// Iluminación
DirectionalLight mainLight;
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

// Control de Tiempo 
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// Shaders 
static const char* vShader = "shaders/shader_light.vert";
static const char* fShader = "shaders/shader_light.frag";

// Estructura para almacenar la posición y rotación de un Keyframe
struct FRAME
{
	float posX;
	float posY;
	float posZ;
	float rotY;
};

// Función para guardar Keyframes en un archivo .txt
void saveKeyframes(const std::vector<FRAME>& keyframes, const std::string& filename) {
	std::ofstream file(filename);
	if (!file.is_open()) {
		std::cerr << "Error al abrir el archivo para guardar!" << std::endl;
		return;
	}
	// Guarda cada variable en una línea
	for (const auto& frame : keyframes) {
		file << frame.posX << " " << frame.posY << " " << frame.posZ << " " << frame.rotY << "\n";
	}
	file.close();
	printf("Keyframes guardados en '%s'\n", filename.c_str());
}

// Función para leer Keyframes desde un archivo .txt
std::vector<FRAME> readKeyframes(const std::string& filename) {
	std::vector<FRAME> keyframes;
	std::ifstream file(filename);
	if (!file.is_open()) {
		std::cerr << "Error al abrir el archivo para leer!" << std::endl;
		return keyframes;
	}
	FRAME frame;
	while (file >> frame.posX >> frame.posY >> frame.posZ >> frame.rotY) {
		keyframes.push_back(frame);
	}
	file.close();
	printf("Keyframes cargados desde '%s'. Total: %d\n", (int)keyframes.size(), filename.c_str());
	return keyframes;
}

// Función de Interpolación 
float interpolation(float a, float b, float t) {
	return a + (b - a) * t;
}

// Función principal de Animación
void animate(float& posX, float& posY, float& posZ, float& rotY,
	std::vector<FRAME>& keyframes, int& frameIndex, float& playIndex, bool& play) {

	if (play && keyframes.size() > 1) {

		// Cuando llega a 1.0, pasamos al siguiente frame.
		if (playIndex > 1.0f) {
			playIndex = 0.0f;
			frameIndex++;
		}

		// Si llegamos al final de la animación, la detenemos y reiniciamos.
		if (frameIndex >= keyframes.size() - 1) {
			frameIndex = 0;
			play = false;
			printf("Animacion terminada.\n");
		}

		// Frames actual y siguiente
		int currentFrame = frameIndex;
		int nextFrame = frameIndex + 1;

		// Interpolamos la posición y rotación entre el frame actual y el siguiente
		posX = interpolation(keyframes[currentFrame].posX, keyframes[nextFrame].posX, playIndex);
		posY = interpolation(keyframes[currentFrame].posY, keyframes[nextFrame].posY, playIndex);
		posZ = interpolation(keyframes[currentFrame].posZ, keyframes[nextFrame].posZ, playIndex);
		rotY = interpolation(keyframes[currentFrame].rotY, keyframes[nextFrame].rotY, playIndex);

		// Aumentamos el índice de interpolación. 
		playIndex += 0.005f;
	}
}

// Variables de Animación por Keyframes del Globo
std::vector<FRAME> KeyFrameGlobo; // Vector para guardar frames
int FrameIndexGlobo = 0;
bool playGlobo = false;            // Bandera para reproducir animacion
float playIndexGlobo = 0.0f;       // Índice de interpolación 

// Posición y Rotación actual del Globo 
float globoPosX = 0.0f;
float globoPosY = 50.5f;
float globoPosZ = -180.0f;
float globoRotY = 90.0f;

// Variables para las teclas de keyframes
bool teclaLPresionada = false;
bool teclaKPresionada = false;
bool teclaGPresionada = false;
bool teclaCPresionada = false;



//funcion de calculo de normales 
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

// Creación de objetos 
void CreateObjects()
{
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

	Mesh* obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);
}

// Creación de Shaders 
void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

int main()
{
	mainWindow = Window(1366, 768);
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();

	// Configuración de Cámara 
	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 50.0f, 0.5f);

	// Carga de Texturas 
	pisoTexture = Texture("Textures/pasto.png");
	pisoTexture.LoadTextureA();

	//// Carga de Modelos 
	Globo_M = Model();
	Globo_M.LoadModel("Models/globo.obj");


	// Configuración de Skybox (Mínimo para el ciclo)
	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");
	skybox = Skybox(skyboxFaces);

	// Configuración de Materiales 
	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);

	// Configuración de Luces 
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.3f, 0.3f,
		0.0f, 0.0f, -1.0f);
	unsigned int pointLightCount = 0;
	unsigned int spotLightCount = 0;
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f, 0.0f, 2.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 5.0f);
	spotLightCount++;


	// Variables Uniform 
	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0, uniformTextureOffset = 0;
	GLuint uniformColor = 0;

	// Proyección 
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	glm::mat4 model(1.0);
	glm::mat4 modelaux(1.0);
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

	// Variables de Avatar 
	int cameraMode = 0; // Modo Libre


	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		lastTime = now;

		glfwPollEvents();
		// Control de Cámara 
		if (cameraMode == 0) {
			camera.keyControl(mainWindow.getsKeys(), deltaTime);
			camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
		}

		// Control del Globo
		float globoMoveSpeed = 20.0f;
		if (mainWindow.getsKeys()[GLFW_KEY_T]) {
			globoPosZ -= globoMoveSpeed * deltaTime;
		}
		if (mainWindow.getsKeys()[GLFW_KEY_B]) {
			globoPosZ += globoMoveSpeed * deltaTime;
		}
		if (mainWindow.getsKeys()[GLFW_KEY_F]) {
			globoPosX -= globoMoveSpeed * deltaTime;
		}
		if (mainWindow.getsKeys()[GLFW_KEY_H]) {
			globoPosX += globoMoveSpeed * deltaTime;
		}
		if (mainWindow.getsKeys()[GLFW_KEY_UP]) {
			globoPosY += globoMoveSpeed * deltaTime;
		}
		if (mainWindow.getsKeys()[GLFW_KEY_DOWN]) {
			globoPosY -= globoMoveSpeed * deltaTime;
		}


		// --- REQUERIDO PARA LA PRÁCTICA (Lógica de Teclado de Keyframes) ---
		if (mainWindow.getsKeys()[GLFW_KEY_L] && !teclaLPresionada) {
			teclaLPresionada = true;
			FRAME newFrame;
			newFrame.posX = globoPosX;
			newFrame.posY = globoPosY;
			newFrame.posZ = globoPosZ;
			newFrame.rotY = globoRotY;
			KeyFrameGlobo.push_back(newFrame);
			printf("Keyframe %d guardado: (%.2f, %.2f, %.2f)\n", (int)KeyFrameGlobo.size(), newFrame.posX, newFrame.posY, newFrame.posZ);
		}
		if (!mainWindow.getsKeys()[GLFW_KEY_L]) {
			teclaLPresionada = false;
		}
		if (mainWindow.getsKeys()[GLFW_KEY_K] && !teclaKPresionada) {
			teclaKPresionada = true;
			if (KeyFrameGlobo.size() > 1) {
				playGlobo = true;
				FrameIndexGlobo = 0;
				playIndexGlobo = 0.0f;
				printf("Reproduciendo animación del globo...\n");
			}
			else {
				printf("¡Error! Necesitas al menos 2 keyframes para reproducir.\n");
			}
		}
		if (!mainWindow.getsKeys()[GLFW_KEY_K]) {
			teclaKPresionada = false;
		}
		if (mainWindow.getsKeys()[GLFW_KEY_G] && !teclaGPresionada) {
			teclaGPresionada = true;
			saveKeyframes(KeyFrameGlobo, "globo_keyframes.txt");
		}
		if (!mainWindow.getsKeys()[GLFW_KEY_G]) {
			teclaGPresionada = false;
		}
		if (mainWindow.getsKeys()[GLFW_KEY_C] && !teclaCPresionada) {
			teclaCPresionada = true;
			KeyFrameGlobo = readKeyframes("globo_keyframes.txt");
		}
		if (!mainWindow.getsKeys()[GLFW_KEY_C]) {
			teclaCPresionada = false;
		}

		animate(globoPosX, globoPosY, globoPosZ, globoRotY,
			KeyFrameGlobo, FrameIndexGlobo, playIndexGlobo, playGlobo);

		// RENDERIZADO 
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//// DIBUJAR EL SKYBOX 
		glm::mat4 viewMatrixSkybox = camera.calculateViewMatrix();
		skybox.DrawSkybox(viewMatrixSkybox, projection);



		// CONFIGURACIÓN DEL SHADER 
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));

		// CÁLCULO DE MATRIZ DE VISTA Y POSICIÓN DE OJO 
		glm::mat4 viewMatrix = camera.calculateViewMatrix();
		glm::vec3 eyePosition = camera.getCameraPosition();
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniform3f(uniformEyePosition, eyePosition.x, eyePosition.y, eyePosition.z);

		// CONFIGURACIÓN DE LUCES 
		glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);

		// Renderizado de la Escena 

		// Piso
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[0]->RenderMesh(); 

		// Renderizar el Globo
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(globoPosX, globoPosY, globoPosZ));
		model = glm::rotate(model, glm::radians(globoRotY), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(14.0f, 14.0f, 14.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Globo_M.RenderModel();


		// Finalización del Frame 
		glUseProgram(0);
		mainWindow.swapBuffers();
	}

	return 0;
}