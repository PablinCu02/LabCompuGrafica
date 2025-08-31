//Práctica 2: índices, mesh, proyecciones, transformaciones geométricas
#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <ctime>   
#include <glew.h>
#include <glfw3.h>
//glm
#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp> 

//clases para dar orden y limpieza al código
#include "Mesh.h"
#include "Shader.h"
#include "Window.h"

const float toRadians = 3.14159265f / 180.0f; //grados a radianes
Window mainWindow;
std::vector<Mesh*>  meshList;   
std::vector<Shader> shaderList; 

//shaders nuevos se crearían acá
static const char* vgreenShader = "shaders/greenshader.vert";
static const char* fgreenShader = "shaders/greenshader.frag";
static const char* vredShader = "shaders/redshader.vert";
static const char* fredShader = "shaders/redshader.frag";
static const char* vblueShader = "shaders/blueshader.vert";
static const char* fblueShader = "shaders/blueshader.frag";
static const char* vbrownShader = "shaders/brownshader.vert";
static const char* fbrownShader = "shaders/brownshader.frag";
static const char* vdarkgreenShader = "shaders/darkgreenshader.vert";
static const char* fdarkgreenShader = "shaders/darkgreenshader.frag";


//Creacion de Pirámide 
void CreaPiramide()
{
    unsigned int indices[] = { 
        0,1,2,  
        1,3,2,
        3,0,2,
        1,0,3 
    };
    GLfloat vertices[] = {
        -0.5f, -0.5f,  0.0f, // 0  base
         0.5f, -0.5f,  0.0f, // 1
         0.0f,  0.5f, -0.25f,// 2  
         0.0f, -0.5f, -0.5f  // 3
    };
    Mesh* obj = new Mesh();
    obj->CreateMesh(vertices, indices, 12, 12);
    meshList.push_back(obj);
}

// Creación del Cubo 
void CrearCubo()
{
    unsigned int cubo_indices[] = {

        0,1,2, 2,3,0,
        1,5,6, 6,2,1,
        7,6,5, 5,4,7,
        4,0,3, 3,7,4,
        4,5,1, 1,0,4,
        3,2,6, 6,7,3
    };

    GLfloat cubo_vertices[] = {

        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f
    };

    Mesh* cubo = new Mesh();
    cubo->CreateMesh(cubo_vertices, cubo_indices, 24, 36);
    meshList.push_back(cubo);
}


void CreateShaders()
{
    // verde
    Shader* s0 = new Shader(); s0->CreateFromFiles(vgreenShader, fgreenShader); shaderList.push_back(*s0);
    // rojo
    Shader* s1 = new Shader(); s1->CreateFromFiles(vredShader, fredShader);   shaderList.push_back(*s1);
    // azul
    Shader* s2 = new Shader(); s2->CreateFromFiles(vblueShader, fblueShader);  shaderList.push_back(*s2);
    // café
    Shader* s3 = new Shader(); s3->CreateFromFiles(vbrownShader, fbrownShader); shaderList.push_back(*s3);
    // verde oscuro
    Shader* s4 = new Shader(); s4->CreateFromFiles(vdarkgreenShader, fdarkgreenShader); shaderList.push_back(*s4);
}

const int WIDTH = 800, HEIGHT = 800;   
GLuint VAO = 0, VBO = 0, shader = 0;   

// Vertex Shader (letras)
static const char* vShaderTxt =
"#version 330\n"
"layout (location =0) in vec3 pos;\n"
"void main(){\n"
"    gl_Position = vec4(pos,1.0);\n"
"}";

// Fragment Shader (letras)
static const char* fShaderTxt =
"#version 330\n"
"out vec4 color;\n"
"uniform vec3 uColor;\n"
"void main(){\n"
"    color = vec4(uColor,1.0);\n"
"}";

void AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType)
{
    GLuint theShader = glCreateShader(shaderType);
    const GLchar* theCode[1] = { shaderCode };
    GLint codeLength[1] = { (GLint)strlen(shaderCode) };
    glShaderSource(theShader, 1, theCode, codeLength);
    glCompileShader(theShader);

    GLint result = 0;
    GLchar eLog[1024] = { 0 };
    glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);
    if (!result)
    {
        glGetShaderInfoLog(theShader, sizeof(eLog), NULL, eLog); 
        printf("Error al compilar shader %d: %s\n", shaderType, eLog);
        return;
    }
    glAttachShader(theProgram, theShader);
}

void CompileShaders()
{
    shader = glCreateProgram();
    if (!shader) { printf("Error creando el shader\n"); return; }

    AddShader(shader, vShaderTxt, GL_VERTEX_SHADER);
    AddShader(shader, fShaderTxt, GL_FRAGMENT_SHADER);

    GLint result = 0;
    GLchar eLog[1024] = { 0 };
    glLinkProgram(shader);
    glGetProgramiv(shader, GL_LINK_STATUS, &result);
    if (!result)
    {
        glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
        printf("Error al linkear: %s\n", eLog); return;
    }
    glValidateProgram(shader);
    glGetProgramiv(shader, GL_VALIDATE_STATUS, &result);
    if (!result)
    {
        glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
        printf("Error al validar: %s\n", eLog); return;
    }
}

// Creación de las iniciales "PGC"
void crearIniciales()
{
    GLfloat vertices[] = {
        // P 
        -0.75f,  0.60f, 0.0f,   -0.70f,  0.60f, 0.0f,   -0.70f, -0.60f, 0.0f,
        -0.75f,  0.60f, 0.0f,   -0.70f, -0.60f, 0.0f,   -0.75f, -0.60f, 0.0f,
        -0.75f,  0.60f, 0.0f,   -0.45f,  0.60f, 0.0f,   -0.45f,  0.50f, 0.0f,
        -0.75f,  0.60f, 0.0f,   -0.45f,  0.50f, 0.0f,   -0.75f,  0.50f, 0.0f,
        -0.50f,  0.60f, 0.0f,   -0.45f,  0.60f, 0.0f,   -0.45f,  0.20f, 0.0f,
        -0.50f,  0.60f, 0.0f,   -0.45f,  0.20f, 0.0f,   -0.50f,  0.20f, 0.0f,
        -0.75f,  0.25f, 0.0f,   -0.50f,  0.25f, 0.0f,   -0.50f,  0.15f, 0.0f,
        -0.75f,  0.25f, 0.0f,   -0.50f,  0.15f, 0.0f,   -0.75f,  0.15f, 0.0f,
        // G 
        -0.15f,  0.60f, 0.0f,   -0.10f,  0.60f, 0.0f,   -0.10f, -0.60f, 0.0f,
        -0.15f,  0.60f, 0.0f,   -0.10f, -0.60f, 0.0f,   -0.15f, -0.60f, 0.0f,
        -0.15f,  0.60f, 0.0f,    0.15f,  0.60f, 0.0f,    0.15f,  0.50f, 0.0f,
        -0.15f,  0.60f, 0.0f,    0.15f,  0.50f, 0.0f,   -0.15f,  0.50f, 0.0f,
        -0.15f, -0.50f, 0.0f,    0.15f, -0.50f, 0.0f,    0.15f, -0.60f, 0.0f,
        -0.15f, -0.50f, 0.0f,    0.15f, -0.60f, 0.0f,   -0.15f, -0.60f, 0.0f,
         0.00f, -0.05f, 0.0f,    0.15f, -0.05f, 0.0f,    0.15f,  0.05f, 0.0f,
         0.00f, -0.05f, 0.0f,    0.15f,  0.05f, 0.0f,    0.00f,  0.05f, 0.0f,
         0.10f,  0.04f, 0.0f,    0.15f,  0.02f, 0.0f,    0.15f, -0.60f, 0.0f,
         0.10f,  0.10f, 0.0f,    0.15f, -0.60f, 0.0f,    0.10f, -0.60f, 0.0f,
         // C 
          0.65f,  0.60f, 0.0f,    0.70f,  0.60f, 0.0f,    0.70f, -0.60f, 0.0f,
          0.65f,  0.60f, 0.0f,    0.70f, -0.60f, 0.0f,    0.65f, -0.60f, 0.0f,
          0.65f,  0.60f, 0.0f,    0.90f,  0.60f, 0.0f,    0.90f,  0.50f, 0.0f,
          0.65f,  0.60f, 0.0f,    0.90f,  0.50f, 0.0f,    0.65f,  0.50f, 0.0f,
          0.65f, -0.50f, 0.0f,    0.90f, -0.50f, 0.0f,    0.90f, -0.60f, 0.0f,
          0.65f, -0.50f, 0.0f,    0.90f, -0.60f, 0.0f,    0.65f, -0.60f, 0.0f,
    };

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


int main()
{
    mainWindow = Window(800, 600);
    mainWindow.Initialise();

    // Geometría y shaders de la Casa
    CreaPiramide();   // meshList[0]
    CrearCubo();      // meshList[1]
    CreateShaders();
    glEnable(GL_DEPTH_TEST);

    // Letras 
    CompileShaders();     
    crearIniciales();     
    GLuint colorLoc = glGetUniformLocation(shader, "uColor");
    GLuint uniformProjection = 0;
    GLuint uniformModel = 0;


    glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);
    glm::mat4 model(1.0f);

    const float Z_HOUSE = -5.0f;          // profundidad 
    const float GROUND_Y = -0.55f;

    // Casa : 
    const glm::vec3 BODY_S(0.60f, 0.60f, 0.50f);
    const float     HOUSE_CY = GROUND_Y + 0.5f * BODY_S.y;      
    const float     HOUSE_TOPY = HOUSE_CY + 0.5f * BODY_S.y;    

	//Profundidad de ventanas y puerta 
    const float zFront = Z_HOUSE + 0.5f * BODY_S.z;  
    const float zOverlay = zFront + 0.01f;

    // Techo 
    const glm::vec3 ROOF_S(0.80f, 0.45f, 0.80f);
    const float     ROOF_GAP = 0.01f;
    const float     ROOF_TY = (HOUSE_TOPY + ROOF_GAP) + 0.5f * ROOF_S.y;

    // Troncos y copas
    const glm::vec3 TRUNK_S(0.10f, 0.12f, 0.10f);
    const float     TRUNK_CY = GROUND_Y + 0.5f * TRUNK_S.y;
    const float     TRUNK_TOPY = TRUNK_CY + 0.5f * TRUNK_S.y;
    const glm::vec3 PINE_S(0.20f, 0.35f, 0.20f);
    const float     PINE_TY = TRUNK_TOPY + 0.5f * PINE_S.y;

    // Loop mientras no se cierra la ventana
    while (!mainWindow.getShouldClose())
    {
        glfwPollEvents();

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // TECHO 
        shaderList[2].useShader();
        uniformModel = shaderList[2].getModelLocation();
        uniformProjection = shaderList[2].getProjectLocation();
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, ROOF_TY, Z_HOUSE));
        model = glm::scale(model, ROOF_S);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        meshList[0]->RenderMesh();  // pirámide


		//Cuerpo de la casa
        
        shaderList[1].useShader();
        uniformModel = shaderList[1].getModelLocation();
        uniformProjection = shaderList[1].getProjectLocation();
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, HOUSE_CY, Z_HOUSE));
        model = glm::scale(model, BODY_S);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        meshList[1]->RenderMesh();  // cubo

        
        // VENTANAS 
        shaderList[0].useShader();
        uniformModel = shaderList[0].getModelLocation();
        uniformProjection = shaderList[0].getProjectLocation();

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-0.20f, HOUSE_CY + 0.15f, zOverlay));
        model = glm::scale(model, glm::vec3(0.14f, 0.14f, 0.01f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        meshList[1]->RenderMesh();

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.20f, HOUSE_CY + 0.15f, zOverlay));
        model = glm::scale(model, glm::vec3(0.14f, 0.14f, 0.01f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        meshList[1]->RenderMesh();

        // PUERTA 
        model = glm::mat4(1.0f);
        const float doorH = 0.26f;
        const float doorCY = GROUND_Y + 0.5f * doorH; // base en el suelo
        model = glm::translate(model, glm::vec3(0.0f, doorCY, zOverlay));
        model = glm::scale(model, glm::vec3(0.16f, doorH, 0.01f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        meshList[1]->RenderMesh();

        
        //TRONCOS 
        shaderList[3].useShader();
        uniformModel = shaderList[3].getModelLocation();
        uniformProjection = shaderList[3].getProjectLocation();

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-0.75f, TRUNK_CY, Z_HOUSE));
        model = glm::scale(model, TRUNK_S);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        meshList[1]->RenderMesh();

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.75f, TRUNK_CY, Z_HOUSE));
        model = glm::scale(model, TRUNK_S);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        meshList[1]->RenderMesh();

        //PINOS 
        shaderList[4].useShader();
        uniformModel = shaderList[4].getModelLocation();
        uniformProjection = shaderList[4].getProjectLocation();

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-0.75f, PINE_TY, Z_HOUSE));
        model = glm::scale(model, PINE_S);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        meshList[0]->RenderMesh();

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.75f, PINE_TY, Z_HOUSE));
        model = glm::scale(model, PINE_S);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        meshList[0]->RenderMesh();

        // INICIALES
        //glDisable(GL_DEPTH_TEST);          
        //glUseProgram(shader);
        //glBindVertexArray(VAO);

        //// P   
        //glUniform3f(colorLoc, 1.0f, 0.0f, 0.0f); // rojo
        //glDrawArrays(GL_TRIANGLES, 0, 24);

        //// G   
        //glUniform3f(colorLoc, 0.0f, 1.0f, 0.0f); // verde
        //glDrawArrays(GL_TRIANGLES, 24, 30);

        //// C   
        //glUniform3f(colorLoc, 0.0f, 0.0f, 1.0f); // azul
        //glDrawArrays(GL_TRIANGLES, 54, 18);

        //glBindVertexArray(0);
        //glUseProgram(0);
        //glEnable(GL_DEPTH_TEST);           

        mainWindow.swapBuffers();
    }
    return 0;
}
