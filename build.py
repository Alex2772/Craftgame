num = 0
try:
    file = open("build", "r")
    line = file.readline()
    num = int(line) + 1
    file.close()
except FileNotFoundError:
    pass
file = open("build", "w")
file.write("%d" % num)
file.close()
file = open("Craftgame/build.h", "w")
file.write("#pragma once\n#define CGE_BUILD {}".format(num))