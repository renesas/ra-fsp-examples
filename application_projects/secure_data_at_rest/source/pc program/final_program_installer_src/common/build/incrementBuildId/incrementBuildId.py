#Reads the master buildId, increments it and writes it back.
#Overwrites the dummy stzBuildId.h obtained from GUI with the real build id.

fp = open("c:\\renesas\\Synergy\\stzBuildId.txt", 'r', 0)
idStr = fp.read()
idInt = int(idStr)
idInt = idInt + 1
print ("build Id = " + str(idInt) + "\n")
fp.close()

fp = open("c:\\renesas\\Synergy\\stzBuildId.txt", 'w', 0)
fp.truncate()
fp.write(str(idInt))
fp.close()

fp = open("c:\\renesas\\Synergy\\stzBuildId.h", 'w', 0)
fp.truncate()
fp.write("#define STZ_BUILD_ID " + str(idInt) + "\n")
fp.close()



