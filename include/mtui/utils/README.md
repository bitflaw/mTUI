### `BaseStyle`:

- `primary_color`: foreground color. Varies between different components.
- `bold`: whether the colors will be bold.
- `intense`: whether intense versions of the colors will be used.
- `bg_color`: background color.


### `MultiByteCharString`

A custom implementation of a String class designed to manage multi-byte characters easier. Single-header file format.
#### Constructors
```cpp
1. MultiByteCharString (std::vector<std::string_view> chars);
2. MultiByteCharString (std::string_view chr, u64 count);
```

##### 1
Takes a vector of std::string_views. The string_views in this case acts as a single character for the MultiByte string.

##### 2
Takes a single 'character' represented as a string_view, and a count, and will repeat the `chr` character, `count` times.
