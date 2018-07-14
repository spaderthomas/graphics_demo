typedef int GLFW_KEY_TYPE;
struct Input {
	bool should_update;
	glm::vec2 px_pos;
	glm::vec2 screen_pos;
	glm::vec2 scroll;

	bool is_down[GLFW_KEY_LAST];
	bool was_down[GLFW_KEY_LAST];

	bool was_pressed(GLFW_KEY_TYPE id) {
		return is_down[id] && !was_down[id];
	}

	void reset_for_next_frame() {
		fox_for(input_id, GLFW_KEY_LAST) {
			was_down[input_id] = is_down[input_id];
		}
	}

	
};
Input global_input;
Input game_input;
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
		if (global_input.was_pressed(key)) {
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

// GLFW Callbacks
static void GLFW_Cursor_Pos_Callback(GLFWwindow* window, double xpos, double ypos) {
	xpos = max<double>(xpos, 0); ypos = max<double>(ypos, 0);
	global_input.px_pos = glm::vec2(xpos, SCREEN_Y - ypos);
	global_input.screen_pos = glm::vec2(xpos / SCREEN_X, (SCREEN_Y - ypos) / SCREEN_Y);
}

void GLFW_Mouse_Button_Callback(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if (action == GLFW_PRESS) {
			global_input.is_down[GLFW_MOUSE_BUTTON_LEFT] = true;
		}
		if (action == GLFW_RELEASE) {
			global_input.is_down[GLFW_MOUSE_BUTTON_LEFT] = false;
		}
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT) {
		if (action == GLFW_PRESS) {
			global_input.is_down[GLFW_MOUSE_BUTTON_RIGHT] = true;
		}
		if (action == GLFW_RELEASE) {
			global_input.is_down[GLFW_MOUSE_BUTTON_RIGHT] = false;
		}
	}
}

void GLFW_Key_Callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) { global_input.is_down[key] = true; }
    if (action == GLFW_RELEASE) { global_input.is_down[key] = false; } 
}

void GLFW_Scroll_Callback(GLFWwindow* window, double xoffset, double yoffset) {
	global_input.scroll = glm::vec2((float)xoffset, (float)yoffset);
	ImGuiIO& io = ImGui::GetIO();
	io.MouseWheelH += (float)xoffset;
	io.MouseWheel += (float)yoffset;
}

void GLFW_Error_Callback(int err, const char* msg) {
	cout << err;
	cout << msg;
}