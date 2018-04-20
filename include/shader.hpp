#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <glm/mat4x4.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat2x2.hpp>

class shader {
	public:
		shader(const char* vertexPath, const char* fragmentPath) {
			std::string vertexCode;
			std::string fragmentCode;

			std::ifstream vShaderFile;
			std::ifstream fShaderFile;

			vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
			fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
			try {
				vShaderFile.open(vertexPath);
				fShaderFile.open(fragmentPath);

				std::stringstream vShaderStream, fShaderStream;

				vShaderStream << vShaderFile.rdbuf();
				fShaderStream << fShaderFile.rdbuf();

				vShaderFile.close();
				fShaderFile.close();

				vertexCode = vShaderStream.str();
				fragmentCode = fShaderStream.str();
			} catch (std::ifstream::failure& e) {
				std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
			}
			const char* vShaderCode = vertexCode.c_str();
			const char * fShaderCode = fragmentCode.c_str();
			unsigned int vertex, fragment;

			vertex = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(vertex, 1, &vShaderCode, NULL);
			glCompileShader(vertex);
			checkCompileErrors(vertex, "VERTEX");

			fragment = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(fragment, 1, &fShaderCode, NULL);
			glCompileShader(fragment);
			checkCompileErrors(fragment, "FRAGMENT");

			id = glCreateProgram();
			glAttachShader(id, vertex);
			glAttachShader(id, fragment);
			glLinkProgram(id);
			checkCompileErrors(id, "PROGRAM");

			glDeleteShader(vertex);
			glDeleteShader(fragment);

		}

		unsigned int getId() const {
			return id;
		}

		void use() {
			glUseProgram(id);
		}

		void set(const std::string &name, bool value) const {
			glUniform1i(glGetUniformLocation(id, name.c_str()), (int) value);
		}

		void set(const std::string &name, int value) const {
			glUniform1i(glGetUniformLocation(id, name.c_str()), value);
		}

		void set(const std::string &name, float value) const {
			glUniform1f(glGetUniformLocation(id, name.c_str()), value);
		}

		void set(const std::string &name, const glm::vec2 &value) const {
			glUniform2fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
		}

		void set(const std::string &name, float x, float y) const {
			glUniform2f(glGetUniformLocation(id, name.c_str()), x, y);
		}

		void set(const std::string &name, const glm::vec3 &value) const {
			glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
		}

		void set(const std::string &name, float x, float y, float z) const {
			glUniform3f(glGetUniformLocation(id, name.c_str()), x, y, z);
		}

		void set(const std::string &name, const glm::vec4 &value) const {
			glUniform4fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
		}

		void set(const std::string &name, float x, float y, float z, float w) {
			glUniform4f(glGetUniformLocation(id, name.c_str()), x, y, z, w);
		}

		void set(const std::string &name, const glm::mat2 &mat) const {
			glUniformMatrix2fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
		}

		void set(const std::string &name, const glm::mat3 &mat) const {
			glUniformMatrix3fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
		}

		void set(const std::string &name, const glm::mat4 &mat) const {
			glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
		}

	private:
		unsigned int id;
		void checkCompileErrors(GLuint shader, std::string type) {
			GLint success;
			GLchar infoLog[1024];
			if (type != "PROGRAM") {
				glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
				if (!success) {
					glGetShaderInfoLog(shader, 1024, NULL, infoLog);
					std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
				}
			} else {
				glGetProgramiv(shader, GL_LINK_STATUS, &success);
				if (!success) {
					glGetProgramInfoLog(shader, 1024, NULL, infoLog);
					std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
				}
			}
		}
};
