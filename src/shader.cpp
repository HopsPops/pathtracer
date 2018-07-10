#include <shader.hpp>
#include <array>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

unique_ptr<Shader> Shader::loadFromFile(const char* vertexPath, const char* fragmentPath) {
	Shader* shader = new Shader();
	string vertexCode;
	string fragmentCode;

	ifstream vShaderFile;
	ifstream fShaderFile;

	vShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
	fShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
	try {
		printf("loading vertex: %s\n", vertexPath);
		vShaderFile.open(vertexPath);
		printf("loading fragment: %s\n", fragmentPath);
		fShaderFile.open(fragmentPath);

		stringstream vShaderStream, fShaderStream;

		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();

		vShaderFile.close();
		fShaderFile.close();

		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	} catch (ifstream::failure& e) {
		cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << endl;
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

	shader->id = glCreateProgram();
	glAttachShader(shader->id, vertex);
	glAttachShader(shader->id, fragment);
	glLinkProgram(shader->id);
	checkCompileErrors(shader->id, "PROGRAM");

	glDeleteShader(vertex);
	glDeleteShader(fragment);

	return unique_ptr<Shader>(shader);
}

unique_ptr<Shader> Shader::loadRaw(const char* vertexCode, const char* fragmentCode) {
	Shader* shader = new Shader();

	unsigned int vertex = 0, fragment = 0;

	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vertexCode, NULL);
	glCompileShader(vertex);
	checkCompileErrors(vertex, "VERTEX");

	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fragmentCode, NULL);
	glCompileShader(fragment);
	checkCompileErrors(fragment, "FRAGMENT");

	shader->id = glCreateProgram();
	glAttachShader(shader->id, vertex);
	glAttachShader(shader->id, fragment);
	glLinkProgram(shader->id);
	checkCompileErrors(shader->id, "PROGRAM");

	glDeleteShader(vertex);
	glDeleteShader(fragment);

	return unique_ptr<Shader>(shader);
}

unsigned int Shader::getId() const {
	return id;
}

void Shader::use() {
	glUseProgram(id);
}

void Shader::set(const string& name, bool value) const {
	glUniform1i(glGetUniformLocation(id, name.c_str()), (int) value);
}

void Shader::set(const string& name, int value) const {
	glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::set(const string& name, float value) const {
	glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::set(const string& name, const Vector2& value) const {
	glUniform2fv(glGetUniformLocation(id, name.c_str()), 1, &value.x);
}

void Shader::set(const string& name, float x, float y) const {
	glUniform2f(glGetUniformLocation(id, name.c_str()), x, y);
}

void Shader::set(const string& name, const Vector3& value) const {
	glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, &value.x);
}

void Shader::set(const string& name, float x, float y, float z) const {
	glUniform3f(glGetUniformLocation(id, name.c_str()), x, y, z);
}

void Shader::set(const string& name, const Vector4&value) const {
	glUniform4fv(glGetUniformLocation(id, name.c_str()), 1, &value.x);
}

void Shader::set(const string& name, float x, float y, float z, float w) {
	glUniform4f(glGetUniformLocation(id, name.c_str()), x, y, z, w);
}

void Shader::set(const string& name, const Matrix4x4& mat) const {
	glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_TRUE, mat.getData().data());
}

Shader::Shader() {
}

void Shader::checkCompileErrors(GLuint shader, const string& type) {
	GLint success;
	GLchar infoLog[1024];
	if (type != "PROGRAM") {
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << endl;
		}
	} else {
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << endl;
		}
	}
}
