#pragma once
#include <raylib.h>
#include "player.h"
struct Cam {
	Camera2D cam;
	float speed;
	short last_direction = 1; // 1 = right, -1 = left
	void Init();
	void Update(Player& pl, float cam_sp);
};
