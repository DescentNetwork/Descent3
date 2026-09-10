#include "chrono_timer.h"
#include <chrono>


namespace d3
{
  namespace chrono
  {
    using clock_type = std::chrono::steady_clock;

    namespace
    {
      clock_type::time_point last_update_stamp = clock_type::now();
      float last_update_time = 0.0f;
      float last_render_time = 0.0f;
    }

    float last_update() { return last_update_time; }

    float last_render() { return last_render_time; }

    void update()
    {
      const clock_type::time_point now = clock_type::now();
      const float delta =
          std::chrono::duration<float>(now - last_update_stamp).count();

      // Advance the game clock (Gametime analog) by the elapsed real time and
      // record that delta as the frame time (Frametime analog).
      last_update_time += delta;
      last_render_time = delta;
      last_update_stamp = now;
    }
  }
}
