#pragma once

#include "App.hpp"
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <memory>

struct Vertex
{
	glm::vec2 position;
	glm::vec4 color;
};

struct Cell
{
	glm::vec2 position;
	glm::vec4 color;
	glm::vec4 nextColor;
};

enum class State
{
	DRAWING,
	RUNNING,
	PAUSED
};

class GameScene
{
	public:
		GameScene(uint32_t width, uint32_t height);

		void Input();
		void Update(float dt);
		void Render();

	private:
		std::vector<Vertex> m_Vertices;
		std::vector<Cell>	m_Cells;

		Cell* m_PrevHoveredCell = nullptr;

		std::unique_ptr<VertexBuffer> m_VBO;
		std::unique_ptr<VertexArray>  m_VAO;
		std::unique_ptr<IndexBuffer>  m_IBO;
		std::unique_ptr<Shader>		  m_Shader;

		Renderer m_Renderer;

		glm::mat4 m_OrthoProj;

		uint32_t m_WindowWidth;
		uint32_t m_WindowHeight;
		
		float m_Scale	= 10.0f;
		float m_Counter = 0.0f;

		State m_CurrState = State::DRAWING;

		void UpdateGeneration();

		void HandleDrawingInput();
		void HandleRunningInput();
		void HandlePausedInput();

		void UpdatePreviousCellColor();

		void ResetBoard();
};