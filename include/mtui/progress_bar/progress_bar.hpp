#pragma once
#include <clocale>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <cwchar>
#include <iostream>
#include <ostream>
#include <stdexcept>
#include <vector>

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t i8;
typedef char8_t c8;

const u8 MAX_CHUNK_SIZE = 64;
struct Chunk
{
  u8 count {0};
  u8 used_bytes {0};
  c8 data[MAX_CHUNK_SIZE] {};
  u8 offsets[MAX_CHUNK_SIZE] {};
};

struct MultiByteCharSlice
{
  u8 size;
  const char* data;

  u8 width ()
  {
    setlocale(LC_ALL, "");
    wchar_t wc;
    i8 bytes_read = mbtowc(&wc, data, size);
    if (bytes_read != size)
      throw std::logic_error("Didn't read all the bytes!");
    if (bytes_read == 0)
      throw std::logic_error("Attempted to get size of a null MultiByteCharSlice!");
    if (bytes_read < 0)
      throw std::runtime_error("Failed to convert charslice to wide char for width!");
    i8 width = wcwidth(wc);
    if (width < 0)
      throw std::runtime_error("Failed to get width!");
    return width;
  }

  friend std::ostream& operator<< (std::ostream& os, const MultiByteCharSlice& mbslice)
  {
    std::string_view slice (mbslice.data, mbslice.size);
    os<<slice;
    return os;
  }
};

class MultiByteCharString
{
private:
  u64 length {0};
  std::vector<Chunk> chunks {};

public:
  MultiByteCharString () = default;
  MultiByteCharString (std::vector<std::string_view> chars)
  {
    chunks.emplace_back(Chunk {});
    append(chars);
  }
  MultiByteCharString (std::string_view chr, u64 count)
  {
    chunks.emplace_back(Chunk {});
    append(chr, count);
  }
  MultiByteCharString (const MultiByteCharString& other)
  {
    chunks = other.chunks;
    length = other.length;
  }
  MultiByteCharString (MultiByteCharString&& other)
  {
    if (this != &other)
    {
      chunks = std::move(other.chunks);
      length = std::move(other.length);
    }
  }
  MultiByteCharString& operator= (const MultiByteCharString& other)
  {
    chunks = other.chunks;
    length = other.length;
    return *this;
  }
  MultiByteCharString& operator= (MultiByteCharString&& other)
  {
    chunks = std::move(other.chunks);
    length = std::move(other.length);
    return *this;
  }
  ~MultiByteCharString () = default;

  u64 size () const { return length; }
  void reserve (u64 n_bytes) { chunks.reserve(n_bytes); }
  MultiByteCharSlice operator[] (u64 index)
  {
    if (index >= length)
      throw std::out_of_range("Index out of range: " + std::to_string(index));

    u64 local_idx = index;

    for (size_t i = 0; i < chunks.size(); ++i)
    {
      const Chunk& chunk = chunks[i];

      if (local_idx < chunk.count)
      {
        u8 start_byte = chunk.offsets[local_idx];

        u8 end_byte = (local_idx + 1 < chunk.count)
          ? chunk.offsets[local_idx + 1]
          : chunk.used_bytes;

        u8 slice_len = end_byte - start_byte;

        return MultiByteCharSlice {
          .size = slice_len,
            .data = (const char*)(chunk.data + start_byte)
        };
      }

      local_idx -= chunk.count;
    }

    throw std::logic_error("Corrupted string state: character count mismatch.");
  }

  void append (std::vector<std::string_view> chars)
  {
    for (std::string_view& chr : chars)
    {
      u8 chr_size = static_cast<u8>(chr.size());
      if (chr_size == 0) continue;
      if (chr_size > 4)
        throw std::runtime_error("Cannot handle character larger than 4 bytes: " + std::to_string(chr_size));

      if (chunks.empty() || (MAX_CHUNK_SIZE - chunks.back().used_bytes) < chr_size)
      {
        chunks.push_back(Chunk{});
      }
      Chunk& current_chunk = chunks.back();
      current_chunk.offsets[current_chunk.count] = current_chunk.used_bytes;

      std::memcpy(current_chunk.data + current_chunk.used_bytes, chr.data(), chr_size);
      current_chunk.used_bytes += chr_size;
      current_chunk.count += 1;
      length++;
    }
  }

  void append (std::string_view chr, u64 count)
  {
    u8 chr_size = static_cast<u8>(chr.size());

    if (chr_size == 0) return;
    if (chr_size > 4)
      throw std::runtime_error("Cannot handle character larger than 4 bytes: " + std::to_string(chr_size));

    length = count;

    u8 chars_per_chunk = MAX_CHUNK_SIZE / chr_size;
    u64 remaining_chars = count;
    while (remaining_chars > 0)
    {
      u8 chars_in_this_chunk = static_cast<u8>(std::min<u32>(remaining_chars, chars_per_chunk));

      Chunk chunk {};
      chunk.count = chars_in_this_chunk;
      chunk.used_bytes = chars_in_this_chunk * chr_size;

      for (u8 i = 0; i < chars_in_this_chunk; ++i)
      {
        u8 offset = i * chr_size;
        std::memcpy(chunk.data + offset, chr.data(), chr_size);
        chunk.offsets[i] = offset;
      }
      chunks.push_back(chunk);

      remaining_chars -= chars_in_this_chunk;
    }
  }

  friend std::ostream& operator<< (std::ostream& os, const MultiByteCharString& mbstr)
  {
    for (const Chunk& c : mbstr.chunks)
    {
      std::string_view slice (reinterpret_cast<const char*>(c.data), c.used_bytes);
      os<<slice;
    }
    return os;
  }
};


// COLORS IMPLEMENTATION
#define RESET "\033[0m"

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

struct ProgressBarStyle
{
  BaseStyle base;
  Color secondary_color {Color::WHITE};
  bool style_border_symbols {false};
  bool show_percentage {false};
};

// PROGRESS BAR IMPLEMENTATION
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
    filled_style = unfilled_style = "\033[";
    unfilled_style.append((style.base.bold ? "1;" : "0;"));
    filled_style.append((style.base.bold ? "1;" : "0;"));
    Color pri_col = style.base.primary_color;
    Color sec_col = style.secondary_color;
    unfilled_style.append((style.base.intense ? std::to_string(pri_col + 60) : std::to_string(pri_col)));
    filled_style.append((style.base.intense ? std::to_string(sec_col + 60) : std::to_string(sec_col)));
    filled_style += "m";
    unfilled_style += "m";
  }

  void draw ()
  {
    MultiByteCharSlice initial_sym = symbols[2];
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
    std::cout<<" 0%"<<std::flush;
  }

  void update (u64 progress)
  {
    u32 target_filled_width = (progress * width) / total;

    MultiByteCharSlice incomplete_sym = symbols[2];
    u8 unfilled_sym_width = incomplete_sym.width();
    MultiByteCharSlice complete_sym = symbols[1];
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

    if (style.style_border_symbols)
      std::cout
        <<"\r"<<filled_style<<symbols[0]<<comp_bar
        <<unfilled_style<<incomp_bar<<padding<<symbols[3]<<RESET;
    else
      std::cout
        <<"\r"<<symbols[0]<<filled_style<<comp_bar
        <<unfilled_style<<incomp_bar<<padding<<RESET<<symbols[3];

    std::cout<<" "<<((progress * 100)/total)<<"%"<<std::flush;
  }
};
