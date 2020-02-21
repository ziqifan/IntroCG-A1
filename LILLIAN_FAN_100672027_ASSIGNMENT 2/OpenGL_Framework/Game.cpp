#define _CRT_SECURE_NO_WARNINGS
#include "Game.h"

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <MiniMath\Core.h>

#define CHAR_BUFFER_SIZE 128
#define BUFFER_OFFSET(i) ((char *)0 + (i))
#define WINDOW_WIDTH			800
#define WINDOW_HEIGHT			432

struct  MeshFace
{
	MeshFace()
	{
		vertices[0] = 0;
		vertices[1] = 0;
		vertices[2] = 0;

		textureUVs[0] = 0;
		textureUVs[1] = 0;
		textureUVs[2] = 0;

		normals[0] = 0;
		normals[1] = 0;
		normals[2] = 0;
	}

	MeshFace(
		unsigned v1, unsigned v2, unsigned v3,
		unsigned t1, unsigned t2, unsigned t3,
		unsigned n1, unsigned n2, unsigned n3)
	{
		vertices[0] = v1;
		vertices[1] = v2;
		vertices[2] = v3;

		textureUVs[0] = t1;
		textureUVs[1] = t2;
		textureUVs[2] = t3;

		normals[0] = n1;
		normals[1] = n2;
		normals[2] = n3;
	}

	unsigned vertices[3];
	unsigned textureUVs[3];
	unsigned normals[3];
};

const char* vertexShaderSource = 
	"#version 330\n"
	"layout(location = 0) in vec3 in_vert;\n"
	"uniform mat4 u_model;\n"
	"uniform mat4 u_view;\n"
	"uniform mat4 u_projection;\n"
	"void main()\n"
	"{\n"
	"	gl_Position = u_projection * u_view * u_model * vec4(in_vert, 1.0);\n"
	"}\n";

const char* fragmentShaderSource = 
	"#version 330\n"
	"out vec4 pixelColor;\n"
	"void main() { pixelColor = vec4(0.8f, 0.8f, 0.8f, 1.0f); }\n";

Game::Game()
{
}

Game::~Game()
{
	delete updateTimer;

	//...
}

void Game::initializeGame()
{
	updateTimer = new Timer();
	
	std::ifstream input;
	input.open("Pikachu.obj");

	if (!input)
	{
		std::cout << "Could not open the file." << std::endl;
	}

	char inputString[CHAR_BUFFER_SIZE];

	// Unique data
	std::vector<vec3> vertexData;
	std::vector<vec2> textureData;
	std::vector<vec3> normalData;
	// Index/Face data
	std::vector<MeshFace> faceData;
	// OpenGl ready data
	std::vector<float> unPackedVertexData;
	std::vector<float> unPackedTextureData;
	std::vector<float> unPackedNormalData;

	while (!input.eof())
	{
		input.getline(inputString, CHAR_BUFFER_SIZE);
		if (std::strstr(inputString, "#") != nullptr)
		{
			// This line is a comment
			continue;
		}
		else if (std::strstr(inputString, "vt") != nullptr)
		{
			// This line has UV data
			vec2 temp;

			std::sscanf(inputString, "vt %f %f", &temp.x, &temp.y);
			textureData.push_back(temp);
		}
		else if (std::strstr(inputString, "vn") != nullptr)
		{
			// This line has normal data
			vec3 temp;

			std::sscanf(inputString, "vn %f %f %f", &temp.x, &temp.y, &temp.z);
			normalData.push_back(temp);
		}
		else if (std::strstr(inputString, "v") != nullptr)
		{
			// This line has vertex data
			vec3 temp;

			std::sscanf(inputString, "v %f %f %f", &temp.x, &temp.y, &temp.z);
			vertexData.push_back(temp);
		}
		else if (std::strstr(inputString, "f") != nullptr)
		{
			//This line contains face data
			MeshFace temp;

			std::sscanf(inputString, "f %u/%u/%u %u/%u/%u %u/%u/%u",
				&temp.vertices[0], &temp.textureUVs[0], &temp.normals[0],
				&temp.vertices[1], &temp.textureUVs[1], &temp.normals[1],
				&temp.vertices[2], &temp.textureUVs[2], &temp.normals[2]);
			faceData.push_back(temp);
		}
	}

	input.close();

	// Unpack the data
	for (unsigned i = 0; i < faceData.size(); i++)
	{
		for (unsigned j = 0; j < 3; j++)
		{
			unPackedVertexData.push_back(vertexData[faceData[i].vertices[j] - 1].x);
			unPackedVertexData.push_back(vertexData[faceData[i].vertices[j] - 1].y);
			unPackedVertexData.push_back(vertexData[faceData[i].vertices[j] - 1].z);

			unPackedTextureData.push_back(textureData[faceData[i].textureUVs[j] - 1].x);
			unPackedTextureData.push_back(textureData[faceData[i].textureUVs[j] - 1].y);

			unPackedNormalData.push_back(normalData[faceData[i].normals[j] - 1].x);
			unPackedNormalData.push_back(normalData[faceData[i].normals[j] - 1].y);
			unPackedNormalData.push_back(normalData[faceData[i].normals[j] - 1].z);
		}
	}

	_NumFaces = faceData.size();
	_NumVertices = _NumFaces * 3;

	// Send deta to OpenGL //
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO_Verticies);
	glGenBuffers(1, &VBO_UVs);
	glGenBuffers(1, &VBO_Normals);

	glBindVertexArray(VAO);

	glEnableVertexAttribArray(0); //Vertex
	glEnableVertexAttribArray(1); //UVs
	glEnableVertexAttribArray(2); //Normals

	glBindBuffer(GL_ARRAY_BUFFER, VBO_Verticies);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * unPackedVertexData.size(), &unPackedVertexData[0], GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, BUFFER_OFFSET(0));

	glBindBuffer(GL_ARRAY_BUFFER, VBO_UVs);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * unPackedTextureData.size(), &unPackedTextureData[0], GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)1, 1, GL_FLOAT, GL_FALSE, sizeof(float) * 2, BUFFER_OFFSET(0));

	glBindBuffer(GL_ARRAY_BUFFER, VBO_Normals);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * unPackedNormalData.size(), &unPackedNormalData[0], GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, BUFFER_OFFSET(0));

	int success = GL_FALSE; 
	// create shader program 
	program = glCreateProgram(); 

	//create vertex shader 
	vertShader = glCreateShader(GL_VERTEX_SHADER); 
	glShaderSource(
		vertShader,
		1,
		&vertexShaderSource,
		nullptr); 

	glCompileShader(vertShader);

	glGetShaderiv(
		vertShader,
		GL_COMPILE_STATUS,
		&success); 
	if (success == GL_FALSE)
	{
		exit(0);
	}

	//create fragment shader 
	fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(
		fragShader,
		1,
		&fragmentShaderSource,
		nullptr);

	glCompileShader(fragShader);

	glGetShaderiv(
		fragShader,
		GL_COMPILE_STATUS,
		&success);
	if (success == GL_FALSE)
	{
		exit(0);
	}

	//Build Program Object
	glAttachShader(program, vertShader);
	glAttachShader(program, fragShader);
	glLinkProgram(program); 
	
	//Setup Scene
	float aspect = static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT); 
	camera.perspective(60.0f, aspect, 1.0f, 1000.0f); 
	camera.m_pLocalPosition = vec3(0.0f, 300.0f, 450.0f);
	camera.setRotationAngleX(-20.0f); 

	// ...
}

void Game::update()
{
	// update our clock so we have the delta time since the last update
	updateTimer->tick();

	float deltaTime = updateTimer->getElapsedTimeSeconds();
	TotalGameTime += deltaTime;

	// Set mesh position and give the mesh some motion over time.
	pikachu.setPosition(vec3(-250.0f, 0.0f, 0.0f));
	pikachu.setRotationAngleY(TotalGameTime * 15.0f);

	pikachu2.setPosition(vec3(250.0f, 0.0f, 0.0f));
	pikachu2.setRotationAngleY(TotalGameTime * 15.0f);

	// Give our Transforms a chance to compute the latest matrices
	camera.update(deltaTime);
	pikachu.update(deltaTime);
	pikachu2.update(deltaTime);
}

void Game::draw()
{
	// Clear the Back-Buffer
	glEnable(GL_DEPTH_TEST); 
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Bind program
	glUseProgram(program); 

	//Send MVP to the shaders 
	int modelLoc = glGetUniformLocation(
		program,
		"u_model"); 
	int viewLoc = glGetUniformLocation(
		program,
		"u_view");
	int projectionLoc = glGetUniformLocation(
		program,
		"u_projection");

	glUniformMatrix4fv(viewLoc, 1, false,
		camera.getLocalToWorldMatrix().GetInverse().data);

	glUniformMatrix4fv(projectionLoc, 1, false,
		camera.getProjection().data);

	// Bind VAO
	glBindVertexArray(VAO);

	glUniformMatrix4fv(modelLoc, 1, false,
		pikachu.getLocalToWorldMatrix().data);
	glDrawArrays(GL_TRIANGLES, 0, 400 * 3);

	glUniformMatrix4fv(modelLoc, 1, false,
		pikachu2.getLocalToWorldMatrix().data);
	glDrawArrays(GL_TRIANGLES, 0, 400 * 3);
 
	// Revert the states we set during the function.
	glBindVertexArray(GL_NONE);
	glUseProgram(GL_NONE);

	glutSwapBuffers();
}

void Game::keyboardDown(unsigned char key, int mouseX, int mouseY)
{
	switch(key)
	{
	case 27: // the escape key
	case 'q': // the 'q' key
		exit(1);
		break;
	}
}

void Game::keyboardUp(unsigned char key, int mouseX, int mouseY)
{
	switch(key)
	{
	case 32: // the space bar
		// Press Space Bar to switch camera between Perspective and Orthographic
		if (camera.getProjectionType() == ProjectionType::Perspective)
		{
			camera.orthographic(-250.0f * 1.89f, 250.0f * 1.89f, -250.0f, 250.0f, 1, 10000.0f);
		}
		else
		{
			camera.perspective(60.0f, static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT), 1.0f, 1000.0f);
		}
		break;
	case 27: // the escape key
	case 'q': // the 'q' key
		exit(1);
		break;
	}
}

void Game::mouseClicked(int button, int state, int x, int y)
{
	if(state == GLUT_DOWN) 
	{
		switch(button)
		{
		case GLUT_LEFT_BUTTON:

			break;
		case GLUT_RIGHT_BUTTON:
		
			break;
		case GLUT_MIDDLE_BUTTON:

			break;
		}
	}
	else
	{

	}
}

/*
 * mouseMoved(x,y)
 * - this occurs only when the mouse is pressed down
 *   and the mouse has moved.  you are given the x,y locations
 *   in window coordinates (from the top left corner) and thus 
 *   must be converted to screen coordinates using the screen to window pixels ratio
 *   and the y must be flipped to make the bottom left corner the origin.
 */
void Game::mouseMoved(int x, int y)
{
}
