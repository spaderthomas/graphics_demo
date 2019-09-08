struct Level {
	string name;
	vector<EntityHandle> tiles;
	vector<EntityHandle> entities;

	EntityHandle get_tile(int x, int y);
	void set_tile(EntityHandle handle, int x, int y);
	EntityHandle get_first_matching_entity(string name);
	EntityHandle erase_first_matching_entity(string name);
	void clear_entities();
	void load_entities(TableNode* entities_table);
	void draw();
	void save();
	void load();
};

unordered_map<string, Level*> levels;
