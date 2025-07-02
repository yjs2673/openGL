#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "Shaders/LoadShaders.h"
#include "Scene_Definitions.h"

// include glm/*.hpp only if necessary
//#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp> //translate, rotate, scale, lookAt, perspective, etc.
#include <glm/gtc/matrix_inverse.hpp> // inverseTranspose, etc.
glm::mat4 ModelViewProjectionMatrix, ModelViewMatrix;
glm::mat3 ModelViewMatrixInvTrans;
glm::mat4 ViewMatrix, ProjectionMatrix;

#define TO_RADIAN 0.01745329252f  
#define TO_DEGREE 57.295779513f
#define BUFFER_OFFSET(offset) ((GLvoid *) (offset))

// left click drag condition, location
int flag_translation_axis;
static bool   g_leftButtonDown = false;
static int    g_leftLastMouseY = 0;

// right click drag condition, location
static bool   g_rightButtonDown = false;
static int    g_rightLastMouseX = 0;
static int    g_rightLastMouseY = 0;

static constexpr float ROTATE_SENSITIVITY = 0.0005f; // mouse dpi
static constexpr float TRANSLATE_SENSITIVITY = 0.1f;

static int selectedCCTV = 0;
static constexpr int NUM_CCTV = 3;

static int selectedORTH = 0;
static constexpr int NUM_ORTH = 3;

int camera_control = 0;
int rotate_control = 0;
int NUM_CONT = 2;

static int toggle = 0;
#define MAX_TOGGLE 3;

Scene scene;

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	/*for (auto camera = scene.camera_list.begin(); camera != scene.camera_list.end(); camera++) {
		if (camera->get().flag_valid == false) continue;
		glViewport(camera->get().view_port.x, camera->get().view_port.y,
			camera->get().view_port.w, camera->get().view_port.h);
		scene.ViewMatrix = camera->get().ViewMatrix;
		scene.ProjectionMatrix = camera->get().ProjectionMatrix;

		scene.draw_world();
	}*/

	// main
	auto& main_cam = scene.camera_list[0].get();
	if (main_cam.flag_valid) {
		glViewport(main_cam.view_port.x, main_cam.view_port.y, main_cam.view_port.w, main_cam.view_port.h);
		scene.ViewMatrix = main_cam.ViewMatrix;
		scene.ProjectionMatrix = main_cam.ProjectionMatrix;
		scene.draw_world();
	}

	// cctv
	int cctv_index = 1 + selectedCCTV;
	if (scene.camera_list.size() > cctv_index) {
		auto& cctv_cam = scene.camera_list[cctv_index].get();
		if (cctv_cam.flag_valid) {
			int cx = 40;
			int cy = 580;
			int cw = 200;
			int ch = 200;

			glViewport(cx, cy, cw, ch);
			scene.ViewMatrix = cctv_cam.ViewMatrix;
			scene.ProjectionMatrix = cctv_cam.ProjectionMatrix;
			scene.draw_world();
		}
	}

	// cctv move
	auto& cctv_mv_cam = scene.camera_list[4].get();
	if (cctv_mv_cam.flag_valid) {
		int mx = 40;
		int my = 360;
		int mw = 200;
		int mh = 200;

		glViewport(mx, my, mw, mh);
		scene.ViewMatrix = cctv_mv_cam.ViewMatrix;
		scene.ProjectionMatrix = cctv_mv_cam.ProjectionMatrix;
		scene.draw_world();
	}

	// orth
	auto& orth_cam0 = scene.camera_list[5].get();
	auto& orth_cam1 = scene.camera_list[6].get();
	auto& orth_cam2 = scene.camera_list[7].get();
	if (orth_cam0.flag_valid) {
			int ox = -100;
			int oy = -70;
			int ow = 550;
			int oh = 375;
			glViewport(ox, oy, ow, oh);
			scene.ViewMatrix = orth_cam0.ViewMatrix;
			scene.ProjectionMatrix = orth_cam0.ProjectionMatrix;
			scene.draw_world();
	}
	if (orth_cam1.flag_valid) {
		int ox = -100;
		int oy = 50;
		int ow = 550;
		int oh = 375;
		glViewport(ox, oy, ow, oh);
		scene.ViewMatrix = orth_cam1.ViewMatrix;
		scene.ProjectionMatrix = orth_cam1.ProjectionMatrix;
		scene.draw_world();
	}
	if (orth_cam2.flag_valid) {
		int ox = 260;
		int oy = -150;
		int ow = 550;
		int oh = 375;
		glViewport(ox, oy, ow, oh);
		scene.ViewMatrix = orth_cam2.ViewMatrix;
		scene.ProjectionMatrix = orth_cam2.ProjectionMatrix;
		scene.draw_world();
	}

	glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y) {
	static int flag_cull_face = 0, polygon_fill_on = 0, depth_test_on = 0;

	switch (key) {
	case 27: // ESC key
		glutLeaveMainLoop(); // Incur destuction callback for cleanups.
		break;
	case 'c':
		flag_cull_face = (flag_cull_face + 1) % 3;
		switch (flag_cull_face) {
		case 0:
			glDisable(GL_CULL_FACE);
			glutPostRedisplay();
			fprintf(stdout, "^^^ No faces are culled.\n");
			break;
		case 1: // cull back faces;
			glCullFace(GL_BACK);
			glEnable(GL_CULL_FACE);
			glutPostRedisplay();
			fprintf(stdout, "^^^ Back faces are culled.\n");
			break;
		case 2: // cull front faces;
			glCullFace(GL_FRONT);
			glEnable(GL_CULL_FACE);
			glutPostRedisplay();
			fprintf(stdout, "^^^ Front faces are culled.\n");
			break;
		}
		break;
	case 'f':
		polygon_fill_on = 1 - polygon_fill_on;
		if (polygon_fill_on) {
		 	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			fprintf(stdout, "^^^ Polygon filling enabled.\n");
		}
		else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			fprintf(stdout, "^^^ Line drawing enabled.\n");
		}
		glutPostRedisplay();
		break;
	case 'd':
		depth_test_on = 1 - depth_test_on;
		if (depth_test_on) {
			glEnable(GL_DEPTH_TEST);
			fprintf(stdout, "^^^ Depth test enabled.\n");
		}
		else {
			glDisable(GL_DEPTH_TEST);
			fprintf(stdout, "^^^ Depth test disabled.\n");
		}
		glutPostRedisplay();
		break;
	///////////////// Camera /////////////////
	case 'x': case 'X':
		flag_translation_axis = 0;
		fprintf(stdout, "^^^ Translation axis set to X.\n");
		break;
	case 'y': case 'Y':
		flag_translation_axis = 1;
		fprintf(stdout, "^^^ Translation axis set to Y.\n");
		break;
	case 'z': case 'Z':
		flag_translation_axis = 2;
		fprintf(stdout, "^^^ Translation axis set to Z.\n");
		break;
	case 's': case 'S':
		selectedCCTV = (selectedCCTV + 1) % NUM_CCTV;
		fprintf(stdout, "Showing CCTV #%d view\n", selectedCCTV);
		glutPostRedisplay();
		break;
	case 'o': case 'O':
		selectedORTH = (selectedORTH + 1) % NUM_ORTH;
		fprintf(stdout, "Showing ORTH #%d view\n", selectedORTH);
		glutPostRedisplay();
		break;
	case 'a': case 'A':
		camera_control = (camera_control + 1) % NUM_CONT;
		if (camera_control == 0) fprintf(stdout, "Start Main Camera Control\n");
		else fprintf(stdout, "Start CCTV Control\n");
		glutPostRedisplay();
		break;
	case 'r': case 'R':
		rotate_control = (rotate_control + 1) % NUM_CONT;
		if (rotate_control == 0) fprintf(stdout, "Rotate World axis\n");
		else fprintf(stdout, "Rotate Local axis\n");
		glutPostRedisplay();
		break;
	case 't': case 'T': {
		toggle = (toggle + 1) % MAX_TOGGLE;
		auto& main_cam = scene.camera_list[0].get();
		auto& dynamic_cam = scene.camera_list[4].get();
		main_cam.toggleUp(toggle);
		dynamic_cam.toggleUp(toggle);
		if (toggle == 0) fprintf(stdout, "Toggle Camera axis as (0, 0, 1)\n");
		else if (toggle == 1) fprintf(stdout, "Toggle Camera axis as (0, 1, 0)\n");
		else fprintf(stdout, "Toggle Camera axis as (1, 0, 0)\n");
		glutPostRedisplay();
		break;
	}
	///////////////// Shading /////////////////
	case '1':
		scene.shader_kind = GOURAUD_CONVERT;
		fprintf(stdout, "Dragon's Shader convert to Gouraud\n");
		glutPostRedisplay();
		break;
	case '2':
		scene.shader_kind = PHONG_CONVERT;
		fprintf(stdout, "Dragon's Shader convert to Phong\n");
		glutPostRedisplay();
		break;
	case 'n': case 'N':
		current_filter_mode = GL_NEAREST;
		fprintf(stdout, "Texture filter mode : Nearest\n");
		glutPostRedisplay();
		break;
	case 'l': case 'L':
		current_filter_mode = GL_LINEAR;
		fprintf(stdout, "Texture filter mode : Linear\n");
		glutPostRedisplay();
		break;
	case '3':
		glutPostRedisplay();
		break;
	case '4':
		fprintf(stdout, "button 4 pressed\n");
		glutPostRedisplay();
		break;
	case '5':
		fprintf(stdout, "button 5 pressed\n");
		glutPostRedisplay();
		break;
	case '6':
		if (!is_transparency_enabled) {
			is_transparency_enabled = true;
			fprintf(stdout, "Control Alpha Active\n");
		}
		else {
			is_transparency_enabled = false;
			current_alpha = 1.0f;
			fprintf(stdout, "Control Alpha Inactive\n");
		}
		glutPostRedisplay();
		break;
	case ',':
		if (is_transparency_enabled) {
			current_alpha = glm::max(0.0f, current_alpha - 0.05f);
			can_change = true;
			fprintf(stdout, "Decrease Alpha 0.05\n", current_alpha);
		}
		break;
	case '.':
		if (is_transparency_enabled) {
			current_alpha = glm::min(1.0f, current_alpha + 0.05f);
			can_change = true;
			fprintf(stdout, "Increase Alpha 0.05\n", current_alpha);
		}
		break;
	case '7':
		fprintf(stdout, "button 7 pressed\n");
		glutPostRedisplay();
		break;
	case '8':
		fprintf(stdout, "button 8 pressed\n");
		glutPostRedisplay();
		break;
	}
}

void mouseMotion(int x, int y) {
	auto& cam = scene.camera_data.cam_main;
	auto& cam_cc = scene.camera_data.cam_cc_mv;

	if (g_leftButtonDown) { // left click drag -> move through axis
		float dy = (y - g_leftLastMouseY) * TRANSLATE_SENSITIVITY;
		g_leftLastMouseY = y;

		glm::vec3 delta(0.0f);
		switch (flag_translation_axis) {
		case 0: delta.x = dy; break;
		case 1: delta.y = dy; break;
		case 2: delta.z = -dy; break;
		}

		cam.cam_view.pos += delta;
		// recalculate view matrix
		cam.ViewMatrix = glm::lookAt(
			cam.cam_view.pos,
			cam.cam_view.pos - cam.cam_view.naxis,
			cam.cam_view.vaxis
		);
		cam.flag_valid = true;

		glutPostRedisplay();
	}
	else if (g_rightButtonDown) { // right click drag -> rotate through axis
		float dx = (x - g_rightLastMouseX) * ROTATE_SENSITIVITY;
		float dy = (y - g_rightLastMouseY) * ROTATE_SENSITIVITY;

		if (camera_control == 0) {
			if (rotate_control == 0) {
				switch (flag_translation_axis) {
				case 0: cam.rotateWorld(dx, 0.0f, 0.0f);; break;
				case 1: cam.rotateWorld(0.0f, dx, 0.0f);; break;
				case 2: cam.rotateWorld(0.0f, 0.0f, dx);; break;
				}
			}
			else cam.rotateLocal(dy, dx, 0.0f);
		}
		else {
			if (rotate_control == 0) {
				switch (flag_translation_axis) {
				case 0: cam_cc.rotateWorld(dx, dx, 0.0f);; break;
				case 1: cam_cc.rotateWorld(0.0f, dx, 0.0f);; break;
				case 2: cam_cc.rotateWorld(0.0f, 0.0f, dx);; break;
				}
			}
			else cam_cc.rotateLocal(dy, dx, 0.0f);
		}

		g_rightLastMouseX = x;
		g_rightLastMouseY = y;
		glutPostRedisplay();
	}
}

void mouseWheel(int button, int state, int x, int y) {
	if (state != GLUT_DOWN) return;
	constexpr float ZOOM_STEP = 2.0f * TO_RADIAN;

	auto& cam = scene.camera_data.cam_main;
	auto& cam_cc = scene.camera_data.cam_cc_mv;
	
	if (camera_control == 0) {
		if (button == 3) cam.zoom(-ZOOM_STEP); // wheel up ¡æ zoom in
		else if (button == 4) cam.zoom(+ZOOM_STEP); // wheel down ¡æ zoom out
	}
	else {
		if (button == 3) cam_cc.zoom(-ZOOM_STEP); // wheel up ¡æ zoom in
		else if (button == 4) cam_cc.zoom(+ZOOM_STEP); // wheel down ¡æ zoom out
	}

	glutPostRedisplay();
}

void mouseButton(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			g_leftButtonDown = true;
			g_leftLastMouseY = y;
		}
		else if (state == GLUT_UP) {
			g_leftButtonDown = false;
		}
	}
	else if (button == GLUT_RIGHT_BUTTON) {
		if (state == GLUT_DOWN) {
			g_rightButtonDown = true;
			g_rightLastMouseX = x;
			g_rightLastMouseY = y;
		}
		else if (state == GLUT_UP) {
			g_rightButtonDown = false;
		}
	}
	else if (button == 3 || button == 4) { // mouse wheel
		mouseWheel(button, state, x, y);
	}
}

void reshape(int width, int height) {
	scene.window.width = width;
	scene.window.height = height;
	scene.window.aspect_ratio = (float)width / height;
	scene.create_camera_list(scene.window.width, scene.window.height, scene.window.aspect_ratio);
	glutPostRedisplay();
}

void timer_scene(int index) {
	scene.clock(0);
	glutPostRedisplay();
	glutTimerFunc(100, timer_scene, 0);
}

void register_callbacks(void) {
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouseButton);
	glutMotionFunc(mouseMotion);
 	glutTimerFunc(100, timer_scene, 0);
//	glutCloseFunc(cleanup_OpenGL_stuffs or else); // Do it yourself!!!
}

void initialize_OpenGL(void) {
	glEnable(GL_DEPTH_TEST); // Default state

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//glClearColor(0.12f, 0.18f, 0.12f, 1.0f);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void initialize_renderer(void) {
	register_callbacks();
	initialize_OpenGL();
	scene.initialize();
}

void initialize_glew(void) {
	GLenum error;

	glewExperimental = GL_TRUE;

	error = glewInit();
	if (error != GLEW_OK) {
		fprintf(stderr, "Error: %s\n", glewGetErrorString(error));
		exit(-1);
	}
	fprintf(stdout, "*********************************************************\n");
	fprintf(stdout, " - GLEW version supported: %s\n", glewGetString(GLEW_VERSION));
	fprintf(stdout, " - OpenGL renderer: %s\n", glGetString(GL_RENDERER));
	fprintf(stdout, " - OpenGL version supported: %s\n", glGetString(GL_VERSION));
	fprintf(stdout, "*********************************************************\n\n");
}

void print_message(const char * m) {
	fprintf(stdout, "%s\n\n", m);
}

void greetings(char *program_name, char messages[][256], int n_message_lines) {
	fprintf(stdout, "**************************************************************\n\n");
	fprintf(stdout, "  PROGRAM NAME: %s\n\n", program_name);
	fprintf(stdout, "    This program was coded for CSE4170/AIE4012 students\n");
	fprintf(stdout, "      of Dept. of Comp. Sci. & Eng., Sogang University.\n\n");

	for (int i = 0; i < n_message_lines; i++)
		fprintf(stdout, "%s\n", messages[i]);
	fprintf(stdout, "\n**************************************************************\n\n");

	initialize_glew();
}

#define N_MESSAGE_LINES 1
void main(int argc, char *argv[]) { 
	char program_name[256] = "Sogang CSE4170/AIE4120 Our_House_GLSL_V_0.55";
	char messages[N_MESSAGE_LINES][256] = { "    - Keys used: fill it yourself!" };

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
	glutInitWindowSize(1200, 800);
	glutInitContextVersion(4, 0);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow(program_name);

	greetings(program_name, messages, N_MESSAGE_LINES);
	initialize_renderer();

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutMainLoop();
}
