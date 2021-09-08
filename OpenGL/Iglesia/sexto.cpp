#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb/stb_image.h>
#include <shader/shader_s.h>
#include <learnopengl/camera.h>
#include <iostream>
#include <vector>
#include <cmath>
using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(const char* path);
unsigned int loadCubemap(vector<string> faces);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 1.0f, 10.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

//posicion de la luz 
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
int ejemplo() {

    //inicializamos glfw
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //Creacion de nuestra ventanita hermosa bella de chula
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Dorime", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "fallaste, mejor llora XD" << std::endl;
        glfwTerminate();//primer
        return -1;
    }

    //agregamos nuevos contextos 
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    //le indicamos a glfw que reciba datos del nuestro mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    //comprobamos nuestro alegre
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "oye wey no se esta ejecutando el alegre" << std::endl;
        return -1;
    }
    //Activamos la comprobacion del buffer de profundidad (Z)
    glEnable(GL_DEPTH_TEST);

    //Declarar nuestro programa shader y sus compilaciones
    Shader nuestroShader("luces1.vs", "luces1.fs"); //nuestro cubo que va a recibir la luz
    Shader segundoShader("luces2.vs", "luces2.fs"); //el origen de la luz
    Shader skyboxShader("skybox.vs", "skybox.fs");
    Shader cruzShader("4.1.light_cube.vs", "4.1.light_cube.fs");
    Shader bloom("bloom.vs", "bloom.fs");
    //declaramos nuestros vertices con sus atributos

    float torre[] = {
        -0.25f, -0.5f, -0.25f,  0.0f,  0.0f, -1.0f,  0.5f,  0.5f,
         0.25f, -0.5f, -0.25f,  0.0f,  0.0f, -1.0f,  1.0f,  0.5f,
         0.25f,  2.0f, -0.25f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.25f,  2.0f, -0.25f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.25f,  2.0f, -0.25f,  0.0f,  0.0f, -1.0f,  0.5f,  1.0f,
        -0.25f, -0.5f, -0.25f,  0.0f,  0.0f, -1.0f,  0.5f,  0.5f,
                          
        -0.25f, -0.5f,  0.25f,  0.0f,  0.0f,  1.0f,  0.5f,  0.5f,
         0.25f, -0.5f,  0.25f,  0.0f,  0.0f,  1.0f,  1.0f,  0.5f,
         0.25f,  2.0f,  0.25f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.25f,  2.0f,  0.25f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.25f,  2.0f,  0.25f,  0.0f,  0.0f,  1.0f,  0.5f,  1.0f,
        -0.25f, -0.5f,  0.25f,  0.0f,  0.0f,  1.0f,  0.5f,  0.5f,
                          
        -0.25f,  2.0f,  0.25f, -1.0f,  0.0f,  0.0f,  0.5f,  0.5f,
        -0.25f,  2.0f, -0.25f, -1.0f,  0.0f,  0.0f,  1.0f,  0.5f,
        -0.25f, -0.5f, -0.25f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.25f, -0.5f, -0.25f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.25f, -0.5f,  0.25f, -1.0f,  0.0f,  0.0f,  0.5f,  1.0f,
        -0.25f,  2.0f,  0.25f, -1.0f,  0.0f,  0.0f,  0.5f,  0.5f,
                          
         0.25f,  2.0f,  0.25f,  1.0f,  0.0f,  0.0f,  0.5f,  0.5f,
         0.25f,  2.0f, -0.25f,  1.0f,  0.0f,  0.0f,  1.0f,  0.5f,
         0.25f, -0.5f, -0.25f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.25f, -0.5f, -0.25f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.25f, -0.5f,  0.25f,  1.0f,  0.0f,  0.0f,  0.5f,  1.0f,
         0.25f,  2.0f,  0.25f,  1.0f,  0.0f,  0.0f,  0.5f,  0.5f,
                          
        -0.25f, -0.5f, -0.25f,  0.0f, -1.0f,  0.0f,  0.5f,  0.5f,
         0.25f, -0.5f, -0.25f,  0.0f, -1.0f,  0.0f,  1.0f,  0.5f,
         0.25f, -0.5f,  0.25f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.25f, -0.5f,  0.25f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
        -0.25f, -0.5f,  0.25f,  0.0f, -1.0f,  0.0f,  0.5f,  1.0f,
        -0.25f, -0.5f, -0.25f,  0.0f, -1.0f,  0.0f,  0.5f,  0.5f,
                          
        -0.25f,  2.0f, -0.25f,  0.0f,  1.0f,  0.0f,  0.5f,  0.5f,
         0.25f,  2.0f, -0.25f,  0.0f,  1.0f,  0.0f,  1.0f,  0.5f,
         0.25f,  2.0f,  0.25f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.25f,  2.0f,  0.25f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
        -0.25f,  2.0f,  0.25f,  0.0f,  1.0f,  0.0f,  0.5f,  1.0f,
        -0.25f,  2.0f, -0.25f,  0.0f,  1.0f,  0.0f,  0.5f,  0.5f
    };

    float base[] = {
//Back
        -1.0f, -0.5f, -2.0f,  0.0f,  0.0f, -1.0f,  0.5f,  0.5f,
         1.0f, -0.5f, -2.0f,  0.0f,  0.0f, -1.0f,  1.0f,  0.5f,
         1.0f,  0.3f, -2.0f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         1.0f,  0.3f, -2.0f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
        -1.0f,  0.3f, -2.0f,  0.0f,  0.0f, -1.0f,  0.5f,  0.0f,
        -1.0f, -0.5f, -2.0f,  0.0f,  0.0f, -1.0f,  0.5f,  0.5f,
//front                2
        -1.0f, -0.5f,  2.0f,  0.0f,  0.0f,  1.0f,  0.5f,  1.0f,
         1.0f, -0.5f,  2.0f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         1.0f,  0.3f,  2.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.5f,
         1.0f,  0.3f,  2.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.5f,
        -1.0f,  0.3f,  2.0f,  0.0f,  0.0f,  1.0f,  0.5f,  0.5f,
        -1.0f, -0.5f,  2.0f,  0.0f,  0.0f,  1.0f,  0.5f,  1.0f,
//Izq                  2
        -1.0f,  0.3f,  2.0f, -1.0f,  0.0f,  0.0f,  0.5f,  0.0f,
        -1.0f,  0.3f, -2.0f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -1.0f, -0.5f, -2.0f, -1.0f,  0.0f,  0.0f,  0.0f,  0.5f,
        -1.0f, -0.5f, -2.0f, -1.0f,  0.0f,  0.0f,  0.0f,  0.5f,
        -1.0f, -0.5f,  2.0f, -1.0f,  0.0f,  0.0f,  0.5f,  0.5f,
        -1.0f,  0.3f,  2.0f, -1.0f,  0.0f,  0.0f,  0.5f,  0.0f,
//der                  2
         1.0f,  0.3f,  2.0f,  1.0f,  0.0f,  0.0f,  0.5f,  0.0f,
         1.0f,  0.3f, -2.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         1.0f, -0.5f, -2.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.5f,
         1.0f, -0.5f, -2.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.5f,
         1.0f, -0.5f,  2.0f,  1.0f,  0.0f,  0.0f,  0.5f,  0.5f,
         1.0f,  0.3f,  2.0f,  1.0f,  0.0f,  0.0f,  0.5f,  0.0f,
//bot                  2
        -1.0f, -0.5f, -2.0f,  0.0f, -1.0f,  0.0f,  0.5f,  0.5f,
         1.0f, -0.5f, -2.0f,  0.0f, -1.0f,  0.0f,  1.0f,  0.5f,
         1.0f, -0.5f,  2.0f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         1.0f, -0.5f,  2.0f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -1.0f, -0.5f,  2.0f,  0.0f, -1.0f,  0.0f,  0.5f,  0.0f,
        -1.0f, -0.5f, -2.0f,  0.0f, -1.0f,  0.0f,  0.5f,  0.5f,
//top                  2
        -1.0f,  0.3f, -2.0f,  0.0f,  1.0f,  0.0f,  0.5f,  0.5f,
         1.0f,  0.3f, -2.0f,  0.0f,  1.0f,  0.0f,  1.0f,  0.5f,
         1.0f,  0.3f,  2.0f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         1.0f,  0.3f,  2.0f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -1.0f,  0.3f,  2.0f,  0.0f,  1.0f,  0.0f,  0.5f,  0.0f,
        -1.0f,  0.3f, -2.0f,  0.0f,  1.0f,  0.0f,  0.5f,  0.5f
    };
    
    float techo[] = {
//Back
        -1.2f,   0.3f, -2.2f,  0.0f,  0.0f, -1.0f,  0.5f,  0.0f,
         1.2f,   0.3f, -2.2f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.25f,  0.8f, -2.2f,  0.0f,  0.0f, -1.0f,  0.0f,  0.5f,
         0.25f,  0.8f, -2.2f,  0.0f,  0.0f, -1.0f,  0.0f,  0.5f,
        -0.25f,  0.8f, -2.2f,  0.0f,  0.0f, -1.0f,  0.5f,  0.5f,
        -1.2f,   0.3f, -2.2f,  0.0f,  0.0f, -1.0f,  0.5f,  0.0f,
//front                   2
        -1.2f,   0.3f,  2.2f,  0.0f,  0.0f,  1.0f,  0.5f,  0.0f,
         1.2f,   0.3f,  2.2f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.25f,  0.8f,  2.2f,  0.0f,  0.0f,  1.0f,  0.0f,  0.5f,
         0.25f,  0.8f,  2.2f,  0.0f,  0.0f,  1.0f,  0.0f,  0.5f,
        -0.25f,  0.8f,  2.2f,  0.0f,  0.0f,  1.0f,  0.5f,  0.5f,
        -1.2f,   0.3f,  2.2f,  0.0f,  0.0f,  1.0f,  0.5f,  0.0f,
//Izq                     2
        -0.25f,  0.8f,  2.2f, -1.0f,  0.0f,  0.0f,  0.5f,  0.0f,
        -0.25f,  0.8f, -2.2f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -1.2f,   0.3f, -2.2f, -1.0f,  0.0f,  0.0f,  0.0f,  0.5f,
        -1.2f,   0.3f, -2.2f, -1.0f,  0.0f,  0.0f,  0.0f,  0.5f,
        -1.2f,   0.3f,  2.2f, -1.0f,  0.0f,  0.0f,  0.5f,  0.5f,
        -0.25f,  0.8f,  2.2f, -1.0f,  0.0f,  0.0f,  0.5f,  0.0f,
//der                     2
         0.25f,  0.8f,  2.2f,  1.0f,  0.0f,  0.0f,  0.5f,  0.0f,
         0.25f,  0.8f, -2.2f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         1.2f,   0.3f, -2.2f,  1.0f,  0.0f,  0.0f,  0.0f,  0.5f,
         1.2f,   0.3f, -2.2f,  1.0f,  0.0f,  0.0f,  0.0f,  0.5f,
         1.2f,   0.3f,  2.2f,  1.0f,  0.0f,  0.0f,  0.5f,  0.5f,
         0.25f,  0.8f,  2.2f,  1.0f,  0.0f,  0.0f,  0.5f,  0.0f,
//bot                     2
        -1.2f,   0.3f, -2.2f,  0.0f, -1.0f,  0.0f,  0.5f,  0.0f,
         1.2f,   0.3f, -2.2f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         1.2f,   0.3f,  2.2f,  0.0f, -1.0f,  0.0f,  1.0f,  0.5f,
         1.2f,   0.3f,  2.2f,  0.0f, -1.0f,  0.0f,  1.0f,  0.5f,
        -1.2f,   0.3f,  2.2f,  0.0f, -1.0f,  0.0f,  0.5f,  0.5f,
        -1.2f,   0.3f, -2.2f,  0.0f, -1.0f,  0.0f,  0.5f,  0.0f,
//top                     2
        -0.25f,  0.8f, -2.2f,  0.0f,  1.0f,  0.0f,  0.5f,  0.0f,
         0.25f,  0.8f, -2.2f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.25f,  0.8f,  2.2f,  0.0f,  1.0f,  0.0f,  1.0f,  0.5f,
         0.25f,  0.8f,  2.2f,  0.0f,  1.0f,  0.0f,  1.0f,  0.5f,
        -0.25f,  0.8f,  2.2f,  0.0f,  1.0f,  0.0f,  0.5f,  0.5f,
        -0.25f,  0.8f, -2.2f,  0.0f,  1.0f,  0.0f,  0.5f,  0.0f
    };
    
    float punta[] = {
//Back
        -0.3f,  0.0f, -0.3f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
         0.3f,  0.0f, -0.3f,  0.0f,  0.0f, -1.0f,  0.5f,  1.0f,
         0.0f,  1.0f, -0.0f,  0.0f,  0.0f, -1.0f,  0.25f,  0.5f,
//front  0.3             3
        -0.3f,  0.0f,  0.3f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
         0.3f,  0.0f,  0.3f,  0.0f,  0.0f,  1.0f,  0.5f,  1.0f,
         0.0f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,  0.25f,  0.5f,
//Izq    0.3             3
        -0.3f,  0.0f,  0.3f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.3f,  0.0f, -0.3f, -1.0f,  0.0f,  0.0f,  0.5f,  1.0f,
        -0.0f,  1.0f, -0.0f, -1.0f,  0.0f,  0.0f,  0.25f,  0.5f,
//der    0.3             3
         0.3f,  0.0f,  0.3f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.3f,  0.0f, -0.3f,  1.0f,  0.0f,  0.0f,  0.5f,  1.0f,
         0.0f,  1.0f, -0.0f,  1.0f,  0.0f,  0.0f,  0.25f,  0.5f,
//bot    0.3             3
        -0.3f,  0.0f, -0.3f,  0.0f, -1.0f,  0.0f,  0.5f,  0.0f,
         0.3f,  0.0f, -0.3f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.3f,  0.0f,  0.3f,  0.0f, -1.0f,  0.0f,  1.0f,  0.5f,
         0.3f,  0.0f,  0.3f,  0.0f, -1.0f,  0.0f,  1.0f,  0.5f,
        -0.3f,  0.0f,  0.3f,  0.0f, -1.0f,  0.0f,  0.5f,  0.5f,
        -0.3f,  0.0f, -0.3f,  0.0f, -1.0f,  0.0f,  0.5f,  0.0f
    };
  
    float cruz[] = {
        // positions          // normals           // texture coords
        -0.1f, -1.0f, -0.1f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.1f, -1.0f, -0.1f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.1f,  1.0f, -0.1f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.1f,  1.0f, -0.1f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.1f,  1.0f, -0.1f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.1f, -1.0f, -0.1f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
//         12   1.0      12
        -0.1f, -1.0f,  0.1f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.1f, -1.0f,  0.1f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.1f,  1.0f,  0.1f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.1f,  1.0f,  0.1f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.1f,  1.0f,  0.1f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.1f, -1.0f,  0.1f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
//         12   1.0      12
        -0.1f,  1.0f,  0.1f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.1f,  1.0f, -0.1f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.1f, -1.0f, -0.1f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.1f, -1.0f, -0.1f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.1f, -1.0f,  0.1f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.1f,  1.0f,  0.1f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
//         12   1.0      12
         0.1f,  1.0f,  0.1f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.1f,  1.0f, -0.1f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.1f, -1.0f, -0.1f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.1f, -1.0f, -0.1f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.1f, -1.0f,  0.1f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.1f,  1.0f,  0.1f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
//         1    1.0      1
        -0.1f, -1.0f, -0.1f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.1f, -1.0f, -0.1f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.1f, -1.0f,  0.1f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.1f, -1.0f,  0.1f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.1f, -1.0f,  0.1f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.1f, -1.0f, -0.1f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
//           2  1.0      1 2
        -0.1f,  1.0f, -0.1f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.1f,  1.0f, -0.1f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.1f,  1.0f,  0.1f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.1f,  1.0f,  0.1f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.1f,  1.0f,  0.1f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.1f,  1.0f, -0.1f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };

    float suelo[] = {
        -100.0f,  -2.5f, -100.0f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
         100.0f,  -2.5f, -100.0f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
         100.0f,  -2.5f,  100.0f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
         100.0f,  -2.5f,  100.0f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
        -100.0f,  -2.5f,  100.0f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
        -100.0f,  -2.5f, -100.0f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
    };

    unsigned int indices[]{
        0, 1, 2,
        3, 4, 5,
        6, 7, 8,
        9, 10, 11,
        12, 13, 14,
        14, 16, 17,
        18, 19, 20,
        21, 22, 23,
        24, 25, 26,
        27, 28, 29,
        30, 31, 32,
        33, 34, 35

        //0, 1, 3,
        //1, 2, 3
    };

    glm::vec3 posicionesCubo[] = {
      glm::vec3(0.0f,  0.0f,  0.0f),
      glm::vec3(0.0f,  0.0f, -1.745f),
      glm::vec3(0.0f,  2.0f, -1.745f),
      glm::vec3(0.0f,  3.0f, -1.745f),
      glm::vec3(0.0f,  3.5f, -1.745f),
      glm::vec3(-1.7f,  3.0f, -7.5f),
      glm::vec3(1.3f, -2.0f, -2.5f),
      glm::vec3(1.5f,  2.0f, -2.5f),
      glm::vec3(1.5f,  0.2f, -1.5f),
      glm::vec3(-1.3f,  1.0f, -1.5f)
    };

    float verticesSkybox[] = {
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    //configuraciones de nuestro skybox
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesSkybox), &verticesSkybox, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    vector<std::string> faces
    {
        ("skybox/right.jpg"),
        ("skybox/left.jpg"),
        ("skybox/top.jpg"),
        ("skybox/bottom.jpg"),
        ("skybox/front.jpg"),
        ("skybox/back.jpg")
    };

    unsigned int cubemapTexture = loadCubemap(faces);

    //configuracion del shader del skybox
    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    //unificar 
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(base), base, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    //atributos de posicion 
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    //generamos un segundo vertex 
    unsigned int VAO2;
    glGenVertexArrays(1, &VAO2);
    glBindVertexArray(VAO2);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    unsigned int diffuseMap = loadTexture("cosa2.jpg");
    unsigned int specularMap = loadTexture("church.png");

    unsigned int texTecho, texSuelo, texPuerta, texMadera;
    texTecho = loadTexture("supchurch.png");
    texSuelo = loadTexture("grass.jpg");
    /*texPuerta = loadTexture("houseFront.jpg");
    texMadera = loadTexture("tablasRojas.jpg");*/

    nuestroShader.use();
    nuestroShader.setInt("material.diffuse", 1);
    nuestroShader.setInt("material.specular", 0);

    /*--------------------------Buffer Techo-----------------------------------------------------------*/
    unsigned int VAOTecho;
    glGenVertexArrays(1, &VAOTecho);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAOTecho);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(techo), techo, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    /*--------------------------Buffer Torre-----------------------------------------------------------*/
    unsigned int VAOTorre;
    glGenVertexArrays(1, &VAOTorre);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAOTorre);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(torre), torre, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    /*--------------------------Buffer Punta-----------------------------------------------------------*/
    unsigned int VAOPunta;
    glGenVertexArrays(1, &VAOPunta);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAOPunta);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(punta), punta, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    /*--------------------------Buffer Suelo-----------------------------------------------------------*/
    unsigned int VAOSuelo;
    glGenVertexArrays(1, &VAOSuelo);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAOSuelo);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(suelo), suelo, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    /*--------------------------Buffer Cruz-----------------------------------------------------------*/
    unsigned int CubeVBO, cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cruz), cruz, GL_STATIC_DRAW);

    glBindVertexArray(cubeVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
    unsigned int lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // note that we update the lamp's position attribute's stride to reflect the updated buffer data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //loop de dibujado
    while (!glfwWindowShouldClose(window))
    {
        //carcular el tiempo entre los frames para poder implementar le movimiento de la camara
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        //entradas
        processInput(window);

        //renderizado
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //activar las caracteristicas de luz en nuestro objeto
        nuestroShader.use();
        //nuestroShader.setVec3("light.position", camera.Position);
        nuestroShader.setVec3("light.position", glm::vec3(1.0f, 15.0f, 5.0f));
        //nuestroShader.setVec3("light.direction", camera.Front);
        nuestroShader.setVec3("light.direction", glm::vec3(0.0f, -1.0f, 0.0f));
        nuestroShader.setFloat("light.cutOff", glm::cos(glm::radians(12.5f)));
        nuestroShader.setFloat("light.outerCutOff", glm::cos(glm::radians(17.5f)));
        nuestroShader.setVec3("viewPos", camera.Position);

        //propiedades de la luz
        nuestroShader.setVec3("light.ambient", 10.0f, 10.0f, 10.0f);
        nuestroShader.setVec3("light.diffuse", 0.0f, 0.0f, 0.0f);
        nuestroShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
        nuestroShader.setFloat("light.constant", 1.0f);
        nuestroShader.setFloat("light.linear", 0.09f);
        nuestroShader.setFloat("light.quadratic", 0.032f);

        //propiedades de los materiales
        nuestroShader.setFloat("material.shininess", 32.0f);

        //mvp 
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        nuestroShader.setMat4("projection", projection);
        nuestroShader.setMat4("view", view);

        //configuraciones del modelo
        glm::mat4 model = glm::mat4(1.0f);
        //model = glm::translate(model, glm::vec3(1.2f,0.0f,0.0f));
        nuestroShader.setMat4("model", model);

        //linkeamos nuestras textr¿uras
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);
        //glm::mat4 model = glm::mat4(1.0f);
        //render del cubo
        /*-----------------Dibujar Base-------------------------*/
        glBindVertexArray(VAO);
        // calculate the model matrix for each object and pass it to shader before drawing
        model = glm::mat4(1.0f);
        model = glm::translate(model, posicionesCubo[0]);
        //float angle = 20.0f * i;
        //model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
        nuestroShader.setMat4("model", model);


        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        
        /*-----------------Dibujar Techo-------------------------*/
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texTecho);
        glBindVertexArray(VAOTecho);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        /*-----------------Dibujar Torre-------------------------*/
        
        glBindVertexArray(VAOTorre);
        model = glm::mat4(1.0f);
        model = glm::translate(model, posicionesCubo[1]);
        nuestroShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        /*-----------------Dibujar Punta-------------------------*/
        glBindVertexArray(VAOPunta);
        model = glm::mat4(1.0f);
        model = glm::translate(model, posicionesCubo[2]);
        nuestroShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 18);
        /*-----------------Dibujar Suelo-------------------------*/
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texSuelo);
        glBindVertexArray(VAOSuelo);
        model = glm::mat4(1.0f);
        model = glm::translate(model, posicionesCubo[2]);
        nuestroShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        /*-----------------Dibujar Cruz-------------------------*/
        //glBindVertexArray(cubeVAO);
        // also draw the lamp object
        cruzShader.use();
        cruzShader.setMat4("projection", projection);
        cruzShader.setMat4("view", view);
        model = glm::mat4(1.0f);
        model = glm::translate(model, posicionesCubo[3]);
        model = glm::scale(model, glm::vec3(1.0f)); // a smaller cube
        cruzShader.setMat4("model", model);

        glBindVertexArray(lightCubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //float angle = 20.0f * i;
        cruzShader.use();
        cruzShader.setMat4("projection", projection);
        cruzShader.setMat4("view", view);
        model = glm::mat4(1.0f);
        model = glm::translate(model, posicionesCubo[4]);
        model = glm::scale(model, glm::vec3(0.7f)); // a smaller cube
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        cruzShader.setMat4("model", model);
        
        glBindVertexArray(lightCubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glDepthFunc(GL_LEQUAL); //funcion de profundidad encargada del calculo entre el usuario y el skybox
        skyboxShader.use();
        view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);

        //dibujado del skybox
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &VAO);
    glDeleteVertexArrays(1, &VAO2);
    glDeleteVertexArrays(1, &VAOTecho);
    glDeleteVertexArrays(1, &VAOTorre);
    glDeleteVertexArrays(1, &VAOPunta);
    glDeleteVertexArrays(1, &VAOSuelo);
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightCubeVAO);
    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteBuffers(1, &skyboxVBO);

    glfwTerminate();
    return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main()
{
     return ejemplo();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera.ProcessKeyboard(UPWARDS, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWNWARDS, deltaTime);

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "fallamos al cargar las texturas del siguiente origen: " << path << std::endl;
        stbi_image_free(data);
    }
    return textureID;
}

unsigned int loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Fallo en cargar las texturas de nuestro skybox la textura que fallo es: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}
