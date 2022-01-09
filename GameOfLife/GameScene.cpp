#include "GameScene.hpp"

#include <iostream>
#include <array>

GameScene::GameScene(uint32_t width, uint32_t height)
	: m_OrthoProj(glm::ortho(0.0f, (float)width, 0.0f, (float)height, -1.0f, 1.0f))
	, m_WindowWidth(width)
	, m_WindowHeight(height)
{
	m_Cells.resize((size_t)((m_WindowWidth / m_Scale + 2) * (m_WindowHeight / m_Scale + 2)));

	for(uint32_t i = 0; i < m_WindowWidth / m_Scale + 2; ++i)
	{
		for(uint32_t j = 0; j < m_WindowHeight / m_Scale + 2; ++j)
		{
			glm::mat4 model	   = glm::translate(glm::mat4(1.0f), glm::vec3(i * m_Scale / 2.0f, j * m_Scale / 2.0f, 1.0f));
			glm::vec4 finalPos = m_OrthoProj * model * glm::vec4(i * m_Scale / 2.0f, j * m_Scale / 2.0f, 0.0f, 1.0f);

			m_Cells[(size_t)(i * (m_WindowHeight / m_Scale + 2) + j)] = { { -2.0f / (m_WindowWidth / m_Scale) + finalPos.x, -2.0f / (m_WindowHeight / m_Scale) + finalPos.y }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f } };
		}
	}

	m_Vertices.reserve((size_t)((4 * m_WindowWidth * m_WindowHeight) / (m_Scale * m_Scale)));

	auto ConstructVertex = [](const glm::vec2& pos, const glm::vec4& color)
	{
		Vertex v{ pos, color };

		return v;
	};

	for(uint32_t i = 1; i < m_WindowWidth / m_Scale + 1; ++i)
	{
		for(uint32_t j = 1; j < m_WindowHeight / m_Scale + 1; ++j)
		{
			Cell& cell = m_Cells[(size_t)(i * (m_WindowHeight / m_Scale + 2) + j)];

			m_Vertices.push_back(ConstructVertex(cell.position, cell.color));
			m_Vertices.push_back(ConstructVertex({ cell.position.x + 2.0f / (m_WindowWidth / m_Scale), cell.position.y }, cell.color));
			m_Vertices.push_back(ConstructVertex({ cell.position.x + 2.0f / (m_WindowWidth / m_Scale), cell.position.y + 2.0f / (m_WindowHeight / m_Scale) }, cell.color));
			m_Vertices.push_back(ConstructVertex({ cell.position.x,									   cell.position.y + 2.0f / (m_WindowHeight / m_Scale) }, cell.color));
		}
	}

	std::vector<uint32_t> indices;

	indices.reserve((size_t)((6 * m_WindowWidth * m_WindowHeight) / (m_Scale * m_Scale)));

	for(uint32_t i = 0; i < m_Cells.size() * 4; i += 4)
	{
		indices.push_back(i);
		indices.push_back(i + 1);
		indices.push_back(i + 2);
		indices.push_back(i + 2);
		indices.push_back(i + 3);
		indices.push_back(i);
	}

	m_VAO	 = std::make_unique<VertexArray>();
	m_IBO	 = std::make_unique<IndexBuffer>(indices.data(), indices.size());
	m_Shader = std::make_unique<Shader>("res/shaders/VertexShader.glsl", "res/shaders/FragmentShader.glsl");
	m_VBO	 = std::make_unique<VertexBuffer>(m_Vertices.data(), m_Vertices.size() * sizeof(Vertex));

	VertexBufferLayout layout;

	layout.Push<float>(2);
	layout.Push<float>(4);
	m_VAO->AddBuffer(*m_VBO, layout);
	m_VAO->Unbind();
}

void GameScene::Input()
{
	switch(m_CurrState)
	{
		case State::DRAWING:
			HandleDrawingInput();
			break;

		case State::RUNNING:
			HandleRunningInput();
			break;

		case State::PAUSED:
			HandlePausedInput();
			break;
			
		default:
			break;
	}
}

void GameScene::Update(float dt)
{
	if(m_CurrState == State::PAUSED)
		return;

	if(m_CurrState == State::RUNNING && m_Counter > 0.1f)
	{
		UpdateGeneration();
		m_Counter = 0.0f;
	}
	else
	{
		for(uint32_t i = 1; i < m_WindowWidth / m_Scale + 1; ++i)
		{
			for(uint32_t j = 1; j < m_WindowHeight / m_Scale + 1; ++j)
			{
				Cell& cell = m_Cells[i * (m_WindowHeight / m_Scale + 2) + j];
				
				m_Vertices[((i - 1) * (m_WindowHeight / m_Scale) + (j - 1)) * 4 + 0].color = cell.color;
				m_Vertices[((i - 1) * (m_WindowHeight / m_Scale) + (j - 1)) * 4 + 1].color = cell.color;
				m_Vertices[((i - 1) * (m_WindowHeight / m_Scale) + (j - 1)) * 4 + 2].color = cell.color;
				m_Vertices[((i - 1) * (m_WindowHeight / m_Scale) + (j - 1)) * 4 + 3].color = cell.color;
			}
		}
	}

	m_VBO->UpdateBuffer(m_Vertices.data(), m_Vertices.size() * sizeof(Vertex));
	m_Counter += dt;
}

void GameScene::Render()
{
	m_Renderer.Clear(1.0f, 0.0f, 0.0f, 1.0f);
	m_Renderer.Draw(*m_VAO, *m_IBO, *m_Shader);
}

void GameScene::UpdateGeneration()
{
	struct Coord
	{
		int8_t x;
		int8_t y;
	};

	static const std::array<Coord, 8> neighbours{ { { -1, -1 }, { -1, 0 }, { -1, 1 }, { 0, 1 }, { 1, 1 }, { 1, 0 }, { 1, -1 }, { 0, -1 } } };
	
	for(uint32_t i = 1; i < m_WindowWidth / m_Scale + 1; ++i)
	{
		for(uint32_t j = 1; j < m_WindowHeight / m_Scale + 1; ++j)
		{
			Cell& cell = m_Cells[i * (m_WindowHeight / m_Scale + 2) + j];
			uint8_t neighboursAlive = 0;

			for(auto& n : neighbours)
				neighboursAlive += 1 - (uint8_t)m_Cells[(i + n.x) * (m_WindowHeight / m_Scale + 2) + j + n.y].color.r;

			if(cell.color.r == 0.0f)
				cell.nextColor = (neighboursAlive == 2 || neighboursAlive == 3) ? cell.color : glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
			else
				cell.nextColor = neighboursAlive == 3 ? glm::vec4(0.0f, 0.0f, 0.0f, 1.0f) : cell.color;

			m_Vertices[((i - 1) * (m_WindowHeight / m_Scale) + (j - 1)) * 4 + 0].color = cell.nextColor;
			m_Vertices[((i - 1) * (m_WindowHeight / m_Scale) + (j - 1)) * 4 + 1].color = cell.nextColor;
			m_Vertices[((i - 1) * (m_WindowHeight / m_Scale) + (j - 1)) * 4 + 2].color = cell.nextColor;
			m_Vertices[((i - 1) * (m_WindowHeight / m_Scale) + (j - 1)) * 4 + 3].color = cell.nextColor;
		}
	}

	for(auto& cell : m_Cells)
		cell.color = cell.nextColor;
}

void GameScene::HandleDrawingInput()
{
	if(glfwGetKey(GetWindow(), GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		if(m_PrevHoveredCell)
			UpdatePreviousCellColor();

		m_CurrState = State::RUNNING;

		return;
	}

	if(glfwGetKey(GetWindow(), GLFW_KEY_C) == GLFW_PRESS)
	{
		ResetBoard();

		return;
	}
	
	double xMousePos = 0.0;
	double yMousePos = 0.0;

	glfwGetCursorPos(GetWindow(), &xMousePos, &yMousePos);

	uint32_t translatedPosX = (uint32_t)xMousePos / m_Scale + 1;
	uint32_t translatedPosY = (uint32_t)(m_WindowHeight - yMousePos) / m_Scale + 1;

	if(translatedPosX <= m_WindowWidth / m_Scale && translatedPosY <= m_WindowHeight / m_Scale)
	{
		Cell* currentlyHoveredCell = &m_Cells[translatedPosX * (m_WindowHeight / m_Scale + 2) + translatedPosY];

		if(currentlyHoveredCell != m_PrevHoveredCell)
		{
			if(m_PrevHoveredCell)
				UpdatePreviousCellColor();

			if(currentlyHoveredCell->color.r == 0.0f)
				currentlyHoveredCell->color = glm::vec4(0.7f, 0.7f, 0.7f, 1.0f);
			else
				currentlyHoveredCell->color = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);

			m_PrevHoveredCell = currentlyHoveredCell;
		}

		if(glfwGetMouseButton(GetWindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
			currentlyHoveredCell->color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		else if(glfwGetMouseButton(GetWindow(), GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
			currentlyHoveredCell->color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	}
}

void GameScene::HandleRunningInput()
{
	if(glfwGetKey(GetWindow(), GLFW_KEY_P) == GLFW_PRESS)
		m_CurrState = State::PAUSED;
	else if(glfwGetKey(GetWindow(), GLFW_KEY_R) == GLFW_PRESS)
		m_CurrState = State::DRAWING;
}

void GameScene::HandlePausedInput()
{
	if(glfwGetKey(GetWindow(), GLFW_KEY_U) == GLFW_PRESS)
		m_CurrState = State::RUNNING;
	else if(glfwGetKey(GetWindow(), GLFW_KEY_R) == GLFW_PRESS)
	{
		ResetBoard();
		m_CurrState = State::DRAWING;
	}
}

void GameScene::UpdatePreviousCellColor()
{
	if(m_PrevHoveredCell->color.r == 0.7f)
		m_PrevHoveredCell->color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	else if(m_PrevHoveredCell->color.r == 0.2f)
		m_PrevHoveredCell->color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
}

void GameScene::ResetBoard()
{
	for(uint32_t i = 0; i < m_WindowWidth / m_Scale + 2; ++i)
	{
		for(uint32_t j = 0; j < m_WindowHeight / m_Scale + 2; ++j)
		{
			m_Cells[(size_t)(i * (m_WindowHeight / m_Scale + 2) + j)].color = m_Cells[(size_t)(i * (m_WindowHeight / m_Scale + 2) + j)].nextColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		}
	}
}
