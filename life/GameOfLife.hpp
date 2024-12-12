#ifndef LIFE_GAMEOFLIFE_HPP
#define LIFE_GAMEOFLIFE_HPP

#include <chrono>
#include <olcPixelGameEngine.h>
#include <Camera.hpp>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace life
{
    enum class CellState : uint8_t { Dead = 0u, Alive };

    // Used in the drawQueue to determine which cells to draw at the end of an epoch.
    struct CellPosition
    {
        uint64_t x, y;
    };

    class GameOfLife : public olc::PixelGameEngine
    {
    public:
        
        GameOfLife(uint64_t w, uint64_t h);
        
        bool OnUserCreate() override;
        bool OnUserUpdate(float fElapsedTime) override;
        bool OnUserDestroy() override;
        void transformStates();

        static void drawCurrentState() noexcept;

        [[nodiscard]] static constexpr GameOfLife* get() { return _self; }

        [[nodiscard]] uint8_t countNeighbors(uint64_t x, uint64_t y);
        
        [[nodiscard]] constexpr bool withinView(float x, float y) const
        {
            return (x >= cam.x && x < cam.x + cam.w && y >= cam.y && y < cam.y + cam.h);
        }

        static constexpr auto _numThreads = 4uz;

    private:
        // Two copies of the map are needed to avoid contaminating the simulation.
        std::vector<CellState> currentState, previousState;
        std::vector<CellPosition> drawQueue;

        uint64_t worldWidth;
        uint64_t worldHeight;
        Camera cam{};

        std::chrono::duration<uint64_t, std::micro> _timeRunSimulation = std::chrono::duration<uint64_t>::zero();
        uint64_t _avgTimeRunSimulation{};

        std::chrono::duration<uint64_t, std::micro> _timeDrawing = std::chrono::duration<uint64_t>::zero();
        uint64_t _avgTimeDrawing{};

        std::vector<std::jthread> _threadPool;
        std::condition_variable _simulationWaitCondition;
        std::mutex _drawQueueLock;

        static GameOfLife* _self;

        bool _simRunning = true, _runLoop = true;
    };
    
}

#endif