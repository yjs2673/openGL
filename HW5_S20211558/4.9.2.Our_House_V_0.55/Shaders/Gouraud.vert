#version 330 core

layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 VertexNormal;

uniform mat4 ModelViewMatrix;
uniform mat4 MVP;
uniform mat3 NormalMatrix;

uniform vec4 Light_Position;
uniform vec3 Light_La, Light_L;
uniform vec3 Material_Ka, Material_Kd, Material_Ks;
uniform float Material_Shininess;

out vec3 Color;

void main() {
	vec3 n = normalize(NormalMatrix * VertexNormal);
	vec3 position_eye = vec3(ModelViewMatrix * vec4(VertexPosition, 1.0));
	vec3 s = normalize(vec3(Light_Position) - position_eye);
	vec3 v = normalize(-position_eye);
	vec3 h = normalize(s + v);

	vec3 ambient = Light_La * Material_Ka;
	vec3 diffuse = Material_Kd * max(dot(s, n), 0.0);
	vec3 specular = vec3(0.0);
	if(dot(s, n) > 0.0)
		specular = Material_Ks * pow(max(dot(n, h), 0.0), Material_Shininess);

	Color = ambient + Light_L * (diffuse + specular);
	gl_Position = MVP * vec4(VertexPosition, 1.0);
}
