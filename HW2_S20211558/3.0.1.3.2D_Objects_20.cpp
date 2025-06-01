#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <vector>

#include "Shaders/LoadShaders.h"
GLuint h_ShaderProgram; // handle to shader program
GLint loc_ModelViewProjectionMatrix, loc_primitive_color; // indices of uniform variables

														  // include glm/*.hpp only if necessary
														  //#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp> //translate, rotate, scale, ortho, etc.
glm::mat4 ModelViewProjectionMatrix;
glm::mat4 ViewMatrix, ProjectionMatrix, ViewProjectionMatrix;

#define TO_RADIAN 0.01745329252f  
#define TO_DEGREE 57.295779513f
#define BUFFER_OFFSET(offset) ((GLvoid *) (offset))

#define LOC_VERTEX 0

int win_width = 1200, win_height = 800;
float centerx = 0.0f, centery = 0.0f, click_centerx = 0.0f, click_centery = 0.0f, rotate_angle = 0.0f;


GLfloat axes[8][2];
GLfloat axes_color[3] = { 0.0f, 0.0f, 0.0f };
GLuint VBO_axes, VAO_axes;

/*****/
int leftbuttonpressed = 0;
int animation_mode = 1;
int draw_wheels_mode = 0, draw_satellite_mode = 0, draw_halt_mode = 0;
int spawn_coin_mode = 0, draw_coin_mode = 0;
unsigned int timestamp = 0;

float angle = 0.0f, coin_angle = 0.0f;
float angle_speed = 200.0f, coin_angle_speed = 400.0f;

std::vector<glm::vec2> coin_positions;

void timer(int value) {
	timestamp = (timestamp + 1) % UINT_MAX;
	angle += angle_speed * 0.016f;
	coin_angle += coin_angle_speed * 0.016f;
	if (angle > 360.0f) angle -= 360.0f;
	glutPostRedisplay();
	if (animation_mode)
		glutTimerFunc(10, timer, 0);
}
/*****/

void prepare_axes(void) { // Draw axes in their MC.
	// ������
	axes[0][0] = -1200.0f; axes[0][1] = 0.0f;
	axes[1][0] = -50.0f; axes[1][1] = 0.0f;

	axes[2][0] = 50.0f; axes[2][1] = 0.0f;
	axes[3][0] = 1200.0f; axes[3][1] = 0.0f;

	//������
	axes[4][0] = 0.0f; axes[4][1] = -800.0f;
	axes[5][0] = 0.0f; axes[5][1] = -50.0f;

	axes[6][0] = 0.0f; axes[6][1] = 50.0f;
	axes[7][0] = 0.0f; axes[7][1] = 800.0f;

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_axes);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_axes);
	glBufferData(GL_ARRAY_BUFFER, sizeof(axes), axes, GL_STATIC_DRAW);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_axes);
	glBindVertexArray(VAO_axes);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_axes);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void update_axes(void) {
	/*
	axes[0][0] = -1200.0f; 
	axes[1][0] = 1200.0f;

	axes[2][1] = -800.0f;
	axes[3][1] = 800.0f;
	*/
	glBindBuffer(GL_ARRAY_BUFFER, VBO_axes);
	glBufferData(GL_ARRAY_BUFFER, sizeof(axes), axes, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void draw_axes(void) {
	glUniform3fv(loc_primitive_color, 1, axes_color);
	glBindVertexArray(VAO_axes);
	glDrawArrays(GL_LINES, 0, 8);
	glBindVertexArray(0);
}

GLfloat line[2][2];
GLfloat line_color[3] = { 1.0f, 0.0f, 0.0f };
GLuint VBO_line, VAO_line;

void prepare_line(void) { 	// y = x - win_height/4
	line[0][0] = (1.0f / 4.0f - 1.0f / 2.5f)*win_height;
	line[0][1] = (1.0f / 4.0f - 1.0f / 2.5f)*win_height - win_height / 4.0f;
	line[1][0] = win_width / 2.5f;
	line[1][1] = win_width / 2.5f - win_height / 4.0f;

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_line);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_line);
	glBufferData(GL_ARRAY_BUFFER, sizeof(line), line, GL_STATIC_DRAW);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_line);
	glBindVertexArray(VAO_line);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_line);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void update_line(void) { 	// y = x - win_height/4
	line[0][0] = (1.0f / 4.0f - 1.0f / 2.5f)*win_height;
	line[0][1] = (1.0f / 4.0f - 1.0f / 2.5f)*win_height - win_height / 4.0f;
	line[1][0] = win_width / 2.5f;
	line[1][1] = win_width / 2.5f - win_height / 4.0f;

	glBindBuffer(GL_ARRAY_BUFFER, VBO_line);
	glBufferData(GL_ARRAY_BUFFER, sizeof(line), line, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void draw_line(void) { // Draw line in its MC.
					   // y = x - win_height/4
	glUniform3fv(loc_primitive_color, 1, line_color);
	glBindVertexArray(VAO_line);
	glDrawArrays(GL_LINES, 0, 2);
	glBindVertexArray(0);
}


//airplane
#define AIRPLANE_BIG_WING 0
#define AIRPLANE_SMALL_WING 1
#define AIRPLANE_BODY 2
#define AIRPLANE_BACK 3
#define AIRPLANE_SIDEWINDER1 4
#define AIRPLANE_SIDEWINDER2 5
#define AIRPLANE_CENTER 6
GLfloat big_wing[6][2] = { { 0.0, 0.0 },{ -20.0, 15.0 },{ -20.0, 20.0 },{ 0.0, 23.0 },{ 20.0, 20.0 },{ 20.0, 15.0 } };
GLfloat small_wing[6][2] = { { 0.0, -18.0 },{ -11.0, -12.0 },{ -12.0, -7.0 },{ 0.0, -10.0 },{ 12.0, -7.0 },{ 11.0, -12.0 } };
GLfloat body[5][2] = { { 0.0, -25.0 },{ -6.0, 0.0 },{ -6.0, 22.0 },{ 6.0, 22.0 },{ 6.0, 0.0 } };
GLfloat back[5][2] = { { 0.0, 25.0 },{ -7.0, 24.0 },{ -7.0, 21.0 },{ 7.0, 21.0 },{ 7.0, 24.0 } };
GLfloat sidewinder1[5][2] = { { -20.0, 10.0 },{ -18.0, 3.0 },{ -16.0, 10.0 },{ -18.0, 20.0 },{ -20.0, 20.0 } };
GLfloat sidewinder2[5][2] = { { 20.0, 10.0 },{ 18.0, 3.0 },{ 16.0, 10.0 },{ 18.0, 20.0 },{ 20.0, 20.0 } };
GLfloat center[1][2] = { { 0.0, 0.0 } };
GLfloat airplane_color[7][3] = {
	{ 150 / 255.0f, 129 / 255.0f, 183 / 255.0f },  // big_wing
	{ 245 / 255.0f, 211 / 255.0f,   0 / 255.0f },  // small_wing
	{ 111 / 255.0f,  85 / 255.0f, 157 / 255.0f },  // body
	{ 150 / 255.0f, 129 / 255.0f, 183 / 255.0f },  // back
	{ 245 / 255.0f, 211 / 255.0f,   0 / 255.0f },  // sidewinder1
	{ 245 / 255.0f, 211 / 255.0f,   0 / 255.0f },  // sidewinder2
	{ 255 / 255.0f,   0 / 255.0f,   0 / 255.0f }   // center
};

GLuint VBO_airplane, VAO_airplane;

#define AIRPLANE_ROTATION_RADIUS 200.0f

int airplane_clock = 0;
float airplane_s_factor = 1.0f;

void prepare_airplane() {
	GLsizeiptr buffer_size = sizeof(big_wing) + sizeof(small_wing) + sizeof(body) + sizeof(back)
		+ sizeof(sidewinder1) + sizeof(sidewinder2) + sizeof(center);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_airplane);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_airplane);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(big_wing), big_wing);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(big_wing), sizeof(small_wing), small_wing);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(big_wing) + sizeof(small_wing), sizeof(body), body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(big_wing) + sizeof(small_wing) + sizeof(body), sizeof(back), back);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(big_wing) + sizeof(small_wing) + sizeof(body) + sizeof(back),
		sizeof(sidewinder1), sidewinder1);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(big_wing) + sizeof(small_wing) + sizeof(body) + sizeof(back)
		+ sizeof(sidewinder1), sizeof(sidewinder2), sidewinder2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(big_wing) + sizeof(small_wing) + sizeof(body) + sizeof(back)
		+ sizeof(sidewinder1) + sizeof(sidewinder2), sizeof(center), center);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_airplane);
	glBindVertexArray(VAO_airplane);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_airplane);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_airplane() { // Draw airplane in its MC.
	glBindVertexArray(VAO_airplane);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_BIG_WING]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 6);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_SMALL_WING]);
	glDrawArrays(GL_TRIANGLE_FAN, 6, 6);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 5);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_BACK]);
	glDrawArrays(GL_TRIANGLE_FAN, 17, 5);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_SIDEWINDER1]);
	glDrawArrays(GL_TRIANGLE_FAN, 22, 5);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_SIDEWINDER2]);
	glDrawArrays(GL_TRIANGLE_FAN, 27, 5);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_CENTER]);
	glPointSize(5.0);
	glDrawArrays(GL_POINTS, 32, 1);
	glPointSize(1.0);
	glBindVertexArray(0);
}

//house
#define HOUSE_ROOF 0
#define HOUSE_BODY 1
#define HOUSE_CHIMNEY 2
#define HOUSE_DOOR 3
#define HOUSE_WINDOW 4

GLfloat roof[3][2] = { { -12.0, 0.0 },{ 0.0, 12.0 },{ 12.0, 0.0 } };
GLfloat house_body[4][2] = { { -12.0, -14.0 },{ -12.0, 0.0 },{ 12.0, 0.0 },{ 12.0, -14.0 } };
GLfloat chimney[4][2] = { { 6.0, 6.0 },{ 6.0, 14.0 },{ 10.0, 14.0 },{ 10.0, 2.0 } };
GLfloat door[4][2] = { { -8.0, -14.0 },{ -8.0, -8.0 },{ -4.0, -8.0 },{ -4.0, -14.0 } };
GLfloat window[4][2] = { { 4.0, -6.0 },{ 4.0, -2.0 },{ 8.0, -2.0 },{ 8.0, -6.0 } };

GLfloat house_color[5][3] = {
	{ 200 / 255.0f, 39 / 255.0f, 42 / 255.0f },
	{ 235 / 255.0f, 225 / 255.0f, 196 / 255.0f },
	{ 255 / 255.0f, 0 / 255.0f, 0 / 255.0f },
	{ 233 / 255.0f, 113 / 255.0f, 23 / 255.0f },
	{ 44 / 255.0f, 180 / 255.0f, 49 / 255.0f }
};

GLuint VBO_house, VAO_house;
void prepare_house() {
	GLsizeiptr buffer_size = sizeof(roof) + sizeof(house_body) + sizeof(chimney) + sizeof(door)
		+ sizeof(window);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_house);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_house);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(roof), roof);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(roof), sizeof(house_body), house_body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(roof) + sizeof(house_body), sizeof(chimney), chimney);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(roof) + sizeof(house_body) + sizeof(chimney), sizeof(door), door);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(roof) + sizeof(house_body) + sizeof(chimney) + sizeof(door),
		sizeof(window), window);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_house);
	glBindVertexArray(VAO_house);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_house);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_house() {
	glBindVertexArray(VAO_house);

	glUniform3fv(loc_primitive_color, 1, house_color[HOUSE_ROOF]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 3);

	glUniform3fv(loc_primitive_color, 1, house_color[HOUSE_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 3, 4);

	glUniform3fv(loc_primitive_color, 1, house_color[HOUSE_CHIMNEY]);
	glDrawArrays(GL_TRIANGLE_FAN, 7, 4);

	glUniform3fv(loc_primitive_color, 1, house_color[HOUSE_DOOR]);
	glDrawArrays(GL_TRIANGLE_FAN, 11, 4);

	glUniform3fv(loc_primitive_color, 1, house_color[HOUSE_WINDOW]);
	glDrawArrays(GL_TRIANGLE_FAN, 15, 4);

	glBindVertexArray(0);
}

//car
#define CAR_BODY 0
#define CAR_FRAME 1
#define CAR_WINDOW 2
#define CAR_LEFT_LIGHT 3
#define CAR_RIGHT_LIGHT 4
#define CAR_LEFT_WHEEL 5
#define CAR_RIGHT_WHEEL 6

GLfloat car_body[4][2] = { { -16.0, -8.0 },{ -16.0, 0.0 },{ 16.0, 0.0 },{ 16.0, -8.0 } };
GLfloat car_frame[4][2] = { { -10.0, 0.0 },{ -10.0, 10.0 },{ 10.0, 10.0 },{ 10.0, 0.0 } };
GLfloat car_window[4][2] = { { -8.0, 0.0 },{ -8.0, 8.0 },{ 8.0, 8.0 },{ 8.0, 0.0 } };
GLfloat car_left_light[4][2] = { { -9.0, -6.0 },{ -10.0, -5.0 },{ -9.0, -4.0 },{ -8.0, -5.0 } };
GLfloat car_right_light[4][2] = { { 9.0, -6.0 },{ 8.0, -5.0 },{ 9.0, -4.0 },{ 10.0, -5.0 } };
GLfloat car_left_wheel[4][2] = { { -10.0, -12.0 },{ -10.0, -8.0 },{ -6.0, -8.0 },{ -6.0, -12.0 } };
GLfloat car_right_wheel[4][2] = { { 6.0, -12.0 },{ 6.0, -8.0 },{ 10.0, -8.0 },{ 10.0, -12.0 } };

GLfloat car_color[7][3] = {
	{ 0 / 255.0f, 149 / 255.0f, 159 / 255.0f },
	{ 0 / 255.0f, 149 / 255.0f, 159 / 255.0f },
	{ 216 / 255.0f, 208 / 255.0f, 174 / 255.0f },
	{ 249 / 255.0f, 244 / 255.0f, 0 / 255.0f },
	{ 249 / 255.0f, 244 / 255.0f, 0 / 255.0f },
	{ 21 / 255.0f, 30 / 255.0f, 26 / 255.0f },
	{ 21 / 255.0f, 30 / 255.0f, 26 / 255.0f }
};

GLuint VBO_car, VAO_car;
void prepare_car() {
	GLsizeiptr buffer_size = sizeof(car_body) + sizeof(car_frame) + sizeof(car_window) + sizeof(car_left_light)
		+ sizeof(car_right_light) + sizeof(car_left_wheel) + sizeof(car_right_wheel);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_car);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_car);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(car_body), car_body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car_body), sizeof(car_frame), car_frame);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car_body) + sizeof(car_frame), sizeof(car_window), car_window);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car_body) + sizeof(car_frame) + sizeof(car_window), sizeof(car_left_light), car_left_light);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car_body) + sizeof(car_frame) + sizeof(car_window) + sizeof(car_left_light),
		sizeof(car_right_light), car_right_light);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car_body) + sizeof(car_frame) + sizeof(car_window) + sizeof(car_left_light)
		+ sizeof(car_right_light), sizeof(car_left_wheel), car_left_wheel);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car_body) + sizeof(car_frame) + sizeof(car_window) + sizeof(car_left_light)
		+ sizeof(car_right_light) + sizeof(car_left_wheel), sizeof(car_right_wheel), car_right_wheel);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_car);
	glBindVertexArray(VAO_car);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_car);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_car() {
	glBindVertexArray(VAO_car);

	glUniform3fv(loc_primitive_color, 1, car_color[CAR_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glUniform3fv(loc_primitive_color, 1, car_color[CAR_FRAME]);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);

	glUniform3fv(loc_primitive_color, 1, car_color[CAR_WINDOW]);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);

	glUniform3fv(loc_primitive_color, 1, car_color[CAR_LEFT_LIGHT]);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);

	glUniform3fv(loc_primitive_color, 1, car_color[CAR_RIGHT_LIGHT]);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);

	glUniform3fv(loc_primitive_color, 1, car_color[CAR_LEFT_WHEEL]);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 4);

	glUniform3fv(loc_primitive_color, 1, car_color[CAR_RIGHT_WHEEL]);
	glDrawArrays(GL_TRIANGLE_FAN, 24, 4);

	glBindVertexArray(0);
}

//hat
#define HAT_LEAF 0
#define HAT_BODY 1
#define HAT_STRIP 2
#define HAT_BOTTOM 3

GLfloat hat_leaf[4][2] = { { 3.0, 20.0 },{ 3.0, 28.0 },{ 9.0, 32.0 },{ 9.0, 24.0 } };
GLfloat hat_body[4][2] = { { -19.5, 2.0 },{ 19.5, 2.0 },{ 15.0, 20.0 },{ -15.0, 20.0 } };
GLfloat hat_strip[4][2] = { { -20.0, 0.0 },{ 20.0, 0.0 },{ 19.5, 2.0 },{ -19.5, 2.0 } };
GLfloat hat_bottom[4][2] = { { 25.0, 0.0 },{ -25.0, 0.0 },{ -25.0, -4.0 },{ 25.0, -4.0 } };

GLfloat hat_color[4][3] = {
	{ 167 / 255.0f, 255 / 255.0f, 55 / 255.0f },
{ 255 / 255.0f, 144 / 255.0f, 32 / 255.0f },
{ 255 / 255.0f, 40 / 255.0f, 33 / 255.0f },
{ 255 / 255.0f, 144 / 255.0f, 32 / 255.0f }
};

GLuint VBO_hat, VAO_hat;

void prepare_hat() {
	GLsizeiptr buffer_size = sizeof(hat_leaf) + sizeof(hat_body) + sizeof(hat_strip) + sizeof(hat_bottom);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_hat);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_hat);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(hat_leaf), hat_leaf);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(hat_leaf), sizeof(hat_body), hat_body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(hat_leaf) + sizeof(hat_body), sizeof(hat_strip), hat_strip);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(hat_leaf) + sizeof(hat_body) + sizeof(hat_strip), sizeof(hat_bottom), hat_bottom);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_hat);
	glBindVertexArray(VAO_hat);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_hat);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_hat() {
	glBindVertexArray(VAO_hat);

	glUniform3fv(loc_primitive_color, 1, hat_color[HAT_LEAF]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glUniform3fv(loc_primitive_color, 1, hat_color[HAT_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);

	glUniform3fv(loc_primitive_color, 1, hat_color[HAT_STRIP]);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);

	glUniform3fv(loc_primitive_color, 1, hat_color[HAT_BOTTOM]);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);

	glBindVertexArray(0);
}

//sword
#define SWORD_BODY 0
#define SWORD_BODY2 1
#define SWORD_HEAD 2
#define SWORD_HEAD2 3
#define SWORD_IN 4
#define SWORD_DOWN 5
#define SWORD_BODY_IN 6

GLfloat sword_body[4][2] = { { -6.0, 0.0 },{ -6.0, -4.0 },{ 6.0, -4.0 },{ 6.0, 0.0 } };
GLfloat sword_body2[4][2] = { { -2.0, -4.0 },{ -2.0, -6.0 } ,{ 2.0, -6.0 },{ 2.0, -4.0 } };
GLfloat sword_head[4][2] = { { -2.0, 0.0 },{ -2.0, 16.0 } ,{ 2.0, 16.0 },{ 2.0, 0.0 } };
GLfloat sword_head2[3][2] = { { -2.0, 16.0 },{ 0.0, 19.46 } ,{ 2.0, 16.0 } };
GLfloat sword_in[4][2] = { { -0.3, 0.7 },{ -0.3, 15.3 } ,{ 0.3, 15.3 },{ 0.3, 0.7 } };
GLfloat sword_down[4][2] = { { -2.0, -6.0 } ,{ 2.0, -6.0 },{ 4.0, -8.0 },{ -4.0, -8.0 } };
GLfloat sword_body_in[4][2] = { { 0.0, -1.0 } ,{ 1.0, -2.732 },{ 0.0, -4.464 },{ -1.0, -2.732 } };

GLfloat sword_color[7][3] = {
	{ 139 / 255.0f, 69 / 255.0f, 19 / 255.0f },
{ 139 / 255.0f, 69 / 255.0f, 19 / 255.0f },
{ 155 / 255.0f, 155 / 255.0f, 155 / 255.0f },
{ 155 / 255.0f, 155 / 255.0f, 155 / 255.0f },
{ 0 / 255.0f, 0 / 255.0f, 0 / 255.0f },
{ 139 / 255.0f, 69 / 255.0f, 19 / 255.0f },
{ 255 / 255.0f, 0 / 255.0f, 0 / 255.0f }
};

GLuint VBO_sword, VAO_sword;

void prepare_sword() {
	GLsizeiptr buffer_size = sizeof(sword_body) + sizeof(sword_body2) + sizeof(sword_head) + sizeof(sword_head2) + sizeof(sword_in) + sizeof(sword_down) + sizeof(sword_body_in);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_sword);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_sword);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(sword_body), sword_body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(sword_body), sizeof(sword_body2), sword_body2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(sword_body) + sizeof(sword_body2), sizeof(sword_head), sword_head);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(sword_body) + sizeof(sword_body2) + sizeof(sword_head), sizeof(sword_head2), sword_head2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(sword_body) + sizeof(sword_body2) + sizeof(sword_head) + sizeof(sword_head2), sizeof(sword_in), sword_in);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(sword_body) + sizeof(sword_body2) + sizeof(sword_head) + sizeof(sword_head2) + sizeof(sword_in), sizeof(sword_down), sword_down);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(sword_body) + sizeof(sword_body2) + sizeof(sword_head) + sizeof(sword_head2) + sizeof(sword_in) + sizeof(sword_down), sizeof(sword_body_in), sword_body_in);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_sword);
	glBindVertexArray(VAO_sword);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_sword);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_sword() {
	glBindVertexArray(VAO_sword);

	glUniform3fv(loc_primitive_color, 1, sword_color[SWORD_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glUniform3fv(loc_primitive_color, 1, sword_color[SWORD_BODY2]);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);

	glUniform3fv(loc_primitive_color, 1, sword_color[SWORD_HEAD]);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);

	glUniform3fv(loc_primitive_color, 1, sword_color[SWORD_HEAD2]);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 3);

	glUniform3fv(loc_primitive_color, 1, sword_color[SWORD_IN]);
	glDrawArrays(GL_TRIANGLE_FAN, 15, 4);

	glUniform3fv(loc_primitive_color, 1, sword_color[SWORD_DOWN]);
	glDrawArrays(GL_TRIANGLE_FAN, 19, 4);

	glUniform3fv(loc_primitive_color, 1, sword_color[SWORD_BODY_IN]);
	glDrawArrays(GL_TRIANGLE_FAN, 23, 4);

	glBindVertexArray(0);
}

//***** coin *****//
#define COIN_OUTLINE 0
#define COIN_INLINE 1
#define COIN_FACE 2
#define COIN_EYE 3

GLfloat coin_outline[8][2] = { {-4.0, 10.0}, {-10.0, 4.0}, {-10.0, -4.0}, {-4.0, -10.0}, 
							{4.0, -10.0}, {10.0, -4.0}, {10.0, 4.0}, {4.0, 10.0} };
GLfloat coin_inline[8][2] = { {-3.0, 8.0}, {-8.0, 3.0}, {-8.0, -3.0}, {-3.0, -8.0},
							{3.0, -8.0}, {8.0, -3.0}, {8.0, 3.0}, {3.0, 8.0} };
GLfloat coin_face[9][2] = { {0.0, 1.0}, {-4.0, 5.0}, {-4.0, -3.0}, {4.0, -3.0}, {4.0, -1.0},
							{-2.0, -1.0}, {-2.0, 3.0}, {4.0, 3.0}, {4.0, 5.0} };
GLfloat coin_eye[4][2] = { {-2.0, 2.0}, {-2.0, 0.0}, {0.0, 0.0}, {0.0, 2.0} };

GLfloat coin_color[4][3] = {
	{ 0.0f, 0.0f, 0.0f },
	{ 0.5f, 0.5f, 0.5f },
	{ 1.0f, 1.0f, 1.0f },
	{ 0.0f, 0.0f, 0.0f }
};

GLuint VBO_coin, VAO_coin;

void prepare_coin() {
	GLsizeiptr buffer_size = sizeof(coin_outline) + sizeof(coin_inline) + sizeof(coin_face) + sizeof(coin_eye);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_coin);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_coin);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(coin_outline), coin_outline);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(coin_outline), sizeof(coin_inline), coin_inline);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(coin_outline) + sizeof(coin_inline), sizeof(coin_face), coin_face);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(coin_outline) + sizeof(coin_inline) + sizeof(coin_face), sizeof(coin_eye), coin_eye);
	
	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_coin);
	glBindVertexArray(VAO_coin);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_coin);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_coin() {
	glBindVertexArray(VAO_coin);

	glUniform3fv(loc_primitive_color, 1, coin_color[COIN_OUTLINE]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 8);

	glUniform3fv(loc_primitive_color, 1, coin_color[COIN_INLINE]);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 8);

	glUniform3fv(loc_primitive_color, 1, coin_color[COIN_FACE]);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 9);

	glUniform3fv(loc_primitive_color, 1, coin_color[COIN_EYE]);
	glDrawArrays(GL_TRIANGLE_FAN, 25, 4);

	glBindVertexArray(0);
}
//***** coin *****//

//***** leaf *****//
#define LEAF_BODY 0
#define LEAF_STEM 1
#define LEAF_BRANCH1 2
#define LEAF_BRANCH2 3
#define LEAF_BRANCH3 4
#define LEAF_BRANCH4 5
#define LEAF_BRANCH5 6
#define LEAF_BRANCH6 7

GLfloat leaf_body[20][2] = { {-1.0, 25.0}, {-2.0, 22.0}, {-3.0, 17.0}, {-7.0, 11.0},
							{-10.0, 1.0}, {-10.0, -7.0}, {-6.0, -15.0}, {-4.0, -19.0},
							{-3.5, -25.0}, {-5.0, -28.0}, {-2.0, -30.0}, {-1.0, -23.0},
							{0.0, -20.0}, {6.0, -15.0}, {8.0, -7.0}, {10.0, 2.0},
							{9.0, 8.0}, {7.0, 16.0}, {3.0, 19.0}, {0.0, 24.0} };
GLfloat leaf_stem[6][2] = { {0.0, 17.0}, {-1.0, 1.0}, {-2.0, -15.0}, {-3.0, -20.0},
							{-1.0, -16.0}, {1.0, -1.0} };
GLfloat leaf_branch1[4][2] = { {0.0, 2.0}, {-5.0, 10.0}, {-4.7, 9.8}, {-0.8, 1.0} };
GLfloat leaf_branch2[4][2] = { {0.0, -6.0}, {-5.0, 2.0}, {-4.8, 1.8}, {-1.0, -7.0} };
GLfloat leaf_branch3[4][2] = { {-1.0, -14.0}, {-6.0, -6.0}, {-5.8, -6.6}, {-2.0, -15.0} };

GLfloat leaf_branch4[4][2] = { {0.0, 2.0}, {5.0, 10.0}, {5.1, 9.8}, {1.0, 1.0} };
GLfloat leaf_branch5[4][2] = { {0.0, -6.0}, {5.0, 2.0}, {5.2, 1.9}, {1.0, -7.0} };
GLfloat leaf_branch6[4][2] = { {-2.0, -14.0}, {3.0, -6.0}, {3.3, -6.7}, {-1.0, -15.0} };

GLfloat leaf_color[8][3] = {
	{ 0.0f, 1.0f, 0.0f },
	{ 0.0f, 0.5f, 0.0f },
	{ 0.0f, 0.5f, 0.0f },
	{ 0.0f, 0.5f, 0.0f },
	{ 0.0f, 0.5f, 0.0f },
	{ 0.0f, 0.5f, 0.0f },
	{ 0.0f, 0.5f, 0.0f },
	{ 0.0f, 0.5f, 0.0f }
};

GLuint VBO_leaf , VAO_leaf;

void prepare_leaf () {
	GLsizeiptr buffer_size = sizeof(leaf_body) + sizeof(leaf_stem) +
		sizeof(leaf_branch1) + sizeof(leaf_branch2) + sizeof(leaf_branch3) +
		sizeof(leaf_branch4) + sizeof(leaf_branch5) + sizeof(leaf_branch6);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_leaf);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_leaf);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(leaf_body), leaf_body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(leaf_body), sizeof(leaf_stem), leaf_stem);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(leaf_body) + sizeof(leaf_stem), sizeof(leaf_branch1), leaf_branch1);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(leaf_body) + sizeof(leaf_stem)+ sizeof(leaf_branch1), sizeof(leaf_branch2), leaf_branch2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(leaf_body) + sizeof(leaf_stem) + sizeof(leaf_branch1) + sizeof(leaf_branch2), sizeof(leaf_branch3), leaf_branch3);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(leaf_body) + sizeof(leaf_stem) + sizeof(leaf_branch1) + sizeof(leaf_branch2) + sizeof(leaf_branch3), sizeof(leaf_branch4), leaf_branch4);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(leaf_body) + sizeof(leaf_stem) + sizeof(leaf_branch1) + sizeof(leaf_branch2) + sizeof(leaf_branch3)+ sizeof(leaf_branch4), sizeof(leaf_branch5), leaf_branch5);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(leaf_body) + sizeof(leaf_stem) + sizeof(leaf_branch1) + sizeof(leaf_branch2) + sizeof(leaf_branch3) + sizeof(leaf_branch4) + sizeof(leaf_branch5), sizeof(leaf_branch6), leaf_branch6);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_leaf);
	glBindVertexArray(VAO_leaf);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_leaf);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_leaf () {
	glBindVertexArray(VAO_leaf);

	glUniform3fv(loc_primitive_color, 1, leaf_color[LEAF_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 20);
	glUniform3fv(loc_primitive_color, 1, leaf_color[LEAF_STEM]);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 6);
	glUniform3fv(loc_primitive_color, 1, leaf_color[LEAF_BRANCH1]);
	glDrawArrays(GL_TRIANGLE_FAN, 26, 4);
	glUniform3fv(loc_primitive_color, 1, leaf_color[LEAF_BRANCH2]);
	glDrawArrays(GL_TRIANGLE_FAN, 30, 4);
	glUniform3fv(loc_primitive_color, 1, leaf_color[LEAF_BRANCH3]);
	glDrawArrays(GL_TRIANGLE_FAN, 34, 4);
	glUniform3fv(loc_primitive_color, 1, leaf_color[LEAF_BRANCH4]);
	glDrawArrays(GL_TRIANGLE_FAN, 38, 4);
	glUniform3fv(loc_primitive_color, 1, leaf_color[LEAF_BRANCH5]);
	glDrawArrays(GL_TRIANGLE_FAN, 42, 4);
	glUniform3fv(loc_primitive_color, 1, leaf_color[LEAF_BRANCH6]);
	glDrawArrays(GL_TRIANGLE_FAN, 46, 4);

	glBindVertexArray(0);
}
//***** leaf *****//

void display(void) {
	glm::mat4 ModelMatrix;

	glClear(GL_COLOR_BUFFER_BIT);

	/***** ������� �ϴܰ� ��ܿ� ����, ���ο� ���� �׸��� *****/
	for (int i = 0; i < 20; i++) {
		int bg_house_clock = (timestamp % 800) / 2 - 360; // -360 <= house_clock <= 360 
		float scaling_factor_bg_house;
		if (i % 2 == 0) scaling_factor_bg_house = 3.0f - fabs(cosf(bg_house_clock * 2.0f * TO_RADIAN));
		else scaling_factor_bg_house = 3.0f - fabs(sinf(bg_house_clock * 2.0f * TO_RADIAN));

		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-575.0f + 60 * i, -370.0f, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(scaling_factor_bg_house, scaling_factor_bg_house, 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_house();
	}

	for (int i = 0; i < 20; i++) {
		int bg_house_clock = (timestamp % 800) / 2 - 360; // -360 <= house_clock <= 360 
		float scaling_factor_bg_house;
		if (i % 2 == 0) scaling_factor_bg_house = 3.0f - fabs(cosf(bg_house_clock * 2.0f * TO_RADIAN));
		else scaling_factor_bg_house = 3.0f - fabs(sinf(bg_house_clock * 2.0f * TO_RADIAN));

		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-575.0f + 60 * i, 370.0f, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(scaling_factor_bg_house, scaling_factor_bg_house, 1.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(-1.0f, -1.0f, 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_house();
	}


	for (int j = 0; j < 3; j++) {
		float positions[4][2] = { {0.0f, 100.0f + 100.0f * j}, {-150.0f - 150.0f * j, 0.0f}, 
								{0.0f, -100.0f - 100.0f * j}, {150.0f + 150.0f * j, 0.0f} };
		for (int i = 0; i < 4; i++) {
			float offset = 10.0f * sin(timestamp * 0.1f);
			if (i == 1 || i == 2) offset *= -1;
			float dx = (i % 2 == 0) ? 0.0f : offset;
			float dy = (i % 2 == 0) ? offset : 0.0f;

			glm::mat4 ModelMatrix = glm::mat4(1.0f);
			ModelMatrix = glm::translate(ModelMatrix, glm::vec3(positions[i][0] + dx, positions[i][1] + dy, 0.0f));
			ModelMatrix = glm::rotate(ModelMatrix, i * 90.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
			ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_hat();
		}
	}
	/*****/

	//***** ���ڸ� ���������� ������ ���� ���� �����̴� �� *****//
	for (int i = 0; i < 3; i++) {
		if (i == 1) continue;

		glm::vec2 diamond_path[4] = { // ����� (200, 300)
			glm::vec2(0.0f, 100.0f + 100 * i),
			glm::vec2(-150.0f - 150 * i, 0.0f),
			glm::vec2(0.0f, -100.0f - 100 * i),
			glm::vec2(150.0f + 150 * i, 0.0f)
		};

		for (int j = 0; j < 4; j++) {
			float speed = 0.01f;
			float total_time = 4.0f;
			float t = fmod(timestamp * speed, total_time);
			int current_segment = (int)t;
			float local_t = t - current_segment;

			glm::vec2 start = diamond_path[(current_segment + j) % 4];	// ���� ��ġ
			glm::vec2 end = diamond_path[(current_segment + j + 1) % 4];// ���� ��ġ

			glm::vec2 pos = (1.0f - local_t) * start + local_t * end;	// ���� ����

			glm::vec2 direction = glm::normalize(end - start);			// ȸ�� ����
			float cangle = atan2(direction.y, direction.x);

			ModelMatrix = glm::mat4(1.0f);
			ModelMatrix = glm::translate(ModelMatrix, glm::vec3(pos, 0.0f));
			ModelMatrix = glm::rotate(ModelMatrix, cangle, glm::vec3(0.0f, 0.0f, 1.0f));
			ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_car();
		}
	}

	for (int i = 0; i < 4; i++) {
		glm::vec2 diamond_path[4] = { // ����� (200, 300)
			glm::vec2(0.0f, 200.0f),
			glm::vec2(300.0f, 0.0f),
			glm::vec2(0.0f, -200.0f),
			glm::vec2(-300.0f, 0.0f)
		};

		float speed = 0.01f;
		float total_time = 4.0f;
		float t = fmod(timestamp * speed, total_time);
		int current_segment = (int)t;
		float local_t = t - current_segment;

		glm::vec2 start = diamond_path[(current_segment + i) % 4];	// ���� ��ġ
		glm::vec2 end = diamond_path[(current_segment + i + 1) % 4];// ���� ��ġ

		glm::vec2 pos = (1.0f - local_t) * start + local_t * end;	// ���� ����

		glm::vec2 direction = glm::normalize(end - start);			// ȸ�� ����
		float cangle = atan2(direction.y, direction.x);

		ModelMatrix = glm::mat4(1.0f);
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(pos, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, cangle, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_car();
	}
	/*****/

	//***** axes & swords �� ������ ǥ�� *****//
	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(centerx, centery, 0.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_axes();

	int x_move[4] = { 40, -40, -40, 40 };
	int y_move[4] = { 40, 40, -40, -40 };

	int sword_clock = (timestamp % 800) / 2 - 360; // -360 <= house_clock <= 360 
	float scaling_factor_sword = 3.0f - fabs(cosf(sword_clock * 2.0f * TO_RADIAN));
	float scaling_factor_center_coin = 2.0f - fabs(sinf(sword_clock * 2.0f * TO_RADIAN));

	for (int i = 0; i < 4; i++) {
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(centerx + x_move[i], centery + y_move[i], 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(scaling_factor_sword, scaling_factor_sword, 1.0f));
		ModelMatrix = glm::rotate(ModelMatrix, (135 + 90.0f * i) * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_sword();
	}

	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(centerx, centery, 0.0f));
	ModelMatrix = glm::scale(ModelMatrix, glm::vec3(scaling_factor_center_coin, scaling_factor_center_coin, 1.0f));
	ModelMatrix = glm::rotate(ModelMatrix, angle * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_coin();
	/*****/

	//***** �������� �߽����� ȸ���ϴ� ������ �� ����⸦ �߽����� ȸ���ϴ� ���� *****//
	for (int i = 1; i <= 7; i++) {
		float pangles[4] = { 0.0f, 90.0f, 180.0f, 270.0f };
		float pangle = (timestamp + pangles[i / 2]) * TO_RADIAN;
		float px = (i + 1) * 50.0f * cos(pangle);
		float py = (i + 1) * 50.0f * sin(pangle);
		if (i % 2 == 0) py *= -1;

		ModelMatrix = glm::mat4(1.0f);
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(centerx + px, centery + py, 0.0f));
		if (i % 2 == 0) ModelMatrix = glm::rotate(ModelMatrix, -pangle, glm::vec3(0.0f, 0.0f, 1.0f));
		else ModelMatrix = glm::rotate(ModelMatrix, pangle, glm::vec3(0.0f, 0.0f, 1.0f));
		if (i % 2 == 0) ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.75f, 0.75f, 1.0f));
		else ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.75f, -0.75f, 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_airplane();

		for (int j = 0; j < 2; j++) {
			glm::mat4 CoinMatrix = glm::mat4(1.0f);
			CoinMatrix = glm::translate(CoinMatrix, glm::vec3(centerx + px, centery + py, 0.0f));
			CoinMatrix = glm::rotate(CoinMatrix, (5.0f * (180.f * j + timestamp) * TO_RADIAN) + glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			CoinMatrix = glm::translate(CoinMatrix, glm::vec3(50.0f, 0.0f, 0.0f));
			CoinMatrix = glm::scale(CoinMatrix, glm::vec3(0.5f, 0.5f, 1.0f));
			ModelViewProjectionMatrix = ViewProjectionMatrix * CoinMatrix;
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_coin();
		}
	}
	/*****/

	//***** ���ؼ� ���� 8�ڷ� ȸ���ϴ� ����� *****//
	glm::vec2 centers[4] = {
		glm::vec2(centerx - 200.0f, centery),
		glm::vec2(centerx + 200.0f, centery),
		glm::vec2(centerx - 400.0f, centery),
		glm::vec2(centerx + 400.0f, centery)
	};

	airplane_clock = (int)(timestamp * 2.0f) % 736; // 0 <= airplane_clock <= 719
	float radius = AIRPLANE_ROTATION_RADIUS * 0.5f;

	for (int i = 0; i < 4; i++) {
		int direction;
		switch (i) {
		case 0:
			direction = 1;
			break;
		case 1:
			direction = -1;
			break;
		case 2:
			direction = -1;
			break;
		default:
			direction = 1;
			break;
		}

		glm::vec2 base = centers[i];
		glm::mat4 ModelMatrix = glm::mat4(1.0f);

		if (airplane_clock <= 360) {
			ModelMatrix = glm::translate(ModelMatrix, glm::vec3(base.x + direction * radius, base.y, 0.0f));
			ModelMatrix = glm::rotate(ModelMatrix, direction * airplane_clock * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
			ModelMatrix = glm::translate(ModelMatrix, glm::vec3(-direction * radius, 0.0f, 0.0f));
		}
		else {
			ModelMatrix = glm::translate(ModelMatrix, glm::vec3(base.x - direction * radius, base.y, 0.0f));
			ModelMatrix = glm::rotate(ModelMatrix, -direction * airplane_clock * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
			ModelMatrix = glm::translate(ModelMatrix, glm::vec3(direction * radius, 0.0f, 0.0f));

			float airplane_s_factor;
			if (airplane_clock <= 540)
				airplane_s_factor = (airplane_clock - 360.0f) / 180.0f + 1.0f;
			else
				airplane_s_factor = -(airplane_clock - 540.0f) / 180.0f + 2.0f;

			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(airplane_s_factor, airplane_s_factor, 1.0f));
		}
	
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_leaf();
	}
	//*****//

	//***** ��Ŭ�� �� ȸ���ϴ� ���� ���� *****//
	int coin_clock = (timestamp % 800) / 2 - 360; // -360 <= house_clock <= 360 
	float scaling_factor_coin = 4.0f - fabs(cosf(coin_clock * 8.0f * TO_RADIAN));

	for (auto& pos : coin_positions) {
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(pos.x, pos.y, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(scaling_factor_coin, scaling_factor_coin, 1.0f));
		ModelMatrix = glm::rotate(ModelMatrix, coin_angle * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_coin();
	}
	/*****/

	glFlush();
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'a':
		animation_mode = 1 - animation_mode;
		if (animation_mode)
			glutTimerFunc(10, timer, 0);
		break;
	case 27: // ESC key
		glutLeaveMainLoop(); // Incur destuction callback for cleanups.
		break;
	}
}

void mouse(int button, int state, int x, int y) {
	if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN)) {
		float cx = x - win_width / 2.0f;
		float cy = (win_height - y) - win_height / 2.0f;
		coin_positions.push_back(glm::vec2(cx, cy));
		glutPostRedisplay();
	}
	else if ((button == GLUT_RIGHT_BUTTON) && (state == GLUT_DOWN)) { // ��Ŭ�� �� ������ ���ε� �����
		coin_positions.clear();
		glutPostRedisplay();
	}
}

void motion(int x, int y) {

}

void passive_motion(int x, int y) {
	centerx = x - win_width / 2.0f, centery = (win_height - y) - win_height / 2.0f;
	glutPostRedisplay();
}

void reshape(int width, int height) {
	win_width = width, win_height = height;

	glViewport(0, 0, win_width, win_height);
	ProjectionMatrix = glm::ortho(-win_width / 2.0, win_width / 2.0,
		-win_height / 2.0, win_height / 2.0, -1000.0, 1000.0);
	ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;

	update_axes();
	update_line();

	glutPostRedisplay();
}

void cleanup(void) {
	glDeleteVertexArrays(1, &VAO_axes);
	glDeleteBuffers(1, &VBO_axes);

	glDeleteVertexArrays(1, &VAO_line);
	glDeleteBuffers(1, &VBO_line);

	glDeleteVertexArrays(1, &VAO_airplane);
	glDeleteBuffers(1, &VBO_airplane);

	glDeleteVertexArrays(1, &VAO_house);
	glDeleteBuffers(1, &VBO_house);

	glDeleteVertexArrays(1, &VAO_hat);
	glDeleteBuffers(1, &VBO_hat);

	glDeleteVertexArrays(1, &VAO_car);
	glDeleteBuffers(1, &VBO_car);

	glDeleteVertexArrays(1, &VAO_sword);
	glDeleteBuffers(1, &VBO_sword);

	glDeleteVertexArrays(1, &VAO_coin);
	glDeleteBuffers(1, &VBO_coin);

	glDeleteVertexArrays(1, &VAO_leaf);
	glDeleteBuffers(1, &VBO_leaf);
}

void register_callbacks(void) {
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutPassiveMotionFunc(passive_motion);
	glutReshapeFunc(reshape);
	glutTimerFunc(10, timer, 0);
	glutCloseFunc(cleanup);
}

void prepare_shader_program(void) {
	ShaderInfo shader_info[3] = {
		{ GL_VERTEX_SHADER, "Shaders/simple.vert" },
		{ GL_FRAGMENT_SHADER, "Shaders/simple.frag" },
		{ GL_NONE, NULL }
	};

	h_ShaderProgram = LoadShaders(shader_info);
	glUseProgram(h_ShaderProgram);

	loc_ModelViewProjectionMatrix = glGetUniformLocation(h_ShaderProgram, "u_ModelViewProjectionMatrix");
	loc_primitive_color = glGetUniformLocation(h_ShaderProgram, "u_primitive_color");
}

void initialize_OpenGL(void) {
	glEnable(GL_MULTISAMPLE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glClearColor(250 / 255.0f, 128 / 255.0f, 114 / 255.0f, 1.0f);
	ViewMatrix = glm::mat4(1.0f);
}

void prepare_scene(void) {
	prepare_axes();
	prepare_line();
	prepare_airplane();
	prepare_house();
	prepare_car();
	prepare_hat();
	prepare_sword();
	prepare_coin(); /* coin �߰� */
	prepare_leaf(); /* leaf �߰� */
}

void initialize_renderer(void) {
	register_callbacks();
	prepare_shader_program();
	initialize_OpenGL();
	prepare_scene();
	glClearColor(0.9f, 0.9f, 0.9f, 1.0f); /* ������ �� ���� */
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

void greetings(char *program_name, char messages[][256], int n_message_lines) {
	fprintf(stdout, "**************************************************************\n\n");
	fprintf(stdout, "  PROGRAM NAME: %s\n\n", program_name);
	fprintf(stdout, "    This program was coded for CSE4170 students\n");
	fprintf(stdout, "      of Dept. of Comp. Sci. & Eng., Sogang University.\n\n");

	for (int i = 0; i < n_message_lines; i++)
		fprintf(stdout, "%s\n", messages[i]);
	fprintf(stdout, "\n**************************************************************\n\n");

	initialize_glew();
}

#define N_MESSAGE_LINES 1
void main(int argc, char *argv[]) {
	char program_name[64] = "Sogang CSE4170 2DObjects_GLSL_3.0.1.3 20211558";
	char messages[N_MESSAGE_LINES][256] = {
		"    - Keys used: 'a', 'b', 'w', 's', 'h', 'ESC'"
		"    - Mouse used: L-click and move"
	};

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_MULTISAMPLE);
	glutInitWindowSize(1200, 800); /* window size ���� */
	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow(program_name);

	greetings(program_name, messages, N_MESSAGE_LINES);
	initialize_renderer();

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutMainLoop();
}


