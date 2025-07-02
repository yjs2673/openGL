#define _CRT_SECURE_NO_WARNINGS

#include "Scene_Definitions.h"
#include <glm/gtc/matrix_inverse.hpp>

/* Building Table Light Teapot New_Chair Frame New_Picture Cow*/

void Static_Object::read_geometry(int bytes_per_primitive) {
	FILE* fp;

	// fprintf(stdout, "Reading geometry from the geometry file %s...\n", filename);
	fp = fopen(filename, "rb");
	if (fp == NULL) {
		fprintf(stderr, "Error: cannot open the object file %s ...\n", filename);
		exit(EXIT_FAILURE);
	}
	fread(&n_triangles, sizeof(int), 1, fp);
	vertices = (float*)malloc(n_triangles * bytes_per_primitive);
	if (vertices == NULL) {
		fprintf(stderr, "Error: cannot allocate memory for the geometry file %s ...\n", filename);
		exit(EXIT_FAILURE);
	}
	fread(vertices, bytes_per_primitive, n_triangles, fp); // assume the data file has no faults.
	// fprintf(stdout, "Read %d primitives successfully.\n\n", n_triangles);
	fclose(fp);
}

void Static_Object::prepare_geom_of_static_object() {
	int i, n_bytes_per_vertex, n_bytes_per_triangle;
	char filename[512];

	n_bytes_per_vertex = n_fields * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	read_geometry(n_bytes_per_triangle);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, n_triangles * n_bytes_per_triangle, vertices, GL_STATIC_DRAW);

	free(vertices);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	//glEnableVertexAttribArray(0);
	// 위치
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	// 법선 (n_fields >= 6)
	if (n_fields >= 6) {
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex,
		BUFFER_OFFSET(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		
	}
	// 텍스처 좌표 (n_fields == 8)
	if (n_fields == 8) {
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, n_bytes_per_vertex,
		BUFFER_OFFSET(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
		
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Building::define_object() {
	Material* cur_material;
	strcpy(filename, "Data/Building1_vnt.geom");
	n_fields = 8;
	front_face_mode = GL_CCW;
	prepare_geom_of_static_object();
	flag_valid = true;

	instances.emplace_back();
	instances.back().ModelMatrix = glm::mat4(1.0f);

	cur_material = &(instances.back().material);
	cur_material->emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	cur_material->ambient = glm::vec4(0.135f, 0.2225f, 0.1575f, 1.0f);
	cur_material->diffuse = glm::vec4(0.54f, 0.89f, 0.63f, 1.0f);
	cur_material->specular = glm::vec4(0.316228f, 0.316228f, 0.316228f, 1.0f);
	cur_material->exponent = 128.0f * 0.1f;
}

void Table::define_object() {
	glm::mat4* cur_MM;
	Material* cur_material;

	strcpy(filename, "Data/Table_vn.geom");
	n_fields = 6;
	front_face_mode = GL_CCW;
	prepare_geom_of_static_object();
	flag_valid = true;

	instances.emplace_back();
	cur_MM = &(instances.back().ModelMatrix);
	*cur_MM = glm::translate(glm::mat4(1.0f), glm::vec3(157.0f, 76.5f, 0.0f));
	*cur_MM = glm::scale(*cur_MM, glm::vec3(0.5f, 0.5f, 0.5f));
	cur_material = &(instances.back().material);
	cur_material->emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	cur_material->ambient = glm::vec4(0.1f, 0.3f, 0.1f, 1.0f);
	cur_material->diffuse = glm::vec4(0.4f, 0.6f, 0.3f, 1.0f);
	cur_material->specular = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
	cur_material->exponent = 15.0f;

	instances.emplace_back();
	cur_MM = &(instances.back().ModelMatrix);
	*cur_MM = glm::translate(glm::mat4(1.0f), glm::vec3(198.0f, 120.0f, 0.0f));
	*cur_MM = glm::scale(*cur_MM, glm::vec3(0.8f, 0.6f, 0.6f));
	cur_material = &(instances.back().material);
	cur_material->emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	cur_material->ambient = glm::vec4(0.05f, 0.05f, 0.05f, 1.0f);
	cur_material->diffuse = glm::vec4(0.7f, 0.7f, 0.7f, 1.0f);
	cur_material->specular = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
	cur_material->exponent = 128.0f * 0.078125f;
}

void LLight::define_object() {
	glm::mat4* cur_MM;
	Material* cur_material;
	strcpy(filename, "Data/Light_vn.geom");
	n_fields = 6;
	front_face_mode = GL_CCW;
	prepare_geom_of_static_object();
	flag_valid = true;

	instances.emplace_back();
	cur_MM = &(instances.back().ModelMatrix);
	*cur_MM = glm::translate(glm::mat4(1.0f), glm::vec3(120.0f, 100.0f, 49.0f));
	*cur_MM = glm::rotate(*cur_MM, 90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	cur_material = &(instances.back().material);
	cur_material->emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	cur_material->ambient = glm::vec4(0.24725f, 0.1995f, 0.0745f, 1.0f);
	cur_material->diffuse = glm::vec4(0.75164f, 0.60648f, 0.22648f, 1.0f);
	cur_material->specular = glm::vec4(0.628281f, 0.555802f, 0.366065f, 1.0f);
	cur_material->exponent = 128.0f * 0.4f;

	instances.emplace_back();
	cur_MM = &(instances.back().ModelMatrix);
	*cur_MM = glm::translate(glm::mat4(1.0f), glm::vec3(80.0f, 47.5f, 49.0f));
	*cur_MM = glm::rotate(*cur_MM, 90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	cur_material = &(instances.back().material);
	cur_material->emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	cur_material->ambient = glm::vec4(0.24725f, 0.1995f, 0.0745f, 1.0f);
	cur_material->diffuse = glm::vec4(0.75164f, 0.60648f, 0.22648f, 1.0f);
	cur_material->specular = glm::vec4(0.628281f, 0.555802f, 0.366065f, 1.0f);
	cur_material->exponent = 128.0f * 0.4f;

	instances.emplace_back();
	cur_MM = &(instances.back().ModelMatrix);
	*cur_MM = glm::translate(glm::mat4(1.0f), glm::vec3(40.0f, 130.0f, 49.0f));
	*cur_MM = glm::rotate(*cur_MM, 90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	cur_material = &(instances.back().material);
	cur_material->emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	cur_material->ambient = glm::vec4(0.24725f, 0.1995f, 0.0745f, 1.0f);
	cur_material->diffuse = glm::vec4(0.75164f, 0.60648f, 0.22648f, 1.0f);
	cur_material->specular = glm::vec4(0.628281f, 0.555802f, 0.366065f, 1.0f);
	cur_material->exponent = 128.0f * 0.4f;

	instances.emplace_back();
	cur_MM = &(instances.back().ModelMatrix);
	*cur_MM = glm::translate(glm::mat4(1.0f), glm::vec3(190.0f, 60.0f, 49.0f));
	*cur_MM = glm::rotate(*cur_MM, 90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	cur_material = &(instances.back().material);
	cur_material->emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	cur_material->ambient = glm::vec4(0.24725f, 0.1995f, 0.0745f, 1.0f);
	cur_material->diffuse = glm::vec4(0.75164f, 0.60648f, 0.22648f, 1.0f);
	cur_material->specular = glm::vec4(0.628281f, 0.555802f, 0.366065f, 1.0f);
	cur_material->exponent = 128.0f * 0.4f;

	instances.emplace_back();
	cur_MM = &(instances.back().ModelMatrix);
	*cur_MM = glm::translate(glm::mat4(1.0f), glm::vec3(210.0f, 112.5f, 49.0));
	*cur_MM = glm::rotate(*cur_MM, 90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	cur_material = &(instances.back().material);
	cur_material->emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	cur_material->ambient = glm::vec4(0.24725f, 0.1995f, 0.0745f, 1.0f);
	cur_material->diffuse = glm::vec4(0.75164f, 0.60648f, 0.22648f, 1.0f);
	cur_material->specular = glm::vec4(0.628281f, 0.555802f, 0.366065f, 1.0f);
	cur_material->exponent = 128.0f * 0.4f;

}

void Teapot::define_object() {
	glm::mat4* cur_MM;
	Material* cur_material;
	strcpy(filename, "Data/Teapotn_vn.geom");
	n_fields = 6;
	front_face_mode = GL_CCW;
	prepare_geom_of_static_object();
	flag_valid = true;

	instances.emplace_back();
	cur_MM = &(instances.back().ModelMatrix);
	*cur_MM = glm::translate(glm::mat4(1.0f), glm::vec3(193.0f, 120.0f, 11.0f));
	*cur_MM = glm::scale(*cur_MM, glm::vec3(2.0f, 2.0f, 2.0f));
	cur_material = &(instances.back().material);
	cur_material->emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	cur_material->ambient = glm::vec4(0.1745f, 0.01175f, 0.01175f, 1.0f);
	cur_material->diffuse = glm::vec4(0.61424f, 0.04136f, 0.04136f, 1.0f);
	cur_material->specular = glm::vec4(0.727811f, 0.626959f, 0.626959f, 1.0f);
	cur_material->exponent = 128.0f * 0.6;
}

void New_Chair::define_object() {
	glm::mat4* cur_MM;
	Material* cur_material;
	strcpy(filename, "Data/new_chair_vnt.geom");
	n_fields = 8;
	front_face_mode = GL_CCW;
	prepare_geom_of_static_object();
	flag_valid = true;

	instances.emplace_back();
	cur_MM = &(instances.back().ModelMatrix);
	*cur_MM = glm::translate(glm::mat4(1.0f), glm::vec3(200.0f, 104.0f, 0.0f));
	*cur_MM = glm::scale(*cur_MM, glm::vec3(0.8f, 0.8f, 0.8f));
	*cur_MM = glm::rotate(*cur_MM, 180.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	cur_material = &(instances.back().material);
	cur_material->emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	cur_material->ambient = glm::vec4(0.05f, 0.05f, 0.0f, 1.0f);
	cur_material->diffuse = glm::vec4(0.5f, 0.5f, 0.4f, 1.0f);
	cur_material->specular = glm::vec4(0.7f, 0.7f, 0.04f, 1.0f);
	cur_material->exponent = 128.0f * 0.078125f;
}

void Frame::define_object() {
	glm::mat4* cur_MM;
	Material* cur_material;
	strcpy(filename, "Data/Frame_vn.geom");
	n_fields = 6;
	front_face_mode = GL_CCW;
	prepare_geom_of_static_object();
	flag_valid = true;

	instances.emplace_back();
	cur_MM = &(instances.back().ModelMatrix);
	*cur_MM = glm::translate(glm::mat4(1.0f), glm::vec3(188.0f, 116.0f, 30.0f));
	*cur_MM = glm::scale(*cur_MM, glm::vec3(0.6f, 0.6f, 0.6f));
	*cur_MM = glm::rotate(*cur_MM, 90.0f * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	cur_material = &(instances.back().material);
	cur_material->emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	cur_material->ambient = glm::vec4(0.19125f, 0.0735f, 0.0225f, 1.0f);
	cur_material->diffuse = glm::vec4(0.7038f, 0.27048f, 0.0828f, 1.0f);
	cur_material->specular = glm::vec4(0.256777f, 0.137622f, 0.086014f, 1.0f);
	cur_material->exponent = 128.0f * 0.1f;
}

void New_Picture::define_object() {
	glm::mat4* cur_MM;
	Material* cur_material;
	strcpy(filename, "Data/new_picture_vnt.geom");
	n_fields = 8;
	front_face_mode = GL_CCW;
	prepare_geom_of_static_object();
	flag_valid = true;

	instances.emplace_back();
	cur_MM = &(instances.back().ModelMatrix);
	*cur_MM = glm::translate(glm::mat4(1.0f), glm::vec3(189.5f, 116.0f, 30.0f));
	*cur_MM = glm::scale(*cur_MM, glm::vec3(13.5f * 0.6f, 13.5f * 0.6f, 13.5f * 0.6f));
	*cur_MM = glm::rotate(*cur_MM, 90.0f * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	cur_material = &(instances.back().material);
	cur_material->emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	cur_material->ambient = glm::vec4(0.25f, 0.25f, 0.25f, 1.0f);
	cur_material->diffuse = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f);
	cur_material->specular = glm::vec4(0.774597f, 0.774597f, 0.774597f, 1.0f);
	cur_material->exponent = 128.0f * 0.6f;
}

void Cow::define_object() {
	glm::mat4* cur_MM;
	Material* cur_material;
	strcpy(filename, "Data/cow_vn.geom");
	n_fields = 6;
	front_face_mode = GL_CCW;
	prepare_geom_of_static_object();
	flag_valid = true;

	instances.emplace_back();
	cur_MM = &(instances.back().ModelMatrix);
	*cur_MM = glm::translate(glm::mat4(1.0f), glm::vec3(215.0f, 100.0f, 9.5f));
	*cur_MM = glm::scale(*cur_MM, glm::vec3(30.0f, 30.0f, 30.0f));
	*cur_MM = glm::rotate(*cur_MM, 90.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	*cur_MM = glm::rotate(*cur_MM, 90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	cur_material = &(instances.back().material);
	cur_material->emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	cur_material->ambient = glm::vec4(0.329412f, 0.223529f, 0.027451f, 1.0f);
	cur_material->diffuse = glm::vec4(0.780392f, 0.568627f, 0.113725f, 1.0f);
	cur_material->specular = glm::vec4(0.992157f, 0.941176f, 0.807843f, 1.0f);
	cur_material->exponent = 0.21794872f * 0.6f;
}

/*****/
void Bike::define_object() {
	glm::mat4* cur_MM;
	Material* cur_material;
	strcpy(filename, "Data/static_objects/bike_vnt.geom");
	n_fields = 8;
	front_face_mode = GL_CCW;
	prepare_geom_of_static_object();
	flag_valid = true;

	instances.emplace_back();
	cur_MM = &(instances.back().ModelMatrix);
	*cur_MM = glm::translate(glm::mat4(1.0f), glm::vec3(100.0f, 140.0f, 0.5f));
	*cur_MM = glm::scale(*cur_MM, glm::vec3(10.0f, 10.0f, 10.0f));
	*cur_MM = glm::rotate(*cur_MM, 90.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	*cur_MM = glm::rotate(*cur_MM, 90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	cur_material = &(instances.back().material);
	cur_material->emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	cur_material->ambient = glm::vec4(0.6f, 0.3f, 0.5f, 1.0f);   // 약간 연한 분홍
	cur_material->diffuse = glm::vec4(0.9f, 0.4f, 0.7f, 1.0f);   // 밝고 진한 분홍
	cur_material->specular = glm::vec4(1.0f, 0.8f, 0.9f, 1.0f);  // 밝고 반짝임 있는 분홍
	cur_material->exponent = 128.0f * 0.5f;                      // 적당한 광택
}

void Bus::define_object() {
	glm::mat4* cur_MM;
	Material* cur_material;
	strcpy(filename, "Data/static_objects/bus_vnt.geom");
	n_fields = 8;
	front_face_mode = GL_CCW;
	prepare_geom_of_static_object();
	flag_valid = true;

	instances.emplace_back();
	cur_MM = &(instances.back().ModelMatrix);
	*cur_MM = glm::translate(glm::mat4(1.0f), glm::vec3(100.0f, 110.0f, 0.5f));
	*cur_MM = glm::scale(*cur_MM, glm::vec3(0.03f, 0.03f, 0.03f));
	*cur_MM = glm::rotate(*cur_MM, 90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	cur_material = &(instances.back().material);
	cur_material->emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	cur_material->ambient = glm::vec4(0.0f, 0.0f, 0.2f, 1.0f);    // 부드러운 하늘색 주변광
	cur_material->diffuse = glm::vec4(0.2f, 0.4f, 1.0f, 1.0f);    // 밝은 하늘색 확산광
	cur_material->specular = glm::vec4(0.6f, 0.8f, 1.0f, 1.0f);   // 은은한 하늘색 반사광
	cur_material->exponent = 128.0f * 0.5f;                       // 적당한 광택
}

void Cat::define_object() {
	glm::mat4* cur_MM;
	Material* cur_material;
	strcpy(filename, "Data/static_objects/cat_vnt.geom");
	n_fields = 8;
	front_face_mode = GL_CCW;
	prepare_geom_of_static_object();
	flag_valid = true;

	instances.emplace_back();
	cur_MM = &(instances.back().ModelMatrix);
	*cur_MM = glm::translate(glm::mat4(1.0f), glm::vec3(210.0f, 25.0f, 0.5f));
	*cur_MM = glm::scale(*cur_MM, glm::vec3(15.0f, 15.0f, 15.0f));
	*cur_MM = glm::rotate(*cur_MM, 180.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	*cur_MM = glm::rotate(*cur_MM, 90.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	*cur_MM = glm::rotate(*cur_MM, 90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	cur_material = &(instances.back().material);
	cur_material->emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	cur_material->ambient = glm::vec4(0.05f, 0.05f, 0.05f, 1.0f);  // 거의 검정 주변광
	cur_material->diffuse = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);     // 아주 어두운 검정 확산광
	cur_material->specular = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);    // 은은한 반사광
	cur_material->exponent = 128.0f * 0.3f;                        // 낮은 광택
}

void Dragon::define_object() {
	glm::mat4* cur_MM;
	Material* cur_material;
	strcpy(filename, "Data/static_objects/dragon_vnt.geom");
	n_fields = 8;
	front_face_mode = GL_CCW;
	prepare_geom_of_static_object();
	flag_valid = true;

	instances.emplace_back();
	cur_MM = &(instances.back().ModelMatrix);
	*cur_MM = glm::translate(glm::mat4(1.0f), glm::vec3(35.0f, 35.0f, 0.5f));
	*cur_MM = glm::scale(*cur_MM, glm::vec3(0.6f, 0.6f, 0.6f));
	*cur_MM = glm::rotate(*cur_MM, -45.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	cur_material = &(instances.back().material);
	cur_material->emission = glm::vec4(0.05f, 0.0f, 0.1f, 1.0f);  // 약간 빛나는 보라색 느낌
	cur_material->ambient = glm::vec4(0.3f, 0.0f, 0.5f, 1.0f);    // 진한 보라 주변광
	cur_material->diffuse = glm::vec4(0.5f, 0.0f, 0.7f, 1.0f);    // 진하고 깊은 보라 확산광
	cur_material->specular = glm::vec4(0.7f, 0.5f, 0.9f, 1.0f);   // 반짝임 있는 보라 반사광
	cur_material->exponent = 128.0f * 0.7f;                       // 적당한 광택
}

void Tank::define_object() {
	glm::mat4* cur_MM;
	Material* cur_material;
	strcpy(filename, "Data/static_objects/tank_vnt.geom");
	n_fields = 8;
	front_face_mode = GL_CCW;
	prepare_geom_of_static_object();
	flag_valid = true;

	/*****/
	has_texture = true;
	glGenTextures(1, &texid);
	glBindTexture(GL_TEXTURE_2D, texid);
	// 반복 및 필터링
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// 이미지 로드
	FREE_IMAGE_FORMAT fif = FreeImage_GetFileType("Data/tank.jpg", 0);
	FIBITMAP* bmp = FreeImage_Load(fif, "Data/tank.jpg", 0);
	if (bmp) {
		FIBITMAP* bmp32 = FreeImage_ConvertTo32Bits(bmp);
		int w = FreeImage_GetWidth(bmp32), h = FreeImage_GetHeight(bmp32);
		void* bits = FreeImage_GetBits(bmp32);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, bits);
		glGenerateMipmap(GL_TEXTURE_2D);
		FreeImage_Unload(bmp32);
		FreeImage_Unload(bmp);
	}
	else {
		fprintf(stderr, "Failed to load Data/tank.jpg\n");
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	/*****/

	instances.emplace_back();
	cur_MM = &(instances.back().ModelMatrix);
	*cur_MM = glm::translate(glm::mat4(1.0f), glm::vec3(5.0f, 95.0f, 0.5f));
	*cur_MM = glm::scale(*cur_MM, glm::vec3(4.0f, 4.0f, 4.0f));
	*cur_MM = glm::rotate(*cur_MM, -45.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	cur_material = &(instances.back().material);
	cur_material->ambient = glm::vec4(0.15f, 0.1f, 0.02f, 1.0f);   // 아주 어두운 갈색 주변광
	cur_material->diffuse = glm::vec4(0.4f, 0.25f, 0.05f, 1.0f);   // 어두운 갈색 확산광
	cur_material->specular = glm::vec4(0.3f, 0.25f, 0.1f, 1.0f);   // 낮은 반사광, 무광 느낌
	cur_material->exponent = 128.0f * 0.3f;                        // 낮은 광택
}
/*****/

void print_mat4(const char* string, glm::mat4 M) {
	fprintf(stdout, "\n***** %s ******\n", string);
	for (int i = 0; i < 4; i++)
		fprintf(stdout, "*** COL[%d] (%f, %f, %f, %f)\n", i, M[i].x, M[i].y, M[i].z, M[i].w);
	fprintf(stdout, "**************\n\n");
}

void Static_Object::draw_object(glm::mat4& ViewMatrix, glm::mat4& ProjectionMatrix, SHADER_ID shader_kind,
	std::vector<std::reference_wrapper<Shader>>& shader_list) {
	glm::mat4 ModelViewProjectionMatrix;
	glFrontFace(front_face_mode);
	for (int i = 0; i < instances.size(); i++) {
		ModelViewProjectionMatrix = ProjectionMatrix * ViewMatrix * instances[i].ModelMatrix;

		if (object_id == STATIC_OBJECT_TANK) shader_kind = SHADER_PHONG_TEXTURE;
		else if (object_id == STATIC_OBJECT_DRAGON);
		else shader_kind = SHADER_SIMPLE;

		switch (shader_kind) {
			case SHADER_SIMPLE: {
				Shader_Simple* shader_simple_ptr = static_cast<Shader_Simple*>(&shader_list[shader_ID_mapper[shader_kind]].get());
				glUseProgram(shader_simple_ptr->h_ShaderProgram);
				glUniformMatrix4fv(shader_simple_ptr->loc_ModelViewProjectionMatrix, 1, GL_FALSE,
					&ModelViewProjectionMatrix[0][0]);
				glUniform3f(shader_simple_ptr->loc_primitive_color, instances[i].material.diffuse.r,
					instances[i].material.diffuse.g, instances[i].material.diffuse.b);
				break; 
			}
			case PHONG_CONVERT: {
				auto* sh = static_cast<Shader_Phong_Convert*>(
					&shader_list[shader_ID_mapper[PHONG_CONVERT]].get());
				glUseProgram(sh->h_ShaderProgram);
				// 행렬 계산
				glm::mat4 MV = ViewMatrix * instances[i].ModelMatrix;
				glm::mat3 NM = glm::transpose(glm::inverse(glm::mat3(MV)));
				glm::mat4 MVP = ProjectionMatrix * MV;
				glUniformMatrix4fv(sh->loc_ModelViewMatrix, 1, GL_FALSE, &MV[0][0]);
				glUniformMatrix3fv(sh->loc_NormalMatrix, 1, GL_FALSE, &NM[0][0]);
				glUniformMatrix4fv(sh->loc_MVP, 1, GL_FALSE, &MVP[0][0]);
				// material
				auto& m = instances[i].material;
				glUniform3f(sh->loc_Ka, m.ambient.r, m.ambient.g, m.ambient.b);
				glUniform3f(sh->loc_Kd, m.diffuse.r, m.diffuse.g, m.diffuse.b);
				glUniform3f(sh->loc_Ks, m.specular.r, m.specular.g, m.specular.b);
				glUniform1f(sh->loc_Shininess, m.exponent);
				break;
			}
			case GOURAUD_CONVERT: {
				auto * sh = static_cast<Shader_Gouraud_Convert*>(
					&shader_list[shader_ID_mapper[GOURAUD_CONVERT]].get());
				glUseProgram(sh->h_ShaderProgram);
				glm::mat4 MV = ViewMatrix * instances[i].ModelMatrix;
				glm::mat3 NM = glm::transpose(glm::inverse(glm::mat3(MV)));
				glm::mat4 MVP = ProjectionMatrix * MV;
				glUniformMatrix4fv(sh->loc_ModelViewMatrix, 1, GL_FALSE, &MV[0][0]);
				glUniformMatrix3fv(sh->loc_NormalMatrix, 1, GL_FALSE, &NM[0][0]);
				glUniformMatrix4fv(sh->loc_MVP, 1, GL_FALSE, &MVP[0][0]);
				auto & m = instances[i].material;
				glUniform3f(sh->loc_Ka, m.ambient.r, m.ambient.g, m.ambient.b);
				glUniform3f(sh->loc_Kd, m.diffuse.r, m.diffuse.g, m.diffuse.b);
				glUniform3f(sh->loc_Ks, m.specular.r, m.specular.g, m.specular.b);
				glUniform1f(sh->loc_Shininess, m.exponent);
				break;
			}
			case SHADER_PHONG_TEXTURE: {
				auto* sh = static_cast<Shader_Phong_Texture*>(
					&shader_list[shader_ID_mapper[SHADER_PHONG_TEXTURE]].get());
				glUseProgram(sh->h_ShaderProgram);

				// 텍스처 필터링 모드 설정
				set_texture_filtering(texid, current_filter_mode);

				// 1) 전역 앰비언트
				glUniform4f(sh->loc_global_ambient_color, 0.05f, 0.05f, 0.05f, 1.0f);

				// 2) 라이트 파라미터 업로드
				for (int i = 0; i < Shader_Phong_Texture::MAX_LIGHT; ++i) {
					glUniform1i(sh->loc_light_on[i], light[i].light_on);
					glUniform4fv(sh->loc_light_position[i], 1, light[i].position);
					glUniform4fv(sh->loc_light_ambient[i], 1, light[i].ambient_color);
					glUniform4fv(sh->loc_light_diffuse[i], 1, light[i].diffuse_color);
					glUniform4fv(sh->loc_light_specular[i], 1, light[i].specular_color);
					glUniform3f(sh->loc_light_spot_direction[i],
						light[i].spot_direction[0],
						light[i].spot_direction[1],
						light[i].spot_direction[2]);
					glUniform1f(sh->loc_light_spot_cutoff_angle[i], light[i].spot_cutoff_angle);
					glUniform1f(sh->loc_light_spot_exponent[i], light[i].spot_exponent);
					glUniform4fv(sh->loc_light_attenuation[i], 1, light[i].light_attenuation_factors);
				}

				// 3) 텍스처 바인딩 및 플래그…
				if (has_texture) {
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, texid);
					glUniform1i(sh->loc_base_texture, 0);
					glUniform1i(sh->loc_flag_texture_mapping, 1);
				}
				else {
					glUniform1i(sh->loc_flag_texture_mapping, 0);
				}

				// 4) 매트릭스 업로드…
				glm::mat4 MV = ViewMatrix * instances[i].ModelMatrix;
				glm::mat4 MVP = ProjectionMatrix * MV;
				glm::mat3 NM = glm::inverseTranspose(glm::mat3(MV));
				glUniformMatrix4fv(sh->loc_MVP, 1, GL_FALSE, &MVP[0][0]);
				glUniformMatrix4fv(sh->loc_ModelViewMatrix, 1, GL_FALSE, &MV[0][0]);
				glUniformMatrix3fv(sh->loc_ModelViewMatrixInvTrans, 1, GL_FALSE, &NM[0][0]);

				// 5) 드로우, 언바인드…
				glBindVertexArray(VAO);
				glDrawArrays(GL_TRIANGLES, 0, n_triangles * 3);
				glBindVertexArray(0);
				glUseProgram(0);
				break;
			}
		}
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3 * n_triangles);
		glBindVertexArray(0);
		glUseProgram(0);
	}
}
