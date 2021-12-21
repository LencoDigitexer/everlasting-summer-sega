d={
    'А':'4', 
    'B':'8',
    'C':'[',
    'D':')',
    'E':'3',
    'F':'|=',
    'G':'6',
    'H':'#',
    'I':'1',
    'J':',_|',
    'K':'>|',
    'L':'2',
    'M':'/\/\',
    'N':'^/',
    'O':'()',
    'P':'|*',
    'Q':'(_,)',
    'R':'I2',
    'S':'$',
    'T':'7',
    'U':'(_)',
    'V':'\/',
    'W':'vv',
    'X':'><',
    'Y':'`/',
    'Z':'7_',

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
                    position = line.replace(line[i], d[line[i]])
                    if(position[i] == '\\'):
                        out += "\\"
                    out += position[i]
                    
                except: pass
            print(line)
            result += f'printMessage("{out}", NULL, NULL, NULL);\n'
            out = ""
            
            
                
            

with open("draft_out.txt", "w") as file:
    file.write(result)
