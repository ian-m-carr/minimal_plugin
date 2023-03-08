@echo off
setlocal

IF %1.==. GOTO RELEASE_BUILD
set BUILD_CONFIG=cmake-build-debug
set SUFFIX=-d
goto INSTALL

:RELEASE_BUILD
set BUILD_CONFIG=cmake-build-release
set SUFFIX=

:INSTALL

set PLUGIN_NAME=minimal_plugin
set INSTALL_DIR="G:\SteamLibrary\steamapps\common\X-Plane 12\Resources\plugins\%PLUGIN_NAME%"

if not exist %INSTALL_DIR% mkdir %INSTALL_DIR%\win_x64

rem Name with _d suffix for debug
set SOURCE_PLUGIN_DLL_NAME=%PLUGIN_NAME%%SUFFIX%.xpl
set SOURCE_PLUGIN_PDB_NAME=%PLUGIN_NAME%%SUFFIX%.pdb

rem name without the suffix in installation folder
set PLUGIN_DLL_NAME=%PLUGIN_NAME%.xpl
set PLUGIN_PDB_NAME=%PLUGIN_NAME%.pdb

echo INSTALLING FROM: %BUILD_CONFIG%\%SOURCE_PLUGIN_DLL_NAME% TO: %INSTALL_DIR%\win_x64\%PLUGIN_DLL_NAME%

rem remove the old
IF EXIST %INSTALL_DIR%\win_x64\%PLUGIN_DLL_NAME%.old del %INSTALL_DIR%\win_x64\%PLUGIN_DLL_NAME%.old

rem rename the existing
IF EXIST %INSTALL_DIR%\win_x64\%PLUGIN_DLL_NAME% ren %INSTALL_DIR%\win_x64\%PLUGIN_DLL_NAME% %PLUGIN_DLL_NAME%.old

rem copy the new
copy %BUILD_CONFIG%\%SOURCE_PLUGIN_DLL_NAME% %INSTALL_DIR%\win_x64\%PLUGIN_DLL_NAME%
copy %BUILD_CONFIG%\%SOURCE_PLUGIN_PDB_NAME% %INSTALL_DIR%\win_x64\%PLUGIN_PDB_NAME%

copy resources\*.jpg %INSTALL_DIR%