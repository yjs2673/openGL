#define _CRT_SECURE_NO_WARNINGS

#include "Scene_Definitions.h"
#include <glm/gtc/matrix_inverse.hpp>

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
		cur_material->emission = glm::vec4(0.0f, 0.0f, 0.0f, current_alpha);
		cur_material->ambient = glm::vec4(0.329412f, 0.223529f, 0.027451f, current_alpha);
		cur_material->diffuse = glm::vec4(0.780392f, 0.568627f, 0.113725f, current_alpha);
		cur_material->specular = glm::vec4(0.992157f, 0.941176f, 0.807843f, current_alpha);
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
			strcpy(object_frames[i].filename, "Data/cow_vnt.geom");
			object_frames[i].n_fields = 8;
			object_frames[i].front_face_mode = GL_CCW;
			object_frames[i].prepare_geom_of_static_object();

			/*****/
			has_texture = true;
			glGenTextures(1, &texid);
			glBindTexture(GL_TEXTURE_2D, texid);
			// wrapping/filter
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			// load "Data/cow.png"
			FREE_IMAGE_FORMAT fif = FreeImage_GetFileType("Data/cow.png", 0);
			FIBITMAP* bmp = FreeImage_Load(fif, "Data/cow.png");
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
				fprintf(stderr, "Failed to load Data/cow.png\n");
			}
			glBindTexture(GL_TEXTURE_2D, 0);
			/*****/

			object_frames[i].instances.emplace_back();
			cur_MM = &(object_frames[i].instances.back().ModelMatrix);
			*cur_MM = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
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
		/*float x, y;
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
		ModelMatrix = glm::rotate(ModelMatrix, angle * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));*/
		rotation_angle = ((time_stamp * 2) % 360) * TO_RADIAN;
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 0.0f, 0.0f));  // 회전 중심으로 이동
		ModelMatrix = glm::rotate(ModelMatrix, rotation_angle, glm::vec3(0.0f, 0.0f, 1.0f));  // 회전
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 0.0f, 0.0f));  // 원위치로 되돌리기
		break;
	case DYNAMIC_OBJECT_COW_2:
		rotation_angle = ((time_stamp * 2) % 360) * TO_RADIAN;
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(195.0f, 65.0f, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, -1 * rotation_angle, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(10.0f, 0.0f, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, 90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
		break;
	}

	for (int i = 0; i < cur_object.instances.size(); i++) {
		glm::mat4 ModelViewProjectionMatrix = ProjectionMatrix * ViewMatrix * ModelMatrix * cur_object.instances[i].ModelMatrix;

		SHADER_ID sk = shader_kind;

		if (object_id == DYNAMIC_OBJECT_COW_2) shader_kind = SHADER_PHONG_TEXTURE;
		else shader_kind = SHADER_SIMPLE;

		/*****/
		if (object_id == DYNAMIC_OBJECT_TIGER && can_change) {
			cur_object.instances[i].material.emission[3] = current_alpha;
			cur_object.instances[i].material.ambient[3] = current_alpha;
			cur_object.instances[i].material.diffuse[3] = current_alpha;
			cur_object.instances[i].material.specular[3] = current_alpha;
			can_change = false;
		}
		/*****/

		switch (shader_kind) {
		case SHADER_SIMPLE: {
			Shader_Simple* shader_simple_ptr = static_cast<Shader_Simple*>(&shader_list[shader_ID_mapper[shader_kind]].get());
			glUseProgram(shader_simple_ptr->h_ShaderProgram);
			glUniformMatrix4fv(shader_simple_ptr->loc_ModelViewProjectionMatrix, 1, GL_FALSE,
				&ModelViewProjectionMatrix[0][0]);
			glUniform3f(shader_simple_ptr->loc_primitive_color, cur_object.instances[i].material.diffuse.r,
				cur_object.instances[i].material.diffuse.g, cur_object.instances[i].material.diffuse.b);
			break;
		}
		case SHADER_PHONG_TEXTURE: {
			auto* sh = static_cast<Shader_Phong_Texture*>(
				&shader_list[shader_ID_mapper[SHADER_PHONG_TEXTURE]].get());
			glUseProgram(sh->h_ShaderProgram);

			// 텍스처 필터링 모드 설정
			set_texture_filtering(texid, current_filter_mode);

			// 전역 앰비언트
			glUniform4f(sh->loc_global_ambient_color, 0.05f, 0.05f, 0.05f, 1.0f);

			// 라이트 파라미터 업로드
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

			// 텍스처 바인딩 및 플래그
			if (has_texture) {
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, texid);
				glUniform1i(sh->loc_base_texture, 0);
				glUniform1i(sh->loc_flag_texture_mapping, 1);
			}
			else {
				glUniform1i(sh->loc_flag_texture_mapping, 0);
			}

			// 매트릭스 업로드
			glm::mat4 MV = ViewMatrix * ModelMatrix;
			glm::mat4 MVP = ProjectionMatrix * MV;
			glm::mat3 NM = glm::inverseTranspose(glm::mat3(MV));
			glUniformMatrix4fv(sh->loc_MVP, 1, GL_FALSE, &MVP[0][0]);
			glUniformMatrix4fv(sh->loc_ModelViewMatrix, 1, GL_FALSE, &MV[0][0]);
			glUniformMatrix3fv(sh->loc_ModelViewMatrixInvTrans, 1, GL_FALSE, &NM[0][0]);
			break;
		}
		}
		glBindVertexArray(cur_object.VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3 * cur_object.n_triangles);
		glBindVertexArray(0);
		glUseProgram(0);
	}
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}