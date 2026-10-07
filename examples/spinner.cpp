#include <mtui/spinner/spinner.hpp>

int main ()
{
  BaseStyle style {
    .primary_color = Color::GREEN,
    .bold = false,
    .intense=false,
  };

  // NPM-like symbols
  MultiByteCharString syms {std::vector<std::string_view> {"⠋", "⠙", "⠹", "⠸", "⠼", "⠴", "⠦", "⠧", "⠇", "⠏"}};
  // Point p { 33, 10 };
  u32 total = 100;
  u32 i = 0;
  Spinner spinner {style, syms};
  using namespace std::chrono_literals;
  while (i <= total)
  {
    spinner.update(50ms);
    // spinner.update(p, 150ms);
    i += 5;
  }
  spinner.finish();
  return 0;
}
