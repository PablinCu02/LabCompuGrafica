#include "Window.h"

Window::Window()
{
	width = 800;
	height = 600;
	for (size_t i = 0; i < 1024; i++)
	{
		keys[i] = 0;
	}
}
Window::Window(GLint windowWidth, GLint windowHeight)
{
	width = windowWidth;
	height = windowHeight;
	rotax = 0.0f;
	rotay = 0.0f;
	rotaz = 0.0f;
	articulacion1 = 0.0f;
	articulacion2 = 0.0f;
	articulacion3 = 0.0f;
	articulacion4 = 0.0f;
	articulacion5 = 0.0f;
	articulacion6 = 0.0f;
	giroLlantaDD = 0.0f;
	giroLlantaDI = 0.0f;
	giroLlantaTD = 0.0f;
	giroLlantaTI = 0.0f;

	// Variables para el animal robot
	pataDISup = 0.0f; pataDIInf = 0.0f;
	pataDDSup = 0.0f; pataDDInf = 0.0f;
	pataTISup = 0.0f; pataTIInf = 0.0f;
	pataTDSup = 0.0f; pataTDInf = 0.0f;
	colaUD = 0.0f;
	colaLR = 0.0f;

	for (size_t i = 0; i < 1024; i++)
	{
		keys[i] = 0;
	}
}
int Window::Initialise()
{
	//Inicialización de GLFW
	if (!glfwInit())
	{
		printf("Falló inicializar GLFW");
		glfwTerminate();
		return 1;
	}
	//Asignando variables de GLFW y propiedades de ventana
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//para solo usar el core profile de OpenGL y no tener retrocompatibilidad
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	//CREAR VENTANA
	mainWindow = glfwCreateWindow(width, height, "Practica: Animal Robot 3D", NULL, NULL);

	if (!mainWindow)
	{
		printf("Fallo en crearse la ventana con GLFW");
		glfwTerminate();
		return 1;
	}
	// Obtener el tamaño de buffer
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	//asignar el contexto
	glfwMakeContextCurrent(mainWindow);

	//MANEJAR TECLADO Y MOUSE
	createCallbacks();


	//permitir nuevas extensiones
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		printf("Falló inicialización de GLEW");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	glEnable(GL_DEPTH_TEST); //HABILITAR BUFFER DE PROFUNDIDAD
	// Asignar valores de la ventana y coordenadas

	// Asignar Viewport
	glViewport(0, 0, bufferWidth, bufferHeight);
	// Callback para detectar que se está usando la ventana
	glfwSetWindowUserPointer(mainWindow, this);
	return 0;
}

void Window::createCallbacks()
{
	glfwSetKeyCallback(mainWindow, ManejaTeclado);
	glfwSetCursorPosCallback(mainWindow, ManejaMouse);
}

GLfloat Window::getXChange()
{
	GLfloat theChange = xChange;
	xChange = 0.0f;
	return theChange;
}

GLfloat Window::getYChange()
{
	GLfloat theChange = yChange;
	yChange = 0.0f;
	return theChange;
}

void Window::ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			theWindow->keys[key] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			theWindow->keys[key] = false;
		}
	}
	if (action == GLFW_PRESS || action == GLFW_REPEAT)
	{
		if (mode == GLFW_MOD_SHIFT) // Mover patas traseras con SHIFT
		{
			// Pata Trasera Izquierda
			if (key == GLFW_KEY_Q) theWindow->pataTISup += 5.0;
			if (key == GLFW_KEY_1) theWindow->pataTISup -= 5.0;
			if (key == GLFW_KEY_Z) theWindow->pataTIInf += 5.0;
			if (key == GLFW_KEY_X) theWindow->pataTIInf -= 5.0;

			// Pata Trasera Derecha
			if (key == GLFW_KEY_E) theWindow->pataTDSup += 5.0;
			if (key == GLFW_KEY_2) theWindow->pataTDSup -= 5.0;
			if (key == GLFW_KEY_C) theWindow->pataTDInf += 5.0;
			if (key == GLFW_KEY_V) theWindow->pataTDInf -= 5.0;
		}
		else // Mover patas delanteras y cola sin SHIFT
		{
			// Pata Delantera Izquierda
			if (key == GLFW_KEY_Q) theWindow->pataDISup += 5.0;
			if (key == GLFW_KEY_1) theWindow->pataDISup -= 5.0;
			if (key == GLFW_KEY_Z) theWindow->pataDIInf += 5.0;
			if (key == GLFW_KEY_X) theWindow->pataDIInf -= 5.0;

			// Pata Delantera Derecha
			if (key == GLFW_KEY_E) theWindow->pataDDSup += 5.0;
			if (key == GLFW_KEY_2) theWindow->pataDDSup -= 5.0;
			if (key == GLFW_KEY_C) theWindow->pataDDInf += 5.0;
			if (key == GLFW_KEY_V) theWindow->pataDDInf -= 5.0;

			// Cola
			if (key == GLFW_KEY_T) theWindow->colaUD += 5.0;
			if (key == GLFW_KEY_G) theWindow->colaUD -= 5.0;
			if (key == GLFW_KEY_F) theWindow->colaLR += 5.0;
			if (key == GLFW_KEY_H) theWindow->colaLR -= 5.0;
		}
	}
}

void Window::ManejaMouse(GLFWwindow* window, double xPos, double yPos)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (theWindow->mouseFirstMoved)
	{
		theWindow->lastX = xPos;
		theWindow->lastY = yPos;
		theWindow->mouseFirstMoved = false;
	}

	theWindow->xChange = xPos - theWindow->lastX;
	theWindow->yChange = theWindow->lastY - yPos;

	theWindow->lastX = xPos;
	theWindow->lastY = yPos;
}

Window::~Window()
{
	glfwDestroyWindow(mainWindow);
	glfwTerminate();
}