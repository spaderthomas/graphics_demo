struct Teleport_Action : Action {
	float x;
	float y;

	bool update(float dt) override;
	string kind() override { return "Teleport_Action"; }
	void imgui_visualizer() override;
	void init(TableNode* table) override;
};