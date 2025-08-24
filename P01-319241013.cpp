#include <stdio.h>
#include <string.h>
#include <glew.h>
#include <glfw3.h>

#include <cstdlib>
#include <ctime>

//Dimensiones de la ventana
const int WIDTH = 800, HEIGHT = 800;
GLuint VAO, VBO, shader;

//LENGUAJE DE SHADER (SOMBRAS) GLSL
//Vertex Shader
//recibir color, salida Vcolor
static const char* vShader = "						\n\
#version 330										\n\
layout (location =0) in vec3 pos;					\n\
void main()											\n\
{													\n\
gl_Position=vec4(pos.x,pos.y,pos.z,1.0f); 			\n\
}";

//Fragment Shader
//recibir Vcolor y dar de salida color
static const char* fShader = "						\n\
#version 330										\n\
out vec4 color;										\n\
void main()											\n\
{													\n\
	color = vec4(0.6f,0.4f,0.7f,1.0f);	 			\n\
}";


// --- NUEVA FUNCIÓN: crear cuadrado y rombo ---
void CrearFiguras()
{
	GLfloat vertices[] = {
	   //2 triangulos para el cuadrado
		-0.6f,  0.3f, 0.0f,   
		-0.2f,  0.3f, 0.0f,   
		-0.2f, -0.3f, 0.0f,   

		-0.6f,  0.3f, 0.0f,   
		-0.2f, -0.3f, 0.0f,   
		-0.6f, -0.3f, 0.0f,  

		//2 triangulos para el rombo
		 0.2f,  0.0f, 0.0f,   
		 0.6f,  0.3f, 0.0f,   
		 1.0f,  0.0f, 0.0f,   

		 0.2f,  0.0f, 0.0f,   
		 1.0f,  0.0f, 0.0f,   
		 0.6f, -0.3f, 0.0f    
	};//Vertices en sentido antihorario

	glGenVertexArrays(1, &VAO); //generar 1 VAO
	glBindVertexArray(VAO);//asignar VAO

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); //pasarle los datos al VBO asignando tamano, los datos y en este caso es estático pues no se modificarán los valores

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (GLvoid*)0);//Stride en caso de haber datos de color por ejemplo, es saltar cierta cantidad de datos
	glEnableVertexAttribArray(0);
	//agregar valores a vèrtices y luego declarar un nuevo vertexAttribPointer
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}


void AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType) //Función para agregar los shaders a la tarjeta gráfica

//the Program recibe los datos de theShader

{
	GLuint theShader = glCreateShader(shaderType);//theShader es un shader que se crea de acuerdo al tipo de shader: vertex o fragment
	const GLchar* theCode[1];
	theCode[0] = shaderCode;//shaderCode es el texto que se le pasa a theCode
	GLint codeLength[1];
	codeLength[0] = strlen(shaderCode);//longitud del texto
	glShaderSource(theShader, 1, theCode, codeLength);//Se le asigna al shader el código
	glCompileShader(theShader);//Se comila el shader
	GLint result = 0;
	GLchar eLog[1024] = { 0 };
	//verificaciones y prevención de errores
	glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		printf("EL error al compilar el shader %d es: %s \n", shaderType, eLog);
		return;
	}
	glAttachShader(theProgram, theShader);//Si no hubo problemas se asigna el shader a theProgram el cual asigna el código a la tarjeta gráfica
}

void CompileShaders() {
	shader = glCreateProgram(); //se crea un programa
	if (!shader)
	{
		printf("Error creando el shader");
		return;
	}
	AddShader(shader, vShader, GL_VERTEX_SHADER);//Agregar vertex shader
	AddShader(shader, fShader, GL_FRAGMENT_SHADER);//Agregar fragment shader
	//Para terminar de linkear el programa y ver que no tengamos errores
	GLint result = 0;
	GLchar eLog[1024] = { 0 };
	glLinkProgram(shader);//se linkean los shaders a la tarjeta gráfica
	//verificaciones y prevención de errores
	glGetProgramiv(shader, GL_LINK_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		printf("EL error al linkear es: %s \n", eLog);
		return;
	}
	glValidateProgram(shader);
	glGetProgramiv(shader, GL_VALIDATE_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		printf("EL error al validar es: %s \n", eLog);
		return;
	}
}

void crearIniciales() {
	GLfloat vertices[] = {

		// LETRA P

		// 2 triángulos para la barra vertical izquierda
		-0.75f,  0.60f, 0.0f,   -0.70f,  0.60f, 0.0f,   -0.70f, -0.60f, 0.0f,
		-0.75f,  0.60f, 0.0f,   -0.70f, -0.60f, 0.0f,   -0.75f, -0.60f, 0.0f,

		// 2 triángulos para la barra horizontal superior
		-0.75f,  0.60f, 0.0f,   -0.45f,  0.60f, 0.0f,   -0.45f,  0.50f, 0.0f,
		-0.75f,  0.60f, 0.0f,   -0.45f,  0.50f, 0.0f,   -0.75f,  0.50f, 0.0f,

		// 2 triángulos para el lateral derecho superior
		-0.50f,  0.60f, 0.0f,   -0.45f,  0.60f, 0.0f,   -0.45f,  0.20f, 0.0f,
		-0.50f,  0.60f, 0.0f,   -0.45f,  0.20f, 0.0f,   -0.50f,  0.20f, 0.0f,

		// 2 triángulos para la barra horizontal del medio
		-0.75f,  0.25f, 0.0f,   -0.50f,  0.25f, 0.0f,   -0.50f,  0.15f, 0.0f,
		-0.75f,  0.25f, 0.0f,   -0.50f,  0.15f, 0.0f,   -0.75f,  0.15f, 0.0f,

		// 2 triángulos para el lateral derecho inferior (cerrar la panza)
		-0.50f,  0.20f, 0.0f,   -0.45f,  0.20f, 0.0f,   -0.45f,  0.15f, 0.0f,
		-0.50f,  0.20f, 0.0f,   -0.45f,  0.15f, 0.0f,   -0.50f,  0.15f, 0.0f,


		// LETRA A  

		// 2 triángulos para la pierna izquierda
		-0.15f,  0.60f, 0.0f,   -0.10f,  0.60f, 0.0f,   -0.10f, -0.60f, 0.0f,
		-0.15f,  0.60f, 0.0f,   -0.10f, -0.60f, 0.0f,   -0.15f, -0.60f, 0.0f,

		// 2 triángulos para la pierna derecha
		0.10f,  0.60f, 0.0f,    0.15f,  0.60f, 0.0f,    0.15f, -0.60f, 0.0f,
		0.10f,  0.60f, 0.0f,    0.15f, -0.60f, 0.0f,    0.10f, -0.60f, 0.0f,

		// 2 triángulos para la barra horizontal superior
		-0.15f,  0.60f, 0.0f,    0.15f,  0.60f, 0.0f,    0.15f,  0.50f, 0.0f,
		-0.15f,  0.60f, 0.0f,    0.15f,  0.50f, 0.0f,   -0.15f,  0.50f, 0.0f,

		// 2 triángulos para la barra horizontal del medio
		-0.10f,  0.05f, 0.0f,    0.10f,  0.05f, 0.0f,    0.10f, -0.05f, 0.0f,
		-0.10f,  0.05f, 0.0f,    0.10f, -0.05f, 0.0f,   -0.10f, -0.05f, 0.0f,

		// 2 triángulos para la barra vertical izquierda
		0.45f,  0.60f, 0.0f,    0.50f,  0.60f, 0.0f,    0.50f, -0.60f, 0.0f,
		0.45f,  0.60f, 0.0f,    0.50f, -0.60f, 0.0f,    0.45f, -0.60f, 0.0f,


		// LETRA B
		
		// 2 triángulos para la barra horizontal superior
		0.45f,  0.60f, 0.0f,    0.75f,  0.60f, 0.0f,    0.75f,  0.50f, 0.0f,
		0.45f,  0.60f, 0.0f,    0.75f,  0.50f, 0.0f,    0.45f,  0.50f, 0.0f,

		// 2 triángulos para el lateral derecho superior (panza arriba)
		0.70f,  0.60f, 0.0f,    0.75f,  0.60f, 0.0f,    0.75f,  0.20f, 0.0f,
		0.70f,  0.60f, 0.0f,    0.75f,  0.20f, 0.0f,    0.70f,  0.20f, 0.0f,

		// 2 triángulos para la barra horizontal del medio (EXTENDIDA a x = 0.75)
		0.45f,  0.20f, 0.0f,    0.75f,  0.20f, 0.0f,    0.75f,  0.10f, 0.0f,
		0.45f,  0.20f, 0.0f,    0.75f,  0.10f, 0.0f,    0.45f,  0.10f, 0.0f,

		// 2 triángulos para la barra horizontal inferior
		0.45f, -0.50f, 0.0f,    0.75f, -0.50f, 0.0f,    0.75f, -0.60f, 0.0f,
		0.45f, -0.50f, 0.0f,    0.75f, -0.60f, 0.0f,    0.45f, -0.60f, 0.0f,

		// 2 triángulos para el lateral derecho inferior (SUBIDO hasta y = 0.10)
		0.70f,  0.10f, 0.0f,    0.75f,  0.10f, 0.0f,    0.75f, -0.60f, 0.0f,
		0.70f,  0.10f, 0.0f,    0.75f, -0.60f, 0.0f,    0.70f, -0.60f, 0.0f,
	};

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// ¡Importante! sizeof(GLfloat), no sizeof(GL_FLOAT)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}


int main()
{
	srand(time(0));//Semilla para la randomización
	double cambioColor = 0.0; //Variable para el cambio de color
	float red = 0.0f, green = 0.0f, blue = 0.0f; //Variables para los colores

	//Inicialización de GLFW
	if (!glfwInit())
	{
		printf("Falló inicializar GLFW");
		glfwTerminate();
		return 1;
	}

	//****  LAS SIGUIENTES 4 LÍNEAS SE COMENTAN EN DADO CASO DE QUE AL USUARIO NO LE FUNCIONE LA VENTANA Y PUEDA CONOCER LA VERSIÓN DE OPENGL QUE TIENE ****/

	//Asignando variables de GLFW y propiedades de ventana
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//para solo usar el core profile de OpenGL y no tener retrocompatibilidad
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	//CREAR VENTANA
	GLFWwindow* mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "Primer ventana", NULL, NULL);

	if (!mainWindow)
	{
		printf("Fallo en crearse la ventana con GLFW");
		glfwTerminate();
		return 1;
	}
	//Obtener tamaño de Buffer
	int BufferWidth, BufferHeight;
	glfwGetFramebufferSize(mainWindow, &BufferWidth, &BufferHeight);

	//asignar el contexto
	glfwMakeContextCurrent(mainWindow);

	//permitir nuevas extensiones
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		printf("Falló inicialización de GLEW");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	// Asignar valores de la ventana y coordenadas
	//Asignar Viewport
	glViewport(0, 0, BufferWidth, BufferHeight);

	//Llamada a las funciones creadas antes del main
	//CrearFiguras();// Comentar (EJERCICIO 2) / Descomentar (EJERCICIO 1)
	CompileShaders();
	crearIniciales();


	//Loop mientras no se cierra la ventana
	while (!glfwWindowShouldClose(mainWindow))
	{
		//Recibir eventos del usuario
		glfwPollEvents();

		double t = glfwGetTime();//tiempo en segundos 
		if (t - cambioColor >= 2.0) //si ya pasaron 2 segundos
		{
			red = (rand() % 101) / 100.0f; //Números aleatorios entre 0 y 1
			green = (rand() % 101) / 100.0f;
			blue = (rand() % 101) / 100.0f;
			cambioColor = t; //Actualizar el tiempo
		}

		glClearColor(red, green, blue, 1.0f); //Cambiar el color de la ventana

		//Limpiar la ventana
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shader);

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 96); // 12 vértices (el cuadrado y el rombo)
		glBindVertexArray(0);

		glUseProgram(0);

		glfwSwapBuffers(mainWindow);

		//NO ESCRIBIR NINGUNA LÍNEA DESPUÉS DE glfwSwapBuffers(mainWindow); 
	}
	return 0;
}
