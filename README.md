# everlasting-summer-sega
Port of the game "Everlasting Summer" on SEGA mega drive.
![image](https://user-images.githubusercontent.com/44779327/159342968-282cd254-bda9-4083-9003-8f163a163a72.png)


# Development status
- develop an engine (progress 100%)
- port the prologue and release a ALPHA version
- automate the conversion of original art into an 8-bit version in photoshop
- port the all 7 day and release a DEMO version
- write an 8-bit OST on the VGM-music algorithm
- testing the game

## The plot of the game
The main character ends up in the pioneer camp "Sovenok", where he must live for 7 days and solve the secret of the mysterious movement, or maybe find new acquaintances or even the love of his life.

## Future plans
- Release the game on https://itch.io/
- Assemble a physical cartridge for the original SEGA

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
    
## Problems and utilities fixes

1) The SDK does not support Cyrillic. There have been attempts to use a common font file from the res folder, but the method is not suitable for other functions, because it uses an array of bytes instead of a string. 

solution: use a single font file from the res folder, replace all English letters with Russian letters.
restriction: Russian alphabet consists of 33 letters, and English of 26. 6 ASCII characters of the code will have to be sacrificed for Russian letters. Moreover, the letters "Ъ", "Ы", "Ь" do not need to be written twice - a small letter is enough.
```python
d={
    'А':'A', 
    'Б':'B',
    'В':'C',
    'Г':'D',
    'Д':'E',
    'Е':'F',
    'Ё':'G',
    'Ж':'H',
    'З':'I',
    'И':'J',
    'К':'K',
    'Л':'L',
    'М':'M',
    'Н':'N',
    'О':'O',
    'П':'P',
    'Р':'Q',
    'С':'R',
    'Т':'S',
    'У':'T',
    'Ф':'U',
    'Х':'V',
    'Ц':'W',
    'Ч':'X',
    'Ш':'Y',
    'Щ':'Z',
    'Ъ':'[',
    'Ы':"\\",
    'Ь':']',
    'Э':'^',
    'Ю':'_',
    'Я':'@',
    '.':'.',
    ',':',',
    ':':':',
    ';':';',
    ' ':' ',
    '!':'!',
    '?':'?',
    '-':'-',

    } # словарь

result = ""
out = ""
with open("draft.txt", "r") as file:
    lines = file.readlines()
    for line in lines:
        if line[0] != '/' and line[0] != '[' and line != '\n' and line[0] != '>':
            line = line[:-1]
            for i in range(len(line)):
                try:
                    position = line.replace(line[i].upper(), d[line[i]])
                    out += position[i] 
                    print(out)
                except: pass
            result += f'printMessage("{out}", NULL, NULL, NULL);\n'    
            

with open("draft_out.txt", "w") as file:
    file.write(result)

```


## Convert original art
SEGA mega drive uses two screen resolution settings depending on the type of console:
 - Two main machine types:

    - NTSC machines

        - "Sega Genesis", 60Hz machines, primarily found in the US

        - Resolutions

            - H40 mode - 320x224 px (40x28 tiles). More common resolution mode

            - H32 mode - 256x224 px (32x28 tiles). Less common resolution mode

        - SGDK games are NTSC use

    - PAL machines

        - "Sega Mega Drive", 50Hz machines, everywhere else

        - Resolution

            - H40 mode - 320x240 px (40x30 tiles). More common resolution mode

            - H32 mode - 256x240 (32x30 tiles). Less common resolution mode

In this project use H40 mode 320x224 px.<br>
All images use 8 bit channel of indexed color.<br>
In Adobe Photoshop used Local(adaptive) pallitre with 32 color and transparency.<br>
The local(adaptive) pallitre used points color for best mixed color on little resolution.
