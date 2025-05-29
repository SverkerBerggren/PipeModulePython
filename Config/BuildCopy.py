import shutil
import json
import sys
import os

Arguments =  sys.argv
with open("TestText.txt","w") as File:
    File.write(str(Arguments))
    
if(len(Arguments) < 3):
    print("To few arguments")
    quit()

if(len(Arguments) > 3):
    print("To many arguments")
    quit()
ConditionalString = Arguments[1]
BinaryDir = Arguments[2]

with open('AdditionalBuildDirectories.json') as File:
    JsonData = json.load(File)
    for OutputVariants in JsonData["OutputVariants"]:
        ConditionalStringValue = OutputVariants["ConditionalString"]
        if(ConditionalString == ConditionalStringValue ):
            for FileToCopy in OutputVariants["FilesToCopy"]:
                Matches = []
                for FileName in FileToCopy["FileNames"]:
                    for File in os.listdir(BinaryDir):
                        if(File == FileName):
                            Matches.append(File)
                print(Matches)
                for OutputDir in FileToCopy["OutputDirs"]:
                    for FileMatch in Matches:
                        shutil.copyfile(BinaryDir + "/" + FileMatch, OutputDir +"/" + FileMatch)
                        print(FileMatch)



 

