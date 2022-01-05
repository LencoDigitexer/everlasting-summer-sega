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
    'Й':'J',
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
    'Ы':"\\\\",
    'Ь':']',
    'Э':'^',
    'Ю':'_',
    'Я':'`',
    'а':'a', 
    'б':'b',
    'в':'c',
    'г':'d',
    'д':'e',
    'е':'f',
    'ё':'g',
    'ж':'h',
    'з':'i',
    'и':'j',
    'й':'j',
    'к':'k',
    'л':'l',
    'м':'m',
    'н':'n',
    'о':'o',
    'п':'p',
    'р':'q',
    'с':'r',
    'т':'s',
    'у':'t',
    'ф':'u',
    'х':'v',
    'ц':'w',
    'ч':'x',
    'ш':'y',
    'щ':'z',
    'ъ':'[',
    'ы':"\\\\",
    'ь':']',
    'э':'$',
    'ю':'&',
    'я':'@',
    '.':'.',
    ',':',',
    ':':':',
    ';':';',
    ' ':' ',
    '!':'!',
    '?':'?',
    '-':'-',
    '"':'',
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
            
            result += f'printMessage("{out}", NULL, NULL, NULL);\n'
            out = ""
            
            
                
            

with open("draft_out.txt", "w") as file:
    file.write(result)
