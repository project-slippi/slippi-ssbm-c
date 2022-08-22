#export TK_PATH=../../MexTK/
#export TK_PATH="/Users/robertperalta/Dev/github/MexTK/MexFF/bin/Release/"
#export COMPILER_PATH="/Users/robertperalta/Dev/github/MexTK/MexFF/bin/Release/MexTK.exe"


#export TK_PATH=/Users/robertperalta/Dev/github/slippi-ssbm-asm/m-ex/MexTK
export TK_PATH=./m-ex/MexTK/
export OUTPUT_PATH=./output/
export BUILD_PATH=./build/
export COMPILER_PATH="$TK_PATH/MexTK.exe"
#export COMPILER_PATH="$TK_PATH/MexTK/MexFF/bin/Release/MexTK.exe"
export SOURCE_FILES=$(find . -type f -regex ".*\.c" -not -path ".*/examples/*" | xargs)

echo "Started Building at: $(date)"
rm -rf $BUILD_PATH

echo "Compiling Ranked Scene..."

#mono $COMPILER_PATH -ff \
#-i "Scenes/Ranked/GameSetup.c" "Components/CharStageBoxSelector.c" "Components/CharStageIcon.c" "Components/Button.c" "Components/FlatTexture.c" "Components/RightArrow.c" "Components/CharPickerDialog.c" "Components/StockIcon.c" "Components/GameResult.c" "Components/TurnIndicator.c" "Game/Characters.c" \
#-s mnFunction \
#-t "$TK_PATH/mnFunction.txt" \
#-l "./melee.link" \
#-b $BUILD_PATH \
#-o "$OUTPUT_PATH/GameSetup.dat" \
#-ow -c

echo "Compiling Slippi CSS..."
mono $COMPILER_PATH -ff \
-i "Scenes/CSS/main.c" \
-s mnFunction \
-t "$TK_PATH/mnFunction.txt" \
-l "./melee.link" \
-b $BUILD_PATH \
-o "$OUTPUT_PATH/SlippiCSS.dat" \
-ow -c -v

cp "$OUTPUT_PATH/SlippiCSS.dat" "/Users/robertperalta/Dev/github/Ishiiruka/build/Binaries/Slippi Dolphin.app/Contents/Resources/Sys/GameFiles/GALE01/SlippiCSS.dat"


#echo "Compiling Everything else..."
#mono $COMPILER_PATH -ff \
#-i $SOURCE_FILES \
#-s slpFunction \
#-t "./slpFunction.txt" \
#-l "./melee.link" \
#-b $BUILD_PATH \
#-o "$OUTPUT_PATH/Slippi.dat" \
#-ow -c

echo "Finished Building at: $(date)"
