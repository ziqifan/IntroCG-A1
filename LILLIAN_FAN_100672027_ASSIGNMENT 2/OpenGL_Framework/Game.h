#pragma once
#include <windows.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <GL/freeglut.h>

#include "Timer.h"
#include "Camera.h"
#include "Transform.h"

class Game
{
public:
	Game();
	~Game();

	void initializeGame();
	void update();
	void draw();

	/* input callback functions */
	void keyboardDown(unsigned char key, int mouseX, int mouseY);
	void keyboardUp(unsigned char key, int mouseX, int mouseY);
	void mouseClicked(int button, int state, int x, int y);
	void mouseMoved(int x, int y);

	/* Data Members */
	Timer *updateTimer	= nullptr;
	float TotalGameTime = 0.0f;

	GLuint VBO_Verticies = 0;
	GLuint VBO_UVs = 0;
	GLuint VBO_Normals = 0;
	GLuint VAO = 0;

private:

	unsigned int program = GL_NONE;
	unsigned int vertShader = GL_NONE;
	unsigned int fragShader = GL_NONE; 

	Camera camera; 
	Transform pikachu;
	Transform pikachu2;

	unsigned int _NumFaces = 0;
	unsigned int _NumVertices = 0;

};
