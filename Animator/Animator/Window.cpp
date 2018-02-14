#include "window.h"

const char* window_title = "CSE169_Project02";
GLint shaderProgram;
GLint skinProgram;

Cube* point;
Model* M;

// Viewport
bool split = false;

// Animation
Animation* A;
float delta = 0.0f;

// IK solver
glm::vec3 goal;
int niter = 0;
int niterLim = 100;
bool IKmode = false;
float goalAngle = 0.0f;
float goalRadius = 7.f;

// Morphing
float phi = 0.0f;

Joint* selected;
glm::vec2 prev_pos;
int selectedInd = -1;
float angleStep = 0.1f;
bool rotate_flag_L = false;
bool rotate_flag_R = false;
bool locked = false;

int mode = 0;
bool wireframe = false;

// On some systems you need to change this to the absolute path
//#define VERTEX_SHADER_PATH "./shader.vert"
//#define FRAGMENT_SHADER_PATH "./shader.frag"

// Default camera parameters
glm::vec3 cam_pos(0.0f, 0.0f, 7.0f);		// e  | Position of camera
glm::vec3 cam_look_at(0.0f, 0.0f, 0.0f);	// d  | This is where the camera looks at
glm::vec3 cam_up(0.0f, 1.0f, 0.0f);			// up | What orientation "up" is

int Window::width;
int Window::height;

glm::mat4 Window::P;
glm::mat4 Window::V;

void Window::mainMenu()
{
	do 
	{
		cout << "-------------------------------" << endl;
		string input;
		getline(cin, input);
		istringstream iss(input);
		do
		{
			// Process each word
			string word;
			iss >> word;

			if (word == "project1")
			{
				string skel;
				iss >> skel;
				string skelFilePath = ".//Resources//skel//" + skel + ".skel.txt";
				ifstream file(skelFilePath);
				if (!file)
				{
					cout << "Cannot open input .skel file.\n" << endl;
					mode = -1;
				}
				else
				{
					cout << "wth" << endl;
					delete M;
					M = new Model();
					M->readSkel(skelFilePath.c_str());
					mode = 0;
				}
				break;
			}
			else if (word == "project2")
			{
				string skel, skin;
				iss >> skel >> skin;
				string skelFilePath = ".//Resources//skel//" + skel + ".skel.txt";
				string skinFilePath = ".//Resources//skin//" + skin + ".skin.txt";

				ifstream file0(skelFilePath);
				ifstream file1(skinFilePath);

				if (file0 && file1)
				{
					delete M;
					M = new Model();
					M->readSkel(skelFilePath.c_str());
					M->readSkin(skinFilePath.c_str());
					mode = 1;

					// Texture
					if (M->skin->isTex)
					{
						string texFilePath = ".//Resources//textures//" + skel + ".bmp";
						M->skin->loadBMP_custom(texFilePath.c_str());
						skinProgram = LoadShaders(".//Shaders//skinTex.vert", ".//Shaders//skinTex.frag");
					}
					else
						skinProgram = LoadShaders(".//Shaders//skin.vert", ".//Shaders//skin.frag");
				}
				else
				{
					cout << "Cannot open input .skel or .skin file.\n" << endl;
					mode = -1;
				}
				break;
			}
			else
			{
				cout << "Invalid project argument" << endl;
				mode = -1;
				break;
			}
		} while (iss);
	} while (mode == -1);
}

void Window::initialize_objects()
{
	// Test zone
	A = new Animation();
	A->readAnim(".//Resources//anim//wasp_walk.anim.txt");

	shaderProgram = LoadShaders(".//Shaders//shader.vert", ".//Shaders//shader.frag");
	skinProgram = LoadShaders(".//Shaders//skin.vert", ".//Shaders//skin.frag");

	M = new Model();

	// By default, read in wasp model
	mode = 1;
	M->readSkel(".//Resources//skel//wasp2.skel.txt");	
	M->readSkin(".//Resources//skin//wasp2.skin.txt");

	// For IK purpose
	goal = glm::vec3(goalRadius * glm::cos(goalAngle), 4, goalRadius * glm::sin(goalAngle));
	point = new Cube();
}

void Window::clean_up()
{
	delete M;
	glDeleteProgram(shaderProgram);
}

GLFWwindow* Window::create_window(int width, int height)
{
	// Initialize GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return NULL;
	}

	// 4x antialiasing
	glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__ // Because Apple hates comforming to standards
	// Ensure that minimum OpenGL version is 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Enable forward compatibility and allow a modern OpenGL context
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// Create the GLFW window
	GLFWwindow* window = glfwCreateWindow(width, height, window_title, NULL, NULL);

	// Check if the window could not be created
	if (!window)
	{
		fprintf(stderr, "Failed to open GLFW window.\n");
		fprintf(stderr, "Either GLFW is not installed or your graphics card does not support modern OpenGL.\n");
		glfwTerminate();
		return NULL;
	}

	// Make the context of the window
	glfwMakeContextCurrent(window);

	// Set swap interval to 1
	glfwSwapInterval(1);

	// Get the width and height of the framebuffer to properly resize the window
	glfwGetFramebufferSize(window, &width, &height);
	// Call the resize callback to make sure things get drawn immediately
	Window::resize_callback(window, width, height);

	return window;
}

void Window::resize_callback(GLFWwindow* window, int width, int height)
{
#ifdef __APPLE__
	glfwGetFramebufferSize(window, &width, &height); // In case your Mac has a retina display
#endif
	Window::width = width;
	Window::height = height;
	// Set the viewport size. This is the only matrix that OpenGL maintains for us in modern OpenGL!
	glViewport(0, 0, width, height);

	if (height > 0)
	{
		P = glm::perspective(45.0f, (float)width / (float)height, 0.1f, 1000.0f);
		V = glm::lookAt(cam_pos, cam_look_at, cam_up);
	}
}

void Window::idle_callback()
{
	// Should prob update skin here as well
	if (IKmode && selectedInd != -1)
	{
		//niter++;
		if (M->IKsolver(selectedInd, goal) == -1)
		{
			//cout << "IK Mode off: goal reached" << endl;
			//IKmode = false;
			niter = 0;
		}
		else if (niter > niterLim)
		{
			cout << "IK Mode off: goal cannot be reached" << endl;
			IKmode = false;
			niter = 0;
		}
	}
}

void Window::draw()
{
	// Skeleton mode
	if (mode == 0)
	{
		glUseProgram(shaderProgram);
		M->draw(shaderProgram);
	}
	else if (mode == 1)
	{
		glUseProgram(skinProgram);
		M->skin->draw(skinProgram);
	}
	else
	{
	}

	if (IKmode)
	{
		glUseProgram(shaderProgram);
		//point->draw(shaderProgram, glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 T = glm::translate(glm::mat4(1.0f), goal) * glm::scale(glm::mat4(1.0f), glm::vec3(0.1f, 0.1f, 0.1f));
		point->draw(shaderProgram, T, glm::vec3(0.0f, 1.0f, 0.0f));
	}
}

void Window::display_callback(GLFWwindow* window)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Viewport test
	if (split)
	{
		/*
		// left bottom
		glViewport(0, 0, width*0.5, height*0.5);
		draw();

		// right bottom
		glViewport(width*0.5, 0, width*0.5, height*0.5);
		draw();
		*/

		// bottom
		glViewport(0, 0, width, height*0.5);
		//draw();
		A->DisplayChannel(M, selectedInd);

		// left top
		glViewport(0, height*0.5, width*0.5, height*0.5);
		glUseProgram(shaderProgram);
		M->draw(shaderProgram);

		// right top
		glViewport(width*0.5, height*0.5, width*0.5, height*0.5);
		glUseProgram(skinProgram);
		M->skin->draw(skinProgram);

		glViewport(0, 0, width, height); //restore default
	}
	else
	{
		draw();
	}

	glfwPollEvents();
	glfwSwapBuffers(window);
}

void Window::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_ESCAPE)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}

		// Go to main menu
		if (key == GLFW_KEY_0)
		{
			mainMenu();
		}

		// Viewport test
		if (key == GLFW_KEY_V)
		{
			if (split)
				split = false;
			else
				split = true;
		}

		// Animation test
		if (key == GLFW_KEY_T)
		{
			if (mods == GLFW_MOD_SHIFT)
			{
				delta += 0.1f;
				A->Play(M, delta);
				M->skin->update(phi, 0);
			}
			else
			{
				delta -= 0.1f;
				A->Play(M, delta);
				M->skin->update(phi, 0);
			}
		}

		// Skeleton/Skin mode
		if (key == GLFW_KEY_S)
		{
			if (mode == 0)
			{
				if (M->skin != nullptr)
					mode = 1;
			}
			else if(mode == 1)
				mode = 0;
		}

		// Wireframe/Shaded mode
		if (key == GLFW_KEY_W)
		{
			if (wireframe)
			{
				cout << "[Shaded Mode] on" << endl;
				wireframe = false;
			}
			else
			{
				cout << "[Wireframe Mode] on" << endl;
				wireframe = true;
			}
		}
		if (key == GLFW_KEY_LEFT)
		{
			goalAngle -= angleStep;
			goal = glm::vec3(goalRadius * glm::cos(goalAngle), goal[1], goalRadius * glm::sin(goalAngle));
		}
		if (key == GLFW_KEY_RIGHT)
		{
			goalAngle += angleStep;
			goal = glm::vec3(goalRadius * glm::cos(goalAngle), goal[1], goalRadius * glm::sin(goalAngle));
		}
		if (key == GLFW_KEY_UP)
		{
			goal[1] += 0.2f;
		}
		if (key == GLFW_KEY_DOWN)
		{
			goal[1] -= 0.2f;
		}
		if (key == GLFW_KEY_I)
		{
			if (IKmode)
			{
				cout << "[IK Mode] off" << endl;
				IKmode = false;
			}
			else
			{
				cout << "[IK Mode] on" << endl;
				IKmode = true;
				niter = 0;
			}
		}

		// Right eyebrow
		if (key == GLFW_KEY_M)
		{
			if (mods == GLFW_MOD_SHIFT)
			{
				phi += 0.1f;
				M->skin->update(phi, 2);
			}
			else
			{
				if (phi <= 0)
					phi = 0;
				else
				{
					phi -= 0.1f;
					M->skin->update(phi, 2);
				}
			}
		}

		// Left cheek
		if (key == GLFW_KEY_N)
		{
			if (mods == GLFW_MOD_SHIFT)
			{
				phi += 0.1f;
				M->skin->update(phi, 1);
			}
			else
			{
				if (phi <= 0)
					phi = 0;
				else
				{
					phi -= 0.1f;
					M->skin->update(phi, 1);
				}
			}
		}

		if (rotate_flag_L)
		{
			int axis = -1;
			if (key == GLFW_KEY_X)
				axis = 0;
			else if (key == GLFW_KEY_Y)
				axis = 1;
			else if (key == GLFW_KEY_Z)
				axis = 2;

			if (locked && axis >= 0)
			{
				if (selected)
				{
					if (mods == GLFW_MOD_SHIFT)
					{
						M->updateJoint(selectedInd, angleStep, axis);
						M->skin->update(phi, 0);
					}
					else
					{
						M->updateJoint(selectedInd, -angleStep, axis);
						M->skin->update(phi, 0);
					}
				}
			}
		}

	}
}

void Window::mouse_callback(GLFWwindow* window, int button, int action, int mods)
{

	// Rotate 
	if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		if (action == GLFW_PRESS)
		{
			double xpos;
			double ypos;
			glfwGetCursorPos(window, &xpos, &ypos);
			int tempInd = M->getNNid(glm::vec2(xpos, ypos), V, P, width, height);
			Joint* temp = nullptr;

			if (tempInd != -1)
			{
				temp = M->allJoints[tempInd];
				cout << "[" << tempInd << "] joint selected" << endl;
			}

			if (!locked)
			{
				if (temp)
				{
					locked = true;
					selected = temp;
					selectedInd = tempInd;
				}
				else
					locked = false;
			}
			rotate_flag_L = true;
		}
		else if (action == GLFW_RELEASE)
		{
			rotate_flag_L = false;
			locked = false;
		}
	}

	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		double xpos;
		double ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		prev_pos = glm::vec2(xpos, ypos);
		rotate_flag_R = true;
	}
	else
		rotate_flag_R = false;

}

glm::vec3 Window::trackball(glm::vec2 point)
{
	glm::vec3 mapped;
	float d;
	mapped.x = (2.0f * point.x - width) / width;
	mapped.y = (height - 2.0f * point.y) / height;
	mapped.z = 0.0f;

	d = glm::length(mapped);
	d = (d < 1.0f) ? d : 1.0f;
	mapped.z = sqrtf(1.001f - d * d);
	mapped = glm::normalize(mapped);

	return mapped;
}

void Window::cursor_pos_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (rotate_flag_R)
	{
		// Calculate angle, axis, and rotation mtx
		glm::vec3 prev_pos_ball = trackball(prev_pos);
		glm::vec3 curr_pos_ball = trackball(glm::vec2(xpos, ypos));

		//glm::vec3 dir = curr_pos_ball - prev_pos_ball;
		float angle = glm::degrees(glm::acos(std::min(1.0f, glm::dot(curr_pos_ball, prev_pos_ball))));
		glm::vec3 axis = glm::normalize(glm::cross(prev_pos_ball, curr_pos_ball));
		glm::mat4 R = glm::rotate(glm::mat4(1.0f), angle * 0.01f, axis);

		// Model rotation
		//M->rotate(R);

		// Camera rotation
		cam_pos = glm::vec3(glm::rotate(glm::mat4(1.0f), -angle * 0.01f, axis) * glm::vec4(cam_pos, 1.0f));
		cam_up = glm::vec3(glm::rotate(glm::mat4(1.0f), -angle * 0.01f, axis) * glm::vec4(cam_up, 1.0f));
		V = glm::lookAt(cam_pos, cam_look_at, cam_up);

		prev_pos = glm::vec2(xpos, ypos);
	}
}

void Window::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	float scalefactor = (yoffset > 0) ? 1.1f : 1 / 1.1f;
	cam_pos = glm::vec3(glm::scale(glm::mat4(1.0f), glm::vec3(scalefactor)) * glm::vec4(cam_pos, 1.0f));
	V = glm::lookAt(cam_pos, cam_look_at, cam_up);
}
