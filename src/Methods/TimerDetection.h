#pragma once
#include <chrono>

// helper static class for things that are detected
// but only for a short duration
class timer_detection
{
public:
    timer_detection(bool detected)
    {
        detection = detected;
        last_call_time = std::chrono::steady_clock::now();
    }

    inline void frame()
    {
        auto current_time = std::chrono::steady_clock::now();
        // short duration, if the button is untoggled the detection will reset
        if (std::chrono::duration_cast<std::chrono::milliseconds>(current_time - last_call_time).count() > 250)
        {
            needs_reset = true;
        }
        else
        {
            needs_reset = false;
        }

        last_call_time = std::chrono::steady_clock::now();
    }

    inline void set_condition(bool cond)
    {
        condition = cond;
    }

    inline void update_detection(bool value)
    {
        if (condition || needs_reset)
        {
            detection = value;
        }
    }

    inline bool get_detected()
    {
        return detection;
    }
private:
    bool condition;
    bool detection;
    bool needs_reset;
    std::chrono::time_point<std::chrono::steady_clock> last_call_time;
};
