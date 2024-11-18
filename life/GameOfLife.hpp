#ifndef LIFE_GAMEOFLIFE_HPP
#define LIFE_GAMEOFLIFE_HPP

#include <olcPixelGameEngine.h>
#include <Camera.hpp>

namespace life
{
    enum class CellState : uint8_t { Dead = 0u, Alive };

    // Used in the drawQueue to determine which cells to draw at the end of an epoch.
    struct CellPosition
    {
        uint32_t x, y;
    };

    class GameOfLife : public olc::PixelGameEngine
    {
    public:
        
        GameOfLife(uint32_t w, uint32_t h);
        
        bool OnUserCreate() override;
        bool OnUserUpdate(float fElapsedTime) override;
        void transformStates();
        [[nodiscard]] uint8_t countNeighbors(uint32_t x, uint32_t y);
        
        [[nodiscard]] constexpr bool withinView(float x, float y) const
        {
            return (x >= cam.x && x < cam.x + cam.w && y >= cam.y && y < cam.y + cam.h);
        }

    private:
        // Two copies of the map are needed to avoid contaminating the simulation.
        std::vector<CellState> currentState, previousState;
        std::vector<CellPosition> drawQueue;

        uint32_t worldWidth;
        uint32_t worldHeight;

        Camera cam{};
        bool simRunning = true;
    };
}

#endif