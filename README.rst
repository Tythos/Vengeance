PacMan - VENGEANCE
==================

"PacMan - VENGEANCE" is a silly PacMan-inversion game I originally developed
near the end of grad school as part of my demo portfolio while shopping around
job applications. It's primary purpose is to showcase a hand-built engine I had
grown over the years, Artemis. I used Artemis for a number of projects, from
computational fluid dynamics (CFD) rocket design simulations to hobby games.

I recently revisited both Artemis and Vengeance and found, much to my surprise,
that it didn't take a lot of effort to update them for SDL2 (and associated
libraries). If you want to mess around with it, or just have fun with a little
demo game (that got me through to the final round of interviews with a
then-tiny studio in west LA called "Riot Games"), here's what you do.

Building the Engine
-------------------

First, you need to build the Artemis engine library. See the following
repository for more details:

  https://github.com/Tythos/ArtemisLib

Once this is done, you should have a collection of libraries. Artemis is
statically linked, but you'll also want runtime (.DLL) files for the following
dependencies, which should be copied to the "msvc" folder:

* glew32d.dll

* libpng16-16.dll (will originally be "libpng16.dll" when you build it, but
  requires some renaming... don't ask...)

* SDL2.dll

* SDL2_image.dll

* SDL2_mixer.dll

* SDL2_ttf.dll

Building the Game
-----------------

To build the game itself, you can use the MSVC solution in the
"msvc/Vengeance.sln" solution. You will need to point the project to the usual
dependency paths for headers and static libraries, of course. But once it
successfully builds, you should be able to debug (F5) straight out of the IDE.

Playing the Game
----------------

Assuming all the paths and resources are configured, you should now see the
game launch! See "help/hotkeys.txt" for a list of commands. The basic premise
is this:

* You control the ghosts (one at first, more as you level up), who are trying
  to hunt down PacMan

* When PacMan eats a large dot, your ghosts will turn blue. While blue, PacMan
  will attempt to chase them down and eat them. Your ghosts can only eat PacMan
  when they are not blue.

* Mazes are randomly generated with each level, and grow in size.

* Successfully completing a maze will let you "level up" an ability of the
  ghosts you have so far "unlocked". Each ghost has a special ability that
  becomes more and more powerful as you level it up.

* When you have multiple ghosts, you can switch between them using the
  spacebar. This is really the core mechanic of the game: ghosts will continue
  moving in the direct you most recently commanded them, unless they run into a
  wall. So, switching between ghosts and "trapping" PacMan between them (with
  the help of their abilities) is key to hunting down PacMan as the maps grow.
