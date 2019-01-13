# GUI Concept Design

![Concept sketch](gui_draft_3.png)

## Choosing a colour

The standard [Dear ImGui](https://github.com/ocornut/imgui) colour selection box is used:

![Choosing a colour](ColourSelection.png)

## Colour Palette

The idea is that there is a standard grid size (make it 6x3, for argument sake) that the user can edit, and the GUI remembers between sessions.

One could make it fancy and let the user add more colours, and delete existing ones, but I think that a fixed grid will be good enough for version 1.0...

## Loading Previous Configuration

When the user selects a shell script (`.bat` or `.sh`) in the file list, and said shell script has a valid previous configuration stored in its header comments, the "add to list" icon changes to a "load previous configuration" icon.

