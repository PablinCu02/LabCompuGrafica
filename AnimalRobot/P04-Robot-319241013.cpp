/*Práctica 4: Modelado Jerárquico.
Se implementa el uso de matrices adicionales para almacenar información de transformaciones geométricas que se quiere
heredar entre diversas instancias para que estén unidas
Teclas de la F a la K para rotaciones de articulaciones
*/
#include <stdio.h>
#include <string.h>
#include<cmath>
#include<vector>
#include <glew.h>
#include <glfw3.h>
//glm
#include<glm.hpp>
#include<gtc\matrix_transform.hpp>
#include<gtc\type_ptr.hpp>
#include <gtc\random.hpp>
//clases para dar orden y limpieza al còdigo
#include"Mesh.h"
#include"Shader.h"
#include"Sphere.h"
#include"Window.h"
#include"Camera.h"
//tecla E: Rotar sobre el eje X
//tecla R: Rotar sobre el eje Y
//tecla T: Rotar sobre el eje Z
using std::vector;
//Dimensiones de la ventana
const float toRadians = 3.14159265f / 180.0; //grados a radianes
const float PI = 3.14159265f;
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;
Camera camera;
Window mainWindow;
vector<Mesh*> meshList;
vector<Shader>shaderList;
//Vertex Shader
static const char* vShader = "shaders/shader.vert";
static const char* fShader = "shaders/shader.frag";
Sphere sp = Sphere(1.0, 20, 20); //recibe radio, slices, stacks

void CrearCubo()
{
	unsigned int cubo_indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// right
		1, 5, 6,
		6, 2, 1,
		// back
		7, 6, 5,
		5, 4, 7,
		// left
		4, 0, 3,
		3, 7, 4,
		// bottom
		4, 5, 1,
		1, 0, 4,
		// top
		3, 2, 6,
		6, 7, 3
	};

	GLfloat cubo_vertices[] = {
		// front
		-0.5f, -0.5f,  0.5f,
		0.5f, -0.5f,  0.5f,
		0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		// back
		-0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f
	};
	Mesh* cubo = new Mesh();
	cubo->CreateMesh(cubo_vertices, cubo_indices, 24, 36);
	meshList.push_back(cubo);
}

// Pirámide triangular regular
void CrearPiramideTriangular()
{
	unsigned int indices_piramide_triangular[] = {
			0,1,2,
			1,3,2,
			3,0,2,
			1,0,3

	};
	GLfloat vertices_piramide_triangular[] = {
		-0.5f, -0.5f,0.0f,	//0
		0.5f,-0.5f,0.0f,	//1
		0.0f,0.5f, -0.25f,	//2
		0.0f,-0.5f,-0.5f,	//3

	};
	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices_piramide_triangular, indices_piramide_triangular, 12, 12);
	meshList.push_back(obj1);

}

/*
Crear cilindro y cono con arreglos dinámicos vector creados en el Semestre 2023 - 1 : por Sánchez Pérez Omar Alejandro
*/
void CrearCilindro(int res, float R) {

	//constantes utilizadas en los ciclos for
	int n, i;
	//cálculo del paso interno en la circunferencia y variables que almacenarán cada coordenada de cada vértice
	GLfloat dt = 2 * PI / res, x, z, y = -0.5f;

	vector<GLfloat> vertices;
	vector<unsigned int> indices;

	//ciclo for para crear los vértices de las paredes del cilindro
	for (n = 0; n <= (res); n++) {
		if (n != res) {
			x = R * cos((n)*dt);
			z = R * sin((n)*dt);
		}
		//caso para terminar el círculo
		else {
			x = R * cos((0) * dt);
			z = R * sin((0) * dt);
		}
		for (i = 0; i < 6; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(y);
				break;
			case 2:
				vertices.push_back(z);
				break;
			case 3:
				vertices.push_back(x);
				break;
			case 4:
				vertices.push_back(0.5);
				break;
			case 5:
				vertices.push_back(z);
				break;
			}
		}
	}

	//ciclo for para crear la circunferencia inferior
	for (n = 0; n <= (res); n++) {
		x = R * cos((n)*dt);
		z = R * sin((n)*dt);
		for (i = 0; i < 3; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(-0.5f);
				break;
			case 2:
				vertices.push_back(z);
				break;
			}
		}
	}

	//ciclo for para crear la circunferencia superior
	for (n = 0; n <= (res); n++) {
		x = R * cos((n)*dt);
		z = R * sin((n)*dt);
		for (i = 0; i < 3; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(0.5);
				break;
			case 2:
				vertices.push_back(z);
				break;
			}
		}
	}

	//Se generan los indices de los vértices
	for (i = 0; i < vertices.size(); i++) indices.push_back(i);

	//se genera el mesh del cilindro
	Mesh* cilindro = new Mesh();
	cilindro->CreateMeshGeometry(vertices, indices, vertices.size(), indices.size());
	meshList.push_back(cilindro);
}



//función para crear pirámide cuadrangular unitaria
void CrearPiramideCuadrangular()
{
	vector<unsigned int> piramidecuadrangular_indices = {
		0,3,4,
		3,2,4,
		2,1,4,
		1,0,4,
		0,1,2,
		0,2,3 // Corregido para formar la base correctamente
	};
	vector<GLfloat> piramidecuadrangular_vertices = {
		// Base
		-0.5f,-0.5f, 0.5f, // 0
		 0.5f,-0.5f, 0.5f, // 1
		 0.5f,-0.5f,-0.5f, // 2
		-0.5f,-0.5f,-0.5f, // 3
		// Pico
		 0.0f, 0.5f, 0.0f, // 4
	};
	Mesh* piramide = new Mesh();
	piramide->CreateMeshGeometry(piramidecuadrangular_vertices, piramidecuadrangular_indices, 15, 18);
	meshList.push_back(piramide);
}


void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}


int main()
{
	mainWindow = Window(1280, 720);
	mainWindow.Initialise();
	//Cilindro y cono reciben resolución (slices, rebanadas) y Radio de circunferencia de la base y tapa


	CrearCubo();//índice 0 en MeshList
	CrearCilindro(20, 1.0f); //Indice 1 en MeshList Se usará para las llantas
	CrearPiramideCuadrangular(); //índice 2 en MeshList. Para la base.
	CreateShaders();


	camera = Camera(glm::vec3(0.0f, 5.0f, 15.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, -10.0f, 0.2f, 0.2f);
	GLuint uniformProjection = 0;
	GLuint uniformModel = 0;
	GLuint uniformView = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(glm::radians(60.0f), mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);


	//Loop mientras no se cierra la ventana
	sp.init();//inicializar esfera
	sp.load();//enviar la esfera al shader

	glm::mat4 model(1.0);//Inicializar matriz de Modelo 4x4
	glm::mat4 modelaux(1.0);//Inicializar matriz de Modelo 4x4 auxiliar para la jerarquía
	glm::mat4 modelaux2(1.0); // Matriz auxiliar adicional para la base y las llantas

	glm::vec3 color = glm::vec3(0.0f, 0.0f, 0.0f);//inicializar Color para enviar a variable Uniform;

	while (!mainWindow.getShouldClose())
	{

		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;
		//Recibir eventos del usuario
		glfwPollEvents();
		//Cámara
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		//Limpiar la ventana
		glClearColor(0.2f, 0.2f, 0.25f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//Se agrega limpiar el buffer de profundidad
		shaderList[0].useShader();
		uniformModel = shaderList[0].getModelLocation();
		uniformProjection = shaderList[0].getProjectLocation();
		uniformView = shaderList[0].getViewLocation();
		uniformColor = shaderList[0].getColorLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));


		/*
		Desarrollo del ejercicio
		******Descripción de botones para manipular el modelo:************
		** Articulaciones (Patas):
		***Tecla Q: Pata delantera izquierda (atrás)
		***Tecla 1: Pata delantera izquierda (delante)
		* 
		***Tecla E: Pata delantera derecha (arriba)
		***Tecla 2: Pata delantera derecha (abajo)
		* 
		***Tecla Shift + Q: Pata trasera izquierda (arriba)
		***Tecla Shift + 1: Pata trasera izquierda (abajo)
		* 
		***Tecla Shift + E: Pata trasera derecha (arriba)
		***Tecla Shift + 2: Pata trasera derecha (abajo)
		* 
		***Tecla Z: Articulación inferior pata delantera izquierda (arriba)
		***Tecla X: Articulación inferior pata delantera izquierda (abajo)
		***Tecla C: Articulación inferior pata delantera derecha (arriba)
		***Tecla V: Articulación inferior pata delantera derecha (abajo)

		**Cola:
		***Tecla T: Cola arriba y abajo
		***Tecla G: Cola abajo y arriba
		***Tecla F: Cola izquierda
		***Tecla H: Cola derecha
		*/

		//para reiniciar la matriz de modelo con valor de la matriz identidad
		model = glm::mat4(1.0);

		// Suelo
		model = glm::translate(model, glm::vec3(0.0f, -1.7f, 0.0f));
		model = glm::scale(model, glm::vec3(50.0f, 0.1f, 50.0f));
		color = glm::vec3(0.2f, 0.6f, 0.2f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh();

		// Volvemos a resetear la matriz de modelo para que el suelo no afecte al robot
		model = glm::mat4(1.0);

		// Cuerpo (Nodo padre)
		model = glm::translate(model, glm::vec3(0.0f, 2.5f, 0.0f));
		modelaux = model;

		model = glm::scale(model, glm::vec3(2.5f, 2.0f, 4.0f));
		color = glm::vec3(0.8f, 0.8f, 0.8f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh();

		// Base para la cabeza (Hija del cuerpo principal)
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 0.25f, 2.2f));

		model = glm::scale(model, glm::vec3(2.0f, 1.5f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[0]->RenderMesh();

		// CABEZA
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 1.5f, 3.5f));

		model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();

		modelaux2 = model;

		//		OREJAS		//

		// Oreja Derecha
		model = modelaux2;
		model = glm::translate(model, glm::vec3(0.5f, 0.7f, 0.0f));
		model = glm::rotate(model, glm::radians(-15.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.6f, 1.0f, 0.6f));

		color = glm::vec3(0.4f, 0.4f, 0.4f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMesh();

		// Oreja Izquierda
		model = modelaux2;
		model = glm::translate(model, glm::vec3(-0.5f, 0.7f, 0.0f));
		model = glm::rotate(model, glm::radians(15.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.6f, 1.0f, 0.6f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[2]->RenderMesh();

		// Ojos (Hijos de la cabeza)
		color = glm::vec3(0.1f, 0.1f, 0.1f); // Color negro para los ojos
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		// Ojo Derecho
		model = modelaux2; // Partimos del centro de la cabeza
		model = glm::translate(model, glm::vec3(0.4f, 0.2f, 0.85f)); // Hacemos traslacion para ubicar de mejor manera los ojos
		model = glm::scale(model, glm::vec3(0.25f)); 
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();

		// Ojo Izquierdo
		model = modelaux2; // Partimos de nuevo del centro de la cabeza
		model = glm::translate(model, glm::vec3(-0.4f, 0.2f, 0.85f)); // Movemos a la izquierda
		model = glm::scale(model, glm::vec3(0.25f)); // Mismo tamaño
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();

		////		 Patas (Hijas del cuerpo principal)		////
		
		// Pata Delantera Izquiera
		model = modelaux;
		model = glm::translate(model, glm::vec3(-1.2f, -0.5f, 1.8f));
		model = glm::rotate(model, glm::radians(mainWindow.getPataDISup()), glm::vec3(1.0f, 0.0f, 0.0f));
		modelaux2 = model;

		// Esfera del hombro
		model = glm::scale(model, glm::vec3(0.5f));
		color = glm::vec3(0.2f, 0.2f, 0.2f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();

		// Parte Superior de la Pata
		model = modelaux2;
		model = glm::translate(model, glm::vec3(0.0f, -0.9f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 1.8f, 0.4f));
		color = glm::vec3(0.8f, 0.8f, 0.8f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMeshGeometry();

		// Rodilla
		model = modelaux2;
		model = glm::translate(model, glm::vec3(0.0f, -1.8f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getPataDIInf()), glm::vec3(1.0f, 0.0f, 0.0f));
		modelaux2 = model;

		// Esfera de la rodilla
		model = glm::scale(model, glm::vec3(0.5f));
		color = glm::vec3(0.2f, 0.2f, 0.2f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();

		// Parte Inferior de la Pata
		model = modelaux2;
		model = glm::translate(model, glm::vec3(0.0f, -0.9f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 1.8f, 0.4f));
		color = glm::vec3(0.8f, 0.8f, 0.8f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMeshGeometry();

		// Pie
		model = modelaux2;
		model = glm::translate(model, glm::vec3(0.0f, -1.8f, 0.1f));
		model = glm::scale(model, glm::vec3(0.5f, 0.4f, 0.6f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMeshGeometry();

		// Para Delantera Derecha
		model = modelaux;
		model = glm::translate(model, glm::vec3(1.2f, -0.5f, 1.8f));
		model = glm::rotate(model, glm::radians(mainWindow.getPataDDSup()), glm::vec3(1.0f, 0.0f, 0.0f));
		modelaux2 = model;

		// Esfera del hombro
		model = glm::scale(model, glm::vec3(0.5f));
		color = glm::vec3(0.2f, 0.2f, 0.2f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();

		// Parte Superior de la Pata
		model = modelaux2;
		model = glm::translate(model, glm::vec3(0.0f, -0.9f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 1.8f, 0.4f));
		color = glm::vec3(0.8f, 0.8f, 0.8f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMeshGeometry();

		// Rodilla
		model = modelaux2;
		model = glm::translate(model, glm::vec3(0.0f, -1.8f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getPataDDInf()), glm::vec3(1.0f, 0.0f, 0.0f));
		modelaux2 = model;

		// Esfera de la rodilla
		model = glm::scale(model, glm::vec3(0.5f));
		color = glm::vec3(0.2f, 0.2f, 0.2f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();

		// Parte Inferior de la Pata
		model = modelaux2;
		model = glm::translate(model, glm::vec3(0.0f, -0.9f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 1.8f, 0.4f));
		color = glm::vec3(0.8f, 0.8f, 0.8f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMeshGeometry();

		// Pie
		model = modelaux2;
		model = glm::translate(model, glm::vec3(0.0f, -1.8f, 0.1f));
		model = glm::scale(model, glm::vec3(0.5f, 0.4f, 0.6f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMeshGeometry();


		// Pata Trasera Derecha
		model = modelaux;
		model = glm::translate(model, glm::vec3(-1.2f, -0.5f, -1.5f));
		model = glm::rotate(model, glm::radians(mainWindow.getPataTISup()), glm::vec3(1.0f, 0.0f, 0.0f));
		modelaux2 = model;

		// Esfera de la cadera
		model = glm::scale(model, glm::vec3(0.5f));
		color = glm::vec3(0.2f, 0.2f, 0.2f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();

		// Parte Superior de la Pata
		model = modelaux2;
		model = glm::translate(model, glm::vec3(0.0f, -0.9f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 1.8f, 0.4f));
		color = glm::vec3(0.8f, 0.8f, 0.8f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMeshGeometry();

		// Rodilla
		model = modelaux2;
		model = glm::translate(model, glm::vec3(0.0f, -1.8f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getPataTIInf()), glm::vec3(1.0f, 0.0f, 0.0f));
		modelaux2 = model;

		// Esfera de la rodilla
		model = glm::scale(model, glm::vec3(0.5f));
		color = glm::vec3(0.2f, 0.2f, 0.2f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();

		// Parte Inferior de la Pata
		model = modelaux2;
		model = glm::translate(model, glm::vec3(0.0f, -0.9f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 1.8f, 0.4f));
		color = glm::vec3(0.8f, 0.8f, 0.8f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMeshGeometry();

		// Pie
		model = modelaux2;
		model = glm::translate(model, glm::vec3(0.0f, -1.8f, 0.1f));
		model = glm::scale(model, glm::vec3(0.5f, 0.4f, 0.6f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMeshGeometry();


		// Pata Trasera Derecha
		model = modelaux;
		model = glm::translate(model, glm::vec3(1.2f, -0.5f, -1.5f));
		model = glm::rotate(model, glm::radians(mainWindow.getPataTDSup()), glm::vec3(1.0f, 0.0f, 0.0f));
		modelaux2 = model;

		// Esfera de la cadera
		model = glm::scale(model, glm::vec3(0.5f));
		color = glm::vec3(0.2f, 0.2f, 0.2f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();

		// Parte Superior de la Pata
		model = modelaux2;
		model = glm::translate(model, glm::vec3(0.0f, -0.9f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 1.8f, 0.4f));
		color = glm::vec3(0.8f, 0.8f, 0.8f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMeshGeometry();

		// Rodilla
		model = modelaux2;
		model = glm::translate(model, glm::vec3(0.0f, -1.8f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getPataTDInf()), glm::vec3(1.0f, 0.0f, 0.0f));
		modelaux2 = model;

		// Esfera de la rodilla
		model = glm::scale(model, glm::vec3(0.5f));
		color = glm::vec3(0.2f, 0.2f, 0.2f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();

		// Parte Inferior de la Pata
		model = modelaux2;
		model = glm::translate(model, glm::vec3(0.0f, -0.9f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 1.8f, 0.4f));
		color = glm::vec3(0.8f, 0.8f, 0.8f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMeshGeometry();

		// Pie
		model = modelaux2;
		model = glm::translate(model, glm::vec3(0.0f, -1.8f, 0.1f));
		model = glm::scale(model, glm::vec3(0.5f, 0.4f, 0.6f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMeshGeometry();

		// COLA (Se descomponerá en 3 segmentos)
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 0.8f, -2.2f));

		model = glm::rotate(model, glm::radians(mainWindow.getColaUD()), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getColaLR()), glm::vec3(0.0f, 1.0f, 0.0f));

		// Segmento 1
		modelaux2 = model;
		model = glm::translate(model, glm::vec3(0.0f, 0.4f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.8f, 0.2f));

		color = glm::vec3(0.2f, 0.2f, 0.2f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMeshGeometry();

		// Segmento 2 
		model = modelaux2;
		model = glm::translate(model, glm::vec3(0.0f, 0.8f, 0.0f));
		model = glm::rotate(model, glm::radians(15.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		modelaux2 = model;

		model = glm::translate(model, glm::vec3(0.0f, 0.4f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.8f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMeshGeometry();

		// Segmento 3
		model = modelaux2;
		model = glm::translate(model, glm::vec3(0.0f, 0.8f, 0.0f));
		model = glm::rotate(model, glm::radians(15.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		modelaux2 = model;

		model = glm::translate(model, glm::vec3(0.0f, 0.4f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.8f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMeshGeometry();

		glUseProgram(0);
		mainWindow.swapBuffers();
	}
	return 0;
}