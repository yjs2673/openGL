#define _CRT_SECURE_NO_WARNINGS

#include "Scene_Definitions.h"

unsigned int static_object_ID_mapper[N_MAX_STATIC_OBJECTS];
unsigned int dynamic_object_ID_mapper[N_MAX_DYNAMIC_OBJECTS];
unsigned int camera_ID_mapper[N_MAX_CAMERAS];
unsigned int shader_ID_mapper[N_MAX_SHADERS];

/*****/
// for simple shaders
GLint loc_ModelViewProjectionMatrix_simple, loc_primitive_color;

// for Phong Shading (Textured) shaders
GLuint h_ShaderProgram_simple, h_ShaderProgram_TXPS; // handles to shader programs

GLint loc_global_ambient_color;
loc_light_Parameters loc_light[NUMBER_OF_LIGHT_SUPPORTED];
loc_Material_Parameters loc_material;
GLint loc_ModelViewProjectionMatrix_TXPS, loc_ModelViewMatrix_TXPS, loc_ModelViewMatrixInvTrans_TXPS;
GLint loc_texture, loc_flag_texture_mapping, loc_flag_fog;

// lights in scene
Light_Parameters light[NUMBER_OF_LIGHT_SUPPORTED];

GLint current_filter_mode = GL_LINEAR;

bool is_transparency_enabled = false;  // 투명도 조절 상태
float current_alpha = 1.0f;  // 초기 투명도 (불투명)
bool can_change = false;
/*****/

void Axis_Object::define_axis() {
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_axes), &vertices_axes[0][0], GL_STATIC_DRAW);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Axis_Object::draw_axis(Shader_Simple* shader_simple, glm::mat4& ViewMatrix, glm::mat4& ProjectionMatrix) {
#define WC_AXIS_LENGTH		60.0f
	glm::mat4 ModelMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(WC_AXIS_LENGTH, WC_AXIS_LENGTH, WC_AXIS_LENGTH));
	glm::mat4 ModelViewProjectionMatrix = ProjectionMatrix * ViewMatrix * ModelMatrix;

	glUseProgram(shader_simple->h_ShaderProgram);
	glUniformMatrix4fv(shader_simple->loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);


	glBindVertexArray(VAO);
	glUniform3fv(shader_simple->loc_primitive_color, 1, axes_color[0]);
	glDrawArrays(GL_LINES, 0, 2);
	glUniform3fv(shader_simple->loc_primitive_color, 1, axes_color[1]);
	glDrawArrays(GL_LINES, 2, 2);
	glUniform3fv(shader_simple->loc_primitive_color, 1, axes_color[2]);
	glDrawArrays(GL_LINES, 4, 2);
	glBindVertexArray(0);
	glUseProgram(0);
}

void Scene::clock(int clock_id) { // currently one clock
	time_stamp = ++time_stamp % UINT_MAX;
}

void Scene::build_static_world() {
	static_geometry_data.building.define_object();
	static_object_ID_mapper[STATIC_OBJECT_BUILDING] = static_objects.size();
	static_objects.push_back(static_geometry_data.building);

	static_geometry_data.table.define_object();
	static_object_ID_mapper[STATIC_OBJECT_BUILDING] = static_objects.size();
	static_objects.push_back(static_geometry_data.table);

	static_geometry_data.llight.define_object();
	static_object_ID_mapper[STATIC_OBJECT_BUILDING] = static_objects.size();
	static_objects.push_back(static_geometry_data.llight);

	static_geometry_data.teapot.define_object();
	static_object_ID_mapper[STATIC_OBJECT_BUILDING] = static_objects.size();
	static_objects.push_back(static_geometry_data.teapot);

	static_geometry_data.new_chair.define_object();
	static_object_ID_mapper[STATIC_OBJECT_BUILDING] = static_objects.size();
	static_objects.push_back(static_geometry_data.new_chair);

	static_geometry_data.frame.define_object();
	static_object_ID_mapper[STATIC_OBJECT_BUILDING] = static_objects.size();
	static_objects.push_back(static_geometry_data.frame);

	static_geometry_data.new_picture.define_object();
	static_object_ID_mapper[STATIC_OBJECT_BUILDING] = static_objects.size();
	static_objects.push_back(static_geometry_data.new_picture);

	static_geometry_data.cow.define_object();
	static_object_ID_mapper[STATIC_OBJECT_BUILDING] = static_objects.size();
	static_objects.push_back(static_geometry_data.cow);

	static_geometry_data.bike.define_object();
	static_object_ID_mapper[STATIC_OBJECT_BUILDING] = static_objects.size();
	static_objects.push_back(static_geometry_data.bike);

	static_geometry_data.bus.define_object();
	static_object_ID_mapper[STATIC_OBJECT_BUILDING] = static_objects.size();
	static_objects.push_back(static_geometry_data.bus);

	static_geometry_data.cat.define_object();
	static_object_ID_mapper[STATIC_OBJECT_BUILDING] = static_objects.size();
	static_objects.push_back(static_geometry_data.cat);

	static_geometry_data.dragon.define_object();
	static_object_ID_mapper[STATIC_OBJECT_BUILDING] = static_objects.size();
	static_objects.push_back(static_geometry_data.dragon);

	static_geometry_data.tank.define_object();
	static_object_ID_mapper[STATIC_OBJECT_BUILDING] = static_objects.size();
	static_objects.push_back(static_geometry_data.tank);
}

void Scene::build_dynamic_world() {
	dynamic_geometry_data.tiger_d.define_object();
	dynamic_object_ID_mapper[DYNAMIC_OBJECT_TIGER] = dynamic_objects.size();
	dynamic_objects.push_back(dynamic_geometry_data.tiger_d);

	// two different moving cows
	/*dynamic_geometry_data.cow_d_1.define_object();
	dynamic_object_ID_mapper[DYNAMIC_OBJECT_COW_1] = dynamic_objects.size();
	dynamic_objects.push_back(dynamic_geometry_data.cow_d_1);*/

	dynamic_geometry_data.cow_d_2.define_object();
	dynamic_object_ID_mapper[DYNAMIC_OBJECT_COW_2] = dynamic_objects.size();
	dynamic_objects.push_back(dynamic_geometry_data.cow_d_2);
}

void Scene::create_camera_list(int win_width, int win_height, float win_aspect_ratio) {
	camera_list.clear();
	// main camera
	camera_data.cam_main.define_camera(win_width, win_height, win_aspect_ratio);
	camera_ID_mapper[CAMERA_MAIN] = camera_list.size();
	camera_list.push_back(camera_data.cam_main);

	// define others here
	camera_data.cam_cc_0.define_camera(win_width, win_height, win_aspect_ratio);
	camera_ID_mapper[CAMERA_CC_0] = camera_list.size();
	camera_list.push_back(camera_data.cam_cc_0);

	camera_data.cam_cc_1.define_camera(win_width, win_height, win_aspect_ratio);
	camera_ID_mapper[CAMERA_CC_1] = camera_list.size();
	camera_list.push_back(camera_data.cam_cc_1);

	camera_data.cam_cc_2.define_camera(win_width, win_height, win_aspect_ratio);
	camera_ID_mapper[CAMERA_CC_2] = camera_list.size();
	camera_list.push_back(camera_data.cam_cc_2);

	camera_data.cam_cc_mv.define_camera(win_width, win_height, win_aspect_ratio);
	camera_ID_mapper[CAMERA_CC_MOVE] = camera_list.size();
	camera_list.push_back(camera_data.cam_cc_mv);

	camera_data.orth_front.define_camera(win_width, win_height, win_aspect_ratio);
	camera_ID_mapper[ORTH_FRONT] = camera_list.size();
	camera_list.push_back(camera_data.orth_front);

	camera_data.orth_side.define_camera(win_width, win_height, win_aspect_ratio);
	camera_ID_mapper[ORTH_SIDE] = camera_list.size();
	camera_list.push_back(camera_data.orth_side);

	camera_data.orth_top.define_camera(win_width, win_height, win_aspect_ratio);
	camera_ID_mapper[ORTH_TOP] = camera_list.size();
	camera_list.push_back(camera_data.orth_top);
}

void Scene::build_shader_list() {
	shader_data.shader_simple.prepare_shader();
	shader_ID_mapper[SHADER_SIMPLE] = shader_list.size();
	shader_list.push_back(shader_data.shader_simple);

	shader_data.shader_phong_convert.prepare_shader();
	shader_ID_mapper[PHONG_CONVERT] = shader_list.size();
	shader_list.push_back(shader_data.shader_phong_convert);

	shader_data.shader_gouraud_convert.prepare_shader();
	shader_ID_mapper[GOURAUD_CONVERT] = shader_list.size();
	shader_list.push_back(shader_data.shader_gouraud_convert);

	shader_data.shader_phong_texture.prepare_shader();
	shader_ID_mapper[SHADER_PHONG_TEXTURE] = shader_list.size();
	shader_list.push_back(shader_data.shader_phong_texture);
}

/*****/
void Scene::prepare_shader_program(void) {
	int i;
	char string[256];
	ShaderInfo shader_info_simple[3] = {
		{ GL_VERTEX_SHADER, "Shaders/simple.vert" },
		{ GL_FRAGMENT_SHADER, "Shaders/simple.frag" },
		{ GL_NONE, NULL }
	};
	ShaderInfo shader_info_TXPS[3] = {
		{ GL_VERTEX_SHADER, "Shaders/Texture.vert" },
		{ GL_FRAGMENT_SHADER, "Shaders/Texture.frag" },
		{ GL_NONE, NULL }
	};

	h_ShaderProgram_simple = LoadShaders(shader_info_simple);
	loc_primitive_color = glGetUniformLocation(h_ShaderProgram_simple, "u_primitive_color");
	loc_ModelViewProjectionMatrix_simple = glGetUniformLocation(h_ShaderProgram_simple, "u_ModelViewProjectionMatrix");

	h_ShaderProgram_TXPS = LoadShaders(shader_info_TXPS);
	loc_ModelViewProjectionMatrix_TXPS = glGetUniformLocation(h_ShaderProgram_TXPS, "u_ModelViewProjectionMatrix");
	loc_ModelViewMatrix_TXPS = glGetUniformLocation(h_ShaderProgram_TXPS, "u_ModelViewMatrix");
	loc_ModelViewMatrixInvTrans_TXPS = glGetUniformLocation(h_ShaderProgram_TXPS, "u_ModelViewMatrixInvTrans");

	loc_global_ambient_color = glGetUniformLocation(h_ShaderProgram_TXPS, "u_global_ambient_color");
	for (i = 0; i < NUMBER_OF_LIGHT_SUPPORTED; i++) {
		sprintf(string, "u_light[%d].light_on", i);
		loc_light[i].light_on = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].position", i);
		loc_light[i].position = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].ambient_color", i);
		loc_light[i].ambient_color = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].diffuse_color", i);
		loc_light[i].diffuse_color = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].specular_color", i);
		loc_light[i].specular_color = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].spot_direction", i);
		loc_light[i].spot_direction = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].spot_exponent", i);
		loc_light[i].spot_exponent = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].spot_cutoff_angle", i);
		loc_light[i].spot_cutoff_angle = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].light_attenuation_factors", i);
		loc_light[i].light_attenuation_factors = glGetUniformLocation(h_ShaderProgram_TXPS, string);
	}

	loc_material.ambient_color = glGetUniformLocation(h_ShaderProgram_TXPS, "u_material.ambient_color");
	loc_material.diffuse_color = glGetUniformLocation(h_ShaderProgram_TXPS, "u_material.diffuse_color");
	loc_material.specular_color = glGetUniformLocation(h_ShaderProgram_TXPS, "u_material.specular_color");
	loc_material.emissive_color = glGetUniformLocation(h_ShaderProgram_TXPS, "u_material.emissive_color");
	loc_material.specular_exponent = glGetUniformLocation(h_ShaderProgram_TXPS, "u_material.specular_exponent");

	loc_texture = glGetUniformLocation(h_ShaderProgram_TXPS, "u_base_texture");

	loc_flag_texture_mapping = glGetUniformLocation(h_ShaderProgram_TXPS, "u_flag_texture_mapping");
	loc_flag_fog = glGetUniformLocation(h_ShaderProgram_TXPS, "u_flag_fog");
}

void Scene::initialize_lights_and_material(void) { // follow OpenGL conventions for initialization
	int i;

	glUseProgram(h_ShaderProgram_TXPS);

	glUniform4f(loc_global_ambient_color, 0.115f, 0.115f, 0.115f, 1.0f);
	for (i = 0; i < NUMBER_OF_LIGHT_SUPPORTED; i++) {
		glUniform1i(loc_light[i].light_on, 0); // turn off all lights initially
		glUniform4f(loc_light[i].position, 0.0f, 0.0f, 1.0f, 0.0f);
		glUniform4f(loc_light[i].ambient_color, 0.0f, 0.0f, 0.0f, 1.0f);
		if (i == 0) {
			glUniform4f(loc_light[i].diffuse_color, 1.0f, 1.0f, 1.0f, 1.0f);
			glUniform4f(loc_light[i].specular_color, 1.0f, 1.0f, 1.0f, 1.0f);
		}
		else {
			glUniform4f(loc_light[i].diffuse_color, 0.0f, 0.0f, 0.0f, 1.0f);
			glUniform4f(loc_light[i].specular_color, 0.0f, 0.0f, 0.0f, 1.0f);
		}
		glUniform3f(loc_light[i].spot_direction, 0.0f, 0.0f, -1.0f);
		glUniform1f(loc_light[i].spot_exponent, 0.0f); // [0.0, 128.0]
		glUniform1f(loc_light[i].spot_cutoff_angle, 180.0f); // [0.0, 90.0] or 180.0 (180.0 for no spot light effect)
		glUniform4f(loc_light[i].light_attenuation_factors, 1.0f, 0.0f, 0.0f, 0.0f); // .w != 0.0f for no ligth attenuation
	}

	glUniform4f(loc_material.ambient_color, 0.2f, 0.2f, 0.2f, 1.0f);
	glUniform4f(loc_material.diffuse_color, 0.8f, 0.8f, 0.8f, 1.0f);
	glUniform4f(loc_material.specular_color, 0.0f, 0.0f, 0.0f, 1.0f);
	glUniform4f(loc_material.emissive_color, 0.0f, 0.0f, 0.0f, 1.0f);
	glUniform1f(loc_material.specular_exponent, 0.0f); // [0.0, 128.0]

	glUseProgram(0);
}

void Scene::set_up_scene_lights(void) {
	// point_light_EC: use light 0
	light[0].light_on = 1;
	light[0].position[0] = 0.0f; light[0].position[1] = 0.0f; 	// point light position in EC
	light[0].position[2] = 100.0f; light[0].position[3] = 1.0f;

	light[0].ambient_color[0] = 0.13f; light[0].ambient_color[1] = 0.13f;
	light[0].ambient_color[2] = 0.13f; light[0].ambient_color[3] = 1.0f;

	light[0].diffuse_color[0] = 0.5f; light[0].diffuse_color[1] = 0.5f;
	light[0].diffuse_color[2] = 0.5f; light[0].diffuse_color[3] = 1.5f;

	light[0].specular_color[0] = 0.8f; light[0].specular_color[1] = 0.8f;
	light[0].specular_color[2] = 0.8f; light[0].specular_color[3] = 1.0f;

	// spot_light_WC: use light 1
	light[1].light_on = 1;
	light[1].position[0] = 100.0f; light[1].position[1] = 75.0f; // spot light position in WC
	light[1].position[2] = 100.0f; light[1].position[3] = 1.0f;

	light[1].ambient_color[0] = 0.152f; light[1].ambient_color[1] = 0.152f;
	light[1].ambient_color[2] = 0.152f; light[1].ambient_color[3] = 1.0f;

	light[1].diffuse_color[0] = 0.572f; light[1].diffuse_color[1] = 0.572f;
	light[1].diffuse_color[2] = 0.572f; light[1].diffuse_color[3] = 1.0f;

	light[1].specular_color[0] = 0.772f; light[1].specular_color[1] = 0.772f;
	light[1].specular_color[2] = 0.772f; light[1].specular_color[3] = 1.0f;

	light[1].spot_direction[0] = 0.0f; light[1].spot_direction[1] = 0.0f; // spot light direction in WC
	light[1].spot_direction[2] = -1.0f;
	light[1].spot_cutoff_angle = 20.0f;
	light[1].spot_exponent = 8.0f;

	glUseProgram(h_ShaderProgram_TXPS);
	glUniform1i(loc_light[0].light_on, light[0].light_on);
	glUniform4fv(loc_light[0].position, 1, light[0].position);
	glUniform4fv(loc_light[0].ambient_color, 1, light[0].ambient_color);
	glUniform4fv(loc_light[0].diffuse_color, 1, light[0].diffuse_color);
	glUniform4fv(loc_light[0].specular_color, 1, light[0].specular_color);

	glUniform1i(loc_light[1].light_on, light[1].light_on);
	glUniform4f(loc_light[1].position,
		light[1].position[0],
		light[1].position[1],
		light[1].position[2],
		light[1].position[3]
	);
	glUniform4fv(loc_light[1].ambient_color, 1, light[1].ambient_color);
	glUniform4fv(loc_light[1].diffuse_color, 1, light[1].diffuse_color);
	glUniform4fv(loc_light[1].specular_color, 1, light[1].specular_color);
	// need to supply direction in EC for shading in this example shader
	// note that the viewing transform is a rigid body transform
	// thus transpose(inverse(mat3(ViewMatrix)) = mat3(ViewMatrix)
	glUniform3f(loc_light[1].spot_direction,
		light[1].spot_direction[0],
		light[1].spot_direction[1],
		light[1].spot_direction[2]
	);
	glUniform1f(loc_light[1].spot_cutoff_angle, light[1].spot_cutoff_angle);
	glUniform1f(loc_light[1].spot_exponent, light[1].spot_exponent);
	glUseProgram(0);
}
/*****/

void Scene::initialize() {
	axis_object.define_axis();
	build_static_world();
	build_dynamic_world();
	create_camera_list(window.width, window.height, window.aspect_ratio);
	build_shader_list();

	/*****/
	prepare_shader_program();
	initialize_lights_and_material();
	/*****/
}

void Scene::draw_static_world() {
	glm::mat4 ModelViewProjectionMatrix;
	for (auto static_object = static_objects.begin(); static_object != static_objects.end(); static_object++) {
		if (static_object->get().flag_valid == false) continue;
		static_object->get().draw_object(ViewMatrix, ProjectionMatrix, shader_kind, shader_list);
	}
}

void Scene::draw_dynamic_world() {
	glm::mat4 ModelViewProjectionMatrix;
	for (auto dynamic_object = dynamic_objects.begin(); dynamic_object != dynamic_objects.end(); dynamic_object++) {
		if (dynamic_object->get().flag_valid == false) continue;
		dynamic_object->get().draw_object(ViewMatrix, ProjectionMatrix, shader_kind, shader_list, time_stamp);
	}
}

void Scene::draw_axis() {
	axis_object.draw_axis(static_cast<Shader_Simple*>(&shader_list[shader_ID_mapper[SHADER_SIMPLE]].get()),
		ViewMatrix, ProjectionMatrix);
}

void Scene::draw_world() {
	draw_axis();

	/*****/
	glUseProgram(h_ShaderProgram_TXPS);
	set_up_scene_lights();
	/*****/

	draw_static_world();
	draw_dynamic_world();

	/*
	/hader& current = shader_list[shader_ID_mapper[shader_kind]].get();
	glUseProgram(PHONG_CONVERT);
	upload_lights(PHONG_CONVERT);
	*/
}

/*****/
void set_texture_filtering(GLuint texid, GLint filter_mode) {
	glBindTexture(GL_TEXTURE_2D, texid);
	if (filter_mode == GL_NEAREST) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
	else if (filter_mode == GL_LINEAR) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
}

/*void update_transparency() {
	dynamic_geometry_data.tiger_d.define_object();  // 알파값을 반영하기 위해 다시 정의

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);;
}*/
/*****/

/*
void Scene::upload_lights(GLuint prog) const {
	glUseProgram(prog);
	// Light 0(point)
	const int i0 = 0;
	glUniform1i(glGetUniformLocation(prog, "u_light[0].light_on"), 1);
	glUniform4f(glGetUniformLocation(prog, "u_light[0].position"), 0.0f, 0.0f, 0.0f, 1.0f);
	glUniform4f(glGetUniformLocation(prog, "u_light[0].ambient_color"), 0.13f, 0.13f, 0.13f, 1.0f);
	glUniform4f(glGetUniformLocation(prog, "u_light[0].diffuse_color"), 0.50f, 0.50f, 0.50f, 1.5f);
	glUniform4f(glGetUniformLocation(prog, "u_light[0].specular_color"), 0.80f, 0.80f, 0.80f, 1.0f);
	// Light 1(spot)
	glUniform1i(glGetUniformLocation(prog, "u_light[1].light_on"), 1);
	glm::vec4 posEC = ViewMatrix * glm::vec4(150, 75, 100, 1);
	glUniform4fv(glGetUniformLocation(prog, "u_light[1].position"), 1, (float*)&posEC);
	glUniform4f(glGetUniformLocation(prog, "u_light[1].ambient_color"), 0.152f, 0.152f, 0.152f, 1.0f);
	glUniform4f(glGetUniformLocation(prog, "u_light[1].diffuse_color"), 0.572f, 0.572f, 0.572f, 1.0f);
	glUniform4f(glGetUniformLocation(prog, "u_light[1].specular_color"), 0.772f, 0.772f, 0.772f, 1.0f);
	glm::vec3 dirEC = glm::mat3(ViewMatrix) * glm::vec3(0, 0, -1);
	glUniform3fv(glGetUniformLocation(prog, "u_light[1].spot_direction"), 1, (float*)&dirEC);
	glUniform1f(glGetUniformLocation(prog, "u_light[1].spot_cutoff_angle"), 20.0f);
	glUniform1f(glGetUniformLocation(prog, "u_light[1].spot_exponent"), 8.0f);
	glUseProgram(0);
}
*/