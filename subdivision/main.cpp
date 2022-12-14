#include <array>
#include <fstream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include <iostream>
#include <string>
#include "ObjectFile.h"
#include "ObjectFileReader.h"

#define		numVBOs			2
#define		numVAOs			1
GLuint		VBO[numVBOs];
GLuint		VAO[numVAOs];

int			window_width = 800;
int			window_height = 800;
char		window_title[] = "Subdivision";

GLboolean	keyboard[512] = { GL_FALSE };
GLFWwindow* window = nullptr;
GLuint		renderingProgram;

int readVertexEntries = 0;
int readFaceEntries = 0;
int readEntries = 0;

unsigned int	modelLoc;
unsigned int	viewLoc;
unsigned int	projectionLoc;
unsigned int	invTMatrixLoc;

GLfloat			cameraRange = 8.0f;
GLfloat			angle = 0.0f;

glm::mat4		model, view, projection = glm::perspective(glm::radians(45.0f), (float)window_width / (float)window_height, 0.1f, 100.0f);
glm::mat4		invTmatrix, rotateM, scaleM, translateM;

GLdouble		currentTime, deltaTime, lastTime = 0.0f;
GLfloat			cameraSpeed;

glm::vec3		cameraPos = glm::vec3(0.0f, 0.0f, cameraRange),
cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f),
cameraUpVector = glm::vec3(0.0f, 1.0f, 0.0f);

std::vector<glm::vec3> object_colors, object_point_colors;


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

void computeModelMatrix() {
	model = glm::mat4(1.0f);
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
}

void computeCameraMatrix() {
	view = glm::lookAt(cameraPos, cameraTarget, cameraUpVector);
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
}

void init(GLFWwindow* window, std::vector<glm::vec3>& coords) 
{
	renderingProgram = createShaderProgram();

	glGenBuffers(numVBOs, VBO);
	glGenVertexArrays(numVAOs, VAO);
	
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, coords.size() * sizeof(glm::vec3), coords.data(), GL_STATIC_DRAW);

	glBindVertexArray(VAO[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	object_colors.resize(coords.size());
	object_point_colors.resize(coords.size());
	std::fill(object_colors.begin(), object_colors.end(), glm::vec3(0.5f, 0.5f, 0.5f));
	std::fill(object_point_colors.begin(), object_point_colors.end(), glm::vec3(1.0f, 0.0f, 0.0f));
	glBufferData(GL_ARRAY_BUFFER, object_colors.size() * sizeof(glm::vec3), object_colors.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glUseProgram(renderingProgram);

	modelLoc = glGetUniformLocation(renderingProgram, "model");
	viewLoc = glGetUniformLocation(renderingProgram, "view");
	projectionLoc = glGetUniformLocation(renderingProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
	invTMatrixLoc = glGetUniformLocation(renderingProgram, "invTMatrix");

	glClearColor(0.0, 0.0, 0.0, 1.0);
}

void display(GLFWwindow* window, double currentTime) 
{
	glClear(GL_COLOR_BUFFER_BIT);

	currentTime = glfwGetTime();
	deltaTime = currentTime - lastTime;
	lastTime = currentTime;
	cameraSpeed = 2.5f * (GLfloat)deltaTime;

	if ((keyboard[GLFW_KEY_W]) || (keyboard[GLFW_KEY_UP])) {
		cameraPos[1] += cameraSpeed * 1.0f;
		cameraTarget[1] = cameraPos[1];
	}

	if ((keyboard[GLFW_KEY_S]) || (keyboard[GLFW_KEY_DOWN])) {
		cameraPos[1] -= cameraSpeed * 1.0f;
		cameraTarget[1] = cameraPos[1];
	}

	if ((keyboard[GLFW_KEY_D]) || (keyboard[GLFW_KEY_RIGHT])) {
		angle += 1.0f;
		cameraPos[0] = cameraRange * glm::sin(glm::radians(angle));
		cameraPos[2] = cameraRange * glm::cos(glm::radians(angle));
	}


	if ((keyboard[GLFW_KEY_A]) || (keyboard[GLFW_KEY_LEFT])) {
		angle -= 1.0f;
		cameraPos[0] = cameraRange * glm::sin(glm::radians(angle));
		cameraPos[2] = cameraRange * glm::cos(glm::radians(angle));
	}

	glUseProgram(renderingProgram);

	computeModelMatrix();
	computeCameraMatrix();

	invTmatrix = glm::inverseTranspose(view * model);
	glUniformMatrix4fv(invTMatrixLoc, 1, GL_FALSE, glm::value_ptr(invTmatrix));
	
	glBindVertexArray(VAO[0]);

	glPointSize(10);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, object_point_colors.size() * sizeof(glm::vec3), object_point_colors.data(), GL_STATIC_DRAW);
	glDrawArrays(GL_POINTS, 0, readEntries);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, object_colors.size() * sizeof(glm::vec3), object_colors.data(), GL_STATIC_DRAW);
	glDrawArrays(GL_TRIANGLES, 0, readEntries);

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

	std::cout << argv[1] << std::endl;

	ObjectFileReader objFileReader;
	ObjectFile objFile = objFileReader.parseObjFile(argv[1]);

	std::vector<glm::vec3> vertexes = objFile.getTriangleVertexes();
	readEntries = vertexes.size();

	std::vector<glm::vec3> rawVertexes = objFile.getRawVertexes();
	readVertexEntries = rawVertexes.size();
	
	std::vector<glm::vec3> faces = objFile.getFaces();
	readFaceEntries = faces.size();

	std::cout << "Read vertexes: " << readVertexEntries << std::endl;
	std::cout << "Read faces: " << readFaceEntries << std::endl;

	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make macOS happy; should not be needed otherwise.
	window = glfwCreateWindow(window_width, window_height, window_title, nullptr, nullptr);

	glfwMakeContextCurrent(window);

	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCursorPosCallback(window, cursorPosCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);


	if (glewInit() != GLEW_OK)
		exit(EXIT_FAILURE);

	glfwSwapInterval(1);
	glfwSetWindowAspectRatio(window, 1, 1);

	init(window,vertexes);

	while (!glfwWindowShouldClose(window)) 
	{
		display(window, glfwGetTime());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	cleanUpScene();

	return EXIT_SUCCESS;
}