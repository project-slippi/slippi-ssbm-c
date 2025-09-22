@ECHO OFF

SET "OUTPUT_FOLDER=output"

:: Necessary prerequisites: devkitPPC and MexTK ----------------------

IF NOT EXIST "C:\devkitPro\devkitPPC\bin\powerpc-eabi-gcc.exe" (
        ECHO ERROR: Compiler not found at C:\devkitPro\devkitPPC\bin\powerpc-eabi-gcc.exe
        ECHO        You must install devkitPro at the root. If you have already installed
        ECHO        it elsewhere, maybe you can symlink the folder to the root?
        EXIT /B 1
) ELSE (
        ECHO Found powerpc-eabi-gcc.exe!
)

IF NOT EXIST "m-ex\MexTK\MexTK.exe" (
        ECHO ERROR: MexTK.exe not found! Did you git submodules?
        ECHO        Try running: "git submodule update --init --recursive"
        EXIT /B 2
) ELSE (
        ECHO Found MexTK.exe!
)

IF NOT EXIST "%OUTPUT_FOLDER%" (
    ECHO Creating output directory "%OUTPUT_FOLDER%"...
    MKDIR "%OUTPUT_FOLDER%"
)

:: GameSetup.dat -----------------------------------------------------

ECHO Building GameSetup...

SET SOURCES="Scenes/Ranked/GameSetup.c"
SET SOURCES=%SOURCES% "Components/CharStageBoxSelector.c"
SET SOURCES=%SOURCES% "Components/CharStageIcon.c"
SET SOURCES=%SOURCES% "Components/Button.c"
SET SOURCES=%SOURCES% "Components/FlatTexture.c"
SET SOURCES=%SOURCES% "Components/RightArrow.c"
SET SOURCES=%SOURCES% "Components/CharPickerDialog.c"
SET SOURCES=%SOURCES% "Components/StockIcon.c"
SET SOURCES=%SOURCES% "Components/GameResult.c"
SET SOURCES=%SOURCES% "Components/TurnIndicator.c"
SET SOURCES=%SOURCES% "Game/Characters.c"

SET FN_TYPE=mnFunction
SET FN_FILE=m-ex/MexTK/mnFunction.txt
SET LINK=melee.link
SET OUTPUT=%OUTPUT_FOLDER%/GameSetup.dat

SET COMMAND=m-ex\MexTK\MexTK.exe -ff -i %SOURCES% -s %FN_TYPE% -o "%OUTPUT%" -t "%FN_FILE%" -q -ow -c -l "%LINK%"
ECHO %COMMAND%
%COMMAND%

:: SlippiCSS.dat -----------------------------------------------------

ECHO Building SlippiCSS...

SET SOURCES="Scenes/CSS/SlippiCSSSetup.c"

SET FN_TYPE=mnFunction
SET FN_FILE=m-ex/MexTK/mnFunction.txt
SET LINK=melee.link
SET OUTPUT=%OUTPUT_FOLDER%/SlippiCSS.dat

SET COMMAND=m-ex\MexTK\MexTK.exe -ff -i %SOURCES% -s %FN_TYPE% -o "%OUTPUT%" -t "%FN_FILE%" -q -ow -c -l "%LINK%"
ECHO %COMMAND%
%COMMAND%

:: Exit --------------------------------------------------------------

EXIT /B 0