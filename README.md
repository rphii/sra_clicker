# sra_clicker
WIP (auto) clicker application / program programmed in C for Windows.
## Implemented
### Mouse
* `sra_mouse_setup(&mouse)` Set up the mouse
* `sra_mouse_free(&mouse)` Free the memory of the mouse
* `mouse.initialize(&mouse)` (Re)Initialize the mouse
* `mouse.clickl_xy(&mouse, x, y)` Left clicking at xy coordinates
* `mouse.clickl(&mouse)` Left pressing at mouse position
* `mouse.releasel(&mouse)` Left releasing
* `mouse.clickl_xy(&mouse, x, y)` Move mouse to xy coordinates
* `mouse.clickl_xy(&mouse, x, y)` Move mouse by xy pixels
* (`mouse.update_dimensions(&mouse)` refresh width and height information of monitor. Usually called when initializing)
### Keyboard
* ...
### Locate
* `sra_locate_setup(&locate)` Set up the locate
* `sra_locate_free(&locate)` Free the memory of the locate
* `locate.initialize(&locate)` (Re)Initialize the locate
* `locate.refresh(&locate)` Refresh; makes a screenshot and stores it in a buffer to later retrieve (A)RGB data
* `locate.locate_color_rgb(&locate, r, g, b)` Locate color; locate a RGB color in the buffer. Searches from left to right and top to bottom
* `locate.get_color_rgb_xy(&locate, x, y)` From the buffer retrieve a pixels ARGB values seperately
* `locate.get_color_xy(&locate, x, y)` From the buffer retrieve a pixels ARGB value at once
## Planned
### Mouse
* Left pressing at xy coordinates
* Add control for right mouse button aswell
* Make movement speed choosable/not instant
* ...
### Keyboard
* Control the application with the keyboard (start/stop clicker)
* Input text
* ...
### Locate
* Greatly improve and expand the functionality...
### Procedures
* Create procedures (preferrably with a gui?)
* When creating procedures they should get saved to a file
* And thus also be loaded from a file
* furthermore, when starting the application, the user shall choose which procedure to run


Compiling with MingW (Version?)

16.02.2021
