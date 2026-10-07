#pragma once
#include <chrono>
#include <mtui/utils/colors.hpp>
#include <mtui/utils/MultiByteCharString.hpp>
#include <mtui/utils/int.h>
#include <mtui/utils/point.hpp>
#include <thread>


class Spinner
{
private:
  MultiByteCharString symbols;
  BaseStyle style {};
  u8 idx {0};
  std::string spinner_style;

public:
  Spinner (
      BaseStyle style,
      MultiByteCharString symbols = std::vector<std::string_view>{"/", "-", "\\", "|"}
      )
    : symbols(symbols), style(style)
  {
    update_style(style);
  }

  void update (Point p, std::chrono::milliseconds interval)
  {
    if (idx >= symbols.size()) idx = 0;
    std::cout
      << "\033["<<p.x<<";"<<p.y<<"H"
      << "\r"
      << RM_CURSOR
      << spinner_style<<symbols[idx]
      << std::flush;
    idx++;
    std::this_thread::sleep_for(interval);
  }

  void update (std::chrono::milliseconds interval)
  {
    if (idx >= symbols.size()) idx = 0;
    std::cout<< "\r"
      << RM_CURSOR
      << spinner_style<<symbols[idx]
      << std::flush;
    idx++;
    std::this_thread::sleep_for(interval);
  }

  void update_style (BaseStyle s)
  {
    spinner_style.clear();
    spinner_style = "\033[";
    spinner_style.append((s.bold ? "1;" : "0;"));
    Color pri_col = s.primary_color;
    spinner_style.append((s.intense ? std::to_string(pri_col + 60) : std::to_string(pri_col)));
    spinner_style += "m";
  }

  void finish ()
  {
    std::cout<<"\r" << RESTORE_CURSOR;
  }
};
