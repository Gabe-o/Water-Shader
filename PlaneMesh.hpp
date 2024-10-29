
class PlaneMesh {
	
	std::vector<float> verts;
	std::vector<float> normals;
	std::vector<int> indices;

	glm::vec4 modelColor;
	int numVerts;
	int numIndices;

	GLuint programID;
	GLuint dispTexID, waterTexID;
	GLuint vertexVBO, normalVBO, indexVBO, VAO;
	GLuint MID, VID, PID, LightPosID, TimeID;


	void planeMeshQuads(float min, float max, float stepsize) {

		// The following coordinate system works as if (min, 0, min) is the origin
		// And then builds up the mesh from that origin down (in z)
		// and then to the right (in x).
		// So, one "row" of the mesh's vertices have a fixed x and increasing z

		//manually create a first column of vertices
		float x = min;
		float y = 0;
		for (float z = min; z <= max; z += stepsize) {
			verts.push_back(x);
			verts.push_back(y);
			verts.push_back(z);
			normals.push_back(0);
			normals.push_back(1);
			normals.push_back(0);
		}

		for (float x = min+stepsize; x <= max; x += stepsize) {
			for (float z = min; z <= max; z += stepsize) {
				verts.push_back(x);
				verts.push_back(y);
				verts.push_back(z);
				normals.push_back(0);
				normals.push_back(1);
				normals.push_back(0);
			}
		}

		int nCols = (max-min)/stepsize + 1;
		int i = 0, j = 0;
		for (float x = min; x < max; x += stepsize) {
			j = 0;
			for (float z = min; z < max; z += stepsize) {
				indices.push_back(i*nCols + j);
				indices.push_back(i*nCols + j + 1);
				indices.push_back((i+1)*nCols + j + 1);
				indices.push_back((i+1)*nCols + j);
				++j;
			}
			++i;
		}


	}

public:

	PlaneMesh(float min, float max, float stepsize) {
		// this->min = min;
		// this->max = max;
		modelColor = glm::vec4(0, 1.0f, 1.0f, 1.0f);

		planeMeshQuads(min, max, stepsize);
		numVerts = verts.size()/3;
		numIndices = indices.size();
		
		//gen and fill buffers
		// Create and bind the VAO
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        // Create and bind the VBO for vertex positions
        glGenBuffers(1, &vertexVBO);
        glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
        glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), &verts[0], GL_STATIC_DRAW);

		// Position attribute
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		// Create and bind the VBO for normals
        glGenBuffers(1, &normalVBO);
        glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
        glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), &normals[0], GL_STATIC_DRAW);

		// Normal attribute
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		// Create and bind the VBO for face indices
        glGenBuffers(1, &indexVBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), &indices[0], GL_STATIC_DRAW);

		// Load the bitmap image
		unsigned char* data;
        GLuint width, height;
		loadBMP("Assets/displacement-map1.bmp", &data, &width, &height);
		glGenTextures(1, &dispTexID);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, dispTexID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);

		// Water texture setup
		unsigned char* data2;
        GLuint width2, height2;
		loadBMP("Assets/water.bmp", &data2, &width2, &height2);
		glGenTextures(1, &waterTexID);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, waterTexID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width2, height2, 0, GL_BGR, GL_UNSIGNED_BYTE, data2);
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
		
		//shaders and uniforms
		programID = LoadShaders( "WaterShader.vertexshader", "WaterShader.tcs", "WaterShader.tes", "WaterShader.geoshader", "WaterShader.fragmentshader");
		MID = glGetUniformLocation(programID, "M");
		VID = glGetUniformLocation(programID, "V");
		PID = glGetUniformLocation(programID, "P");
		LightPosID = glGetUniformLocation(programID, "LightPosition_worldspace");
		TimeID = glGetUniformLocation(programID, "time");

		// unbind
		glBindVertexArray(0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, 0);

		glUseProgram(0);
	}

	void draw(glm::vec3 lightPos, glm::mat4 V, glm::mat4 P) {
		//Bind some stuff
		glPatchParameteri(GL_PATCH_VERTICES, 4);
		// Set the shader program
		glUseProgram(programID);
		
		// Setting the Textures
		glEnable(GL_TEXTURE_2D);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, dispTexID);
		GLuint displacementTextureLocation = glGetUniformLocation(programID, "displacementTexture");
		glUniform1i(displacementTextureLocation, 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, waterTexID);
		GLuint waterTextureLocation = glGetUniformLocation(programID, "waterTexture");
		glUniform1i(waterTextureLocation, 1);

		glm::mat4 M = glm::mat4(1.0f);

		float time = glfwGetTime();

		// Pass the V matrix to the shader
		glUniformMatrix4fv(MID, 1, GL_FALSE, &M[0][0]);
		glUniformMatrix4fv(VID, 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(PID, 1, GL_FALSE, &P[0][0]);
		glUniform3f(LightPosID, lightPos.x, lightPos.y, lightPos.z);
		glUniform1f(TimeID, time);

		// Bind the VAO
		glBindVertexArray(VAO);

		// Draw the mesh
		glDrawElements(GL_PATCHES, numIndices, GL_UNSIGNED_INT, (void*)0);

		// Unbind the VAO
		glBindVertexArray(0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, 0);

		glUseProgram(0);
	}


};
