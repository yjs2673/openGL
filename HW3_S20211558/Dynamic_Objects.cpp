#define _CRT_SECURE_NO_WARNINGS

#include "Scene_Definitions.h"

void Tiger_D::define_object() {
#define N_TIGER_FRAMES 12
	glm::mat4* cur_MM;
	Material* cur_material;
	flag_valid = true;

	for (int i = 0; i < N_TIGER_FRAMES; i++) {
		object_frames.emplace_back();
		sprintf(object_frames[i].filename, "Data/Tiger_%d%d_triangles_vnt.geom", i / 10, i % 10);
		object_frames[i].n_fields = 8;
		object_frames[i].front_face_mode = GL_CW;
		object_frames[i].prepare_geom_of_static_object();

		object_frames[i].instances.emplace_back();
		cur_MM = &(object_frames[i].instances.back().ModelMatrix);
		*cur_MM = glm::scale(glm::mat4(1.0f), glm::vec3(0.2f, 0.2f, 0.2f));
		cur_material = &(object_frames[i].instances.back().material);
		cur_material->emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		cur_material->ambient = glm::vec4(0.329412f, 0.223529f, 0.027451f, 1.0f);
		cur_material->diffuse = glm::vec4(0.780392f, 0.568627f, 0.113725f, 1.0f);
		cur_material->specular = glm::vec4(0.992157f, 0.941176f, 0.807843f, 1.0f);
		cur_material->exponent = 128.0f * 0.21794872f;
	}
}

void Cow_D::define_object() {
#define N_FRAMES_COW_1 1
#define N_FRAMES_COW_2 1
	glm::mat4* cur_MM;
	Material* cur_material;
	flag_valid = true;
	switch (object_id) {

		int n_frames;
	case DYNAMIC_OBJECT_COW_1:
		n_frames = N_FRAMES_COW_1;
		for (int i = 0; i < n_frames; i++) {
			object_frames.emplace_back();
			strcpy(object_frames[i].filename, "Data/cow_vn.geom");
			object_frames[i].n_fields = 6;
			object_frames[i].front_face_mode = GL_CCW;
			object_frames[i].prepare_geom_of_static_object();
			object_frames[i].instances.emplace_back();
			cur_MM = &(object_frames[i].instances.back().ModelMatrix);
			*cur_MM = glm::scale(glm::mat4(1.0f), glm::vec3(30.0f, 30.0f, 30.0f));
			cur_material = &(object_frames[i].instances.back().material);
			cur_material->emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			cur_material->ambient = glm::vec4(0.329412f, 0.223529f, 0.027451f, 1.0f);
			cur_material->diffuse = glm::vec4(0.780392f, 0.568627f, 0.113725f, 1.0f);
			cur_material->specular = glm::vec4(0.992157f, 0.941176f, 0.807843f, 1.0f);
			cur_material->exponent = 128.0f * 0.21794872f;
		}
		break;
	case DYNAMIC_OBJECT_COW_2:
		n_frames = N_FRAMES_COW_2;
		for (int i = 0; i < n_frames; i++) {
			object_frames.emplace_back();
			strcpy(object_frames[i].filename, "Data/cow_vn.geom");
			object_frames[i].n_fields = 6;
			object_frames[i].front_face_mode = GL_CCW;
			object_frames[i].prepare_geom_of_static_object();

			object_frames[i].instances.emplace_back();
			cur_MM = &(object_frames[i].instances.back().ModelMatrix);
			*cur_MM = glm::scale(glm::mat4(1.0f), glm::vec3(20.0f, 20.0f, 20.0f));
			cur_material = &(object_frames[i].instances.back().material);
			glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			cur_material->ambient = glm::vec4(0.25f, 0.25f, 0.25f, 1.0f);
			cur_material->diffuse = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f);
			cur_material->specular = glm::vec4(0.774597f, 0.774597f, 0.774597f, 1.0f);
			cur_material->exponent = 128.0f * 0.6f;
		}
		break;
	}
}

void Dynamic_Object::draw_object(glm::mat4& ViewMatrix, glm::mat4& ProjectionMatrix, SHADER_ID shader_kind,
	std::vector<std::reference_wrapper<Shader>>& shader_list, int time_stamp) {
	int cur_object_index = time_stamp % object_frames.size();
	Static_Object& cur_object = object_frames[cur_object_index];
	glFrontFace(cur_object.front_face_mode);

	float rotation_angle = 0.0f;
	glm::mat4 ModelMatrix = glm::mat4(1.0f);
	float perimeter = 2 * (220.0f + 150.0f);
	float t = ((time_stamp % (int)perimeter) * 2) % 820;

	switch (object_id) {
	case DYNAMIC_OBJECT_TIGER:
		/*rotation_angle = (time_stamp % 360) * TO_RADIAN;
		ModelMatrix = glm::rotate(ModelMatrix, -rotation_angle, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(100.0f, 0.0f, 0.0f));*/ // 원형으로 이동
		float x, y;
		float angle;

		if (t < 240) {
			x = 0.0f + t;
			y = 0.0f;
			angle = 90.0f;
		}
		else if (t < 240 + 170) {
			x = 240.0f;
			y = -10.0f + (t - 240);
			angle = 180.0f;
		}
		else if (t < 240 + 170 + 240) {
			x = 230.0f - (t - 240 - 170);
			y = 170.0f;
			angle = 270.0f;
		}
		else { // total cycle time : 480 + 340 = 820
			x = 0.0f;
			y = 160.0f - (t - 240 - 170 - 240);
			angle = 360.0f;
		}
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, angle * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		break;
	case DYNAMIC_OBJECT_COW_2:
		rotation_angle = ((time_stamp * 2) % 360) * TO_RADIAN;
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(195.0f, 65.0f, 10.0f));
		ModelMatrix = glm::rotate(ModelMatrix, rotation_angle, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, -10.0f, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, 90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
		break;
	}

	for (int i = 0; i < cur_object.instances.size(); i++) {
		glm::mat4 ModelViewProjectionMatrix = ProjectionMatrix * ViewMatrix * ModelMatrix * cur_object.instances[i].ModelMatrix;
		switch (shader_kind) {
		case SHADER_SIMPLE:
			Shader_Simple* shader_simple_ptr = static_cast<Shader_Simple*>(&shader_list[shader_ID_mapper[shader_kind]].get());
			glUseProgram(shader_simple_ptr->h_ShaderProgram);
			glUniformMatrix4fv(shader_simple_ptr->loc_ModelViewProjectionMatrix, 1, GL_FALSE,
				&ModelViewProjectionMatrix[0][0]);
			glUniform3f(shader_simple_ptr->loc_primitive_color, cur_object.instances[i].material.diffuse.r,
				cur_object.instances[i].material.diffuse.g, cur_object.instances[i].material.diffuse.b);
			break;
		}
		glBindVertexArray(cur_object.VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3 * cur_object.n_triangles);
		glBindVertexArray(0);
		glUseProgram(0);
	}
}