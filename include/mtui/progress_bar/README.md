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
  u32 total {200};
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
- `symbols`: Symbols to use for the bar. Limited to `length = 4`.
            The first and last characters represent the enclosing symbols of the bar, the second character represents
            the symbol to be used for the filled parts of the bar, and the third character to represent the unfilled parts of the bar.
- `width` : Number of columns the progress bar will use in the terminal.

### `ProgressBarStyle`:

- `base` : Basestyle struct (see `utils/README.md`), interpreted as follows for the progress bar:
    - `primary_color`: For the progress bar, it signifies the color to be used for the incomplete side of the bar.
    - `bold`: whether the colors (both primary and secondary) will be bold.
    - `intense`: whether intense versions of the colors (both primary and secondary) will be used.
    - `bg_color`: not used here
- `secondary_color`: Color to be used for the complete side of the bar.
- `style_border_symbols` : This is used to decide whether to include the start and end symbols of the bar in the styling.
- `show_percentage`: whether to show percentage of the bar or not.

### Other functions:
```cpp
void update_style (ProgressBarStyle s);
```
Updates the style of the progress bar. Takes as an argument the new style struct.

```cpp
void draw ();
```
Draws the initial bar, tho this can be ignored and update(0) can be used in its place. Hides the cursor.

```cpp
void update (u64 progress);
```
Draws the bar with the progress of the bar determined by the argument `progress` passed in. Note that in the case that the
width of the characters used is not divisible by the total width of the bar(minus the start and end characters),
then whitespace padding will be used. Hides the cursor.

```cpp
void finish ();
```
This restores the cursor that was hidden by both the `draw` and `update` functions.
