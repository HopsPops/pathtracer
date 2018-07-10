#pragma once

#include <glad.h>
#include <math.hpp>
#include <memory>
#include <string>

class Shader {

	private:
		unsigned int id = 0;

		Shader();

		static void checkCompileErrors(GLuint shader, const std::string&);
	public:
		static std::unique_ptr<Shader> loadFromFile(const char*, const char*);

		static std::unique_ptr<Shader> loadRaw(const char*, const char*);

		unsigned int getId() const;

		void use();

		void set(const std::string&, bool) const;
		void set(const std::string&, int) const;
		void set(const std::string&, float) const;
		void set(const std::string&, const Vector2&) const;
		void set(const std::string&, float, float) const;
		void set(const std::string&, const Vector3&) const;
		void set(const std::string&, float, float, float) const;
		void set(const std::string&, const Vector4&) const;
		void set(const std::string&, float, float, float, float);
		void set(const std::string&, const Matrix4x4&) const;
};
