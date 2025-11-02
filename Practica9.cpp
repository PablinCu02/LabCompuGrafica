/*
Animación:
Sesión 1:
Simple o básica:Por banderas y condicionales (más de 1 transformación geométrica se ve modificada
Compleja: Por medio de funciones y algoritmos.
Textura Animada
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

//variables para animación
float angulovaria = 0.0f;



Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture AgaveTexture;

Texture AdvTime_FontTexture;
Texture Marco;
Texture Puerta;
Texture dadoTexture;

Skybox skybox;

Model Marco_M;
Model Letrero_M;
Model PuertaDerecha_M;
Model PuertaIzquierda_M;
Model Dado_M;

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
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";


//cálculo del promedio de las normales para sombreado de Phong
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

	unsigned int AdvTimeIndices[] = {
		0, 1, 2,
		0, 2, 3,
	};

	GLfloat AdvTimeVertices[] = {
		//   X      Y      Z     U       V        Nx    Ny    Nz
		-0.5f, 0.0f,  0.5f,   0.1f,   0.98f,   0.0f, -1.0f, 0.0f, // arriba dere  
		 0.5f, 0.0f,  0.5f,   0.01f,     0.98f,   0.0f, -1.0f, 0.0f, // arriba izq
		 0.5f, 0.0f, -0.5f,   0.01f,     0.9f,  0.0f, -1.0f, 0.0f, // abajo izq
		-0.5f, 0.0f, -0.5f,   0.1f,   0.9f,  0.0f, -1.0f, 0.0f  // abajo der
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

	Mesh* obj5 = new Mesh();
	obj5->CreateMesh(AdvTimeVertices, AdvTimeIndices, 32, 6);
	meshList.push_back(obj5);
}


void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

void setLetterUV(float* vertices, int col, int row)
{
	float uStep = 1.0f / 9.0f;  // 9 columnas
	float vStep = 1.0f / 3.0f;  // 3 filas
	float u0 = col * uStep;
	float v0 = 1.0f - (row + 1) * vStep;
	float u1 = u0 + uStep;
	float v1 = v0 + vStep;

	// Actualiza las coordenadas UV del plano
	vertices[3] = u0; vertices[4] = v1;  // arriba izq
	vertices[11] = u1; vertices[12] = v1;  // arriba der
	vertices[19] = u1; vertices[20] = v0;  // abajo der
	vertices[27] = u0; vertices[28] = v0;  // abajo izq
}


int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.5f, 0.5f);

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();
	AgaveTexture = Texture("Textures/Agave.tga");
	AgaveTexture.LoadTextureA();
	Marco = Texture("Textures/Corrugated_Metal_Sheet_teendf3q_1K_BaseColor.jpg");
	Marco.LoadTextureA();
	Puerta = Texture("Textures/Wooden_Log_Wall_udlmecoew_1K_BaseColor.jpg");
	Puerta.LoadTextureA();
	dadoTexture = Texture("Textures/texturaDado.png");
	dadoTexture.LoadTextureA();

	AdvTime_FontTexture = Texture("Textures/AdvTime_Font.png");
	AdvTime_FontTexture.LoadTextureA();

	Marco_M = Model();
	Marco_M.LoadModel("Models/Marco.fbx");
	Letrero_M = Model();
	Letrero_M.LoadModel("Models/Letrero.fbx");
	PuertaDerecha_M = Model();
	PuertaDerecha_M.LoadModel("Models/PuertaDerecha.fbx");
	PuertaIzquierda_M = Model();
	PuertaIzquierda_M.LoadModel("Models/PuertaIzquierda.fbx");
	Dado_M = Model();
	Dado_M.LoadModel("Models/Dado.fbx");

	glm::vec2 toffset = glm::vec2(0.0f, 0.0f);
	float toffsetletraU = 0.0f;
	float toffsetletraV = 0.0f;
	
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


	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.3f, 0.3f,
		0.0f, 0.0f, -1.0f);
	//contador de luces puntuales
	unsigned int pointLightCount = 0;
	//Declaración de primer luz puntual
	pointLights[0] = PointLight(1.0f, 0.0f, 0.0f,
		0.0f, 1.0f,
		0.0f, 2.5f, 1.5f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;

	unsigned int spotLightCount = 0;
	//linterna
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		5.0f);
	spotLightCount++;

	//luz fija
	spotLights[1] = SpotLight(0.0f, 0.0f, 1.0f,
		1.0f, 2.0f,
		5.0f, 10.0f, 0.0f,
		0.0f, -5.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		15.0f);
	spotLightCount++;



	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0, uniformTextureOffset = 0;
	GLuint uniformColor = 0;


	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);

	glm::mat4 model(1.0);
	glm::mat4 modelaux(1.0);
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

	std::vector<glm::vec2> offsets = {
	glm::vec2(0.65f, 0.57f), // P
	glm::vec2(0.88f, 0.57f), // R
	glm::vec2(0.57f, 0.56f), // O
	glm::vec2(0.651f,0.46f), // Y
	glm::vec2(0.45f, 0.67f), // E
	glm::vec2(0.23f, 0.67f), // C
	glm::vec2(0.11f, 0.46f), // T
	glm::vec2(0.57f, 0.56f), // O
	glm::vec2(0.23f, 0.67f), // C
	glm::vec2(0.655f,0.67f), // G
	glm::vec2(0.45f, 0.67f), // E
	glm::vec2(0.86f, 0.67f), // I
	glm::vec2(0.77f, 0.67f), // H
	glm::vec2(0.23f, 0.67f)  // C
	};


	static float tiempoAcumulado = 0.0f;
	static int indiceInicio = 0;          
	int letrasVisibles = 4;               
	float intervaloCambio = 40.0f;        
	glm::vec3 basePos(-12.0f, 2.5f, 9.5f);
	float separacion = 1.5f;
	glm::vec3 escalaLetras(1.5f, 2.0f, 2.0f);

	bool abierta = false;
	float ang = 0.0f;      
	float vel = 90.0f;     
	static bool teclaOPresionada = false; 


	
	bool puertaDerechaAbierta = false;
	bool puertaDerechaAnimando = false;
	bool teclaPPresionada = false;

	float progresoAnim = 0.0f;
	float duracionFase = 1.0f;
	int fase = 0; 
	glm::vec3 puertaDerechaPos(0.0f);

	//Animación del Dado 
	bool dadoAnimando = false;
	bool teclaIPresionada = false;
	bool dadoEnSuelo = false;

	glm::vec3 posDado(5.0f, 10.0f, -5.0f);
	glm::vec3 velDado(0.0f, 0.0f, 0.0f);  
	float gravedad = -9.8f;        
	float rotacionDado = 0.0f;
	glm::vec3 rotDadoActual(0.0f); 
	glm::vec3 rotVelocidad(180.0f, 240.0f, 200.0f); 

	int caraDado = 0;                      
	
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		angulovaria += 0.5f * deltaTime;



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
		uniformTextureOffset = shaderList[0].getOffsetLocation();

		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		// luz ligada a la cámara de tipo flash
		glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);

		tiempoAcumulado += deltaTime;
		if (tiempoAcumulado >= intervaloCambio) {
			tiempoAcumulado = 0.0f;
			indiceInicio++;
			if (indiceInicio > (int)offsets.size() - letrasVisibles)
				indiceInicio = 0;              
		}

		// Abrir/cerrar con tecla O 
		if (mainWindow.getsKeys()[GLFW_KEY_O]) {
			if (!teclaOPresionada) {
				abierta = !abierta; 
				teclaOPresionada = true;
			}
		}
		else {
			teclaOPresionada = false;
		}

		// Animación 
		float targetAng = abierta ? -90.0f : 0.0f; 
		if (fabs(ang - targetAng) > 0.1f) {
			
			float step = vel * deltaTime/10;
			if (ang < targetAng) ang = std::min(ang + step, targetAng);
			else ang = std::max(ang - step, targetAng);
		}

		
		// Animación de Puerta Derecha con tecla P
		if (mainWindow.getsKeys()[GLFW_KEY_P]) {
			if (!teclaPPresionada && !puertaDerechaAnimando) {
				puertaDerechaAnimando = true;
				teclaPPresionada = true;
				if (!puertaDerechaAbierta) {
					fase = 1;
				}
				else {
					fase = 3;
				}
				progresoAnim = 0.0f;
			}
		}
		else {
			teclaPPresionada = false;
		}

		// Lógica de movimiento de la puerta 
		if (puertaDerechaAnimando) {
			progresoAnim += deltaTime/20;
			float t = progresoAnim / duracionFase;
			if (fase == 1) { 
				puertaDerechaPos.z = glm::min(t, 1.0f) * 0.5f;
				if (t >= 1.0f) { fase = 2; progresoAnim = 0.0f; }
			}
			else if (fase == 2) { 
				puertaDerechaPos = glm::vec3(glm::min(t, 1.0f) * 4.0f, 0.0f, 1.0f);
				if (t >= 1.0f) {
					puertaDerechaAnimando = false;
					puertaDerechaAbierta = true;
					fase = 0;
				}
			}
			else if (fase == 3) { 
				puertaDerechaPos = glm::vec3((1.0f - glm::min(t, 1.0f)) * 4.0f, 0.0f, 1.0f);
				if (t >= 1.0f) { fase = 4; progresoAnim = 0.0f; }
			}
			else if (fase == 4) { 
				puertaDerechaPos.z = (1.0f - glm::min(t, 1.0f)) * 0.5f;
				if (t >= 1.0f) {
					puertaDerechaAnimando = false;
					puertaDerechaAbierta = false;
					fase = 0;
				}
			}
		}

		// Animacion del Dado con la tecla I
		if (mainWindow.getsKeys()[GLFW_KEY_I]) {
			if (!teclaIPresionada) {
				teclaIPresionada = true;
				if (!dadoAnimando && !dadoEnSuelo) {
					dadoAnimando = true;
					velDado = glm::vec3(0.0f, 0.0f, 0.0f);
					rotDadoActual = glm::vec3(0.0f);
					rotVelocidad = glm::vec3(
						180.0f + rand() % 90,
						180.0f + rand() % 90,
						180.0f + rand() % 90
					);
					caraDado = rand() % 9;
				}
				else if (dadoEnSuelo) {
					dadoEnSuelo = false;
					dadoAnimando = false;
					posDado = glm::vec3(5.0f, 10.0f, -5.0f);
					velDado = glm::vec3(0.0f);
					rotDadoActual = glm::vec3(0.0f);
				}
			}
		}
		else {
			teclaIPresionada = false;
		}

		// Actualizacion del movimiento y rotacion
		if (dadoAnimando) {
			// aplicar gravedad
			velDado.y += gravedad * deltaTime;
			posDado += velDado * deltaTime;

			// rotación continua en tres ejes
			rotDadoActual += rotVelocidad * deltaTime;

			// detener al tocar el suelo
			if (posDado.y <= 0.5f) {
				posDado.y = 0.5f;
				dadoAnimando = false;
				dadoEnSuelo = true;
			}
		}
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh(); // piso

		/*color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));*/
		//Agave ¿qué sucede si lo renderizan antes del coche y de la pista?
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.5f, -2.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//blending: transparencia o traslucidez
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		AgaveTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		/*
		* Arco con letrero
		*/
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-10.0f, -1.5f, -2.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		modelaux = model;
		
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(glm::vec2(0.0f)));
		Marco.UseTexture();
		Marco_M.RenderModel(); // modelo de arco
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		(uniformTextureOffset, 1, glm::value_ptr(glm::vec2(0.0f)));
		Letrero_M.RenderModel(); // letrero blanco
		

		glm::mat4 letraModel = glm::mat4(1.0f);
		letraModel = glm::rotate(letraModel, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.5f, 2.0f, 2.0f));
		AdvTime_FontTexture.UseTexture();

		for (int i = 0; i < letrasVisibles; i++) {
			
			int idx = (indiceInicio + i) % offsets.size();

			glm::vec2 toffset = offsets[idx];
			glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));

			glm::mat4 letraModel = glm::mat4(1.0f);
			letraModel = glm::rotate(letraModel, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
			float startX = basePos.x + (letrasVisibles - 1) * separacion;
			letraModel = glm::translate(letraModel,
				glm::vec3(startX - i * separacion, basePos.y, basePos.z));

			letraModel = glm::scale(letraModel, escalaLetras);
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(letraModel));

			meshList[4]->RenderMesh();
		}

		//Puertas
		model = modelaux;
		model = glm::translate(model, glm::vec3(-3.3f, 0.0f, 4.7f));
		model = glm::rotate(model, -ang * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(glm::vec2(0.0f)));
		Puerta.UseTexture();
		PuertaIzquierda_M.RenderModel(); 
		
		model = modelaux;
		model = glm::translate(model, glm::vec3(puertaDerechaPos.x, puertaDerechaPos.z, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(glm::vec2(0.0f)));
		Puerta.UseTexture();
		PuertaDerecha_M.RenderModel(); 

		//dado
		model = glm::mat4(1.0);
		model = glm::translate(model, posDado);
		model = glm::rotate(model, glm::radians(40.0f), glm::vec3(1, 0, 0));
		model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0, 0, 1));


		if (dadoAnimando) {
			model = glm::rotate(model, glm::radians(rotDadoActual.x), glm::vec3(1, 0, 0));
			model = glm::rotate(model, glm::radians(rotDadoActual.y), glm::vec3(0, 1, 0));
			model = glm::rotate(model, glm::radians(rotDadoActual.z), glm::vec3(0, 0, 1));
		}
		else if (dadoEnSuelo) {
			switch (caraDado) {
			case 0: break;
			case 1: model = glm::rotate(model, glm::radians(80.0f), glm::vec3(0, 1, 0));
					model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 0, 1));
					break;
			case 2: model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0)); break;
			case 3: model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0)); 
					model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0, 0, 1));
					break;
			case 4: model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 1, 0));
					model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0, 0, 1));
					break; 
			case 5: model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 0, 1)); break;
			case 6: model = glm::rotate(model, glm::radians(360.0f), glm::vec3(0, 1, 0)); break; 
			case 7: model = glm::rotate(model, glm::radians(270.0f), glm::vec3(0, 1, 0)); break;
			case 8: model = glm::rotate(model, glm::radians(270.0f), glm::vec3(0, 1, 0));
					model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0, 0, 1));
					break;
			}
		}

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		dadoTexture.UseTexture();
		Dado_M.RenderModel();
		glDisable(GL_BLEND);
		glUseProgram(0);
		mainWindow.swapBuffers();
	}

	return 0;
}