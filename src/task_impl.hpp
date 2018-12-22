Action* action_from_table(sol::table table, EntityHandle actor) {
	bool good_action = false;

	string kind = table["kind"];
	if (kind == "Wait_For_Interaction_Action") {
		Wait_For_Interaction_Action* action = new Wait_For_Interaction_Action;
		action->actor = actor;
		init_is_blocking(action, table);

		return action;
	}
	else if (kind == "Dialogue_Action") {
		Dialogue_Tree* tree = new Dialogue_Tree;
		sol::table dialogue = table["dialogue"];
		tree->init_from_table(dialogue);

		Dialogue_Action* action = new Dialogue_Action;
		action->init();
		action->tree = tree;
		action->actor = actor;
		init_is_blocking(action, table);

		return action;
	}
	else if (kind == "Movement_Action") {
		Movement_Action* action = new Movement_Action;
		action->dest.x = table["dest"]["x"];
		action->dest.y = table["dest"]["y"];
		action->actor = actor;
		init_is_blocking(action, table);

		return action;
	}
	else if (kind == "And_Action") {
		And_Action* action = new And_Action;
		init_is_blocking(action, table);
		sol::table actions = table["actions"];
		for (auto& kvp : actions) {
			sol::table action_definition = kvp.second;
			action->actions.push_back(action_from_table(action_definition, actor));
		}

		return action;
	}
	else if (kind == "Set_State_Action") {
		Set_State_Action* action = new Set_State_Action;
		action->var = table["var"];
		action->value = table["value"];
		init_is_blocking(action, table);

		return action;
	}
	else if (kind == "Teleport_Action") {
		Teleport_Action* action = new Teleport_Action;
		action->actor = actor;
		action->x = table["dest"]["x"];
		action->y = table["dest"]["y"];
		init_is_blocking(action, table);

		return action;
	}

	tdns_log.write("Tried to create an action with an invalid kind: " + kind);
	return nullptr;
}

void init_is_blocking(Action* action, sol::table& table) {
	sol::optional<bool> maybe_is_blocking = table["is_blocking"];
	if (maybe_is_blocking) {
		action->is_blocking = maybe_is_blocking.value();
	}
}

bool And_Action::update(float dt) {
	bool done = true;
	for (auto it = actions.begin(); it != actions.end();) {
		Action* action = *it;
		if (action->update(dt)) {
			it = actions.erase(it);
		}
		else {
			done = false;
			it++;
		}
	}

	return done;
}

bool Movement_Action::update(float dt) {
	def_get_cmp(mc, actor.deref(), Movement_Component);
	def_get_cmp(pc, actor.deref(), Position_Component);

	if (vec_almost_equals(pc->world_pos, dest)) {
		return true;
	}

	bool up = pc->world_pos.y < dest.y;
	bool down = pc->world_pos.y > dest.y;
	bool right = pc->world_pos.x < dest.x;
	bool left = pc->world_pos.x > dest.x;

	move_entity(actor, up, down, left, right);
	return false;
}	

bool Wait_For_Interaction_Action::update(float dt) {
	def_get_cmp(ic, actor.deref(), Interaction_Component);
	if (ic->was_interacted_with) {
		ic->on_interact(actor, ic->other);

		// Clear the IC so we don't call this again next frame.
		ic->was_interacted_with = false;
		ic->other = { 0, nullptr };

		return true;
	}

	return false;
}

void Dialogue_Action::init() {
	is_blocking = true;
}
bool Dialogue_Action::update(float dt) {
	Dialogue_Node* node = tree->traverse();
	if (game.input.was_pressed(GLFW_KEY_1)) {
		node->set_response(0);
	}
	else if (game.input.was_pressed(GLFW_KEY_2)) {
		node->set_response(1);
	}
	else if (game.input.was_pressed(GLFW_KEY_3)) {
		node->set_response(2);
	}
	else if (game.input.was_pressed(GLFW_KEY_4)) {
		node->set_response(3);
	}

	node->show_line();

	// Probably keep this here. 
	if (game.input.was_pressed(GLFW_KEY_SPACE)) {
		// If the dialogue has fully shown
		if (text_box.is_all_text_displayed()) {
			// Break if node is terminal
			if (node->terminal) {
				text_box.reset_and_hide();
				return true;
			}
			// Show responses if it is not
			else {
				string all_response_text;
				for (auto& response : node->responses) {
					all_response_text += response + "\r";
				}
				text_box.begin(all_response_text);
			}
		}
		// If the set has shown fully (but not ALL dialogue), go to the next set
		else if (text_box.is_current_set_displayed()) {
			text_box.resume();
		}
		// If the dialogue has partially shown, skip to the end of the line set
		else {
			Line_Set& set = text_box.current_set();
			set.point = set.max_point;
		}
	}

	return false;
}

bool Set_State_Action::update(float dt) {
	state_system.update_state(this->var, this->value);
	return true;
}

bool Teleport_Action::update(float dt) {
	teleport_entity(this->actor, this->x, this->y);
	return true;
}

bool Task::update(float dt) {
	// Run through actions until we reach one which blocks or the queue is empty
	Action* next_action;
	while (true) {
		next_action = action_queue.next();
		if (!next_action) break;

		if (next_action->update(dt)) {
			action_queue.remove(next_action);
		}

		if (next_action->is_blocking) break;
	}

	action_queue.reset_top(); // Reset the index to the top of the queue

	// If there are no more actions, this task is done
	if (!action_queue.actions.size()) return true;

	return false;
}

void Task::add_action(Action* a) {
	action_queue.push(a);
}

void Task::init_from_table(sol::table table, EntityHandle actor) {
	this->actor = actor;

	for (auto it : table) {
		sol::table action_table = it.second.as<sol::table>();
		Action* action = action_from_table(action_table, actor);
		this->add_action(action);
	}
}

vector<TaskEditorNode*> make_task_graph(Task* task, ImVec2 base) {
	static int id = 0;
	vector<TaskEditorNode*> graph;
	for (auto& action : task->action_queue.actions) {
		TaskEditorNode* node = new TaskEditorNode;
		node->action = action;
		node->pos = base;
		node->id = id++;
		graph.push_back(node);

		base = base + ImVec2(100, 0);
	}

	return graph;
}
vector<TaskEditorNode*> make_task_graph(string entity, string scene, ImVec2 base) {
	sol::table task_table = Lua.get_task(entity, scene);
	EntityHandle dummy = { -1, nullptr };
	Task* task = new Task;
	task->init_from_table(task_table, dummy);
	return make_task_graph(task, base);
}

void TaskEditor::show() {
	ImGui::Begin("Task Editor", 0, ImGuiWindowFlags_AlwaysAutoResize);

	// Create child canvas
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1, 1));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, IM_COL32(60, 60, 70, 200));
	ImGui::BeginChild("scrolling_region", ImVec2(0, 0), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove);
	ImGui::PushItemWidth(120.0f);

	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	// Display grid
	ImU32 GRID_COLOR = IM_COL32(200, 200, 200, 40);
	float GRID_SZ = 64.0f;
	ImVec2 win_pos = ImGui::GetCursorScreenPos();
	ImVec2 canvas_sz = ImGui::GetWindowSize();
	for (float x = fmodf(scrolling.x, GRID_SZ); x < canvas_sz.x; x += GRID_SZ)
		draw_list->AddLine(ImVec2(x, 0.0f) + win_pos, ImVec2(x, canvas_sz.y) + win_pos, GRID_COLOR);
	for (float y = fmodf(scrolling.y, GRID_SZ); y < canvas_sz.y; y += GRID_SZ)
		draw_list->AddLine(ImVec2(0.0f, y) + win_pos, ImVec2(canvas_sz.x, y) + win_pos, GRID_COLOR);

	ImVec2 offset = ImGui::GetCursorScreenPos() + scrolling;

	draw_list->ChannelsSplit(2);
	for (auto* node : task_graph) {
		ImGui::PushID(node->id);
		ImVec2 node_rect_min = offset + node->pos;

		const float NODE_SLOT_RADIUS = 4.0f;
		const ImVec2 NODE_WINDOW_PADDING(8.0f, 8.0f);


		// Set the screen position to the bottom of the node
		ImGui::SetCursorScreenPos(node_rect_min + NODE_WINDOW_PADDING);

		// Display the node's contents in the foreground channel
		draw_list->ChannelsSetCurrent(1);
		bool old_any_active = ImGui::IsAnyItemActive();
		ImGui::BeginGroup(); // Lock horizontal position
		string kind = node->action->kind();
		ImGui::Text(kind.c_str());
		if (kind == "Movement_Action") {
			Movement_Action* move_action = dynamic_cast<Movement_Action*>(node->action);
			ImGui::SliderFloat2("Destination", glm::value_ptr(move_action->dest), 0, 1);
		}
		ImGui::EndGroup();
		bool node_widgets_active = !old_any_active && ImGui::IsAnyItemActive();

		// Set the size of the node to the size of the group containing its contents
		node->size = ImGui::GetItemRectSize() + NODE_WINDOW_PADDING + NODE_WINDOW_PADDING;
		ImVec2 node_rect_max = node_rect_min + node->size;

		// Set the screen position to the bottom left of the node w/o padding
		ImGui::SetCursorScreenPos(node_rect_min);
		ImGui::InvisibleButton("node", node->size);

		bool is_node_being_dragged = ImGui::IsItemActive();
		if (node_widgets_active || is_node_being_dragged)
			node_selected = node;
		if (is_node_being_dragged && ImGui::IsMouseDragging(0))
			node->pos = node->pos + ImGui::GetIO().MouseDelta;

		ImU32 node_bg_color = ImGui::IsItemHovered() || node_selected == node
			? IM_COL32(75, 75, 75, 255)
			: IM_COL32(60, 60, 60, 255);
		draw_list->ChannelsSetCurrent(0);
		draw_list->AddRectFilled(node_rect_min, node_rect_max, node_bg_color, 4.0f);

		ImGui::PopID();
	}

	draw_list->ChannelsSetCurrent(0);
	fox_for(node_idx, task_graph.size() - 1) {
		TaskEditorNode* node = task_graph[node_idx];
		TaskEditorNode* child = task_graph[node_idx + 1];

		ImVec2 p1 = offset + node->pos + ImVec2(node->size.x, node->size.y / 2);
		ImVec2 p2 = offset + child->pos + ImVec2(0, child->size.y / 2);
		draw_list->AddCircleFilled(p1, NODE_SLOT_RADIUS, IM_COL32(150, 150, 150, 150));
		draw_list->AddCircleFilled(p2, NODE_SLOT_RADIUS, IM_COL32(150, 150, 150, 150));
		draw_list->AddBezierCurve(p1, p1 + ImVec2(+100, 0), p2 + ImVec2(-100, 0), p2, IM_COL32(200, 200, 100, 255), 3.0f, 1);
	}
	draw_list->ChannelsMerge();

	if (ImGui::IsWindowHovered() && !ImGui::IsAnyItemActive() && ImGui::IsMouseDragging(2, 0.0f))
		scrolling = scrolling + ImGui::GetIO().MouseDelta;

	ImGui::PopItemWidth();
	ImGui::EndChild();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar(2);
	ImGui::End();


}