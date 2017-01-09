REM ROBOCOPY $(OutputPath) $(HOME_DROPBOX)eng\my_app\ $(TargetFileName) /L /V
REM %1 - $(OutputPath)
REM %2 - $(HOME_DROPBOX)eng\my_app\
REM %3 - $(TargetFileName)
ROBOCOPY %1 %2 %3 /L /V