result = ""
with open("draft.txt", "r") as file:
    lines = file.readlines()
    for line in lines:
        if line[0] != '/' and line[0] != '[' and line != '\n' and line[0] != '>':
            line = line[:-1]
            result += f'printMessage("{line}", NULL, NULL, NULL);\n'
with open("draft_out.txt", "w") as file:
    file.write(result)
    