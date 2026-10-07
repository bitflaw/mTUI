#pragma once
#include <iostream>
#include <ostream>
#include <vector>
#include <mtui/utils/int.h>
#include <mtui/utils/point.hpp>
#include <mtui/utils/colors.hpp>
#include <mtui/utils/MultiByteCharString.hpp>

struct ProgressBarStyle
{
  BaseStyle base;
  Color secondary_color {Color::WHITE};
  bool style_border_symbols {false};
  bool show_percentage {false};
};

class ProgressBar
{
private:
  MultiByteCharString symbols;
  ProgressBarStyle style {};
  u64 total {0};
  u32 width {40};
  std::string filled_style;
  std::string unfilled_style;

public:
  ProgressBar (
      u64 total,
      ProgressBarStyle style,
      MultiByteCharString symbols = std::vector<std::string_view>{"[", "#", "-", "]"},
      u32 width = 40
      )
    : symbols(symbols), style(style), total(total), width(width)
  {
    update_style (style);
  }

  void update_style (ProgressBarStyle s)
  {
    filled_style.clear(); unfilled_style.clear();
    filled_style = unfilled_style = "\033[";
    unfilled_style.append((s.base.bold ? "1;" : "0;"));
    filled_style.append((s.base.bold ? "1;" : "0;"));
    Color pri_col = s.base.primary_color;
    Color sec_col = s.secondary_color;
    unfilled_style.append((s.base.intense ? std::to_string(pri_col + 60) : std::to_string(pri_col)));
    filled_style.append((s.base.intense ? std::to_string(sec_col + 60) : std::to_string(sec_col)));
    filled_style += "m";
    unfilled_style += "m";
  }

  void draw ()
  {
    MultiByteChar initial_sym = symbols[2];
    u8 sym_width = initial_sym.width();
    u8 start_end_sym_width = symbols[0].width() + symbols[3].width();
    width -= start_end_sym_width;
    u8 n_chars_to_draw = width/sym_width;
    MultiByteCharString initial_bar {
      std::string_view {
        static_cast<const char*>(initial_sym.data),
          initial_sym.size
      },
      n_chars_to_draw
    };
    if (style.style_border_symbols)
      std::cout<<"\r"<<unfilled_style<<symbols[0]<<initial_bar<<symbols[3]<<RESET;
    else
      std::cout<<"\r"<<symbols[0]<<unfilled_style<<initial_bar<<RESET<<symbols[3];
    if (style.show_percentage)
      std::cout<<" 0%"<<std::flush;
    else std::cout<<std::flush;
  }

  void draw (Point p)
  {
    std::cout<<"\033["<<p.x<<";"<<p.y<<"H";
    draw();
  }

  void update (u64 progress)
  {
    u32 target_filled_width = (progress * width) / total;

    MultiByteChar incomplete_sym = symbols[2];
    u8 unfilled_sym_width = incomplete_sym.width();
    MultiByteChar complete_sym = symbols[1];
    u8 complete_sym_width = complete_sym.width();

    u8 n_complete_chars_to_draw = target_filled_width/complete_sym_width;
    u32 actual_complete_width = n_complete_chars_to_draw * complete_sym_width;

    MultiByteCharString comp_bar {
      std::string_view {
        static_cast<const char*>(complete_sym.data),
          complete_sym.size
      },
        n_complete_chars_to_draw
    };

    u32 target_unfilled_width = width - actual_complete_width;
    u32 n_unfilled_chars_to_draw = target_unfilled_width / unfilled_sym_width;
    u32 actual_unfilled_width = n_unfilled_chars_to_draw * unfilled_sym_width;
    u32 remaining_width = target_unfilled_width - actual_unfilled_width;
    std::string padding (remaining_width, ' ');

    MultiByteCharString incomp_bar {
      std::string_view {
        static_cast<const char*>(incomplete_sym.data),
          incomplete_sym.size
      },
        n_unfilled_chars_to_draw
    };

    std::cout <<"\r" << RM_CURSOR;
    if (style.style_border_symbols)
      std::cout
        <<filled_style<<symbols[0]<<comp_bar
        <<unfilled_style<<incomp_bar<<padding<<symbols[3]<<RESET;
    else
      std::cout
        <<symbols[0]<<filled_style<<comp_bar
        <<unfilled_style<<incomp_bar<<padding<<RESET<<symbols[3];

    if (style.show_percentage)
      std::cout<<" "<<((progress * 100)/total)<<"%"<<std::flush;
    else std::cout<<std::flush;
  }

  // void update (u64 progress, Point p)
  // {
  //   std::cout<<"\033["<<p.x<<";"<<p.y<<"H";
  //   update(progress);
  // }

  void finish ()
  {
    std::cout<<"\r" << RESTORE_CURSOR;
  }
};
