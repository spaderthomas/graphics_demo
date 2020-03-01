namespace NewStuff {
	struct Component {
		std::string name;
		int entity;
		int id;
		
		static int next_id;
	
		static Component* create(std::string name, int entity);
		virtual void update(float dt);
		std::string get_name();
		int get_id();
		int get_entity();
	};
	int Component::next_id = 0;

	struct Entity {
		static int next_id;
		int id;
		std::string name;
		std::map<std::string, Component*> components;

		Entity(std::string name, int id);
		void update(float dt);
		Component* add_component(std::string name);
		Component* get_component(std::string name);
		int get_id();
		std::string get_name();
	};
	int Entity::next_id = 0;
	
	struct EntityManager;
	struct EntityHandle {
		int id;

		operator bool() const;
		Entity* operator->() const;
		Entity* get() const;
	};

	struct EntityManager {
		std::map<int, std::unique_ptr<Entity>> entities;
		
		Entity* get_entity(int id);
		bool has_entity(int id);
		EntityHandle create_entity(std::string name);
		void update(float dt);
	};
	EntityManager& get_entity_manager();

	struct Animation : Asset {
		std::vector<std::string> frames;
		void add_frames(std::vector<std::string>& frames_to_add);
		void add_frame(std::string& sprite_name);
		std::string get_frame(int frame);
	};

	struct SceneManager {
		std::map<std::string, EntityHandle> scenes;
		
		EntityHandle create_scene(std::string name);
		EntityHandle get_scene(std::string name);
		EntityHandle add_entity(std::string scene, std::string entity);
	};
	
	SceneManager& get_scene_manager();
	
	enum Render_Flags {
		None = 0,
		Highlighted = 1 << 0,
	};
	struct Render_Element {
		Component* graphic;
		Component* position;
		Component* animation;
		Render_Flags flags;
	};

	struct RenderEngine {
		std::vector<std::function<void()>> primitives;
		std::vector<Render_Element> render_list;
		void draw(EntityHandle entity, Render_Flags flags);
		void render();
	};

	RenderEngine& GetRenderEngine();

	// API
	void draw_entity(EntityHandle me, Render_Flags flags = Render_Flags::None);
	void add_entity_to_scene(std::string scene, std::string entity);
	void register_animation(std::string name, std::vector<std::string> frames);
	std::vector<std::string> get_frames(std::string animation_name);
	
	Sprite* get_frame(std::string animation, int frame);
}