<<<<<<< HEAD:UnixUtilites/Readme.md
# Sample of the Output
 
=======
# Readme for the Third Task "Pico Shell"
### note : you have to add their pathes to the env path variable to run from all dir as shell cmd
## Edits from femtoshell 
### executing external (cmd) binary file from file name and from it's path 
### fixed built in pwd 
### Support Pressing Enter only 
### Edit built in echo to support getting enviroment variables and echo them 

## Sample of the Output of FemtoShell

' $ ./mypicoshell.exe '
### start
'abokhalil@stm-linux:$ echo $USER'
'abokhalil '
'abokhalil@stm-linux:$ ls '
'main.c	mypicoshell  PicoShell	Readme.md'
'abokhalil@stm-linux:$ cd  ..'
'abokhalil@stm-linux:$ pwd'
'/home/abokhalil/Desktop/myrepos/STM_SystemProgramming_Linux_Tasks
'abokhalil@stm-linux:$ cd ls'
'errno = 2:Error occurred with cd'
'abokhalil@stm-linux:$ cd  .. '
'abokhalil@stm-linux:$ pwd'
'/home/abokhalil/Desktop/myrepos'
'abokhalil@stm-linux:$ cd STM_SystemProgramming_Linux_Tasks                '
'abokhalil@stm-linux:$ pwd'
'/home/abokhalil/Desktop/myrepos/STM_SystemProgramming_Linux_Tasks'
'abokhalil@stm-linux:$ '
'abokhalil@stm-linux:$ '
'abokhalil@stm-linux:$ '
'abokhalil@stm-linux:$ '
'abokhalil@stm-linux:$ '
'abokhalil@stm-linux:$ echo $PWD           '
'/home/abokhalil/Desktop/myrepos/STM_SystemProgramming_Linux_Tasks/Pico_Shell '
'abokhalil@stm-linux:$ pwd'
'/home/abokhalil/Desktop/myrepos/STM_SystemProgramming_Linux_Tasks'
'abokhalil@stm-linux:$ exit'


### end


## Comilation from main.c dir 
' gcc main.c ./PicoShell/picoshell.c -o mypicoshell ' 


