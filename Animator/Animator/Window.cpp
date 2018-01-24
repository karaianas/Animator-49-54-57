#include "window.h"

const char* window_title = "CSE169_Project01";
GLint shaderProgram;
vector<Model*> models;
int modelId;
Cube* point;
Model* M;
Model* test;
Model* wasp;
Model* dragon;
Model* custom;
Joint* selected;
glm::vec2 prev_pos;

int niter = 0;
int niterLim = 100;
int selectedInd = -1;
float goalAngle = 0.0f;
float goalRadius = 7.f;
float angleStep = 0.1f;
bool rotate_flag_L = false;
bool rotate_flag_R = false;
bool locked = false;
bool wireframe = false;
bool IKmode = false;


//glm::vec3 goal(0.0f, 2.0f, 8.0f);
glm::vec3 goal;

// On some systems you need to change this to the absolute path
#define VERTEX_SHADER_PATH "./shader.vert"
#define FRAGMENT_SHADER_PATH "./shader.frag"

// Default camera parameters
glm::vec3 cam_pos(0.0f, 0.0f, 3.0f);		// e  | Position of camera
glm::vec3 cam_look_at(0.0f, 0.0f, 0.0f);	// d  | This is where the camera looks at
glm::vec3 cam_up(0.0f, 1.0f, 0.0f);			// up | What orientation "up" is

int Window::width;
int Window::height;

glm::mat4 Window::P;
glm::mat4 Window::V;

void Window::initialize_objects()
{
	test = new Model();
	test->readFile(".//Resources//test.skel.txt");
	models.push_back(test);

	wasp = new Model();
	wasp->readFile(".//Resources//wasp.skel.txt");
	models.push_back(wasp);

	dragon = new Model();
	dragon->readFile(".//Resources//dragon.skel.txt");
	models.push_back(dragon);

	custom = new Model();
	custom->readFile(".//Resources//custom.skel.txt");
	models.push_back(custom);
	
	modelId = 0;
	M = models[modelId];

	shaderProgram = LoadShaders(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH);

	goal = glm::vec3(goalRadius * glm::cos(goalAngle), 4, goalRadius * glm::sin(goalAngle));
	point = new Cube();
	//point->color = glm::vec3(0.0f, 1.0f, 0.0f);
	//point->worldM = glm::translate(glm::mat4(1.0f), goal);
}

void Window::clean_up()
{
	for (auto ptr : models)
		delete ptr;
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
	// Call the update function the cube
	//cube->update();
	if (IKmode && selectedInd != -1)
	{
		//niter++;
		if (M->IKsolver(selectedInd, goal) == -1)
		{
			cout << "IK Mode off: goal reached" << endl;
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

void Window::display_callback(GLFWwindow* window)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glUseProgram(shaderProgram);
	M->draw(shaderProgram);

	glm::mat4 T = glm::translate(glm::mat4(1.0f), goal) * glm::scale(glm::mat4(1.0f), glm::vec3(0.1f, 0.1f, 0.1f));
	point->draw(shaderProgram, T, glm::vec3(0.0f, 1.0f, 0.0f));

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

		if (key == GLFW_KEY_1)
		{
			M = models[0];
		}
		if (key == GLFW_KEY_2)
		{
			M = models[1];
		}
		if (key == GLFW_KEY_3)
		{
			M = models[2];
		}
		if (key == GLFW_KEY_4)
		{
			M = models[3];
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

					/*glm::vec2 limit = M->getLimit(selectedInd, axis);
					cout << "axis: " << axis << endl;
					cout << "selected: " << selectedInd << endl;
					cout << "limit: " << limit.x << " " << limit.y << endl;
					cout << selected->localA.x << " " << selected->localA.y << " " << selected->localA.z << endl;*/

					if (mods == GLFW_MOD_SHIFT)
						M->updateJoint(selectedInd, angleStep, axis);
					else
						M->updateJoint(selectedInd, -angleStep, axis);
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

	//if (!IKmode)
	//{
	//	cam_pos = glm::vec3(glm::scale(glm::mat4(1.0f), glm::vec3(scalefactor)) * glm::vec4(cam_pos, 1.0f));
	//	V = glm::lookAt(cam_pos, cam_look_at, cam_up);
	//}
	//else
	//{
	//	goal = glm::vec3(glm::scale(glm::mat4(1.0f), glm::vec3(scalefactor)) * glm::vec4(goal, 1.0f));
	//	niter = 0;
	//}
}
