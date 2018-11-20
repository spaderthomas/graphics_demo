struct LuaState {
	sol::state state;
	map<string, vector<string>> script_to_definitions;
	map<string, string> definitions_to_script;

	// Some scripts are things we don't wanna be able to select in ImGui, so just load those in separately
	vector<string> imgui_shown_scripts;
	vector<string> imgui_ignored_scripts;

	void init() {
		state.open_libraries(sol::lib::base, sol::lib::coroutine, sol::lib::string, sol::lib::io, sol::lib::package, sol::lib::math, sol::lib::table);

		bind_functions();

		// Userdata
		state.new_usertype<EntityHandle>(
			"EntityHandle"
		);	

		state.new_usertype<Entity>(
			"Entity",
			"get_component", &Entity::get_component,
			"lua_id", &Entity::lua_id
		);
		
		state.new_usertype<Level>(
			"Level",
			"name", &Level::name,
			"load", &Level::load,
			"draw", &Level::draw,
			"entities", &Level::entity_handles
		);

		
		// Load scripts
		auto error_handler = [](lua_State*, sol::protected_function_result pfr) {
			sol::error err = pfr;
			string msg = string("Error in script: ") + err.what();
			tdns_log.write(msg);
			return pfr;
		};
		state.safe_script_file(absolute_path("src\\scripts\\meta.lua"), error_handler);
		sol::table files = state["meta"]["scripts"];

		// Load everything that needs to be in the global namespace from scripts
		for (auto& kvp : files) {
			sol::table file = kvp.second;
			string script = file["name"];
			string path = absolute_path("src\\scripts\\") + script + ".lua";

			file["imgui_ignore"] ? 
				imgui_ignored_scripts.push_back(script) :
				imgui_shown_scripts.push_back(script);

			state.safe_script_file(path, error_handler);
			if (file["check_for_defs"]) {
				vector<string> defined;
				sol::optional<sol::table> entities = state[script];
				if (entities) {
					for (auto& entity : entities.value()) {
						string name = entity.first.as<string>();
						defined.push_back(name);
						definitions_to_script[name] = script;
					}
				}

				sort(defined.begin(), defined.end());
				script_to_definitions[script] = defined;
			}
		}
	}

	void init_after_load() {
		// Initialize game objects and things once all scripts + engine is loaded
		sol::table files = state["meta"]["scripts"];
		for (auto& kvp : files) {
			sol::table file = kvp.second;
			string script = file["name"];
			string path = absolute_path("src\\scripts\\") + script + ".lua";

			sol::optional<sol::function> after_load = state["meta"][script]["after_load"];
			if (after_load != sol::nullopt) {
				(*after_load)();
			}
		}
	}

	void run_game_update(float dt) {
		//@hack either a better way to find the function with middleclass or dont use middleclass
		sol::function lua_update = state["Game"]["__declaredMethods"]["update"]; 
		lua_update(state["game"], dt);
	}
};
LuaState Lua;
