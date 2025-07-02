#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>

enum Camera_Projection_TYPE {
	CAMERA_PROJECTION_PERSPECTIVE = 0, CAMERA_PROJECTION_ORTHOGRAPHIC
};

enum Camera_ID {
	CAMERA_MAIN = 0, CAMERA_CC_0, CAMERA_CC_1, CAMERA_CC_2,
	CAMERA_CC_MOVE, ORTH_FRONT, ORTH_SIDE, ORTH_TOP
};
	

struct Camera_View {
	glm::vec3 pos;
	glm::vec3 uaxis, vaxis, naxis;
	/*****/
	float fovy, aspect_ratio, near_c, far_c;
	int move;
	/*****/
};
 
struct Camera_Projection {
	Camera_Projection_TYPE projection_type;
	union {
		struct {
			float fovy, aspect, n, f;
		} pers;
		struct {
			float left, right, bottom, top, n, f;
		} ortho;
	} params;
};

struct View_Port {
	float x, y, w, h;
};

struct Camera {
	Camera_ID camera_id;
	Camera_View cam_view;
	glm::mat4 ViewMatrix;
	Camera_Projection cam_proj;
	glm::mat4 ProjectionMatrix;
	View_Port view_port;

	Camera(Camera_ID _camera_id) : camera_id(_camera_id) {}
	bool flag_valid;
	bool flag_move; // what else?

	virtual void toggleUp(int toggle) {};
	virtual ~Camera() {};
};

struct Perspective_Camera : public Camera {
	Perspective_Camera(Camera_ID _camera_id) : Camera(_camera_id) {}
	void define_camera(int win_width, int win_height, float win_aspect_ratio);

	void zoom(float deltaFovy); /*****/
	void moveLocal(const glm::vec3& delta);    // eye += delta.x*right + delta.y*up + delta.z*forward
	void rotateLocal(float pitch, float yaw, float roll); // 로컬 축 회전
	void rotateWorld(float pitch, float yaw, float roll); // 월드 축 회전
	void toggleUp(int toggle);
};

struct Orthographic_Camera : public Camera {
	Orthographic_Camera(Camera_ID _camera_id) : Camera(_camera_id) {}
	void define_camera(int win_width, int win_height, float win_aspect_ratio);
};

struct Camera_Data {
	Perspective_Camera cam_main { CAMERA_MAIN };
	Perspective_Camera cam_cc_0 { CAMERA_CC_0 };
	Perspective_Camera cam_cc_1 { CAMERA_CC_1 };
	Perspective_Camera cam_cc_2 { CAMERA_CC_2 };
	Perspective_Camera cam_cc_mv{ CAMERA_CC_MOVE };

	Orthographic_Camera orth_front { ORTH_FRONT };
	Orthographic_Camera orth_side { ORTH_SIDE };
	Orthographic_Camera orth_top { ORTH_TOP };
};
