#include "OpenGLStuff.hpp"

#include <iostream>
#include <sstream>

void GLClearErrors()
{
	while(glGetError() != GL_NO_ERROR);
}

bool GLCheckForError(const char* func, const char* file, int line)
{
	if(GLenum error = glGetError())
	{
		std::cout << "[OpenGL ERROR " << error << "]: " << file << ":" << line << " while calling " << func << std::endl;

		return false;
	}

	return true;
}

VertexBuffer::VertexBuffer(const void* data, uint32_t size)
	: m_ID(0)
{
	GLCall(glGenBuffers(1, &m_ID));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_ID));
	GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW));
}

VertexBuffer::~VertexBuffer()
{
	if(m_ID != 0)
		GLCall(glDeleteBuffers(1, &m_ID));
}

void VertexBuffer::Bind() const
{
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_ID));
}

void VertexBuffer::Unbind() const
{
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

void VertexBuffer::UpdateBuffer(const void* data, uint32_t size)
{
	Bind();
	GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, size, data));
}

IndexBuffer::IndexBuffer(const uint32_t* data, uint32_t count)
	: m_ID(0), m_Count(count)
{
	GLCall(glGenBuffers(1, &m_ID));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), data, GL_DYNAMIC_DRAW));
}

IndexBuffer::~IndexBuffer()
{
	if(m_ID != 0)
		GLCall(glDeleteBuffers(1, &m_ID));
}

void IndexBuffer::Bind() const
{
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID));
}

void IndexBuffer::Unbind() const
{
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

uint32_t VertexBufferElement::GetSizeOfType(uint32_t t)
{
	switch(t)
	{
		case GL_FLOAT:			return sizeof(GLfloat);
		case GL_UNSIGNED_INT:	return sizeof(GLuint);
		case GL_UNSIGNED_BYTE:	return sizeof(GLbyte);
	}

	return 0;
}

VertexBufferLayout::VertexBufferLayout()
	: m_Stride(0)
{}

VertexArray::VertexArray()
	: m_ID(0)
{
	GLCall(glGenVertexArrays(1, &m_ID));
	GLCall(glBindVertexArray(m_ID));
}

VertexArray::~VertexArray()
{
	if(m_ID != 0)
		GLCall(glDeleteVertexArrays(1, &m_ID));
}

void VertexArray::AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout)
{
	Bind();
	vb.Bind();

	const auto& elements = layout.GetElements();
	uint32_t offset = 0;

	for(uint32_t i = 0; i < elements.size(); ++i)
	{
		const auto& element = elements[i];

		GLCall(glEnableVertexAttribArray(i));
		GLCall(glVertexAttribPointer(i, element.count, element.type, element.normalized, layout.GetStride(), (const void*)offset));

		offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
	}
}

void VertexArray::Bind() const
{
	GLCall(glBindVertexArray(m_ID));
}

void VertexArray::Unbind() const
{
	GLCall(glBindVertexArray(0));
}

Shader::Shader(const std::string& vertexShaderFilepath, const std::string& fragmentShaderFilepath)
	: m_ID(0)
{
	m_ID = CreateShader(ParseShader(vertexShaderFilepath), ParseShader(fragmentShaderFilepath));
}

Shader::~Shader()
{
	GLCall(glDeleteProgram(m_ID));
}

void Shader::Bind() const
{
	GLCall(glUseProgram(m_ID));
}

void Shader::Unbind() const
{
	GLCall(glUseProgram(0));
}

void Shader::SetUniformVec4(const std::string& name, const glm::vec4& vec)
{
	GLCall(glUniform4f(GetUniformLocation(name), vec.x, vec.y, vec.z, vec.w));
}

void Shader::SetUniformMat4(const std::string& name, const glm::mat4& mat)
{
	GLCall(glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &mat[0][0]));
}

std::string Shader::ParseShader(const std::string& filepath)
{
	FILE* file;

	fopen_s(&file, filepath.c_str(), "r");

	if(!file)
	{
		std::cout << "[ERROR] Unable to open file: " << filepath << std::endl;

		return "";
	}

	char line[512];
	std::stringstream ss;

	while(fgets(line, 512, file) != NULL)
		ss << line;

	fclose(file);

	return ss.str();
}

uint32_t Shader::CompileShader(uint32_t type, const std::string& sourceCode)
{
	GLCall(uint32_t id = glCreateShader(type));
	const char* src = sourceCode.c_str();
	int result = 0;

	GLCall(glShaderSource(id, 1, &src, nullptr));
	GLCall(glCompileShader(id));
	GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));

	if(result == GL_FALSE)
	{
		int len = 0;

		GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &len));

		char* message = (char*)_malloca(len * sizeof(char));

		GLCall(glGetShaderInfoLog(id, len, &len, message));
		std::cout << "[ERROR] Failed to compile shader:\n" << message << std::endl;
		GLCall(glDeleteShader(id));

		return 0;
	}

	return id;
}

uint32_t Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	GLCall(uint32_t program = glCreateProgram());
	uint32_t vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	uint32_t fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	GLCall(glAttachShader(program, vs));
	GLCall(glAttachShader(program, fs));
	GLCall(glLinkProgram(program));
	GLCall(glValidateProgram(program));

	int success = 0;

	GLCall(glGetProgramiv(program, GL_LINK_STATUS, &success));

	if(success == GL_FALSE)
	{
		int len = 0;

		GLCall(glGetShaderiv(program, GL_INFO_LOG_LENGTH, &len));

		char* message = (char*)_malloca(len * sizeof(char));

		GLCall(glGetShaderInfoLog(program, len, &len, message));

		std::cout << "[ERROR] Failed to link shaders:\n" << message << std::endl;

		GLCall(glDeleteShader(program));

		return 0;
	}

	GLCall(glDeleteShader(vs));
	GLCall(glDeleteShader(fs));

	return program;
}

int Shader::GetUniformLocation(const std::string& name)
{
	if(m_UniformLocations.find(name) != m_UniformLocations.end())
		return m_UniformLocations[name];

	GLCall(int location = glGetUniformLocation(m_ID, name.c_str()));

	if(location == -1)
		std::cout << "[WARNING] Uniform " << name << " does not exist!" << std::endl;
	else
		m_UniformLocations[name] = location;

	return location;
}

void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const
{
	va.Bind();
	ib.Bind();
	shader.Bind();

	GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));

	va.Unbind();
}

void Renderer::Clear(float r, float g, float b, float a) const
{
	GLCall(glClearColor(r, g, b, a));
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}