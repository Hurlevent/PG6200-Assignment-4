#include "GameManager.h"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <assert.h>
#include <stdexcept>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <IL/il.h>
#include <IL/ilu.h>
#include "GlTools.h"

using std::cerr;
using std::endl;
using GLUtils::VBO;
using GLUtils::Program;
using GLUtils::readFile;


GameManager::GameManager() : m_rendering_mode(RENDERING_REGULAR), m_rotating(false), m_rotation_speed(2.0f) {
	my_timer.restart();
	mesh = createTriangleStripMesh(2, 2);
}

GameManager::~GameManager() {
}

void GameManager::createOpenGLContext() {
	//Set OpenGL major an minor versions
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	// Set OpenGL attributes
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); // Use double buffering
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16); // Use framebuffer with 16 bit depth buffer
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8); // Use framebuffer with 8 bit for red
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8); // Use framebuffer with 8 bit for green
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8); // Use framebuffer with 8 bit for blue
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8); // Use framebuffer with 8 bit for alpha
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

	// Initalize video
	main_window = SDL_CreateWindow("Westerdals - PG6200 Assignment 4", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		window_width, window_height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	if (!main_window) {
		THROW_EXCEPTION("SDL_CreateWindow failed");
	}

	//Create OpenGL context
	main_context = SDL_GL_CreateContext(main_window);

	// Init glew
	// glewExperimental is required in openGL 3.3 
	// to create forward compatible contexts 
	glewExperimental = GL_TRUE;
	GLenum glewErr = glewInit();
	if (glewErr != GLEW_OK) {
		std::stringstream err;
		err << "Error initializing GLEW: " << glewGetErrorString(glewErr);
		THROW_EXCEPTION(err.str());
	}

	// Unfortunately glewInit generates an OpenGL error, but does what it's
	// supposed to (setting function pointers for core functionality).
	// Lets do the ugly thing of swallowing the error....
	glGetError();

	std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
}

void GameManager::setOpenGLStates() {
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_PRIMITIVE_RESTART);
	glClearColor(0.0, 0.0, 0.5, 1.0);
	glViewport(0, 0, window_width, window_height);
}

// HUSK � TRANSFORMERE NORMALER
// ALLE VERTEXER TRENGER EN NORMAL!

void GameManager::createMatrices() {
	projection_matrix = glm::perspective(45.0f, window_width / (float) window_height, 0.2f, 3.0f);

	model_matrix = glm::mat4(1.0);
	//model_matrix = glm::rotate(model_matrix, 90.0f, glm::vec3(0.0f, 0.0f, 0.0f));
	//model_matrix = glm::rotate(model_matrix, 25.0f, glm::vec3(0.0f, 1.0f, 0.0f)); // Kan slettes
	model_matrix = glm::translate(model_matrix, glm::vec3(-0.5f, -0.5f, -0.5f));

	view_matrix = glm::mat4(1.0f);
	//view_matrix = glm::rotate(view_matrix, 25.0f, glm::vec3(1.0f, 0.0f, 0.0f)); // Kan slettesre
	view_matrix = glm::translate(view_matrix, glm::vec3(0.0f, 0.0f, -1.0f));

	normals = glm::vec3(0.0f, 0.0f, 1.0f);

	light_pos = glm::vec3(0.0f, 200.0f, 200.0f);

}

void GameManager::createSimpleProgram() {
	std::string fs_src = readFile("shaders/regular.frag");
	std::string vs_src = readFile("shaders/regular.vert");

	//Compile shaders, attach to program object, and link
	regular_program.reset(new Program(vs_src, fs_src));

	//Set uniforms for the program.
	regular_program->use();

	glUniformMatrix4fv(regular_program->getUniform("reg_projection"), 1, 0, glm::value_ptr(projection_matrix));
	glUniformMatrix4fv(regular_program->getUniform("reg_view"), 1, 0, glm::value_ptr(view_matrix));
	glUniformMatrix4fv(regular_program->getUniform("reg_model"), 1, 0, glm::value_ptr(model_matrix));
	glUniform3fv(regular_program->getUniform("reg_light_pos"), 1, glm::value_ptr(light_pos));
	
	regular_program->disuse();

	normal_program.reset(new Program(readFile("shaders/normal.vert"), readFile("shaders/normal.frag")));

	normal_program->use();

	some_transform_matrix_for_normal_program = glm::mat4(1.0f);
	glUniformMatrix4fv(normal_program->getUniform("n_some_transform_matrix"), 1, 0, glm::value_ptr(some_transform_matrix_for_normal_program));
	glUniform3fv(normal_program->getUniform("n_normal"), 1, glm::value_ptr(normals));

	normal_program->disuse();

	normal_map_program.reset(new Program(readFile("shaders/normal_mapping.vert"), readFile("shaders/normal_mapping.frag")));

	normal_map_program->use();

	glUniformMatrix4fv(normal_map_program->getUniform("nm_projection"), 1, 0, glm::value_ptr(projection_matrix));
	glUniformMatrix4fv(normal_map_program->getUniform("nm_view"), 1, 0, glm::value_ptr(view_matrix));
	glUniformMatrix4fv(normal_map_program->getUniform("nm_model"), 1, 0, glm::value_ptr(model_matrix));
	glUniform3fv(normal_map_program->getUniform("nm_light_pos"), 1, glm::value_ptr(light_pos));

	normal_map_program->disuse();

	bump_map_program.reset(new Program(readFile("shaders/bump.vert"), readFile("shaders/bump.frag")));

	bump_map_program->use();

	glUniformMatrix4fv(bump_map_program->getUniform("bump_projection"), 1, 0, glm::value_ptr(projection_matrix));
	glUniformMatrix4fv(bump_map_program->getUniform("bump_view"), 1, 0, glm::value_ptr(view_matrix));
	glUniformMatrix4fv(bump_map_program->getUniform("bump_model"), 1, 0, glm::value_ptr(model_matrix));
	glUniform3fv(bump_map_program->getUniform("bump_light_pos"), 1, glm::value_ptr(light_pos));

	bump_map_program->disuse();

	CHECK_GL_ERROR();
}

void GameManager::createVAO() {
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	CHECK_GL_ERROR();

	//Generate VBO
	glGenBuffers(1, &vertex_bo);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_bo);
	glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size()*sizeof(float), mesh.vertices.data(), GL_STATIC_DRAW);

	//Generate IBO
	glGenBuffers(1, &index_bo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_bo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), mesh.indices.data(), GL_STATIC_DRAW);

	//Set input to the shader
	regular_program->setAttributePointer("in_Position", 2, GL_FLOAT, GL_FALSE, 0, 0);
	CHECK_GL_ERROR();

	normal_program->setAttributePointer("in_Position", 2, GL_FLOAT, GL_FALSE, 0, 0);
	CHECK_GL_ERROR();

	bump_map_program->setAttributePointer("in_Position", 2, GL_FLOAT, GL_FALSE, 0, 0);
	CHECK_GL_ERROR();


	//NORMALS

	glGenBuffers(1, &normal_bo);
	glBindBuffer(GL_ARRAY_BUFFER, normal_bo);
	glBufferData(GL_ARRAY_BUFFER, mesh.normals.size() * sizeof(float), mesh.normals.data(), GL_STATIC_DRAW);

	// set normal input for regular shader program!
	regular_program->setAttributePointer("in_Normal", 2, GL_FLOAT, GL_FALSE, 0, 0);
	CHECK_GL_ERROR();



	//Unbind VBOs and VAO
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); 
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	CHECK_GL_ERROR();
}

void GameManager::createNormalFBO()
{
	normal_map.reset(new GLUtils::FBO(window_width, window_height));
	bump_map.reset(new GLUtils::FBO(window_width, window_height));

	CHECK_GL_ERROR();
}

void GameManager::init() {

	// Initialize SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		std::stringstream err;
		err << "Could not initialize SDL: " << SDL_GetError();
		THROW_EXCEPTION(err.str());
	}
	atexit( SDL_Quit);

	createOpenGLContext();
	setOpenGLStates();
	createMatrices();
	createSimpleProgram();
	createVAO();
	
	createNormalFBO();

	CHECK_GL_ERROR();

	//Initialize IL and ILU
	ilInit();
	iluInit();


	color_texture = loadTexture("resources/color_texture.jpg");

	bump_map_texture = loadTexture("resources/normal_map.jpg");

}

void GameManager::render() {
	double elapsed = my_timer.elapsedAndRestart();
	float rotate_degrees = static_cast<float>(elapsed) * m_rotation_speed;

	//Clear screen, and set the correct program
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Rotate the model
	if (m_rotating) {
		model_matrix = glm::translate(model_matrix, glm::vec3(0.5f, 0.5f, 0.0f));
		model_matrix = glm::rotate(model_matrix, rotate_degrees, glm::vec3(0.0f, 1.0f, 0.0f));
		model_matrix = glm::translate(model_matrix, glm::vec3(-0.5f, -0.5f, 0.0f));
	}
	
	glm::mat4 transpose_inverse_modelview = glm::transpose(glm::inverse(view_matrix * model_matrix));

	if (m_rendering_mode == RENDERING_REGULAR) {
		regular_program->use();
	
		glUniformMatrix4fv(regular_program->getUniform("reg_model"), 1, 0, glm::value_ptr(model_matrix));
		glUniformMatrix4fv(regular_program->getUniform("reg_transpose_inverse_modelview"), 1, 0, glm::value_ptr(transpose_inverse_modelview));



		glActiveTexture(GL_TEXTURE0);
		CHECK_GL_ERROR();
		glBindTexture(GL_TEXTURE_2D, color_texture); 
		CHECK_GL_ERROR();
		glUniform1i(regular_program->getUniform("reg_color_texture"), 0);
		CHECK_GL_ERROR();

	//Render geometry
		glPrimitiveRestartIndex(mesh.restart_token);
		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLE_STRIP, mesh.indices.size(), GL_UNSIGNED_INT, BUFFER_OFFSET(0));
		CHECK_GL_ERROR();

		glBindTexture(GL_TEXTURE_2D, 0);
		regular_program->disuse();
		CHECK_GL_ERROR();
	} else if(m_rendering_mode == RENDERING_NORMAL)
	{
		normal_map_program->use();
		
		
		glUniformMatrix4fv(normal_map_program->getUniform("nm_model"), 1, 0, glm::value_ptr(model_matrix));
		glUniformMatrix4fv(normal_map_program->getUniform("nm_transpose_inverse_modelview"), 1, 0, glm::value_ptr(transpose_inverse_modelview));

		glActiveTexture(GL_TEXTURE0);
		CHECK_GL_ERROR();
		glBindTexture(GL_TEXTURE_2D, color_texture);
		CHECK_GL_ERROR();
		glUniform1i(normal_map_program->getUniform("nm_color_texture"), 0);
		CHECK_GL_ERROR();
		
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, normal_map->getTexture());
		glUniform1i(normal_map_program->getUniform("nm_normal_map"), 1);
		CHECK_GL_ERROR();
		
		//Render geometry
		glPrimitiveRestartIndex(mesh.restart_token);
		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLE_STRIP, mesh.indices.size(), GL_UNSIGNED_INT, BUFFER_OFFSET(0));
		CHECK_GL_ERROR();

		glBindTexture(GL_TEXTURE_2D, 0);
		normal_map_program->disuse();
		CHECK_GL_ERROR();
	} else
	{
		bump_map_program->use();

		glUniformMatrix4fv(bump_map_program->getUniform("bump_model"), 1, 0, glm::value_ptr(model_matrix));
		glUniformMatrix4fv(bump_map_program->getUniform("bump_transpose_inverse_modelview"), 1, 0, glm::value_ptr(transpose_inverse_modelview));

		glActiveTexture(GL_TEXTURE0);
		CHECK_GL_ERROR();
		glBindTexture(GL_TEXTURE_2D, color_texture);
		CHECK_GL_ERROR();
		glUniform1i(bump_map_program->getUniform("bump_color_texture"), 0);
		CHECK_GL_ERROR();

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, bump_map_texture);
		glUniform1i(bump_map_program->getUniform("bump_normal_map"), 1);
		CHECK_GL_ERROR();

		//Render geometry
		glPrimitiveRestartIndex(mesh.restart_token);
		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLE_STRIP, mesh.indices.size(), GL_UNSIGNED_INT, BUFFER_OFFSET(0));
		CHECK_GL_ERROR();

		glBindTexture(GL_TEXTURE_2D, 0);
		bump_map_program->disuse();
		CHECK_GL_ERROR();
	}
}

void GameManager::renderNormals(bool use_bumb_map)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	normal_map->bind();
	normal_program->use();

	//Render geometry
	glPrimitiveRestartIndex(mesh.restart_token);
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLE_STRIP, mesh.indices.size(), GL_UNSIGNED_INT, BUFFER_OFFSET(0));
	CHECK_GL_ERROR();

	normal_program->disuse();
	normal_map->unbind();
}

void GameManager::play() {
	bool doExit = false;

	//SDL main loop
	while (!doExit) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {// poll for pending events
			switch (event.type) {
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_ESCAPE) //Esc
					doExit = true;
				if (event.key.keysym.sym == SDLK_q
						&& event.key.keysym.mod & KMOD_CTRL) //Ctrl+q
					doExit = true;
				if (event.key.keysym.sym == SDLK_1) {
					m_rendering_mode = RENDERING_REGULAR;
					std::cout << "Regular rendering" << std::endl;
				}
				if (event.key.keysym.sym == SDLK_2) {
					m_rendering_mode = RENDERING_NORMAL;
					std::cout << "Normalmap rendering" << std::endl;
				}
				if (event.key.keysym.sym == SDLK_3) {
					m_rendering_mode = RENDERING_BUMP;
					std::cout << "bumpmap rendering" << std::endl;
				}
				if (event.key.keysym.sym == SDLK_SPACE)
					m_rotating = !m_rotating;
				if (event.key.keysym.sym == SDLK_x) {
					m_rotation_speed += 1.0f;
				}
				if (event.key.keysym.sym == SDLK_z) {
					m_rotation_speed -= 1.0f;
				}
				break;
			case SDL_QUIT: //e.g., user clicks the upper right x
				doExit = true;
				break;
			}
		}

		//Render, and swap front and back buffers
		renderNormals(false);
		render();
		SDL_GL_SwapWindow(main_window);
	}
	quit();
}

void GameManager::quit() {
	std::cout << "Bye bye..." << std::endl;
}

GameManager::TerrainMesh GameManager::createTriangleStripMesh(unsigned int nx, unsigned int ny) {
	TerrainMesh mesh;
	
	//Set the restart token: this index will never appear in the mesh.indeces-vector, unless 
	//it means reset, and start a new primitive (triangle strip)
	mesh.restart_token = nx*ny*2+1;

	float dx = 1.0f/static_cast<float>(nx);
	float dy = 1.0f/static_cast<float>(ny);

	//Vertices
	mesh.vertices.reserve((nx+1)*(ny+1));
	for (unsigned int j=0; j<=ny; ++j) {
		for (unsigned int i=0; i<=nx; ++i) {
			mesh.vertices.push_back(i*dx);	//x
			mesh.vertices.push_back(j*dy);	//y
			mesh.normals.push_back(0.0f);
			mesh.normals.push_back(1.0f);
		}
	}

	//Indices
	for (unsigned int j=0; j<ny; ++j) {
		for (unsigned int i=0; i<=nx; ++i) {
			mesh.indices.push_back(    j*(nx+1)+i);
			mesh.indices.push_back((j+1)*(nx+1)+i);
		}
		//Finished with one triangle strip: restart
		mesh.indices.push_back(mesh.restart_token); 
	}

	return mesh;
}

GLuint GameManager::loadTexture(std::string filename) {
	std::vector<unsigned char> data;
	ILuint ImageName;
	unsigned int width, height;
	GLuint texture;
		
	ilGenImages(1, &ImageName); // Grab a new image name.
	ilBindImage(ImageName);

	if (!ilLoadImage(filename.c_str())) {
		ILenum e;
		std::stringstream error;
		while ((e = ilGetError()) != IL_NO_ERROR) {
			error << e << ": " << iluErrorString(e) << std::endl;
		}
		ilDeleteImages(1, &ImageName); // Delete the image name. 
		throw std::runtime_error(error.str());
	}
				
	width = ilGetInteger(IL_IMAGE_WIDTH); // getting image width
	height = ilGetInteger(IL_IMAGE_HEIGHT); // and height
	data.resize(width*height*3);
				
	ilCopyPixels(0, 0, 0, width, height, 1, IL_RGB, IL_UNSIGNED_BYTE, data.data());
	ilDeleteImages(1, &ImageName); // Delete the image name. 
		
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data.data());
	glBindTexture(GL_TEXTURE_2D, 0);
	CHECK_GL_ERROR();

	return texture;
}