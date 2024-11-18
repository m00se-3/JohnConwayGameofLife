#include <GameOfLife.hpp>
#include <chrono>
#include <cstdint>
#include <random>
#include <algorithm>
#include <execution>

namespace life
{
    GameOfLife::GameOfLife(uint32_t w, uint32_t h)
		: currentState(std::vector<CellState>(static_cast<size_t>(w) * static_cast<size_t>(h))),
		previousState(std::vector<CellState>(static_cast<size_t>(w) * static_cast<size_t>(h))),
		worldWidth(w), worldHeight(h)
	{
		sAppName = "Game of Life Demo";
	}

    bool GameOfLife::OnUserCreate()
	{
		auto seedTime = std::chrono::steady_clock::now().time_since_epoch().count();

		// Prime the random generator before building the world.
		std::minstd_rand random{ static_cast<unsigned int>(seedTime) };

		auto numCells = static_cast<size_t>(worldWidth) * static_cast<size_t>(worldHeight);

		drawQueue.reserve(numCells);

		std::ranges::generate(currentState, 
		[&random](){
			const auto num = random() % 2;

			if(num == 1) { return CellState::Alive; }			
			return CellState::Dead;
		});
		
		cam = { .x=0.f, .y=0.f, .w=static_cast<float>(ScreenWidth()), .h=static_cast<float>(ScreenHeight()) };

		return true;
	}

    uint8_t GameOfLife::countNeighbors(uint32_t x, uint32_t y)
    {
		auto wrap = [](uint32_t v, uint32_t size)
		{
			if (v == std::numeric_limits<uint32_t>::max()) { return size - 1u; }
			if (v == size) { return 0u; }

			return v;
		};

		uint32_t lx = wrap(x - 1, worldWidth),
			rx = wrap(x + 1, worldWidth),
			ty = wrap(y - 1, worldHeight),
			by = wrap(y + 1, worldHeight);

		uint8_t result{};

		result += static_cast<uint8_t>(previousState[(ty * worldWidth) + lx]);
		result += static_cast<uint8_t>(previousState[(ty * worldWidth) + x]);
		result += static_cast<uint8_t>(previousState[(ty * worldWidth) + rx]);
		result += static_cast<uint8_t>(previousState[(y * worldWidth) + lx]);
		result += static_cast<uint8_t>(previousState[(y * worldWidth) + rx]);
		result += static_cast<uint8_t>(previousState[(by * worldWidth) + lx]);
		result += static_cast<uint8_t>(previousState[(by * worldWidth) + x]);
		result += static_cast<uint8_t>(previousState[(by * worldWidth) + rx]);

		return result;
	}

	void GameOfLife::transformStates()
	{		
		std::copy(std::execution::par_unseq, currentState.cbegin(), currentState.cend(), previousState.begin());

		for (auto y = 0u; y < worldHeight; y++)
		{
			for (auto x = 0u; x < worldWidth; x++)
			{
				uint8_t neighbors = countNeighbors(x, y);
				CellState cell = previousState[(y * worldWidth) + x];

				if (cell == CellState::Alive)
				{
					if (neighbors == 2 || neighbors == 3)
					{
						currentState[(y * worldWidth) + x] = CellState::Alive;
						if (withinView(static_cast<float>(x), static_cast<float>(y))) { drawQueue.push_back({ .x=x, .y=y }); }
					}
					else
					{ 
						currentState[(y * worldWidth) + x] = CellState::Dead;
					}
				}
				else
				{
					if (neighbors == 3)
					{
						currentState[(y * worldWidth) + x] = CellState::Alive;
						if (withinView(static_cast<float>(x), static_cast<float>(y))) { drawQueue.push_back({ .x=x, .y=y }); }
					}
					else
					{ 
						currentState[(y * worldWidth) + x] = CellState::Dead;
					}
				}
			}
		}
	}

    bool GameOfLife::OnUserUpdate(float fElapsedTime)
    {
		if (GetKey(olc::Key::SPACE).bPressed) { simRunning = !simRunning; }
		if (GetKey(olc::Key::W).bHeld) { cam.y -= 100.f * fElapsedTime; }
		if (GetKey(olc::Key::S).bHeld) { cam.y += 100.f * fElapsedTime; }
		if (GetKey(olc::Key::A).bHeld) { cam.x -= 100.f * fElapsedTime; }
		if (GetKey(olc::Key::D).bHeld) { cam.x += 100.f * fElapsedTime; }

		cam.y = std::max(cam.y, 0.f);
		cam.x = std::max(cam.x, 0.f);

		const auto dWidth = static_cast<float>(worldWidth);
		const auto dHeight = static_cast<float>(worldHeight);

		if (cam.y + cam.h > dHeight) { cam.y = dHeight - cam.h; }
		if (cam.x + cam.w > dWidth) { cam.x = dWidth - cam.w; }

		if (simRunning) 
		{
			transformStates();

			Clear(olc::BLACK);

			std::for_each(std::execution::par_unseq, drawQueue.cbegin(), drawQueue.cend(),
			[this](const auto& cell)
			{
				Draw(olc::vi2d{ static_cast<int>(cell.x), static_cast<int>(cell.y) });
			});

			drawQueue.clear();

		}

		return true;
	}
}