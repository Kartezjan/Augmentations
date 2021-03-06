#pragma once

//#include "asset.h"
//
namespace augs {
	class texture;
}

#include "math/vec2.h"

namespace assets {
	enum texture_id {
		BLANK,

		CRATE,
		CAR_INSIDE,
		CAR_FRONT,

		TRUCK_INSIDE,
		TRUCK_FRONT,

		MOTORCYCLE_FRONT,
		MOTORCYCLE_INSIDE,

		TEST_CROSSHAIR,
		TEST_PLAYER,

		TORSO_MOVING_FIRST,
		TORSO_MOVING_LAST = TORSO_MOVING_FIRST + 5,

		ASSAULT_RIFLE,
		SHOTGUN,

		TEST_BACKGROUND,
		TEST_BACKGROUND2,
		TEST_SPRITE,
		MOTOR,

		SAMPLE_MAGAZINE,
		PINK_CHARGE,
		BACKPACK,
		RIGHT_HAND_ICON,
		LEFT_HAND_ICON,

		GUI_CURSOR,
		ATTACHMENT_CIRCLE_FILLED,
		ATTACHMENT_CIRCLE_BORDER,
		PRIMARY_HAND_ICON,
		SECONDARY_HAND_ICON,
		SHOULDER_SLOT_ICON,
		ARMOR_SLOT_ICON,

		CUSTOM = 10000
	};
	
	vec2i get_size(texture_id);
}

augs::texture& operator*(const assets::texture_id& id);
bool operator!(const assets::texture_id& id);