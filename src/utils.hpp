// Small quality of life macros and typedefs
#define fox_max(a, b) (a) > (b) ? (a) : (b)
#define fox_min(a, b) (a) > (b) ? (b) : (a)
#define fox_for(iterName, iterCount) for (unsigned int iterName = 0; iterName < (iterCount); ++iterName)
#define fox_iter(iter_name, container) for (auto iter_name = (container).begin(); (iter_name) != (container).end(); (iter_name)++)
#define rand_float(max) (static_cast<float>(rand()) / static_cast<float>(RAND_MAX / (max)))

//Assert
#ifdef _MSC_VER
#	ifdef assert
#		undef assert
#	endif
#	define fox_assert(expr) if (!(expr)) _CrtDbgBreak()
#else
#	define fox_assert(expr) assert(expr)
#endif
typedef unsigned int uint;
typedef int32_t int32;


// STL extensions 
template<typename vec_type>
void concat(vector<vec_type>& append_to, vector<vec_type>& append_from) {
	append_to.insert(append_to.end(), append_from.begin(), append_from.end());
}

vector<string> split(const string &str, char delim) {
	stringstream stream(str);
	string item;
	vector<string> tokens;
	while (getline(stream, item, delim)) {
		tokens.push_back(item);
	}
	return tokens;
}

#define tdns_find(vector, item) (find((vector).begin(), (vector).end(), (item)) != (vector).end()) 

glm::vec2 tdns_normalize(glm::vec2 vec) {
	if (vec.x == 0.f && vec.y == 0.f) {
		return vec;
	}

	return glm::normalize(vec);
}
glm::vec3 tdns_normalize(glm::vec3 vec) {
	if (vec.x == 0.f && vec.y == 0.f && vec.z == 0.f) {
		return vec;
	}

	return glm::normalize(vec);
}
// Colors
glm::vec4 hannah_color = glm::vec4(.82f, .77f, 0.57f, 1.0f); // Note: Hannah's favorite three floating point numbers.
glm::vec4 red = glm::vec4(1.f, 0.f, 0.f, 1.f);
glm::vec4 green = glm::vec4(0.f, 1.f, 0.f, 1.f);
glm::vec4 blue = glm::vec4(0.f, 0.f, 1.f, 1.f);
glm::vec4 brown = glm::vec4(173.f / 255.f, 133.f / 255.f, 74.f / 255.f, 1.f);
glm::vec4 white4 = glm::vec4(1.f, 1.f, 1.f, 1.f);
glm::vec3 white3 = glm::vec3(1.f, 1.f, 1.f);


// Screen stuff!
float SCREEN_X = 640.f;
float SCREEN_Y = 360.f;
float CELL_SIZE = 16.f;
float SCR_TILESIZE_X = CELL_SIZE / SCREEN_X;
float SCR_TILESIZE_Y = CELL_SIZE / SCREEN_Y;
float GLSCR_TILESIZE_X = 2 * SCR_TILESIZE_X;
float GLSCR_TILESIZE_Y = 2 * SCR_TILESIZE_Y;

void use_640_360(GLFWwindow* window) {
	SCREEN_X = 640.f;
	SCREEN_Y = 360.f;
	CELL_SIZE = 16.f;
	SCR_TILESIZE_X = CELL_SIZE / SCREEN_X;
	SCR_TILESIZE_Y = CELL_SIZE / SCREEN_Y;
	GLSCR_TILESIZE_X = 2 * SCR_TILESIZE_X;
	GLSCR_TILESIZE_Y = 2 * SCR_TILESIZE_Y;
	glfwSetWindowSize(window, 640, 360);
	glViewport(0, 0, (int)SCREEN_X, (int)SCREEN_Y);
}
void use_720p(GLFWwindow* window) {
	SCREEN_X = 1280.f;
	SCREEN_Y = 720.f;
	CELL_SIZE = 32.f;
	SCR_TILESIZE_X = CELL_SIZE / SCREEN_X;
	SCR_TILESIZE_Y = CELL_SIZE / SCREEN_Y;
	GLSCR_TILESIZE_X = 2 * SCR_TILESIZE_X;
	GLSCR_TILESIZE_Y = 2 * SCR_TILESIZE_Y;
	glfwSetWindowSize(window, 1280, 720);
	glViewport(0, 0, (int)SCREEN_X, (int)SCREEN_Y);
}
void use_1080p(GLFWwindow* window) {
	SCREEN_X = 1920.f;
	SCREEN_Y = 1080.f;
	CELL_SIZE = 64.f;
	SCR_TILESIZE_X = CELL_SIZE / SCREEN_X;
	SCR_TILESIZE_Y = CELL_SIZE / SCREEN_Y;
	GLSCR_TILESIZE_X = 2 * SCR_TILESIZE_X;
	GLSCR_TILESIZE_Y = 2 * SCR_TILESIZE_Y;
	glfwSetWindowSize(window, 1920, 1080);
	glViewport(0, 0, (int)SCREEN_X, (int)SCREEN_Y);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}
/*
Some unit definitions:
GL coordinates have 
	the leftmost coordinate at -1, 
	the rightmost at +1,
	the bottommost at -1,
	the topmost at +1

Screen coordinates have
	the leftmost coordinate at 0,
	the rightmost at +1,
	the bottommost at 0,
	the topmost at +1

Grid coordinates have
	the leftmost coordinate at 0,
	the rightmost at +1,
	the bottommost at 0,
	the topmost at +1

Pixel coordinates have
	the leftmost coordinate at 0,
	the rightmost at SCREEN_X,
	the bottommost at 0,
	the topmost at SCREEN_Yd

Below are all the conversion functions. Using them and proper units is a bit verbose, 
but is worth it to save the confusion of exchanging units (which is unavoidable). Also
of note is that since we're using GLM vectors, it's not really convenient to have 
conversions which use our typedefs, so conversions that take GLM vectors and single
points are defined
*/
typedef int pixel_unit;
typedef float subpixel_unit;
typedef float screen_unit;
typedef float gl_unit;

// Converting to grid units
glm::ivec2 grid_pos_from_px_pos(glm::vec2 px_pos) {
	int closest_x = (int)floor(px_pos.x / CELL_SIZE);
	int closest_y = (int)floor(px_pos.y / CELL_SIZE);
	return glm::ivec2(closest_x, closest_y);
}

// Converting to GL units
glm::vec2 gl_from_screen(glm::vec2 screen_coords) {
	return glm::vec2(screen_coords.x * 2 - 1, screen_coords.y * 2 - 1);
}
gl_unit gl_from_screen(screen_unit s) {
	return s * 2 - 1;
}

// Converting to screen units
screen_unit magnitude_screen_from_gl(gl_unit u) {
	return (screen_unit)(u / 2);
}
screen_unit screen_x_from_px(pixel_unit px) {
	return px / SCREEN_X;
}
screen_unit screen_y_from_px(pixel_unit px) {
	return px / SCREEN_Y;
}
glm::vec2 screen_from_px(glm::ivec2 px) {
	return glm::vec2(px.x / SCREEN_X, px.y / SCREEN_Y);
}
// Puts it in the center of the grid tile
glm::vec2 screen_from_grid(glm::ivec2 grid_pos) { 
	screen_unit x = (screen_unit)(grid_pos.x * SCR_TILESIZE_X);
	screen_unit y = (screen_unit)(grid_pos.y * SCR_TILESIZE_Y);
	x += .5 * SCR_TILESIZE_X;
	y += .5 * SCR_TILESIZE_Y;
	return glm::vec2(x, y);
}

// Converting to pixel units
glm::ivec2 px_coords_from_gl_coords(glm::vec2 gl_coords) {
	float y = (gl_coords.y + 1) / 2;
	float x = (gl_coords.x + 1) / 2;
	return glm::ivec2(floor(x * SCREEN_X), floor(y * SCREEN_Y));
}


/* Some utilities for dealing with files, directories, and paths */
// Takes in a directory or file -- returns everything after the first double backslash
string name_from_full_path(string path) {
	string asset_name;
	for (int ichar = path.size() - 1; ichar > -1; ichar--) {
		if (path.at(ichar) == '\\') { break; }
		if (path.at(ichar) == '/') {
			string msg = "Don't use forward slashes in your directory names. Failing path was: " + path;
			tdns_log.write(msg);
			exit(0);
		}
		asset_name.insert(asset_name.begin(), path.at(ichar));
	}

	return asset_name;
}

// Accepts a filename, not a path. Returns all the characters before the first period.
string strip_extension(string filename) {
	string stripped;
	for (unsigned int ichar = 0; ichar < filename.size(); ichar++) {
		if (filename.at(ichar) == '.') {
			return stripped;
		}
		stripped.push_back(filename.at(ichar));
	}

	return stripped;
}

bool is_alphanumeric(string& str) {
	auto is_numeric = [](char c) -> bool { return c >= '0' && c <= '9'; };
	auto is_alpha = [](char c) -> bool { return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'); };

	for (unsigned int ichar = 0; ichar < str.size(); ichar++) {
		char c = str.at(ichar);
		if (!(is_numeric(c) || is_alpha(c))) {
			return false;
		}
	}

	return true;
}

// Allowing alphanumerics, underscores, and periods
bool is_valid_filename(string& str) {
	auto is_numeric = [](char c) -> bool { return c >= '0' && c <= '9'; };
	auto is_alpha = [](char c) -> bool { return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'); };
	auto is_misc_valid = [](char c) -> bool { return (c == '_') || c == '.'; };

	for (unsigned int ichar = 0; ichar < str.size(); ichar++) {
		char c = str.at(ichar);
		if (!(is_numeric(c) || is_alpha(c) || is_misc_valid(c))) {
			return false;
		}
	}

	return true;
}

// @hack I'm sure there are PNG headers I could try parsing, but this works!
bool is_png(string& asset_path) {
	if (asset_path.size() < 5) { return false; } // "x.png" is the shortest name
	string should_be_png_extension = asset_path.substr(asset_path.size() - 4, 4);
	if (should_be_png_extension.compare(".png")) return false;
	return true;
}


/* ImGui options */
bool show_grid = false;
bool snap_to_grid = false;
bool debug_show_aabb = false;
bool debug_show_minkowski = false;
bool show_imgui_demo = false;
bool show_console = false;


/* Random shit */
void __stdcall gl_debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void *userParam) {
	(void)userParam;

	switch (id) {
	case 131169: // The driver allocated storage for renderbuffer
		return;
	case 131185: // glBufferData
		return;
	case 481131: // buffer info
		return;
	case 131184: // buffer info
		return;
	}

	string debug_msg;
	debug_msg += "OpenGL Debug Message: ";
	debug_msg += "\nSource: ";
	switch (source) {
	case GL_DEBUG_SOURCE_API:
		debug_msg += "API";
		break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
		debug_msg += "Window System";
		break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER:
		debug_msg += "Shader Compiler";
		break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:
		debug_msg += "Third Party";
		break;
	case GL_DEBUG_SOURCE_APPLICATION:
		debug_msg += "Application";
		break;
	case GL_DEBUG_SOURCE_OTHER:
		debug_msg += "Other";
		break;
	}

	debug_msg += "\nType: ";
	switch (type) {
	case GL_DEBUG_TYPE_ERROR:
		debug_msg += "Error";
		break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		debug_msg += "Deprecated Behaviour";
		break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		debug_msg += "Undefined Behaviour";
		break;
	case GL_DEBUG_TYPE_PORTABILITY:
		debug_msg += "Portability";
		break;
	case GL_DEBUG_TYPE_PERFORMANCE:
		debug_msg += "Performance";
		break;
	case GL_DEBUG_TYPE_MARKER:
		debug_msg += "Marker";
		break;
	case GL_DEBUG_TYPE_PUSH_GROUP:
		debug_msg += "Push Group";
		break;
	case GL_DEBUG_TYPE_POP_GROUP:
		debug_msg += "Pop Group";
		break;
	case GL_DEBUG_TYPE_OTHER:
		debug_msg += "Other";
		break;
	}

	debug_msg += "\nID: ";
	debug_msg += to_string(id);

	debug_msg += "\nSeverity: ";
	switch (severity) {
	case GL_DEBUG_SEVERITY_HIGH:
		debug_msg += "High";
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		debug_msg += "Medium";
		break;
	case GL_DEBUG_SEVERITY_LOW:
		debug_msg += "Low";
		break;
	case GL_DEBUG_SEVERITY_NOTIFICATION:
		debug_msg += "Notification";
		break;
	}

	debug_msg += "\nGL message: " + string(message);
	debug_msg += "\n\n";
	tdns_log.write(debug_msg);
}

// This defines which tile is on the upper left of the screen
glm::ivec2 camera = glm::ivec2(0);
float seconds_per_update = 1.f / 60.f;
float pi = 3.14159;