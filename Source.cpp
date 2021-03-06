#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include <random>
#include "fmt/format.h"
#include "fmt/ostream.h"	
// olcPixelGameEngine already includes <vector> <chrono> and <iostream>.

constexpr uint8_t cellAlive = 1;
constexpr uint8_t cellDead = 0;

// Used in the drawQueue to determine which cells to draw at the end of an epoch.
struct CellPosition
{
	int x, y;
};

struct Camera
{
	float x, y, w, h;
};

class GameOfLife : public olc::PixelGameEngine
{
	// Two copies of the map are needed to avoid contaminating the simulation.
	std::vector<uint8_t> currentState, previousState;

	std::vector<CellPosition> drawQueue;
	bool simRunning = true;

	const long seed = 0;

	const size_t worldWidth;
	const size_t worldHeight;

	Camera cam;

public:
	
	// To use a parameterized constructor, you must explicitly call the
	// olcPixelGameEngine constructor.
	GameOfLife(size_t w, size_t h, long s)
		: olc::PixelGameEngine(),
		worldWidth(w), worldHeight(h),
		currentState(std::vector<uint8_t>(w*h)),
		previousState(std::vector<uint8_t>(w* h)),
		seed(s)
	{
		sAppName = "Game of Life Demo";
	}

	bool OnUserCreate() override
	{
		// Prime the random generator before building the world.
		std::minstd_rand rand;
		rand.seed(seed);

		size_t numCells = worldWidth * worldHeight;

		if(numCells == 0) numCells = ((size_t)ScreenWidth() * (size_t)ScreenHeight()); 

		drawQueue.reserve((size_t)ScreenWidth() * (size_t)ScreenHeight());

		for (unsigned long s = 0; s < numCells; s++)
		{
			currentState.at(s) = rand() % 2;
		}
		
		cam = {0.f, 0.f, (float)ScreenWidth(), (float)ScreenHeight()};

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{

		if (GetKey(olc::Key::SPACE).bPressed) simRunning = !simRunning;
		if (GetKey(olc::Key::W).bHeld) cam.y -= 100.f * fElapsedTime;
		if (GetKey(olc::Key::S).bHeld) cam.y += 100.f * fElapsedTime;
		if (GetKey(olc::Key::A).bHeld) cam.x -= 100.f * fElapsedTime;
		if (GetKey(olc::Key::D).bHeld) cam.x += 100.f * fElapsedTime;

		if (cam.y < 0.f) cam.y = 0.f;
		if (cam.y + cam.h > worldHeight) cam.y = worldHeight - cam.h;
		if (cam.x < 0.f) cam.x = 0.f;
		if (cam.x + cam.w > worldWidth) cam.x = worldWidth - cam.w;

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
							if (withinView(x, y)) drawQueue.push_back({ x, y });
						}
						else
							currentState[y * worldWidth + x] = cellDead;
					}
					else
					{
						if (neighbors == 3)
						{
							currentState[y * worldWidth + x] = cellAlive;
							if (withinView(x, y)) drawQueue.push_back({ x, y });
						}
						else
							currentState[y * worldWidth + x] = cellDead;
					}
				}
			}

			Clear(olc::BLACK);

			for (auto& cell : drawQueue)
				Draw({ cell.x - (int)cam.x, cell.y - (int)cam.y });

			drawQueue.clear();

		}

		return true;
	}

	uint8_t countNeighbors(int x, int y)
	{
		auto wrap = [](int v, size_t size)
		{
			if (v == -1) return size - 1;
			if (v == size) return (size_t)0;

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

	bool withinView(int x, int y)
	{
		return (x >= cam.x && x < cam.x + cam.w && y >= cam.y && y < cam.y + cam.h);
	}
};


/*
	The user can specify a custom world size using:

	--width
	--height

	Default: 256 x 192 

	The user can specify a seed for the world to generate with:

	--seed

	Default: 0
*/
int main(int argc, char** argv)
{
	size_t wWidth = 256;
	size_t wHeight = 192;
	long seed = 0;

	// Validate arguments and catch any user errors.
	if (argc > 1)
	{
		for (int c = 1; c < argc; c += 2)
		{
			try
			{
				if (strcmp(argv[c], "--width") == 0 && argc > c)
				{
					wWidth = static_cast<unsigned int>(std::stoi(argv[c + 1]));
				}
				else if (strcmp(argv[c], "--height") == 0 && argc >c)
				{
					wHeight = static_cast<unsigned int>(std::stoi(argv[c + 1]));
				}
				else if (strcmp(argv[c], "--seed") == 0 && argc > c)
				{
					seed = std::stol(argv[c + 1]);
				}
				else
				{
					throw std::invalid_argument(fmt::format("Argument: '{}' is invalid...\n", argv[c]));
				}
			}
			catch (const std::invalid_argument& err)
			{
				fmt::print(stdout, "{}", err.what());
				
				exit(EXIT_FAILURE);
			}
		}
	}

	int cw, ch;

	// Calculate if cell sizes larger than 1x1 pixels are needed.
	if (wWidth < 1024)
		cw = 1024 / wWidth;
	else
		cw = 1;

	if (wHeight < 768)
		ch = 768 / wHeight;
	else
		ch = 1;

	GameOfLife g(wWidth, wHeight, seed);

	if (g.Construct(1024 / cw, 768 / ch, cw, ch))
		g.Start();
}