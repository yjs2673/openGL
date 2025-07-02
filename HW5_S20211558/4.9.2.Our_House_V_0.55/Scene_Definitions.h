#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <FreeImage/FreeImage.h>
#include "Shaders/LoadShaders.h"
#include "Camera.h"

#include "My_Shading.h"

#define BUFFER_OFFSET(offset) ((GLvoid *) (offset))
#define TO_RADIAN 0.01745329252f  
#define TO_DEGREE 57.295779513f

#define N_MAX_STATIC_OBJECTS		20
#define N_MAX_DYNAMIC_OBJECTS		10
#define N_MAX_CAMERAS		10
#define N_MAX_SHADERS		10

extern unsigned int static_object_ID_mapper[N_MAX_STATIC_OBJECTS];
extern unsigned int dynamic_object_ID_mapper[N_MAX_DYNAMIC_OBJECTS];
extern unsigned int camera_ID_mapper[N_MAX_CAMERAS];
extern unsigned int shader_ID_mapper[N_MAX_SHADERS];

/*****/
#define NUMBER_OF_LIGHT_SUPPORTED 4 
extern Light_Parameters light[NUMBER_OF_LIGHT_SUPPORTED];

extern GLint current_filter_mode;

extern bool is_transparency_enabled;  // 투명도 조절 상태
extern float current_alpha;  // 초기 투명도 (불투명)
extern bool can_change;
/*****/

enum STATIC_OBJECT_ID {
	STATIC_OBJECT_BUILDING = 0, STATIC_OBJECT_TABLE, STATIC_OBJECT_LIGHT, 
	STATIC_OBJECT_TEAPOT, STATIC_OBJECT_NEW_CHAIR, 
	STATIC_OBJECT_FRAME, STATIC_OBJECT_NEW_PICTURE, STATIC_OBJECT_COW,
	STATIC_OBJECT_BIKE, STATIC_OBJECT_BUS, STATIC_OBJECT_CAT,
	STATIC_OBJECT_DRAGON, STATIC_OBJECT_TANK
};

enum DYNAMIC_OBJECT_ID {
	DYNAMIC_OBJECT_TIGER = 0, DYNAMIC_OBJECT_COW_1, DYNAMIC_OBJECT_COW_2,
};

enum SHADER_ID {
	SHADER_SIMPLE = 0, PHONG_CONVERT, GOURAUD_CONVERT, SHADER_PHONG_TEXTURE
};

struct Shader {
	ShaderInfo shader_info[3];
	GLuint h_ShaderProgram; // handle to shader program
	GLint loc_ModelViewProjectionMatrix;

	GLint loc_base_texture;
	GLint loc_flag_texture_mapping;
	GLint loc_flag_fog;

	Shader() {
		h_ShaderProgram = NULL;
		loc_ModelViewProjectionMatrix = -1;
		shader_info[0] = shader_info[1] = shader_info[2] = { NULL, NULL };
	}
	virtual void prepare_shader() = 0;
};

struct Shader_Simple : Shader {
	GLint loc_primitive_color;
	void prepare_shader();
};

struct Shader_Phong_Convert : Shader {
	GLint loc_ModelViewMatrix, loc_NormalMatrix, loc_MVP;
	GLint loc_Ka, loc_Kd, loc_Ks, loc_Shininess;
	void prepare_shader();
};

struct Shader_Gouraud_Convert : Shader {
	GLint loc_ModelViewMatrix, loc_NormalMatrix, loc_MVP;
	GLint loc_Ka, loc_Kd, loc_Ks, loc_Shininess;
	void prepare_shader();
};

struct Shader_Phong_Texture : Shader {
	GLint loc_ModelViewMatrix,
		loc_NormalMatrix,
		loc_MVP,
		loc_ModelViewMatrixInvTrans;
	GLint loc_flag_texture_mapping,
		loc_base_texture,
		loc_global_ambient_color;

	static constexpr int MAX_LIGHT = 4;
	GLint loc_light_on[MAX_LIGHT];
	GLint loc_light_position[MAX_LIGHT];
	GLint loc_light_ambient[MAX_LIGHT];
	GLint loc_light_diffuse[MAX_LIGHT];
	GLint loc_light_specular[MAX_LIGHT];
	GLint loc_light_spot_direction[MAX_LIGHT];
	GLint loc_light_spot_cutoff_angle[MAX_LIGHT];
	GLint loc_light_spot_exponent[MAX_LIGHT];
	GLint loc_light_attenuation[MAX_LIGHT];

	void prepare_shader();
};

struct Shader_Data {
	Shader_Simple shader_simple;
	Shader_Phong_Convert shader_phong_convert;
	Shader_Gouraud_Convert shader_gouraud_convert;
	Shader_Phong_Texture shader_phong_texture;
};

struct Material {
	glm::vec4 emission, ambient, diffuse, specular;
	GLfloat exponent;
};

struct Instance {
	glm::mat4 ModelMatrix;
	Material material;
};

struct Axis_Object {
	GLuint VBO, VAO;
	GLfloat vertices_axes[6][3] = {
		{ 0.0f, 0.0f, 0.0f },{ 1.0f, 0.0f, 0.0f },{ 0.0f, 0.0f, 0.0f },{ 0.0f, 1.0f, 0.0f },
		{ 0.0f, 0.0f, 0.0f },{ 0.0f, 0.0f, 1.0f }
	};
	GLfloat axes_color[3][3] = { { 1.0f, 0.0f, 0.0f },{ 0.0f, 1.0f, 0.0f },{ 0.0f, 0.0f, 1.0f } };

	void define_axis();
	void draw_axis(Shader_Simple* shader_simple, glm::mat4& ViewMatrix, glm::mat4& ProjectionMatrix);
};

struct Static_Object { // an object that does not move
	STATIC_OBJECT_ID object_id;
	char filename[512]; // where to read geometry data
	int n_fields; // 3/6/8 where3 floats for vertex, 3 floats for normal, and 2 floats for texcoord
	int n_triangles;
	GLfloat* vertices; // pointer to vertex array data
	GLuint VBO, VAO; // handles to vertex buffer object and vertex array object
	GLenum front_face_mode; // clockwise or counter-clockwise

	std::vector<Instance> instances;
	bool flag_valid;

	GLuint texid = 0;       // 텍스처 GL 아이디 추가
	bool has_texture = false;   // 텍스처 사용 여부 플래그 추가

	Static_Object() {}
	Static_Object(STATIC_OBJECT_ID _object_id) : object_id(_object_id) {
		instances.clear();
	}
	void read_geometry(int bytes_per_primitive);  
	void prepare_geom_of_static_object();
	void draw_object(glm::mat4& ViewMatrix, glm::mat4& ProjectionMatrix, SHADER_ID shader_kind,
		std::vector<std::reference_wrapper<Shader>>& shader_list);
};

struct Building : public Static_Object { 
	Building(STATIC_OBJECT_ID _object_id) : Static_Object(_object_id) {}
	void define_object(); 
};

struct Table : public Static_Object { 
	Table(STATIC_OBJECT_ID _object_id) : Static_Object(_object_id) {}
	void define_object(); 
};

struct LLight : public Static_Object { 
	LLight(STATIC_OBJECT_ID _object_id) : Static_Object(_object_id) {}
	void define_object(); 
};
struct Teapot : public Static_Object { 
	Teapot(STATIC_OBJECT_ID _object_id) : Static_Object(_object_id) {}
	void define_object(); 
};
struct New_Chair : public Static_Object { 
	New_Chair(STATIC_OBJECT_ID _object_id) : Static_Object(_object_id) {}
	void define_object(); 
};

struct Frame : public Static_Object { 
	Frame(STATIC_OBJECT_ID _object_id) : Static_Object(_object_id) {}
	void define_object(); 
};

struct New_Picture : public Static_Object { 
	New_Picture(STATIC_OBJECT_ID _object_id) : Static_Object(_object_id) {}
	void define_object(); 
};

struct Cow : public Static_Object { 
	Cow(STATIC_OBJECT_ID _object_id) : Static_Object(_object_id) {}
	void define_object(); 
};

/*****/
struct Bike : public Static_Object {
	Bike(STATIC_OBJECT_ID _object_id) : Static_Object(_object_id) {}
	void define_object();
};

struct Bus : public Static_Object {
	Bus(STATIC_OBJECT_ID _object_id) : Static_Object(_object_id) {}
	void define_object();
};

struct Cat : public Static_Object {
	Cat(STATIC_OBJECT_ID _object_id) : Static_Object(_object_id) {}
	void define_object();
};

struct Dragon : public Static_Object {
	Dragon(STATIC_OBJECT_ID _object_id) : Static_Object(_object_id) {}
	void define_object();
};

struct Tank : public Static_Object {
	Tank(STATIC_OBJECT_ID _object_id) : Static_Object(_object_id) {}
	void define_object();
};
/*****/

struct Static_Geometry_Data {
	Building building{ STATIC_OBJECT_BUILDING };
	Table table{ STATIC_OBJECT_TABLE };
	LLight llight{ STATIC_OBJECT_LIGHT };
	Teapot teapot{ STATIC_OBJECT_TEAPOT };
	New_Chair new_chair{ STATIC_OBJECT_NEW_CHAIR };
	Frame frame{ STATIC_OBJECT_FRAME };
	New_Picture new_picture{ STATIC_OBJECT_NEW_PICTURE };
	Cow cow{ STATIC_OBJECT_COW };
	Bike bike{ STATIC_OBJECT_BIKE };
	Bus bus{ STATIC_OBJECT_BUS };
	Cat cat{ STATIC_OBJECT_CAT };
	Dragon dragon{ STATIC_OBJECT_DRAGON };
	Tank tank{ STATIC_OBJECT_TANK };
};

struct Dynamic_Object { // an object that moves
	DYNAMIC_OBJECT_ID object_id;
	std::vector<Static_Object> object_frames;
	bool flag_valid;

	GLuint texid = 0;       // 텍스처 GL 아이디 추가
	bool has_texture = false;   // 텍스처 사용 여부 플래그 추가

	Dynamic_Object() {}
	Dynamic_Object(DYNAMIC_OBJECT_ID _object_id) : object_id(_object_id) {
		object_frames.clear();
	}

	void draw_object(glm::mat4& ViewMatrix, glm::mat4& ProjectionMatrix, SHADER_ID shader_kind,
		std::vector<std::reference_wrapper<Shader>>& shader_list, int time_stamp);
};

struct Tiger_D : public Dynamic_Object { 
	Tiger_D(DYNAMIC_OBJECT_ID _object_id) : Dynamic_Object(_object_id) {}
	void define_object(); 
};

struct Cow_D : public Dynamic_Object { 
	Cow_D(DYNAMIC_OBJECT_ID _object_id) : Dynamic_Object(_object_id) {}
	void define_object(); 
};

struct Dynamic_Geometry_Data {
	Tiger_D tiger_d{ DYNAMIC_OBJECT_TIGER };
	// Cow_D cow_d_1{ DYNAMIC_OBJECT_COW_1 };
	Cow_D cow_d_2{ DYNAMIC_OBJECT_COW_2 };
};

struct Window {
	int width, height;
	float aspect_ratio;
};

struct Scene {
	unsigned int time_stamp;
	Static_Geometry_Data static_geometry_data;
	std::vector<std::reference_wrapper<Static_Object>> static_objects;

	Dynamic_Geometry_Data dynamic_geometry_data;
	std::vector<std::reference_wrapper<Dynamic_Object>> dynamic_objects;

	Camera_Data camera_data;
	std::vector<std::reference_wrapper<Camera>> camera_list;

	Shader_Data shader_data;
	std::vector<std::reference_wrapper<Shader>> shader_list;
	SHADER_ID shader_kind;

	Window window; // for a better code, this must be defined in another structure!

	glm::mat4 ViewMatrix;
	glm::mat4 ProjectionMatrix;

	Axis_Object axis_object;

	Scene() {
		time_stamp = 0;
		static_objects.clear();
		shader_list.clear();
		shader_kind = SHADER_SIMPLE;
		ViewMatrix = ProjectionMatrix = glm::mat4(1.0f);
	}

	void clock(int clock_id);
	void build_static_world();
	void build_dynamic_world();
	void create_camera_list(int win_width, int win_height, float win_aspect_ratio);
	void build_shader_list();
	void initialize();
	void draw_static_world();
	void draw_dynamic_world();
	void draw_axis();
	void draw_world();

	/*****/
	void prepare_shader_program();
	void initialize_lights_and_material();
	void set_up_scene_lights();
	//void upload_lights(GLuint prog) const;
	/*****/
};

void set_texture_filtering(GLuint texid, GLint filter_mode);
//void update_transparency();