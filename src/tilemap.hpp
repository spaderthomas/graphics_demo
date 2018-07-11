struct Chunk_Index {
	int x;
	int y;

	Chunk_Index(int x, int y) {
		this->x = x;
		this->y = y;
	}

	bool operator==(const Chunk_Index &other) const {
		return (x == other.x && y == other.y);
	}
};

// https://en.wikipedia.org/wiki/Pairing_function#Cantor_pairing_function
// Implementation of a bijective mapping from NxN to N
template <>
struct hash<Chunk_Index> {
	std::size_t operator()(const Chunk_Index& chunk) const {
		return (chunk.x + chunk.y) * (chunk.x + chunk.y + 1) / 2 + chunk.y;
	}
};

#define CHUNK_SIZE 8
#define chunk_of(x) ((x) / CHUNK_SIZE)
#define index_into_chunk(x) ((x) % CHUNK_SIZE)
struct Chunk {
	Entity* tiles[CHUNK_SIZE][CHUNK_SIZE];
};

struct Level {
	string name;
	unordered_map<Chunk_Index, Chunk> chunks;
	vector<Entity*> entities;

	Entity* get_tile(int x, int y) {
		Chunk& chunk = chunks[Chunk_Index(chunk_of(x), chunk_of(y))];
		return chunk.tiles[index_into_chunk(x)][index_into_chunk(y)];
	}

	void set_tile(Entity* tile, int x, int y) {
		Chunk& chunk = chunks[Chunk_Index(chunk_of(x), chunk_of(y))];
		chunk.tiles[index_into_chunk(x)][index_into_chunk(y)] = tile;
	}

	void draw() {
		for (auto it : chunks) {
			Chunk& chunk = it.second;
			fox_for(itilex, CHUNK_SIZE) {
				fox_for(itiley, CHUNK_SIZE) {
					Entity* ent = chunk.tiles[itilex][itiley];
					if (ent) {
						ent->draw(Render_Flags::None);
					}
				}
			}
		}

		for (auto entity : entities) {
			entity->draw(Render_Flags::None);
		}
	}

	// The chunk index is used as an index into the JSON save object
	void save() {
		json j;
		for (auto it : chunks) {
			Chunk_Index index = it.first;
			string index_key = to_string(index.x) + "," + to_string(index.y);
			Chunk& chunk = it.second;
			fox_for(itilex, CHUNK_SIZE) {
				fox_for(itiley, CHUNK_SIZE) {
					Entity* tile = chunk.tiles[itilex][itiley];
					if (tile != nullptr) { tile->save(j["chunks"][index_key][itilex][itiley]); }
					else { j["chunks"][index_key][itilex][itiley] = "NULL"; }
				}
			}
		}

		int indx = 0;
		for (auto entity : entities) {
			entity->save(j["entities"][indx++]);
		}

		string path = string("../../save/") + name + string(".json");
		ofstream save_file(path);
		save_file << std::setw(4) << j << std::endl;
	}

	//@leak We never free up any tiles that were previously allocated.
	void load() {
		json j;
		string path = "../../save/" + name + ".json";
		ifstream load_file(path);
		load_file >> j;

		// Load tile chunks
		for (json::iterator it = j["chunks"].begin(); it != j["chunks"].end(); it++) {
			// Parse the chunk indices out of the string (which is of the form "x,y"
			stringstream index_stream = stringstream(it.key());
			string index;
			getline(index_stream, index, ',');
			int chunk_x = stoi(index);
			getline(index_stream, index, ',');
			int chunk_y = stoi(index);

			Chunk& chunk = chunks[Chunk_Index(chunk_x, chunk_y)];
			json chunk_as_json = it.value();
			fox_for(itilex, CHUNK_SIZE) {
				fox_for(itiley, CHUNK_SIZE) {
					auto tile_json  = chunk_as_json[itilex][itiley];
					if (tile_json != "NULL") {
						Entity* new_ent = Entity::create(tile_json);
						chunk.tiles[itilex][itiley] = new_ent;
					}
					else {
						chunk.tiles[itilex][itiley] = nullptr;
					}
				}
			}
		}

		// Load entities
		entities.clear();
		for (unsigned int i = 0; i < j["entities"].size(); i++) {
			json entity_as_json = j["entities"][i];
			Entity* entity = Entity::create(entity_as_json);
			entities.push_back(entity);
		}
	}
};