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

	//Enum that holds the diffrent types of shaders types
	enum class ShaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	std::string line;						//Create string that holds the current line in the shader
	std::stringstream ss[2];				//Create a string stream that holds the full source code of the shader
	ShaderType type = ShaderType::NONE;		//Create a variable that holds the type of shader from the enum
	
	//Loop over the shader code and find the type of shader, then store the source code in the string stream
	while (getline(stream, line))
	{
		//Find #shader in the shader file and check if it is a vertex or fragment shader
		if (line.find("#shader") != std::string::npos)
		{
			//Now check if the line contains vertex or fragment
			if (line.find("vertex") != std::string::npos)
			{
				type = ShaderType::VERTEX;
			}
			else if (line.find("fragment") != std::string::npos)
			{
				type = ShaderType::FRAGMENT;
			}
		}
		//If the line does not contain #shader then store the line in the string stream, this is the actual source code
		else
		{
			ss[(int)type] << line << '\n';
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
    float positions[] = {
        -0.5f, -0.5f,	//0
		 0.5f, -0.5f,	//1
         0.5f,  0.5f,	//2
		-0.5f,  0.5f,	//3
    };

	//Indices of the vertices
	unsigned int indices[] = {
		0, 1, 2, //Triangle 1
		2, 3, 0  //Triangle 2
	};
    
	//Creates a vertex buffer and stores the id in buffer
	unsigned int buffer;
	glGenBuffers(1, &buffer);				//Creates 1 buffer and stores the id in "buffer"
	glBindBuffer(GL_ARRAY_BUFFER, buffer);  //Binds the buffer to the vertex buffer
				//target		//Size of the buffer	//Data	  //How the data is going to be used
	glBufferData(GL_ARRAY_BUFFER, 6 * 2 *sizeof(float), positions, GL_STATIC_DRAW);	//Sets the data of the buffer

	//Enables the vertex attribute array
	glEnableVertexAttribArray(0);
    
	//Specifies the layout of the vertex attribute array
	//                   index, size, type, normalized, stride, pointer
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
    
	//Creates a index buffer and stores the id in buffer
	unsigned int ibo;
	glGenBuffers(1, &ibo);						//Creates 1 buffer and stores the id in "buffer"
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);  //Binds the buffer to the vertex buffer
				//target				//Size of the buffer		//Data						6//How the data is going to be used
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);	//Sets the data of the buffer
	
	//imlicit converting the parseshader return type to a ShaderProgrammSource
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
        
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents(); 
    }

	glDeleteProgram(shader); 

    glfwTerminate();
    return 0;
}