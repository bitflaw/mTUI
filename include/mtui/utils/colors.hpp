
#define RESET "\033[0m"
#define RM_CURSOR "\033[?25l"
#define RESTORE_CURSOR "\033[?25h"

enum Color
{
  BLACK = 30,
  RED,
  GREEN,
  YELLOW,
  BLUE,
  MAGENTA,
  CYAN,
  WHITE
};

struct BaseStyle
{
  Color primary_color {Color::WHITE};
  Color bg_color {Color::BLACK};
  bool bold {false};
  bool intense {false};
};

// bool italic {false};
// bool underline {false};
// bool strikethrough {false};
// bool rounded {false};
