#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include <random>
// olcPixelGameEngine already includes <vector> <chrono> and <iostream>.

constexpr uint8_t cellAlive = 1;
constexpr uint8_t cellDead = 0;

struct cellPosition
{
	int x, y;
};

class GameOfLife : public olc::PixelGameEngine
{
	std::vector<uint8_t> currentState, previousState;
	std::vector<cellPosition> drawQueue;
	bool simRunning = true;

	// This number represents an epoch of time in the simulation.
	// The time it took to complete a world update is subtracted from
	// this value to determine the amount of time the thread needs to 
	// sleep before the next update can begin.
	float simEpoch;

public:
	size_t worldWidth;
	size_t worldHeight;

	// To use a parameterized constructor, you must explicitly call the
	// olcPixelGameEngine constructor.
	GameOfLife(size_t w, size_t h, float u) : olc::PixelGameEngine()
	{
		sAppName = "Game of Life Demo";

		worldWidth = w;
		worldHeight = h;
		simEpoch = 1000.f / u;
	}

	bool OnUserCreate() override
	{
		// Prime the random generator before building the world.
		std::minstd_rand rand;
		rand.seed(std::chrono::high_resolution_clock::now().time_since_epoch().count());

		size_t numCells = worldWidth * worldHeight;

		if(numCells == 0) numCells = (ScreenWidth() * ScreenHeight()); 

		currentState.reserve(numCells);
		previousState.reserve(numCells);
		drawQueue.reserve(numCells * 0.6f);

		for (unsigned int s = 0; s < numCells; s++)
		{
			currentState.push_back((rand() % 256 < 150) ? cellAlive : cellDead);
		}


		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		auto beginTime = std::chrono::high_resolution_clock::now();

		if (GetKey(olc::Key::SPACE).bReleased) simRunning = !simRunning;

		if (simRunning) 
		{
			previousState = currentState;

			for (int y = 0; y < worldHeight; y++)
			{
				for (int x = 0; x < worldWidth; x++)
				{
					uint8_t neighbors = countNeighbors(x, y);
					uint8_t cell = previousState[y * worldWidth + x];

					if (cell == cellAlive)
					{
						if (neighbors == 2 || neighbors == 3)
						{
							currentState[y * worldWidth + x] = cellAlive;
							drawQueue.push_back({ x, y });
						}
						else
							currentState[y * worldWidth + x] = cellDead;
					}
					else
					{
						if (neighbors == 3)
						{
							currentState[y * worldWidth + x] = cellAlive;
							drawQueue.push_back({ x, y });
						}
						else
							currentState[y * worldWidth + x] = cellDead;
					}
				}
			}

			Clear(olc::BLACK);

			for (auto& cell : drawQueue)
				Draw({ cell.x, cell.y });

			drawQueue.clear();

		}

		auto endTime = std::chrono::high_resolution_clock::now();
		auto simTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime.time_since_epoch() - beginTime.time_since_epoch());

		std::this_thread::sleep_for(std::chrono::duration<float, std::milli>(simEpoch - simTime.count()));

		return true;
	}

	uint8_t countNeighbors(int x, int y)
	{
		auto wrap = [](int v, size_t size)
		{
			if (v == -1) return size - 1;
			if (v == size) return 0u;

			return (size_t)v;
		};

		int lx = wrap(x - 1, worldWidth),
			rx = wrap(x + 1, worldWidth),
			ty = wrap(y - 1, worldHeight),
			by = wrap(y + 1, worldHeight);

		return previousState[ty * worldWidth + lx] + previousState[ty * worldWidth + x] + previousState[ty * worldWidth + rx]
			+ previousState[y * worldWidth + lx] + previousState[y * worldWidth + rx] + previousState[by * worldWidth + lx]
			+ previousState[by * worldWidth + x] + previousState[by * worldWidth + rx];
	}
};


/*
	The user can specify a custom world size using:

	--worldWidth
	--worldHeight

	Default: 256 x 192 

	The user can specify a number of world updates per second using:

	--updatesPerSecond

	Default: 60
*/
int main(int argc, char** argv)
{
	size_t wWidth = 256;
	size_t wHeight = 192;
	float updatesPerSecond = 60.f;

	// Validate arguments and catch any user errors.
	if (argc > 1)
	{
		for (int c = 1; c < argc; c += 2)
		{
			try
			{
				if (argv[c] == "--worldWidth" && argc > c + 1)
				{
					wWidth = static_cast<unsigned int>(std::stoi(argv[c + 1]));
				}
				else if (argv[c] == "--worldHeight" && argc > c + 1)
				{
					wHeight = static_cast<unsigned int>(std::stoi(argv[c + 1]));
				}
				else if (argv[c] == "--updatesPerSecond" && argc > c + 1)
				{
					float ups = static_cast<float>(std::stof(argv[c + 1]));
					if (ups == 0.f) throw std::invalid_argument("The value of --updatesPerSecond cannot be zero...");
					updatesPerSecond = ups;
				}
				else
				{
					std::cout << "Argument: '" << argv[c] << "' is invalid...\n";
					exit(EXIT_FAILURE);
				}
			}
			catch (const std::invalid_argument& err)
			{
				std::cout << err.what();
				
				exit(EXIT_FAILURE);
			}
		}
	}

	GameOfLife g(wWidth, wHeight, updatesPerSecond);

	if (g.Construct(256, 192, 4, 4))
		g.Start();
}