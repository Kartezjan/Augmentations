--function default_weapon_table(entries)
--	local default_component = gun_component()
--	
--	local default_table = {}
--	for k, v in pairs(class_info(default_component).attributes) do default_table[v] = default_component[v] end
--	
--	local new_table = {}
--	recursive_write(new_table, default_table)
--	
--	print ("another compo \n")
--	for k, v in pairs(archetyped(new_table, entries)) do if type(v) ~= "userdata" then print(k, v) end end
--	return archetyped(new_table, entries)
--end

world_item = {
	render = {
		model = nil,
		layer = render_layers.ON_GROUND
	},
	
	transform = {},
	
	physics = {
		body_type = Box2D.b2_dynamicBody,
		
		body_info = {
			filter = filter_items,
			shape_type = physics_info.RECT,
			--rect_size = blank_red.size,
			
			linear_damping = 4,
			angular_damping = 4,
			fixed_rotation = false,
			density = 0.1,
			friction = 0,
			restitution = 0.4,
			sensor = false
		}
	}
}



assault_rifle_sprite = create_sprite {
	image = images.assault
}

shotgun_sprite = create_sprite {
	image = images.shotgun
}

fireaxe_sprite = create_sprite {
	image = images.fireaxe
}

shotgun_wielded_sprite = create_sprite {
	image = images.shotgun_wielded
}

assault_wielded_sprite = create_sprite {
	image = images.assault_wielded
}

wielded_item_archetype = {
	render = {
		model = nil,
		layer = render_layers.WIELDED
	},
	
	transform = {},
	chase = {
		chase_rotation = true,
		relative = false,
		chase_type = chase_component.ORBIT
	}
}

bare_hands = {
	weapon_info = create_gun({
		bullet_damage = minmax(110, 140),
		swing_interval_ms = 400,
		swing_duration = 150,
		current_rounds = 0,
		
		bullet_body = {
			filter = filter_bullets,
			shape_type = physics_info.RECT,
			rect_size = bullet_sprite.size,
			fixed_rotation = true,
			density = 0.1
		},
		
		swing_radius = 80,
		
		swing_angle = 90,
		
		swing_angular_offset = 0,
		target_camera_to_shake = world_camera,
		
		shake_radius = 25.5,
		shake_spread_degrees = 45
		--query_vertices = 7
	}),
	
	animation_index = "BARE_HANDS",
	
	wielded_entity = wielded_item_archetype
}

assault_rifle_info = {
	bullets_once = 1,
	bullet_distance_offset = vec2(70, 10),
	bullet_damage = minmax(80, 110),
	bullet_speed = minmax(6000, 7000),
	bullet_render = { model = bullet_sprite, layer = render_layers.BULLETS },
	is_automatic = true,
	max_rounds = 30,
	shooting_interval_ms = 80,
	spread_degrees = 0.5,
	shake_radius = 9.5,
	shake_spread_degrees = 45,
	
	bullet_body = {
		filter = filter_bullets,
		shape_type = physics_info.RECT,
		rect_size = bullet_sprite.size,
		fixed_rotation = true,
		density = 0.1
	},
	
	max_bullet_distance = 5000,
	current_rounds = 30,
	
	swing_radius = 100,
	swing_angle = 90,
	swing_angular_offset = 0,
	swing_duration = 300,
	swing_interval_ms = 600,
	target_camera_to_shake = world_camera 
}

assault_rifle = {
	weapon_info = create_gun (assault_rifle_info),
	
	animation_index = "ASSAULT_RIFLE",
	
	world_orbit_offset = vec2(45, -7),
	
	item_entity = archetyped(world_item, { 
		render = { 
			model = assault_rifle_sprite
		}
	}),
	
	wielded_entity = archetyped(wielded_item_archetype, {
		render = { 
			model = assault_wielded_sprite,
			layer = render_layers.WIELDED_GUNS
			--model = assault_rifle_topdown_sprite
		}
	}),

	bullet_shell_sprite = assault_shell_sprite,
	
	shell_offset = vec2(50, 0)
}

shotgun = {
	weapon_info = create_gun(archetyped(assault_rifle_info, {
		bullets_once = 12,
		bullet_damage = minmax(80, 100),
		bullet_speed = minmax(4000, 8000),
		is_automatic = false,
		shooting_interval_ms = 700,
		spread_degrees = 8,
		
		current_rounds = 6,
		max_rounds = 2,
		
		shake_radius = 15.5
	})),
	
	animation_index = "SHOTGUN",
	world_orbit_offset = vec2(50, -8),
	
	item_entity = archetyped(world_item, { 
		render = { 
			model = shotgun_sprite
		}
	}),
	
	wielded_entity = archetyped(wielded_item_archetype, {
		render = { 
			--model = nil
			model = shotgun_wielded_sprite,
			layer = render_layers.WIELDED_GUNS
		}
	}),

	bullet_shell_sprite = shotgun_shell_sprite,
	
	shell_offset = vec2(35, 0)
}

fireaxe = {
	weapon_info = create_gun({
	bullet_damage = minmax(110, 140),
	swing_interval_ms = 500,
	swing_duration = 300,
	current_rounds = 0,
	
	bullet_body = {
		filter = filter_bullets,
		shape_type = physics_info.RECT,
		rect_size = bullet_sprite.size,
		fixed_rotation = true,
		density = 0.1
	},
	
	swing_radius = 100,
	 
	swing_angle = 90,
	 
	swing_angular_offset = 0,
	target_camera_to_shake = world_camera,
	
	shake_radius = 25.5,
	shake_spread_degrees = 45
	--query_vertices = 7
	}),
	
	animation_index = "FIREAXE",
	world_orbit_offset = vec2(0, 0),
	
	item_entity = archetyped(world_item, { 
		render = { 
			model = fireaxe_sprite
		}
	}),
	
	wielded_entity = archetyped(wielded_item_archetype, {
		render = { 
			model = fireaxe_sprite
		}
	})	
}