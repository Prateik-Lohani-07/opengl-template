#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void checkShaderCompileStatus(unsigned int shader);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const char* vertexShaderSource = "#version 460 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"	gl_Position = vec4(aPos.x, aPos.y, aPos.y, 1.0);\n"
"}\0";

const char* fragmentShaderSource = "#version 460 core\n"
"out vec4 FragColor;\n"
"\n"
"void main()\n"
"{\n"
"	FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\n";

int main(void)

{
	// init glfw 
	glfwInit();
	
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// create glfw window
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "My Triangle!!!", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK) {
		std::cout << "Error while trying to setup function pointers for OpenGL!" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// opengl code truly begins
	float vertices[][3] = {
		{-0.5f, -0.5f, 0.0f},
		{0.5f, -0.5f, 0.0f},
		{0.0f, 0.5f, 0.0f}
	};

	// vertex shader
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER); // specify our type of shader- vertex shader here
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL); // attach that GLSL source code this shader
	glCompileShader(vertexShader);

	// to check if the above compilation of the source code was successful, check the output and logs
	// like so
	int success;
	char infoLog[512];
	// the glGetShaderiv is supposed to simply get any parameters from a vertex buffer object (VBO)
	// kinda like how in OOP, an object has many attributes in it.
	// Now here, we get the compilation status of the VBO in order to check if it was correctly
	// compiled and is good to use
	// here are the possible values here:
	// - GL_SHADER_TYPE
	// - GL_DELETE_STATUS
	// - GL_COMPILE_STATUS
	// - GL_INFO_LOG_LENGTH
	// - GL_SHADER_SOURCE_LENGTH
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}


	// -------------------------------------
	// fragment shader time!
	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	checkShaderCompileStatus(fragmentShader);


	// now create the entire program for OpenGL
	unsigned int shaderProgram;
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	// ONLY ONCE THE SHADERS ARE LINKED, can you finally delete them off memory
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	/*
	And OpenGL FINALLY has our data !!! 🥳
	problem is, it still doesn't know how to use that data
	Q) but what do you mean??? we said to interpret that as vertex data right?
		--> well the thing is, opengl still doesn't know how to connect the vertex data to the
			inputs to the shaders yet
			basically, you have an array like [1, 2, 3, 4, 5, 6]
			while opengl knows that it's part of an array buffer - it says
			"boy i don't know if these are 2 pairs of 3d coordinates or 3 pairs of 2d coordinates
			like [(1, 2, 3), (4, 5, 6)] ?? or is it [(1, 2), (3, 4), (5, 6)] ??"
			my point is that, just given an array, it has no clue on how to parse 
			the array into vector variables.
	
			it's like reading an incoming language (array values in our case) and packing them
			into words of a different language (vec3 and vec4 objects for opengl)
	This configuration then connects the parsed data to shader
	attributes such as:
	layout(location = 0) in vec3 aPos;
	
	read more about this method here: https://docs.gl/gl4/glVertexAttribPointer
	
	now what the heck is an attribute ??
			an attribute is nothing but a property of a vertex
			we usually think of a vertex as nothing more than just a position
		however, it's more than that -> a vertex can have a position, a normal, a texture, color etc. 
			EACH of these values can specified in one line of that vertices 2D array above
			float vertices[][3] = {
				{-0.5f, -0.5f, 0.0f},
				{0.5f, -0.5f, 0.0f},
				{0.0f, 0.5f, 0.0f}
			};
			
		in here, each vertex right now has 3 floats in it but OpenGl doesn't know which of those 3 valuse
		belong to what attribute --> 
				for eg.- does {-0.5f, -0.5f, 0.0f} mean that the position is {-0.5f, -0.5f} (a 2D coordinate) and normal is 0.0f ?? 
						or does it mean it's just a 3D coordinate with {-0.5f, -0.5f, 0.0f} as position and nothign else?

						this seems confusing because to us we know what those contents mean, but you have to understand, opengl
						DOES NOT take in a pointer of floats --> it takes in a void pointer -> meaning at runtime, it doesn't know
						what the heck that byte of array points to.

						for more context take C++ itself as an eg- both int and float are 4 bytes, so if i do something like this:
						float x = 4.0f;
						int *ptr = (int*) &x;
						int val = *ptr;

						then val isn't gonna be 4.0 --> it will become something ENTIRELY different and that's because we can
						change the way C++ looks at memory; and because int and float are organized within those 32 bits (4B) in 
						different manner, then interpreting a float's layout as an int is gonna make a very weird random value 
						appear to us, even though behind the scenes C++ is working absolutely fine.

						Same goes for OpenGL where it has to be told how the memory is laid out so that when it sees the vertex
						, which you now KNOW is WAY MORE THAN JUST POSITIONS (it can consist of other values like texture, color,
						normal etc.), it knows how to interpret and then use it within those shader functions that we've written
						so far.

			so glVertexAttribPointer, what does it do??

			void glVertexAttribIPointer(	
				GLuint index,
				GLint size,
				GLenum type,
				GLsizei stride,
				const GLvoid * pointer
			);

				it basically looks at a piece of memory and specifies:
					for a given index (remember that location = 0 we did before) in the attributes slot, THIS is how the attribute
					is defined: so for the parameters, we have the type of the value for the attribute, 
					the no. of values we have for that type (so for eg. 3 GL_FLOATS make up the 'normal' attribute of 
					the vertex, let's say). 
					We also specify the stride which forms the offset in order to jump to the next vertex 
					and finally of course the pointer, which is the byte offset INSIDE SAID VERTEX to this attribute 
					that we're defining. 

				this is a lot to take in, so just read through that again if you need to. 

				Now comes the index, which is the one that's quite important to understand. 
				In your shader you can do things like this:

				layout (location = 0) in vec3 aPos;
				layout (location = 1) in vec3 aNormal;

				does it click now?
				YOUR SHADERS DON'T RECEIVE VERTICES, THEY RECIEVE ATTRIBUTES
				and those attributes are parsed from memory using the definition you passed into glVertexAttribPointer

				So index = 0 in glVertexAttribPointer(0, ...) means that you're defining the attribute at 0th slot in the vertex

				But what do i mean by slot, coz i've said it like twice so far --> well apparently there's a limit on how many
				attributes you can have per vertex, usually it's 16. 

				What this means is that there can be locations from 0 to 15:

				layout (location = 0) in vec3 position;
				layout (location = 1) in vec3 normal;
				layout (location = 2) in vec3 color;
				...
				layout (location = 14) in vec3 someOtherAttribute;
				layout (location = 15) in vec3 texture;

				So you can have near infinite no. of vertices but only a limited no. of attributes-per-vertex.

	*/
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (const void*)0);
	/*
		apparently you also need to enable an attribute before you can use it further
		you can switch the ordering b/w glEnableVertexAttribArray and glVertexAttribPointer easily, there's no issues with that
		just remember to call enable though, that's all that matters

		aaand congratulations, that's 2 more lines of code after an hour's worth of explanation 🥲
	*/
	glEnableVertexAttribArray(0); 

	unsigned int vbo, vao;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	
	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	/*
		1st arg -> which vertex attribute we wanna configure
			if you recall we did layout (location = 0), setting the location of the vertex attribute to 0
	*/
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	while (!glfwWindowShouldClose(window)) {
		processInput(window);

		// render begin
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(shaderProgram);
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		// render end

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteProgram(shaderProgram);

	// free up memory and return to the real world :3
	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void checkShaderCompileStatus(unsigned int shader) {
	// to check if the above compilation of the source code was successful, check the output and logs
	// like so
	int success;
	char infoLog[512];
	// the glGetShaderiv is supposed to simply get any parameters from a vertex buffer object (VBO)
	// kinda like how in OOP, an object has many attributes in it.
	// Now here, we get the compilation status of the VBO in order to check if it was correctly
	// compiled and is good to use
	// here are the possible values here:
	// - GL_SHADER_TYPE
	// - GL_DELETE_STATUS
	// - GL_COMPILE_STATUS
	// - GL_INFO_LOG_LENGTH
	// - GL_SHADER_SOURCE_LENGTH
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::_::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
}