#include <GameOfLife.hpp>

#include <cstdlib>
#include <cerrno>
#include <ranges>
#include <span>
#include <stdexcept>
#include <string_view>
#include <limits>
#include <fmt/format.h>
#include <fmt/ostream.h>

// olcPixelGameEngine already includes <vector> <chrono> and <iostream>.

constexpr int def_worldW = 256;
constexpr int def_worldH = 192;

/*
	The user can specify a custom world size using:

	--width
	--height

	Default: 256 x 192 
*/
int main(int argc, const char** argv)
{
	int wWidth = def_worldW;
	int wHeight = def_worldH;

	const auto args = std::span<const char*>{ argv, static_cast<size_t>(argc) };

	// Validate arguments and catch any user errors.
	if (argc > 1)
	{
		constexpr int base10 = 10;
		std::string argumentToSet{};

		for (const std::string_view arg : std::ranges::drop_view{args, 1})
		{
			try
			{
				if(argumentToSet.empty())
				{
					if (arg == "--width" || arg == "--height")
					{
						argumentToSet = arg;
					}
					
					continue;
				}

				// NOLINTBEGIN(bugprone-suspicious-stringview-data-usage)
				// These string_views are constructed from arguments passed to main,
				// which are required to be null-terminated by the C++ Standard
				if(argumentToSet == "--width")
				{
					auto result = std::strtol(arg.data(), nullptr, base10);
					if(errno != ERANGE && result > 0 && result < std::numeric_limits<int>::max()) 
					{ 
						wWidth = result; 
					} 
					else 
					{
						throw std::invalid_argument{ fmt::format("Option: '{}', value: '{}'", argumentToSet, arg) };
					}
				}
				else if(argumentToSet == "--height")
				{
					auto result = std::strtol(arg.data(), nullptr, base10);
					if(errno != ERANGE && result > 0 && result < std::numeric_limits<int>::max()) 
					{ 
						wHeight = result; 
					} 
					else 
					{
						throw std::invalid_argument{ fmt::format("Option: '{}', value: '{}'", argumentToSet, arg) };
					}
				}
				// NOLINTEND(bugprone-suspicious-stringview-data-usage)
				
				argumentToSet.clear();
				
			}
			catch (const std::invalid_argument& err)
			{
				fmt::println(stdout, "Invalid command argument: {}", err.what());
				std::exit(EXIT_FAILURE);
			}
		}
	}

	int cw{}, ch{};

	constexpr int def_windowW = 1024;
	constexpr int def_windowH = 768;

	// Calculate if cell sizes larger than 1x1 pixels are needed.
	if (wWidth < def_windowW)
	{
		cw = def_windowW / wWidth;
	}
	else
	{
		cw = 1;
	}

	if (wHeight < def_windowH)
	{
		ch = def_windowH / wHeight;
	}
	else
	{
		ch = 1;
	}

	life::GameOfLife g{static_cast<uint32_t>(wWidth), static_cast<uint32_t>(wHeight)};

	if (g.Construct(def_windowW / cw, def_windowH / ch, cw, ch, false, true) == olc::rcode::OK)
	{
		g.Start();
	}
}