#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>


static unsigned int CompileShader( unsigned int type, const std::string& source) 
{
	unsigned int id = glCreateShader(type); //Creates the shader
	const char* src = source.c_str();                   //Converts the source to a c string
	glShaderSource(id, 1, &src, nullptr);               //Sets the source of the shader
	glCompileShader(id);                                //Compiles the shader

	//Error handling
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result); 	    //Checks if the shader compiled correctly
	if (result == GL_FALSE)
	{
		int length; 
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);         //Gets the length of the error message
		char* message = (char*)alloca(length * sizeof(char));   //Allocates memory for the error message
		glGetShaderInfoLog(id, length, &length, message); 	    //Gets the error message
		std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl; //Prints the error message
		std::cout << message << std::endl; 
		glDeleteShader(id); 
		return 0;
	}
    
    return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	unsigned int program = glCreateProgram();                           //Creates the program
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);    //Creates the vertex shader
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);//Creates the fragment shader

	glAttachShader(program, vs);    //Attaches the shaders to the program
	glAttachShader(program, fs);    //Attaches the shaders to the program
	glLinkProgram(program);         //Links the shaders to the program
	glValidateProgram(program);     //Checks if the program can run on the current hardware

	glDeleteShader(vs);             //Deletes the vertex shader
	glDeleteShader(fs);             //Deletes the fragment shader

	return program;
}

int main(void)
{
    //Create window pointer
    GLFWwindow* window; 

    // Initialize the library
    if (!glfwInit())
    {
        return -1;
    }

    //Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    //Make the window's context current
	glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
    {
	    std::cout << "Glew is not OK" << std::endl;
    }

	//Print OpenGL version
	std::cout << glGetString(GL_VERSION) << std::endl;
    
    
	//Positions of the vertices
    float positions[6] = {
        -0.5f, -0.5f,
         0.0f,  0.5f,
         0.5f, -0.5f
    };
    
	//Creates a buffer and stores the id in buffer
    unsigned int buffer;
	glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW);

	//Enables the vertex attribute array
	glEnableVertexAttribArray(0);
    
	//Specifies the layout of the vertex attribute array
	//                   index, size, type, normalized, stride, pointer
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
    
    std::string vertexShader = 
		"#version 440 core\n"
		"\n"
		"layout(location = 0) in vec4 position;\n"
		"void main()\n"
		"{\n"
		"   gl_Position = position;\n"
		"}\n";
	
	std::string fragmentShader =
		"#version 440 core\n"
		"\n"
		"layout(location = 0) out vec4 color;\n"
		"void main()\n"
		"{\n"
		"   color = vec4(1.0, 0.0, 0.0, 1.0);\n"
		"}\n";
	
	unsigned int shader = CreateShader(vertexShader, fragmentShader);
	glUseProgram(shader);
    
    //Loop until the user closes the window
    while (!glfwWindowShouldClose(window))
    {
        // Render here
        glClear(GL_COLOR_BUFFER_BIT);
        
		glDrawArrays(GL_TRIANGLES, 0, 3);

        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

	glDeleteProgram(shader); 

    glfwTerminate();
    return 0;
}
