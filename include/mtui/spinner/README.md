## SPINNER

A single header file containing an implementation of a Spinner class.

An example of how to use it:
```cpp
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
    spinner.update(150ms);
    // spinner.update(p, 150ms);
    i += 5;
  }
  spinner.finish();
  return 0;
}
```

### Constructor:
```cpp
Spinner (
      BaseStyle style,
      MultiByteCharString symbols = std::vector<std::string_view>{"/", "-", "\\", "|"}
      )
```

- `style` : Struct representing the styling of the bar (see utils/README.md).
- `symbols`: Symbols to use for the bar (see utils/README.md). One can provide as many symbols as they would like.

### Other functions:
```cpp
void update_style (BaseStyle s);
```
Updates the style of the spinner. Takes as an argument the new style struct.

```cpp
void update (std::chrono::milliseconds interval)
```
Cycles throuhg the symbols wrapping around each time it gets to the end, sleeping `interval` milliseconds after each flush of the next symbol to the screen.

```cpp
void finish ();
```
This restores the cursor that was hidden by both the `update` function.
