struct InputManager {
	bool should_update;
	glm::vec2 px_pos;
	glm::vec2 screen_pos;
	glm::vec2 world_pos;
	glm::vec2 scroll;
	int8 mask = INPUT_MASK_ALL;

	bool is_down[GLFW_KEY_LAST];
	bool was_down[GLFW_KEY_LAST];

	bool was_pressed(GLFW_KEY_TYPE id, int mask = INPUT_MASK_NONE) {
		if (!(this->mask & mask)) return false;
		return is_down[id] && !was_down[id];
	}

	bool chord(GLFW_KEY_TYPE mod_key, GLFW_KEY_TYPE cmd_key) {
		bool mod_is_down = false;
		if (mod_key == GLFW_KEY_CONTROL) {
			mod_is_down |= is_down[GLFW_KEY_RIGHT_CONTROL];
			mod_is_down |= is_down[GLFW_KEY_LEFT_CONTROL];
		}
		if (mod_key == GLFW_KEY_SUPER) {
			mod_is_down |= is_down[GLFW_KEY_LEFT_SUPER];
			mod_is_down |= is_down[GLFW_KEY_RIGHT_SUPER];
		}
		if (mod_key == GLFW_KEY_SHIFT) {
			mod_is_down |= is_down[GLFW_KEY_LEFT_SHIFT];
			mod_is_down |= is_down[GLFW_KEY_RIGHT_SHIFT];
		}
		
		return mod_is_down && was_pressed(cmd_key);
	}

	void end_frame() {
		fox_for(input_id, GLFW_KEY_LAST) {
			was_down[input_id] = is_down[input_id];
		}
	}
};

// new
InputManager& get_input_manager() {
	static InputManager manager;
	return manager;
}

InputManager global_input;
bool game_input_active = true;

// ImGui gets the mouse coordinates every frame, so it knows if we're hovering it
void give_imgui_mouse_input() {
	ImGuiIO& io = ImGui::GetIO();
	io.MousePos = ImVec2((float)global_input.px_pos.x, (float)global_input.px_pos.y);
	io.MouseDown[0] = global_input.is_down[GLFW_MOUSE_BUTTON_LEFT];
	io.MouseDown[1] = global_input.is_down[GLFW_MOUSE_BUTTON_RIGHT];
}
// If ImGui says it wants our input, then this will give it the rest
void fill_imgui_input() {
	ImGuiIO& io = ImGui::GetIO();

	// Fill in the raw ImGui buffer
	for (int key = 0; key < GLFW_KEY_LAST; key++) {
		io.KeysDown[key] = global_input.is_down[key];
	}
	// Fill in the input characters
	for (int key = GLFW_KEY_SPACE; key < GLFW_KEY_A; key++) {
		// @hack @spader 8/23/2019 Should make this more generic, but who cares
		// @spader 9/20/2019 Used this hack again
		bool used_shift = false;
		if (key == GLFW_KEY_MINUS && global_input.chord(GLFW_KEY_SHIFT, GLFW_KEY_MINUS)) {
			io.AddInputCharacter(ASCII_UNDERSCORE);
			used_shift = true;
		}
		if (key == GLFW_KEY_SLASH && global_input.chord(GLFW_KEY_SHIFT, GLFW_KEY_SLASH)) {
			io.AddInputCharacter(ASCII_QMARK);
			used_shift = true;
		}
		if (key == GLFW_KEY_9 && global_input.chord(GLFW_KEY_SHIFT, GLFW_KEY_9)) {
			io.AddInputCharacter(ASCII_OPAREN);
			used_shift = true;
		}
		if (key == GLFW_KEY_0 && global_input.chord(GLFW_KEY_SHIFT, GLFW_KEY_0)) {
			io.AddInputCharacter(ASCII_CPAREN);
			used_shift = true;
		}
		
		if (!used_shift && global_input.was_pressed(key)) {
			io.AddInputCharacter(key);
		}
	}
	for (int key = GLFW_KEY_A; key <= GLFW_KEY_Z; key++) {
		if (global_input.was_pressed(key)) {
			// GLFW character keys are the same as ASCII, so we can do this (also convert to lowercase)
			io.AddInputCharacter(key + 0x20);
		}
	}


	// Add controller keys
	io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
	io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
	io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
	io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];
}
