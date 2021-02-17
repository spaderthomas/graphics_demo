bool are_colliding(Collider a, Collider b, glm::vec2& penetration) {
	// First, calculate the Minkowski difference
	Center_Box minkowski;
	minkowski.extents = a.extents + b.extents;
	float a_left = a.origin.x + a.offset.x - .5f * a.extents.x;
	float b_right = b.origin.x + b.offset.x + .5f * b.extents.x;
	minkowski.origin.x = a_left - b_right + .5f * minkowski.extents.x;
	float a_top = a.origin.y + a.offset.y + .5f * a.extents.y;
	float b_bottom = b.origin.y + b.offset.y - .5f * b.extents.y;
	minkowski.origin.y = a_top - b_bottom - .5f * minkowski.extents.y;

	// If the Minkowski difference intersects the origin, there's a collision
	auto verts = minkowski.as_points();
	if (verts.right >= 0 && verts.left <= 0 && verts.top >= 0 && verts.bottom <= 0) {
		// The pen vector is the shortest vector from the origin of the MD to an edge.
		// You know this has to be a vertical or horizontal line from the origin (these are by def. the shortest)
		float min = 100000.f;
		if (abs(verts.left) < min) {
			min = abs(verts.left);
			penetration = glm::vec2(verts.left, 0.f);
		}
		if (abs(verts.right) < min) {
			min = abs(verts.right);
			penetration = glm::vec2(verts.right, 0.f);
		}
		if (abs(verts.top) < min) {
			min = abs(verts.top);
			penetration = glm::vec2(0.f, verts.top);
		}
		if (abs(verts.bottom) < min) {
			min = abs(verts.bottom);
			penetration = glm::vec2(0.f, verts.bottom);
		}

		return true;
	}

	penetration = glm::vec2(0.f);
	return false;
}

bool are_boxes_colliding(Center_Box a, Center_Box b, glm::vec2& penetration) {
	// First, calculate the Minkowski difference
	Center_Box minkowski;
	minkowski.extents = a.extents + b.extents;
	float a_left = a.origin.x - .5f * a.extents.x;
	float b_right = b.origin.x + .5f * b.extents.x;
	minkowski.origin.x = a_left - b_right + .5f * minkowski.extents.x;
	float a_top = a.origin.y + .5f * a.extents.y;
	float b_bottom = b.origin.y - .5f * b.extents.y;
	minkowski.origin.y = a_top - b_bottom - .5f * minkowski.extents.y;

	// If the Minkowski difference intersects the origin, there's a collision
	auto verts = minkowski.as_points();
	if (verts.right >= 0 && verts.left <= 0 && verts.top >= 0 && verts.bottom <= 0) {
		// The pen vector is the shortest vector from the origin of the MD to an edge.
		// You know this has to be a vertical or horizontal line from the origin (these are by def. the shortest)
		float min = 100000.f;
		if (abs(verts.left) < min) {
			min = abs(verts.left);
			penetration = glm::vec2(verts.left, 0.f);
		}
		if (abs(verts.right) < min) {
			min = abs(verts.right);
			penetration = glm::vec2(verts.right, 0.f);
		}
		if (abs(verts.top) < min) {
			min = abs(verts.top);
			penetration = glm::vec2(0.f, verts.top);
		}
		if (abs(verts.bottom) < min) {
			min = abs(verts.bottom);
			penetration = glm::vec2(0.f, verts.bottom);
		}

		return true;
	}

	penetration = glm::vec2(0.f);
	return false;
}
bool are_boxes_colliding(Points_Box a, Points_Box b, glm::vec2& penetration) {
	//@hack just write this
	return are_boxes_colliding(a.as_center_box(), b.as_center_box(), penetration);
}

Points_Box Center_Box::as_points() {
	float left = origin.x - extents.x / 2;
	float right = origin.x + extents.x / 2;
	float top = origin.y + extents.y / 2;
	float bottom = origin.y - extents.y / 2;

	return { top, bottom, left, right };
}
Center_Box Center_Box::from_points(Points_Box& points) {
	Center_Box box;
	box.origin = glm::vec2((points.left + points.right) / 2, (points.top + points.bottom) / 2);
	box.extents = glm::vec2(points.right - points.left, points.top - points.bottom);
	return box;
}

void Points_Box::convert_screen_to_gl() {
	left = gl_from_screen(left);
	right = gl_from_screen(right);
	top = gl_from_screen(top);
	bottom = gl_from_screen(bottom);
}
Center_Box Points_Box::as_center_box() {
	Center_Box center_box;
	center_box.origin = {
		(left + right) / 2.f,
		(top + bottom) / 2.f
	};
	center_box.extents = {
		(right - left),
		(top - bottom)
	};

	return center_box;
}

bool point_inside_box(glm::vec2& screen_pos, Center_Box& box) {
	Points_Box points = box.as_points();
	if (screen_pos.x > points.left && screen_pos.x < points.right
		&& screen_pos.y > points.bottom && screen_pos.y < points.top) {
		return true;
	}

	return false;
}
bool point_inside_collider(float x, float y, const Collider& collider) {
	float xl = collider.origin.x + collider.offset.x + (collider.extents.x / 2);
	float xs = collider.origin.x + collider.offset.x - (collider.extents.x / 2);
	bool inside_x = (xs < x) && (x < xl);

	float yl = collider.origin.y + collider.offset.y + (collider.extents.y / 2);
	float ys = collider.origin.y + collider.offset.y - (collider.extents.y / 2);
	bool inside_y = (ys < y) && (y < yl);

	return inside_x && inside_y;
}

void PhysicsEngine::update(float dt) {
	collisions.clear();
	
	for (auto& request : requests) {
		auto collider = get_collider(request.entity);
		if (!collider) continue;

		// @hack 2020/10/04: Normally, wish describes an offset from where you currently are.
		// To make teleport_entity() not be complicated, when we don't care about collision we treat it as a position, not an offset
		if (request.flags & MoveFlags::BypassCollision) {
			collider->origin = request.wish;
			continue;
		}

		collider->origin += request.wish;

		for (auto& [id, other] : colliders) {
			if (id == request.entity) continue;
			
			glm::vec2 penetration;
			if (are_colliding(*collider, other, penetration)) {
				collider->origin -= penetration;

				CollisionInfo info;
				info.entity = request.entity;
				info.other = id;
				collisions.push_back(info);
			}
		}
	}

	for (auto request : requests) {
		auto collider = get_collider(request.entity);
		if (!collider) continue;

		// Update the positions in Lua
		auto position = Lua.get_component(request.entity, "Position");
		position["world"]["x"] = collider->origin.x;
		position["world"]["y"] = collider->origin.y;
	}

	requests.clear();
}

void PhysicsEngine::add_collider(int entity, Collider collider) {
	colliders[entity] = collider;
}

bool PhysicsEngine::has_collider(int entity) {
	return colliders.find(entity) != colliders.end();
}

Collider* PhysicsEngine::get_collider(int entity_id) {
	auto it = colliders.find(entity_id);
	if (it == colliders.end()) {
		auto& entity_manager = get_entity_manager();
		auto entity = entity_manager.get_entity(entity_id);
		std::string message = "The physics engine tried to get a collider for ";
		message += entity->get_name() + "(" + std::to_string(entity_id) + ")";
		message += ", but it never registered a collider.";
		tdns_log.write(message);
		return nullptr;
	}

	return &(it->second);

}

void PhysicsEngine::remove_entity(int entity) {
	colliders.erase(entity);
}

PhysicsEngine& get_physics_engine() {
	static PhysicsEngine engine;
	return engine;
}
