# GUI Concept Design

![Second Draft](gui_draft_3.png)

## Choosing a colour

The standard [Dear ImGui](https://github.com/ocornut/imgui) colour selection box is used:

![Choosing a colour](ColourSelection.png)

## Strokes to Fills

Note that the engine applies the same setting to all instances of that Gerber file.  The GUI must therefore ensure that the check-boxes are linked across instances.

## Colour Palette

The idea is that there is a standard grid size (make it 6x3, for argument sake) that the user can edit, and the GUI remembers between sessions (see this card and this card).

One could make it fancy and let the user add more colours, and delete existing ones, but I think that a fixed grid will be good enough for version 1.0...

