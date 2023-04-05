#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>


struct ShaderProgramSource
{
	std::string VertexSource;
	std::string FragmentSource;
};

static ShaderProgramSource ParseShader(const std::string& filepath)
{
	std::ifstream stream(filepath);

	enum class ShaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	ShaderType type = ShaderType::NONE;

	std::string line;
	std::stringstream ss[2];

	while (getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
			{
				type = ShaderType::VERTEX;
			}
			else if (line.find("fragment") != std::string::npos)
			{
				type = ShaderType::FRAGMENT;
			}
			else
			{
				ss[(int)type] << line << '\n';
			}
		}
	}

	return { ss[0].str(), ss[1].str() };
};

//Compiles the shader
static unsigned int CompileShader( unsigned int type, const std::string& source) 
{
	unsigned int id = glCreateShader(type);				//Creates the shader
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
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);		//Sets the major version of OpenGL
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4.6);	//Sets the minor version of OpenGL
	
	//Error handling if window fails to create
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    //Make the window's context current
	glfwMakeContextCurrent(window);

	//Initialize GLEW and print error when it fails
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
	glGenBuffers(1, &buffer);				//Creates 1 buffer and stores the id in "buffer"
	glBindBuffer(GL_ARRAY_BUFFER, buffer);  //Binds the buffer to the vertex buffer
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW);	//Sets the data of the buffer

	//Enables the vertex attribute array
	glEnableVertexAttribArray(0);
    
	//Specifies the layout of the vertex attribute array
	//                   index, size, type, normalized, stride, pointer
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
    
	ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
	std::cout << source.VertexSource << std::endl;
	std::cout << source.FragmentSource << std::endl;

	unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
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