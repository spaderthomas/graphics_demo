struct Vision_Component : Component {
	float width;
	float depth;

	TableNode* make_template() const override;
	void init(TableNode* table) override;
	string name() override;
	void imgui_visualizer() override;
};
