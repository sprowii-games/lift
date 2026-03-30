#include "player.h"

void Player::Update() {
	auto dt = GetFrameTime();
	is_running = false;

	direction = 0;

	if (IsKeyDown(KEY_D)) {

		direction = 1;

	}
	if (IsKeyDown(KEY_A)) {

		direction = -1;

	}
	if (IsKeyDown(KEY_LEFT_SHIFT) && direction != 0 && can_run) {

		is_running = true;

		pos.x += run_speed * direction * dt;

	}
	else if (direction != 0) {

		pos.x += base_speed * direction * dt;
	}
}
bool Player::is_in_area(float& x)
{
	if (x-15 < pos.x && pos.x < x+45) {// типа, считая, что x считается от левого верхнего края
		return true;
	}
	else {
		return false;
	}
}

void Player::Draw() const {
    DrawCircle(pos.x, pos.y, 10, GREEN);
}