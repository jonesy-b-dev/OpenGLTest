#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>


const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;";

const char* fragmentShaderSource;

int windowHeight = 800;
int windowWidth = 800;

int main()
{
	///Init program
	glfwInit();



	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//GLfloat 

	//Create GLFWwindow
	GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Epik OpenGLEngine", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	gladLoadGL();

	glViewport(0, 0, windowWidth, windowHeight);

	glClearColor(0.04f, 0.13f, 0.17, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	///Run programm
	glfwSwapBuffers(window);

	//Main while loop
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
	}

	//End programm
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}