## PROGRESS BAR

A single header file containing an implementation of a ProgressBar class.

An example of how to use it:
```cpp
#include <thread>
#include <progress_bar.hpp>

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
  u32 total {500};
  ProgressBar x {total, style, bar_symbols};
  x.draw();
  u32 i = 0;
  using namespace std::chrono_literals;
  while (i <= total)
  {
    x.update(i);
    std::this_thread::sleep_for(50ms);
    i += 5;
  }
  return 0;
}
```

### Constructor:
```cpp
ProgressBar (
      u64 total,
      ProgressBarStyle style,
      MultiByteCharString symbols = std::vector<std::string_view>{"[", "#", "-", "]"},
      u32 width = 40
);
```

- `total` : Number representing the total of what the bar should be representing.
- `style` : Struct representing the styling of the bar (see below).
- `symbols`: Symbols to use for the bar (see below).
- `width` : Number of columns the progress bar will use in the terminal.

### `ProgressBarStyle`:

- `base` : Base style struct, defined in the file, containing the following fields:
    - `primary_color`: For the progress bar, it signifies the color to be used for the incomplete side of the bar.
    - `bold`: whether the colors (both primary and secondary) will be bold.
    - `intense`: whether intense versions of the colors (both primary and secondary) will be used.
    - `bg_color`: not used here
- `secondary_color`: Color to be used for the complete side of the bar.
- `style_border_symbols` : This is used to decide whether to include the start and end symbols of the bar in the styling.
- `show_percentage`: whether to show percentage of the bar or not.

### `MultiByteCharString`

A custom implementation of a String class designed to manage multi-byte characters easier. Its implementation is amalgamated into the single
header file.
For the Progress Bar class, a `MultiByteCharString` is required to have only four characters (can be of varying size,
represented as string_views) where the first and last characters represent the enclosing symbols of the bar, the second character represents
the symbol to be used for the filled parts of the bar, and the third character to represent the unfilled parts of the bar.
