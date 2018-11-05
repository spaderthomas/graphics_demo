struct {
	sol::state state;
	map<string, vector<string>> script_to_definitions;
	map<string, string> definitions_to_script;

	// Some scripts are things we don't wanna be able to select in ImGui, so just load those in separately
	vector<string> imgui_shown_scripts;
	vector<string> imgui_ignored_scripts;

	void init() {
		state.open_libraries(sol::lib::base, sol::lib::coroutine, sol::lib::string, sol::lib::io, sol::lib::package, sol::lib::math, sol::lib::table);

		// Load scripts
		auto error_handler = [](lua_State*, sol::protected_function_result pfr) {
			sol::error err = pfr;
			string msg = string("Error in script: ") + err.what();
			tdns_log.write(msg);
			return pfr;
		};
		state.safe_script_file(absolute_path("src\\scripts\\meta.lua"), error_handler);
		sol::table files = state["scripts"];
		for (auto& kvp : files) {
			sol::table file = kvp.second;
			string script = file["name"];
			string path = absolute_path("src\\scripts\\") + script + ".lua";

			file["imgui_ignore"] ? 
				imgui_ignored_scripts.push_back(script) :
				imgui_shown_scripts.push_back(script);

			state.safe_script_file(path, error_handler);

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

		// Register C++ functions in Lua
		state.set_function("show_text", &Text_Box::begin, &game.text_box);
		state.set_function("go_through_door", &Game::go_through_door, &game);
		state.set_function("begin_dialogue", &Game::begin_dialogue, &game);

		state.set("game", game);

		#pragma region TYPE_BINDINGS
		sol::table lua_types = Lua.state.create_named_table("types");

		lua_types.new_usertype<Game>(
			"Game",
			"scene", &Game::scene);

		lua_types.new_usertype<Entity>(
			"Entity",
			"get_component", &Entity::get_component,
			"lua_id", &Entity::lua_id);

		lua_types.new_usertype<any_component>(
			"any_component",
			sol::meta_function::garbage_collect, sol::destructor(&any_component::fake_destructor_for_sol),
			"position", &any_component::position_component,
			"state", &any_component::state_component,
			"door", &any_component::door_component);

		lua_types.new_usertype<Position_Component>(
			"Position_Component",
			"name", &Position_Component::name);
		lua_types.new_usertype<State_Component>(
			"State_Component",
			"current_state", &State_Component::current_state,
			"set_state", &State_Component::set_state,
			"name", &State_Component::name);
		lua_types.new_usertype<Door_Component>(
			"Door_Component",
			"name", &Door_Component::name,
			"to", &Door_Component::to);
		#pragma endregion
	}
} Lua;
