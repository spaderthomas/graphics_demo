#define MAP_SIZE 128
struct Tilemap {
	string name;
	Entity* tiles[MAP_SIZE][MAP_SIZE] = { nullptr };

	void draw() {
		fox_for(itilex, MAP_SIZE) {
			fox_for(itiley, MAP_SIZE) {
				Entity* ent = tiles[itilex][itiley];
				if (ent) { 
					ent->draw(); 
				}
			}
		}
	}

	void save() {
		json j;
		fox_for(itilex, MAP_SIZE) {
			fox_for(itiley, MAP_SIZE) {
				Entity* tile = tiles[itilex][itiley];
				if (tile != nullptr) { tile->save(j[itilex][itiley]); }
				else { j[itilex][itiley] = "NULL"; }
			}
		}

		string path = string("../../save/") + name + string(".json");
		ofstream save_file(path);
		save_file << std::setw(4) << j << std::endl;
	}

	//@leak We never free up any tiles that were previously allocated.
	void load() {
		string path = "../../save/" + name + ".json";
		ifstream load_file(path);
		json j;

		load_file >> j;
		fox_for(itilex, MAP_SIZE) {
			fox_for(itiley, MAP_SIZE) {
				// Grab the entry and check if it's the special key we use for nonexistent entry
				auto tile_json = j[itilex][itiley];
				if (tile_json != "NULL") {
					// Get the correct subclass create method from the table, call it to get a new template entity
					string type_name = tile_json["lua_id"];
					Entity* new_ent = Basic_Tile::create(type_name);
					// Load in the specific instance's information from JSON
					new_ent->load(tile_json);
					tiles[itilex][itiley] = new_ent;
				} else {
					tiles[itilex][itiley] = nullptr;
				}
			}
		}
	}
};