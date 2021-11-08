# cautious-octo-sniffle
Port of the game "Everlasting Summer" on SEGA mega drive.

## Concept and DevKit
The idea is to rewrite the [original game](https://everlastingsummer.su/) on the 16-bit Sega MEGA drive platform. <br>
The development uses the [SGDK](https://github.com/Stephane-D/SGDK) library - a free and open development kit for the Sega Mega Drive.<br>
IDE - VSCode is used as a lightweight and quickly configurable integrated development environment. <br>
Using the [instructions](https://under-prog.ru/sgdk-%D1%81%D0%BE%D0%B7%D0%B4%D0%B0%D0%B5%D0%BC-hello-world/), the environment was configured for simplified compilation and launch.

## Project workspace
    out/ # game binary (auto generate)
    res/ # pictures, musics and fonts
    src/ # code folder
        boot/     # autogenerate folder for run game and set Game name on emulator
        Game.h    # include SGDK Libs and res/ folder for project
        main.c    # code for game
        Structs.h # includes information from the res/folder for initializing the type of variables
      
    compile.bat # VSCode cmd script for compile project
