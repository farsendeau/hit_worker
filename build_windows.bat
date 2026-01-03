@echo off
REM Script de compilation pour Windows
REM Assurez-vous d'avoir CMake et MinGW ou MSVC installés

echo ========================================
echo Compilation de hit_worker pour Windows
echo ========================================

REM Définir le chemin MinGW
set MINGW_PATH=E:\Softs\MinGW
set PATH=%MINGW_PATH%\bin;%PATH%

REM Vérifier que MinGW est accessible
where gcc >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo ERREUR: MinGW GCC non trouve!
    echo Verifiez que MinGW est installe dans: %MINGW_PATH%
    pause
    exit /b 1
)

echo MinGW trouve: %MINGW_PATH%

REM Créer le dossier de build s'il n'existe pas
if not exist "build_win" mkdir build_win
cd build_win

REM Configuration avec CMake
echo.
echo Configuration du projet...
cmake .. -G "MinGW Makefiles" ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DCUSTOM_OUTPUT_DIR="C:/Users/sixpo/Desktop" ^
    -DALLEGRO_PREFIX="C:/allegro" ^
    -DCMAKE_C_COMPILER="%MINGW_PATH%\bin\gcc.exe" ^
    -DCMAKE_CXX_COMPILER="%MINGW_PATH%\bin\g++.exe"

if %ERRORLEVEL% NEQ 0 (
    echo Erreur lors de la configuration!
    pause
    exit /b %ERRORLEVEL%
)

REM Compilation
echo.
echo Compilation...
cmake --build . --config Release

if %ERRORLEVEL% NEQ 0 (
    echo Erreur lors de la compilation!
    pause
    exit /b %ERRORLEVEL%
)

echo.
echo ========================================
echo Compilation terminee avec succes!
echo L'executable se trouve dans: C:\Users\sixpo\Desktop\hit_worker.exe
echo ========================================
pause
