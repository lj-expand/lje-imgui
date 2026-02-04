# lje-imgui

An [LJE](https://github.com/lj-expand/lj-expand) binary module that
provides [Dear ImGui](https://github.com/ocornut/imgui)
and [imnodes](https://github.com/Nelarius/imnodes) bindings for Lua. Hooks DirectX 9 to render an ImGui overlay with a
full widget API accessible from LJE scripts.

## Features

- Split-frame rendering: Lua prepares UI, overlay renders on the next `EndScene` call
  - Allows for safe ImGui rendering with no detection possible.
  - However, this does introduce a chance of flickering since the D3D9 thread is multithreaded if `mat_queue_mode` is
    set to `2` (which is default in GMod).
- Toggle overlay visibility with the `INSERT` key
- Input blocking: mouse/keyboard events are consumed by ImGui when the overlay is active
- Full ImGui widget API exposed to Lua
- imnodes node editor API exposed to Lua

## Building

Requires Windows, MSVC, CMake 3.31+, and Ninja.

```bash
git clone --recurse-submodules https://github.com/user/lje-imgui.git
cd lje-imgui
cmake --preset x64-windows-rel
cmake --build --preset x64-windows-rel
```

The output DLL is located at `build/x64-windows-rel/lje-imgui.dll`.

A Debug build is also available via the `x64-windows-dbg` preset.

## Lua API

The module registers two tables in the LJE environment: `imgui` and `imnodes`.

### Frame control

Every frame that uses ImGui must be wrapped in `new_frame` / `render`:

```lua
imgui.new_frame()
-- draw widgets here
imgui.render()
```

Only **one** `imgui.render()` and `imgui.new_frame()` call is allowed per frame. Also, styles are global and persistent
across the entire
application lifetime, so multiple scripts may modify styles and affect each other.

### imgui

#### Windows

| Function       | Signature                                       | Returns         |
|----------------|-------------------------------------------------|-----------------|
| `begin_window` | `(name, [open], [flags])`                       | `visible, open` |
| `end_window`   | `()`                                            | -               |
| `begin_child`  | `(id, [w], [h], [child_flags], [window_flags])` | `visible`       |
| `end_child`    | `()`                                            | -               |

#### Text

| Function       | Signature           | Returns |
|----------------|---------------------|---------|
| `text`         | `(str)`             | -       |
| `text_colored` | `(r, g, b, a, str)` | -       |
| `text_wrapped` | `(str)`             | -       |

#### Buttons & inputs

| Function               | Signature                                         | Returns          |
|------------------------|---------------------------------------------------|------------------|
| `button`               | `(label, [w], [h])`                               | `clicked`        |
| `small_button`         | `(label)`                                         | `clicked`        |
| `checkbox`             | `(label, value)`                                  | `changed, value` |
| `input_text`           | `(label, current, [max_size])`                    | `changed, text`  |
| `input_text_multiline` | `(label, current, [max_size], [w], [h], [flags])` | `changed, text`  |
| `input_float`          | `(label, value)`                                  | `changed, value` |
| `input_int`            | `(label, value)`                                  | `changed, value` |

#### Sliders & drags

| Function       | Signature                               | Returns          |
|----------------|-----------------------------------------|------------------|
| `slider_float` | `(label, value, min, max)`              | `changed, value` |
| `slider_int`   | `(label, value, min, max)`              | `changed, value` |
| `drag_float`   | `(label, value, [speed], [min], [max])` | `changed, value` |
| `drag_int`     | `(label, value, [speed], [min], [max])` | `changed, value` |

#### Layout

| Function              | Signature               |
|-----------------------|-------------------------|
| `same_line`           | `([offset], [spacing])` |
| `separator`           | `()`                    |
| `spacing`             | `()`                    |
| `new_line`            | `()`                    |
| `indent`              | `([width])`             |
| `unindent`            | `([width])`             |
| `set_next_item_width` | `(width)`               |
| `push_id`             | `(id)`                  |
| `pop_id`              | `()`                    |

#### Trees & collapsing

| Function            | Signature | Returns |
|---------------------|-----------|---------|
| `collapsing_header` | `(label)` | `open`  |
| `tree_node`         | `(label)` | `open`  |
| `tree_pop`          | `()`      | -       |

#### Combo & selectable

| Function      | Signature             | Returns   |
|---------------|-----------------------|-----------|
| `begin_combo` | `(label, preview)`    | `open`    |
| `end_combo`   | `()`                  | -         |
| `selectable`  | `(label, [selected])` | `clicked` |

#### Color

| Function        | Signature             | Returns               |
|-----------------|-----------------------|-----------------------|
| `color_edit4`   | `(label, r, g, b, a)` | `changed, r, g, b, a` |
| `color_picker4` | `(label, r, g, b, a)` | `changed, r, g, b, a` |

#### Tabs

| Function         | Signature | Returns    |
|------------------|-----------|------------|
| `begin_tab_bar`  | `(id)`    | `open`     |
| `end_tab_bar`    | `()`      | -          |
| `begin_tab_item` | `(label)` | `selected` |
| `end_tab_item`   | `()`      | -          |

#### Tooltips

| Function          | Signature | Returns   |
|-------------------|-----------|-----------|
| `set_tooltip`     | `(text)`  | -         |
| `begin_tooltip`   | `()`      | -         |
| `end_tooltip`     | `()`      | -         |
| `is_item_hovered` | `()`      | `hovered` |

#### Popups & modals

| Function              | Signature        | Returns         |
|-----------------------|------------------|-----------------|
| `open_popup`          | `(id)`           | -               |
| `begin_popup`         | `(id)`           | `open`          |
| `begin_popup_modal`   | `(name, [open])` | `visible, open` |
| `end_popup`           | `()`             | -               |
| `close_current_popup` | `()`             | -               |
| `is_popup_open`       | `(id)`           | `open`          |

#### Plotting

| Function         | Signature                                                        |
|------------------|------------------------------------------------------------------|
| `plot_lines`     | `(label, values, [overlay], [scale_min], [scale_max], [w], [h])` |
| `plot_histogram` | `(label, values, [overlay], [scale_min], [scale_max], [w], [h])` |

#### Scrolling

| Function                                  | Signature          | Returns  |
|-------------------------------------------|--------------------|----------|
| `get_scroll_x` / `get_scroll_y`           | `()`               | `scroll` |
| `set_scroll_x` / `set_scroll_y`           | `(value)`          | -        |
| `get_scroll_max_x` / `get_scroll_max_y`   | `()`               | `max`    |
| `set_scroll_here_x` / `set_scroll_here_y` | `([center_ratio])` | -        |

#### Progress

| Function       | Signature                         |
|----------------|-----------------------------------|
| `progress_bar` | `(fraction, [w], [h], [overlay])` |

#### Fonts

| Function           | Signature        | Returns |
|--------------------|------------------|---------|
| `load_font`        | `(path, [size])` | `font`  |
| `push_font`        | `(font)`         | -       |
| `pop_font`         | `()`             | -       |
| `set_default_font` | `(font)`         | -       |
| `get_default_font` | `()`             | `font`  |

#### Visibility & input queries

| Function                | Signature   | Returns   |
|-------------------------|-------------|-----------|
| `set_visible`           | `(visible)` | -         |
| `is_visible`            | `()`        | `visible` |
| `want_capture_mouse`    | `()`        | `wants`   |
| `want_capture_keyboard` | `()`        | `wants`   |

#### Styling

`set_style` accepts a table with optional `colors`, `rounding`, and `padding` subtables:

```lua
imgui.set_style({
  colors = {
    window_bg = { r = 0.1, g = 0.1, b = 0.1, a = 0.9 },
    button     = { 0.2, 0.4, 0.8, 1.0 },  -- array style also works
  },
  rounding = {
    window = 4, frame = 2, tab = 4,
  },
  padding = {
    window = { x = 8, y = 8 },
    frame  = { 4, 3 },  -- array style also works
  },
})
```

#### Flags

Window flags, child flags, and input text flags are available as constants on the `imgui` table (e.g.
`imgui.WindowFlags_NoTitleBar`, `imgui.InputTextFlags_ReadOnly`).

### imnodes

#### Node editor

| Function            | Signature                       | Returns   |
|---------------------|---------------------------------|-----------|
| `begin_node_editor` | `()`                            | -         |
| `end_node_editor`   | `()`                            | -         |
| `minimap`           | `([size_fraction], [location])` | -         |
| `is_editor_hovered` | `()`                            | `hovered` |

#### Nodes

| Function              | Signature |
|-----------------------|-----------|
| `begin_node`          | `(id)`    |
| `end_node`            | `()`      |
| `begin_node_titlebar` | `()`      |
| `end_node_titlebar`   | `()`      |

#### Attributes (pins)

| Function                 | Signature       |
|--------------------------|-----------------|
| `begin_input_attribute`  | `(id, [shape])` |
| `end_input_attribute`    | `()`            |
| `begin_output_attribute` | `(id, [shape])` |
| `end_output_attribute`   | `()`            |
| `begin_static_attribute` | `(id)`          |
| `end_static_attribute`   | `()`            |

#### Links

| Function            | Signature                         | Returns                         |
|---------------------|-----------------------------------|---------------------------------|
| `link`              | `(link_id, start_attr, end_attr)` | -                               |
| `is_link_created`   | `()`                              | `created, start_attr, end_attr` |
| `is_link_destroyed` | `()`                              | `destroyed, link_id`            |
| `is_link_started`   | `()`                              | `started, attr_id`              |
| `is_link_dropped`   | `([include_detached])`            | `dropped, attr_id`              |
| `is_link_hovered`   | `()`                              | `hovered, link_id`              |

#### Selection & hover

| Function               | Signature     | Returns            |
|------------------------|---------------|--------------------|
| `is_node_hovered`      | `()`          | `hovered, node_id` |
| `is_pin_hovered`       | `()`          | `hovered, attr_id` |
| `num_selected_nodes`   | `()`          | `count`            |
| `num_selected_links`   | `()`          | `count`            |
| `get_selected_nodes`   | `()`          | `{node_ids}`       |
| `get_selected_links`   | `()`          | `{link_ids}`       |
| `select_node`          | `(node_id)`   | -                  |
| `select_link`          | `(link_id)`   | -                  |
| `is_node_selected`     | `(node_id)`   | `selected`         |
| `is_link_selected`     | `(link_id)`   | `selected`         |
| `clear_node_selection` | `([node_id])` | -                  |
| `clear_link_selection` | `([link_id])` | -                  |

#### Node positioning

| Function              | Signature         | Returns |
|-----------------------|-------------------|---------|
| `set_node_position`   | `(node_id, x, y)` | -       |
| `set_node_editor_pos` | `(node_id, x, y)` | -       |
| `set_node_grid_pos`   | `(node_id, x, y)` | -       |
| `get_node_position`   | `(node_id)`       | `x, y`  |
| `get_node_editor_pos` | `(node_id)`       | `x, y`  |
| `get_node_grid_pos`   | `(node_id)`       | `x, y`  |
| `get_node_dimensions` | `(node_id)`       | `w, h`  |

#### Panning

| Function               | Signature    | Returns |
|------------------------|--------------|---------|
| `editor_reset_panning` | `([x], [y])` | -       |
| `editor_get_panning`   | `()`         | `x, y`  |

#### Styling

| Function              | Signature                 |
|-----------------------|---------------------------|
| `push_color_style`    | `(color_enum, color_u32)` |
| `pop_color_style`     | `()`                      |
| `push_style_var`      | `(var_enum, value)`       |
| `push_style_var_vec2` | `(var_enum, x, y)`        |
| `pop_style_var`       | `([count])`               |
| `push_attribute_flag` | `(flag)`                  |
| `pop_attribute_flag`  | `()`                      |

Pin shape, minimap location, attribute flag, color, and style var constants are available on the `imnodes` table (e.g.
`imnodes.PinShape_CircleFilled`, `imnodes.Col_NodeBackground`).

## License

See individual library
licenses: [Dear ImGui](https://github.com/ocornut/imgui/blob/master/LICENSE.txt), [imnodes](https://github.com/Nelarius/imnodes/blob/master/LICENSE.md), [MinHook](https://github.com/TsudaKageyu/minhook/blob/master/LICENSE.txt).
