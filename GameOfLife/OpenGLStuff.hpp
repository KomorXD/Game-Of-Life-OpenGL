#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <vector>
#include <unordered_map>

#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#define ASSERT(x) if(!(x)) __debugbreak();
#define GLCall(f) GLClearErrors();\
	f;\
	ASSERT(GLCheckForError(#f, __FILENAME__, __LINE__))

void GLClearErrors();
bool GLCheckForError(const char* func, const char* file, int line);

class VertexBuffer
{
	public:
		VertexBuffer(const void* data, uint32_t size);
		~VertexBuffer();

		void Bind() const;
		void Unbind() const;

		void UpdateBuffer(const void* data, uint32_t size);

	private:
		uint32_t m_ID;
};

class IndexBuffer
{
	public:
		IndexBuffer(const uint32_t* data, uint32_t count);
		~IndexBuffer();

		void Bind() const;
		void Unbind() const;

		inline const uint32_t GetCount() const { return m_Count; }

	private:
		uint32_t m_ID;
		uint32_t m_Count;
};

struct VertexBufferElement
{
	uint32_t type;
	uint32_t count;
	uint8_t  normalized;

	static uint32_t GetSizeOfType(uint32_t t);
};

class VertexBufferLayout
{
	public:
		VertexBufferLayout();

		template<typename T>
		void Push(uint32_t count)
		{
			static_assert(false);
		}

		template<>
		void Push<float>(uint32_t count)
		{
			m_Elements.push_back({ GL_FLOAT, count, GL_FALSE });
			m_Stride += count * sizeof(GLfloat);
		}

		template<>
		void Push<uint32_t>(uint32_t count)
		{
			m_Elements.push_back({ GL_UNSIGNED_INT, count, GL_FALSE });
			m_Stride += count * sizeof(GLuint);
		}

		template<>
		void Push<uint8_t>(uint32_t count)
		{
			m_Elements.push_back({ GL_UNSIGNED_BYTE, count, GL_FALSE });
			m_Stride += count * sizeof(GLbyte);
		}

		inline const std::vector<VertexBufferElement>& GetElements() const { return m_Elements; }
		inline const uint32_t GetStride() const { return m_Stride; }

	private:
		std::vector<VertexBufferElement> m_Elements;
		uint32_t m_Stride;
};

class VertexArray
{
	public:
		VertexArray();
		~VertexArray();

		void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout);
		void Bind() const;
		void Unbind() const;

	private:
		uint32_t m_ID;
};

class Shader
{
	public:
		Shader(const std::string& vs, const std::string& fs);
		~Shader();

		void Bind() const;
		void Unbind() const;

		void SetUniformVec4(const std::string& name, const glm::vec4& vec);
		void SetUniformMat4(const std::string& name, const glm::mat4& mat);

	private:
		uint32_t m_ID;
		std::unordered_map<std::string, int32_t> m_UniformLocations;

		std::string ParseShader(const std::string& filepath);
		uint32_t CompileShader(uint32_t type, const std::string& filepath);
		uint32_t CreateShader(const std::string& vs, const std::string& fs);

		int32_t GetUniformLocation(const std::string& name);
};

class Renderer
{
	public:
		void Clear(float r, float g, float b, float a) const;
		void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;
};