#include "Scene_Definitions.h"

void Shader_Simple::prepare_shader() {
	shader_info[0] = { GL_VERTEX_SHADER, "Shaders/simple.vert" };
	shader_info[1] = { GL_FRAGMENT_SHADER, "Shaders/simple.frag" };
	shader_info[2] = { GL_NONE, NULL };

	h_ShaderProgram = LoadShaders(shader_info);
	glUseProgram(h_ShaderProgram);

	loc_ModelViewProjectionMatrix = glGetUniformLocation(h_ShaderProgram, "u_ModelViewProjectionMatrix");
	loc_primitive_color = glGetUniformLocation(h_ShaderProgram, "u_primitive_color");
	glUseProgram(0);
}

void Shader_Phong_Convert::prepare_shader() {
    shader_info[0] = { GL_VERTEX_SHADER,   "Shaders/Phong.vert" };
    shader_info[1] = { GL_FRAGMENT_SHADER, "Shaders/Phong.frag" };
    shader_info[2] = { GL_NONE, NULL };
    h_ShaderProgram = LoadShaders(shader_info);

    glUseProgram(h_ShaderProgram);
    loc_ModelViewMatrix = glGetUniformLocation(h_ShaderProgram, "ModelViewMatrix");
    loc_NormalMatrix = glGetUniformLocation(h_ShaderProgram, "NormalMatrix");
    loc_MVP = glGetUniformLocation(h_ShaderProgram, "MVP");
    loc_Ka = glGetUniformLocation(h_ShaderProgram, "Material.Ka");
    loc_Kd = glGetUniformLocation(h_ShaderProgram, "Material.Kd");
    loc_Ks = glGetUniformLocation(h_ShaderProgram, "Material.Ks");
    loc_Shininess = glGetUniformLocation(h_ShaderProgram, "Material.Shininess");

    // 간단한 방향광 설정 (eye coords)
    GLint loc_Lpos = glGetUniformLocation(h_ShaderProgram, "Light.Position");
    GLint loc_La = glGetUniformLocation(h_ShaderProgram, "Light.La");
    GLint loc_L = glGetUniformLocation(h_ShaderProgram, "Light.L");
    glUniform4f(loc_Lpos, 0.0f, 0.0f, 1.0f, 0.0f);
    glUniform3f(loc_La, 0.2f, 0.2f, 0.2f);
    glUniform3f(loc_L, 0.8f, 0.8f, 0.8f);

    glUseProgram(0);
}

void Shader_Gouraud_Convert::prepare_shader() {
    shader_info[0] = { GL_VERTEX_SHADER,   "Shaders/Gouraud.vert" };
    shader_info[1] = { GL_FRAGMENT_SHADER, "Shaders/Gouraud.frag" };
    shader_info[2] = { GL_NONE, NULL };
    h_ShaderProgram = LoadShaders(shader_info);

    glUseProgram(h_ShaderProgram);
    loc_ModelViewMatrix = glGetUniformLocation(h_ShaderProgram, "ModelViewMatrix");
    loc_NormalMatrix = glGetUniformLocation(h_ShaderProgram, "NormalMatrix");
    loc_MVP = glGetUniformLocation(h_ShaderProgram, "MVP");
    loc_Ka = glGetUniformLocation(h_ShaderProgram, "Material_Ka");
    loc_Kd = glGetUniformLocation(h_ShaderProgram, "Material_Kd");
    loc_Ks = glGetUniformLocation(h_ShaderProgram, "Material_Ks");
    loc_Shininess = glGetUniformLocation(h_ShaderProgram, "Material_Shininess");

    // 동일한 방향광 정보
    GLint loc_Lpos = glGetUniformLocation(h_ShaderProgram, "Light_Position");
    GLint loc_La = glGetUniformLocation(h_ShaderProgram, "Light_La");
    GLint loc_L = glGetUniformLocation(h_ShaderProgram, "Light_L");
    glUniform4f(loc_Lpos, 0.0f, 0.0f, 1.0f, 0.0f);
    glUniform3f(loc_La, 0.2f, 0.2f, 0.2f);
    glUniform3f(loc_L, 0.8f, 0.8f, 0.8f);

    glUseProgram(0);
}

// Shaders.cpp

void Shader_Phong_Texture::prepare_shader() {
    shader_info[0] = { GL_VERTEX_SHADER,   "Shaders/Texture.vert" };
    shader_info[1] = { GL_FRAGMENT_SHADER, "Shaders/Texture.frag" };
    shader_info[2] = { GL_NONE, NULL };
    h_ShaderProgram = LoadShaders(shader_info);

    glUseProgram(h_ShaderProgram);
    loc_global_ambient_color = glGetUniformLocation(h_ShaderProgram, "u_global_ambient_color");
    loc_flag_texture_mapping = glGetUniformLocation(h_ShaderProgram, "u_flag_texture_mapping");
    loc_base_texture = glGetUniformLocation(h_ShaderProgram, "u_base_texture");
    // 매트릭스 uniform
    loc_ModelViewMatrix = glGetUniformLocation(h_ShaderProgram, "u_ModelViewMatrix");
    loc_ModelViewMatrixInvTrans = glGetUniformLocation(h_ShaderProgram, "u_ModelViewMatrixInvTrans");
    loc_MVP = glGetUniformLocation(h_ShaderProgram, "u_ModelViewProjectionMatrix");

    // 라이트 배열 유니폼 위치 얻기
    for (int i = 0; i < MAX_LIGHT; ++i) {
        char buf[64];
        sprintf_s(buf, sizeof(buf), "u_light[%d].light_on", i);
        loc_light_on[i] = glGetUniformLocation(h_ShaderProgram, buf);

        sprintf_s(buf, sizeof(buf), "u_light[%d].position", i);
        loc_light_position[i] = glGetUniformLocation(h_ShaderProgram, buf);

        sprintf_s(buf, sizeof(buf), "u_light[%d].ambient_color", i);
        loc_light_ambient[i] = glGetUniformLocation(h_ShaderProgram, buf);

        sprintf_s(buf, sizeof(buf), "u_light[%d].diffuse_color", i);
        loc_light_diffuse[i] = glGetUniformLocation(h_ShaderProgram, buf);

        sprintf_s(buf, sizeof(buf), "u_light[%d].specular_color", i);
        loc_light_specular[i] = glGetUniformLocation(h_ShaderProgram, buf);

        sprintf_s(buf, sizeof(buf), "u_light[%d].spot_direction", i);
        loc_light_spot_direction[i] = glGetUniformLocation(h_ShaderProgram, buf);

        sprintf_s(buf, sizeof(buf), "u_light[%d].spot_cutoff_angle", i);
        loc_light_spot_cutoff_angle[i] = glGetUniformLocation(h_ShaderProgram, buf);

        sprintf_s(buf, sizeof(buf), "u_light[%d].spot_exponent", i);
        loc_light_spot_exponent[i] = glGetUniformLocation(h_ShaderProgram, buf);

        sprintf_s(buf, sizeof(buf), "u_light[%d].light_attenuation_factors", i);
        loc_light_attenuation[i] = glGetUniformLocation(h_ShaderProgram, buf);
    }
}
