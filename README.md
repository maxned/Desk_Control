# Desk Control

Problem: I have a motorized sit/stand desk (VertDesk V3 with the old
electronics) that has 4 presets and up/down buttons, but in order to move the
desk to a specific preset I must hold the preset button until the desk stops
moving. This is really annoying because I want to be able to just click a button
and have the desk move automatically to the preset height.

Solution: The desk controller uses simple buttons to move the desk so I added
2 wires to each button, an input and an output. The Arduino Nano senses when
a button is pressed and holds the signal for the button as if I'm holding the
button. End result is that I can now tap a preset and the desk will move
without me needing to hold the preset.

Notice: I support Arduino hijacking the up/down buttons but I disabled that
for now because those are better used for minor adjustments instead of
automatically moving the desk up/down.
