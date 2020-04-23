struct Entity_Tree {
	std::string dir;
	std::vector<pool_handle<Entity>> entities;
	std::vector<Entity_Tree*> children;
	int size = 0;
	
	static Entity_Tree* create(std::string dir);
	pool_handle<Entity> find(std::string name);
};

struct Entity_Info {
	std::vector<std::string> entities;
	std::map<std::string, std::vector<std::string>> file_map;
	std::map<std::string, std::string> entity_to_file;

	void init();
};

struct Entity_Wizard {
	char name[256];
	char path[256];
	std::map<std::string, bool> component_checked;

	void init();
	bool draw();
};

enum Editor_State {
	IDLE,
	INSERT,
	EDIT,
	DRAG,
	RECTANGLE_SELECT
};
enum Selection_Kind {
	NONE,
	TILE,
	ENTITY
};
struct Editor : Layer {
	Entity_Wizard entity_wizard;
	Entity_Info entity_info;
	Entity_Tree* tile_tree;
	void draw_tile_tree(Entity_Tree* root);
	int draw_tile_tree_recursive(Entity_Tree* root, int unique_btn_index);
	Editor_State state = IDLE;
	Selection_Kind kind = NONE;
	TaskEditor task_editor;
	void show_entity_wizard();
	
	// State for moving things around cleanly
	glm::ivec2 last_grid_drawn = { 0, 0 };
	glm::vec2 top_left_drag = { 0.f, 0.f };
	glm::vec2 smooth_drag_offset = { 0.f, 0.f };
	
	// Grid state
	bool show_grid = false;
	bool last_show_grid = false;
	bool snap_to_grid = false;
	bool last_snap_to_grid = false;
	
	// State about selected thing
	EntityHandle selected;
	void translate();
	void delete_selected();
	
	// Undo 
	std::vector<std::function<void()>> action_stack;
	void undo_action();
	std::vector<std::function<void()>> mark_stack;
	void undo_mark();
	
	// Big stuff
	void reload_assets();
	void reload() override;
	void exec_console_cmd(char* cmd) override;
	void update(float dt) override;
	void render() override;
	void init() override;
};
Editor editor;