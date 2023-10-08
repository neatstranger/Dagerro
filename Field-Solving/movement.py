lines = []
with open ("coords.txt") as file:
    for line in file:
        lines.append(line.strip('\n').split(' ')[1])


ra = float(lines[0])
dec = float(lines[1])