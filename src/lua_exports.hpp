// THIS FILE IS AUTOGENERATED BY METAPROGRAMMING.PY

tdapi Level* get_level(string name);
tdapi void add_entity(Level* level, EntityHandle entity);
tdapi string entity_name(EntityHandle entity);
tdapi int entity_id(EntityHandle entity);
tdapi void on_collide(EntityHandle me, EntityHandle other);
tdapi void update_animation(EntityHandle me, float dt);
tdapi int collider_kind(EntityHandle me);
tdapi void draw_entity(EntityHandle me, Render_Flags flags);
tdapi void teleport_entity(EntityHandle me, float x, float y);
tdapi void move_entity(EntityHandle me, float dx, float dy);
tdapi EntityHandle create_entity(string lua_id);
tdapi void set_animation(EntityHandle me, string wish_name);
tdapi void set_animation_no_reset(EntityHandle me, string wish_name);
tdapi void update_entity(EntityHandle me, float dt);
tdapi bool are_interacting(EntityHandle initiator, EntityHandle receiver);
tdapi void draw_aabb(EntityHandle me);
tdapi bool are_entities_colliding(EntityHandle a, EntityHandle b);
tdapi void register_potential_collision(EntityHandle me, EntityHandle other);
tdapi bool is_down(GLFW_KEY_TYPE key);
tdapi bool was_down(GLFW_KEY_TYPE key);
tdapi bool was_pressed(GLFW_KEY_TYPE key);
tdapi void go_through_door(EntityHandle door, EntityHandle other);
tdapi void begin_dialogue(EntityHandle entity, string scene);
tdapi void camera_follow(EntityHandle entity);

void bind_functions();
