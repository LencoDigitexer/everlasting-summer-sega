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
    
## Problems and utilities fixes

1) The SDK does not support Cyrillic. There have been attempts to use a common font file from the res folder, but the method is not suitable for other functions, because it uses an array of bytes instead of a string. 

solution: use a single font file from the res folder, replace all English letters with Russian letters.
Russian Russian alphabet has 20 letters, and 33. 13 ASCII characters of the code will have to be sacrificed for Russian letters. Moreover, the letters "B", "Y", "B" do not need to be written twice - a small letter is enough.
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
