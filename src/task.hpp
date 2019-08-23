struct Action {
	EntityHandle actor;
	bool is_blocking = false;
	virtual bool update(float dt) = 0;
	virtual void init() {};
	virtual string kind() = 0;
};
Action* action_from_table(TableNode* table, EntityHandle actor);
void init_is_blocking_tds(Action* action, TableNode* table);

// Conjunction of actions which blocks until all are complete
struct And_Action : Action {
	vector<Action*> actions;
	bool update(float dt) override;
	string kind() override { return "And_Action"; }
};

struct Movement_Action : Action {
	glm::vec2 dest;

	bool update(float dt) override;
	string kind() override { return "Movement_Action"; }
};

struct Wait_For_Interaction_Action : Action {
	bool update(float dt) override;
	string kind() override { return "Wait_For_Interaction_Action"; }
};

struct Dialogue_Action : Action {
	Dialogue_Tree* tree;
	bool update(float dt) override;
	void init() override;
	string kind() override { return "Dialogue_Action"; }
};

struct Set_State_Action : Action {
	string var;
	bool value;

	bool update(float dt) override;
	string kind() override { return "Set_State_Action"; }
};

struct Teleport_Action : Action {
	float x;
	float y;

	bool update(float dt) override;
	string kind() override { return "Teleport_Action"; }
};

struct Camera_Pan_Action : Action {
	glm::vec2 dest;
	int count_frames;
	int frames_elapsed = 0;

	bool update(float dt) override;
	string kind() override { return "Camera_Pan_Action"; };
};

struct Camera_Follow_Action : Action {
	string who;
	
	bool update(float dt) override;
	string kind() override { return "Camera_Follow_Action"; };
};

struct Action_Queue {
	deque<Action*> actions;
	int index = 0;

	void push(Action* action) {
		actions.push_back(action);
	}

	void push_front(Action* action) {
		actions.push_front(action);
	}

	Action* next() {
		if (index < (int)actions.size()) {
			Action* ret = actions[index];
			index++;
			return ret;
		}
		return nullptr;
	}

	void remove(Action* action) {
		for (auto it = actions.begin(); it != actions.end();) {
			if (*it == action) {
				it = actions.erase(it);
				if (it - actions.begin() <= index)  
					index = fox_max(index - 1, 0);
			} else {
				it++;
			}
		}
	}

	void reset_top() {
		index = 0;
	}
};

struct Task {
	Action_Queue action_queue;
	EntityHandle actor;

	bool update(float dt);
	void add_action(Action* a);
	void init_from_table(TableNode* table, EntityHandle actor);
};

struct TaskEditorNode {
	TaskEditorNode() { action = nullptr; pos = ImVec2(0, 0); size = ImVec2(0, 0); }
	Action* action;
	ImVec2 pos;
	ImVec2 size;
	int id;
}; 

vector<TaskEditorNode*> make_task_graph(Task* task, ImVec2 base);
vector<TaskEditorNode*> make_task_graph(string entity, string scene, ImVec2 base);
struct TaskEditor {
	const float NODE_SLOT_RADIUS = 4.0f;
	const ImVec2 NODE_WINDOW_PADDING = ImVec2(8.0f, 8.0f);
	ImVec2 scrolling = ImVec2(0.f, 0.f);
	vector<TaskEditorNode*> task_graph;
	TaskEditorNode* node_selected;
	
	void show();
};
