struct Spin_Action : Action {
	bool update(float dt) override;
	string kind() override { return "Spin_Action"; };
	void imgui_visualizer() override;
	void init(TableNode* table) override;
};
