#include <array>
#include <fstream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <string>
#include "ObjectFile.h"
#include "ObjectFileReader.h"

/* Vertex buffer objektumok �s vertex array objektum az adatt�rol�shoz. */
/* Vertex buffer object IDs and vertex array object ID for data storing.*/
#define		numVBOs			2
#define		numVAOs			1
GLuint		VBO[numVBOs];
GLuint		VAO[numVAOs];

int			window_width = 600;
int			window_height = 600;
char		window_title[] = "Subdivision";
/** A norm�l billenty�k a [0..255] tartom�nyban vannak, a nyilak �s a speci�lis billenty�k pedig a [256..511] tartom�nyban helyezkednek el. */
/** Normal keys are fom [0..255], arrow and special keys are from [256..511]. */
GLboolean	keyboard[512] = { GL_FALSE };
GLFWwindow* window = nullptr;
GLuint		renderingProgram;


bool checkOpenGLError() 
{
	bool	foundError = false;
	int		glErr = glGetError();

	while (glErr != GL_NO_ERROR) {
		std::cout << "glError: " << glErr << std::endl;

		foundError = true;
		glErr = glGetError();
	}

	return foundError;
}

void printShaderLog(GLuint shader) 
{
	int		length = 0;
	int		charsWritten = 0;
	char* log = nullptr;

	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

	if (length > 0) {
		log = (char*)malloc(length);

		glGetShaderInfoLog(shader, length, &charsWritten, log);
		std::cout << "Shader Info Log: " << log << std::endl;
		free(log);
	}
}

void printProgramLog(int prog) 
{
	int		length = 0;
	int		charsWritten = 0;
	char* log = nullptr;

	glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &length);

	if (length > 0) {
		log = (char*)malloc(length);

		glGetProgramInfoLog(prog, length, &charsWritten, log);
		std::cout << "Program Info Log: " << log << std::endl;
		free(log);
	}
}

std::string readShaderSource(const char* filePath) 
{
	std::ifstream	fileStream(filePath, std::ios::in);
	std::string		content;
	std::string		line;

	while (!fileStream.eof()) {
		std::getline(fileStream, line);
		content.append(line + "\n");
	}

	fileStream.close();

	return content;
}

GLuint createShaderProgram() 
{
	GLint		vertCompiled;
	GLint		fragCompiled;
	GLint		linked;

	std::string		vertShaderStr = readShaderSource("vertexShader.glsl");
	std::string		fragShaderStr = readShaderSource("fragmentShader.glsl");

	GLuint		vShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint		fShader = glCreateShader(GL_FRAGMENT_SHADER);

	const char* vertShaderSrc = vertShaderStr.c_str();
	const char* fragShaderSrc = fragShaderStr.c_str();

	glShaderSource(vShader, 1, &vertShaderSrc, NULL);
	glShaderSource(fShader, 1, &fragShaderSrc, NULL);

	glCompileShader(vShader);

	checkOpenGLError();
	glGetShaderiv(vShader, GL_COMPILE_STATUS, &vertCompiled);
	if (vertCompiled != 1) {
		std::cout << "Vertex compilation failed." << std::endl;
		printShaderLog(vShader);
	}

	glCompileShader(fShader);

	checkOpenGLError();
	glGetShaderiv(fShader, GL_COMPILE_STATUS, &fragCompiled);
	if (fragCompiled != 1) {
		std::cout << "Fragment compilation failed." << std::endl;
		printShaderLog(fShader);
	}

	GLuint		vfProgram = glCreateProgram();
	glAttachShader(vfProgram, vShader);
	glAttachShader(vfProgram, fShader);

	glLinkProgram(vfProgram);
	checkOpenGLError();
	glGetProgramiv(vfProgram, GL_LINK_STATUS, &linked);
	if (linked != 1) {
		std::cout << "Shader linking failed." << std::endl;
		printProgramLog(vfProgram);
	}

	glDeleteShader(vShader);
	glDeleteShader(fShader);

	return vfProgram;
}

void init(GLFWwindow* window) 
{
	renderingProgram = createShaderProgram();

	glGenBuffers(numVBOs, VBO);
	glGenVertexArrays(numVAOs, VAO);

	glUseProgram(renderingProgram);

	glClearColor(0.0, 0.0, 0.0, 1.0);
}

void display(GLFWwindow* window, double currentTime) 
{
	glClear(GL_COLOR_BUFFER_BIT);

	glBindVertexArray(0);
}

void cleanUpScene() 
{
	glDeleteProgram(renderingProgram);
	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) 
{
	window_width = width;
	window_height = height;

	glViewport(0, 0, width, height);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) 
{
	if ((action == GLFW_PRESS) && (key == GLFW_KEY_ESCAPE))
		cleanUpScene();

	if (action == GLFW_PRESS)
		keyboard[key] = GL_TRUE;
	else if (action == GLFW_RELEASE)
		keyboard[key] = GL_FALSE;
}

void cursorPosCallback(GLFWwindow* window, double xPos, double yPos) 
{

}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) 
{

}

int main(int argc, char** argv) 
{
	if (argc < 2)
	{
		std::cerr << "Usage:\nsubdivision.exe <path-to-the-obj-file>" << std::endl;
		return -1;
	}

	ObjectFileReader objFileReader;
	ObjectFile objFile = objFileReader.parseObjFile(argv[1]);

	std::vector<glm::vec3> vertexes = objFile.getTriangleVertexes();

	

	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make macOS happy; should not be needed.
	window = glfwCreateWindow(window_width, window_height, window_title, nullptr, nullptr);

	glfwMakeContextCurrent(window);

	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCursorPosCallback(window, cursorPosCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);

	if (glewInit() != GLEW_OK)
		exit(EXIT_FAILURE);

	glfwSwapInterval(1);
	glfwSetWindowSizeLimits(window, 400, 400, 800, 800);
	glfwSetWindowAspectRatio(window, 1, 1);

	init(window);

	while (!glfwWindowShouldClose(window)) 
	{
		display(window, glfwGetTime());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	cleanUpScene();

	return EXIT_SUCCESS;
}