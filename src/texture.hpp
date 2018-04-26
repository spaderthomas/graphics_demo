struct Texture {
	GLuint handle;
	
	unsigned char* data;
	int width;
	int height;
	int num_channels;

	void init(const char* path) {
		// Generate OpenGL stuff
		glGenTextures(1, &handle);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, handle);

		// Some sane defaults
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Load image data
		stbi_set_flip_vertically_on_load(true);
		unsigned char *img_data = stbi_load(path, &width, &height, &num_channels, 0);
		if (img_data) {
			data = img_data;
			glGenerateMipmap(GL_TEXTURE_2D);
			//stbi_image_free(img_data);
		} else {
			cout << "Failed to load texture!";
			tdns_log.write("Failed to load texture!");
		}

	}

	void bind() {
		glActiveTexture(GL_TEXTURE0);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	}
};
