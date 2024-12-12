#include <GameOfLife.hpp>
#include <chrono>
#include <cstdint>
#include <limits>
#include <mutex>
#include <random>
#include <algorithm>
#include <execution>
#include <span>
#include <barrier>

namespace life
{	
	static const auto doDraw = []() noexcept { GameOfLife::drawCurrentState(); };

	static auto& getBarrier() 
	{
		static std::barrier drawSync{GameOfLife::_numThreads, doDraw};
		return drawSync;
	}

	GameOfLife* GameOfLife::_self;
	
	GameOfLife::GameOfLife(uint64_t w, uint64_t h)
		: currentState(std::vector<CellState>(w * h)),
		previousState(std::vector<CellState>(w * h)),
		worldWidth(w), worldHeight(h)
	{
		GameOfLife::_self = this;
		sAppName = "Game of Life Demo";
		getBarrier();	// Construct the barrier.
	}

    bool GameOfLife::OnUserCreate()
	{
		auto seedTime = std::chrono::steady_clock::now().time_since_epoch().count();

		// Prime the random generator before building the world.
		std::minstd_rand random{ static_cast<unsigned int>(seedTime) };

		auto numCells = worldWidth * worldWidth;

		drawQueue.reserve(numCells);

		std::ranges::generate(currentState, 
		[&random](){
			const auto num = random() % 2;

			if(num == 1) { return CellState::Alive; }			
			return CellState::Dead;
		});

		std::copy(std::execution::par_unseq, currentState.cbegin(), currentState.cend(), previousState.begin());
		
		cam = { .x=0.f, .y=0.f, .w=static_cast<float>(ScreenWidth()), .h=static_cast<float>(ScreenHeight()) };

		static auto simulatPartialWorld = 
		[this](std::span<CellState> states, uint64_t offset)
		{
			std::mutex m;
			while(_runLoop)
			{
				uint64_t x = 0uz, y = offset / worldWidth;

				for(const auto state : states)
				{
					uint8_t neighbors = countNeighbors(x, y);

					if (state == CellState::Alive)
					{
						if (neighbors == 2 || neighbors == 3)
						{
							currentState[(y * worldWidth) + x] = CellState::Alive;
							std::scoped_lock l{_drawQueueLock};
							drawQueue.push_back(CellPosition{.x=x, .y=y});
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
							std::scoped_lock l{_drawQueueLock};
							drawQueue.push_back(CellPosition{.x=x, .y=y});
						}
						else
						{ 
							currentState[(y * worldWidth) + x] = CellState::Dead;
						}
					}

					++x;
					if(x == worldWidth)
					{
						x = 0uz;
						++y;
					}
				}

				getBarrier().arrive_and_wait();

				if(!_simRunning)
				{
					std::unique_lock lock{m};
					_simulationWaitCondition.wait(lock, [this](){ return _simRunning; });
				}
			}
		};

		const auto rowsPerThread = worldHeight / _numThreads;
		const auto rowsExtra = worldHeight % _numThreads;
		const auto chunkSize = rowsPerThread * worldWidth;

		for(auto i = 0u; i < _numThreads - 1uz; ++i)
		{
			_threadPool.emplace_back(simulatPartialWorld, std::span<CellState>{ &previousState[chunkSize * i], chunkSize }, chunkSize * i);
		}

		_threadPool.emplace_back(simulatPartialWorld, std::span<CellState>{ &previousState[chunkSize * 3u], chunkSize + rowsExtra }, chunkSize * (_numThreads - 1uz));

		return true;
	}

    uint8_t GameOfLife::countNeighbors(uint64_t x, uint64_t y)
    {
		auto wrap = [this](uint64_t x, uint64_t y, uint64_t width, uint64_t height)
		{
			if (x == std::numeric_limits<uint64_t>::max() || y == std::numeric_limits<uint64_t>::max()) { return 0u; }
			if (x >= width || y >= height) { return 0u; }

			return (previousState[(y * width) + x] == CellState::Alive) ? 1u : 0u;
		};

		uint8_t result{};

		result += wrap(x - 1, y - 1, worldWidth, worldHeight);
		result += wrap(x, y - 1, worldWidth, worldHeight);
		result += wrap(x + 1, y - 1, worldWidth, worldHeight);
		result += wrap(x - 1, y, worldWidth, worldHeight);
		result += wrap(x + 1, y, worldWidth, worldHeight);
		result += wrap(x - 1, y + 1, worldWidth, worldHeight);
		result += wrap(x, y + 1, worldWidth, worldHeight);
		result += wrap(x + 1, y + 1, worldWidth, worldHeight);

		return result;
	}

    bool GameOfLife::OnUserUpdate(float fElapsedTime)
    {		
		if (GetKey(olc::Key::SPACE).bPressed) 
		{
			_simRunning = !_simRunning;
			if(_simRunning)
			{
				_simulationWaitCondition.notify_all();
			}
		}

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

		return true;
	}

	bool GameOfLife::OnUserDestroy()
	{
		_runLoop = false;
		for(auto& thread : _threadPool)
		{
			thread.join();
		}

		return true;
	}

	void GameOfLife::drawCurrentState() noexcept
	{
		auto* self = GameOfLife::get();

		self->Clear(olc::BLACK);

		std::copy(std::execution::par_unseq, self->currentState.cbegin(), self->currentState.cend(), self->previousState.begin());

		std::for_each(std::execution::par_unseq, self->drawQueue.cbegin(), self->drawQueue.cend(),
		[self](const auto& cell)
		{
			self->Draw(olc::vi2d{ static_cast<int>(cell.x), static_cast<int>(cell.y) });
		});

		self->drawQueue.clear();

	}
}