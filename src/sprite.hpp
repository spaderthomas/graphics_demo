struct Texture_Atlas;
struct Sprite : Asset {
	int height, width, num_channels;
	Texture_Atlas* atlas;
	vector<float> tex_coords;

	GLvoid* tex_coord_offset;

	static GLuint vert_buffer;
	static GLuint elem_buffer;
	static GLuint vao;
};

// OpenGL 
GLuint Sprite::vert_buffer;
GLuint Sprite::elem_buffer;
GLuint Sprite::vao;

void bind_sprite_buffers() {
	glBindVertexArray(Sprite::vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Sprite::elem_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, Sprite::vert_buffer);
}

void fill_gpu_sprite_buffers() {
	vector<float> vert_data;
	vector<float> tex_coords;

	// Push each layer of square verts into the buffer, and mark offsets
	concat(vert_data, square_verts);

	// Fill tex coordinate buffer
	for (auto asset : asset_table.assets) {
		Sprite* sprite = dynamic_cast<Sprite*>(asset);
		if (sprite) {
			sprite->tex_coord_offset = (GLvoid*)(sizeof(float) * vert_data.size());
			concat(vert_data, sprite->tex_coords);
		}
	}

	// Send all the data to OpenGL buffers
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Sprite::elem_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, square_indices.size() * sizeof(uint), square_indices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, Sprite::vert_buffer);
	glBufferData(GL_ARRAY_BUFFER, vert_data.size() * sizeof(float), vert_data.data(), GL_STATIC_DRAW);
}
