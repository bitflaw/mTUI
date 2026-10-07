#include <thread>
#include <mtui/progress_bar/progress_bar.hpp>

int main ()
{
  ProgressBarStyle style {
    .base = {
      .primary_color = Color::RED,
      .bold = true,
      .intense=false,
    },
    .secondary_color = Color::GREEN,
    .style_border_symbols = true,
    .show_percentage = true,
  };

  MultiByteCharString bar_symbols {std::vector<std::string_view> {" ", "━", "━"," "}};
  u32 total {100};
  ProgressBar pb {total, style, bar_symbols};
  // Point p {
  //   .x = 43,
  //     .y = 50
  // };
  // pb.draw(p);

  pb.draw();
  u32 i = 0;
  using namespace std::chrono_literals;
  while (i <= total)
  {
    pb.update(i);
    std::this_thread::sleep_for(50ms);
    i += 5;
  }
  pb.finish();
  return 0;
}
