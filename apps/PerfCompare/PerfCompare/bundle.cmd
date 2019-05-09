@echo off
setlocal

set AppName=PerfCompareRN

set AppRoot=%~dp0
set CurrentRoot=%AppRoot%..\..\..\current
set VNextRoot=%AppRoot%..\..\..\current

pushd %CurrentRoot%

rmdir /s /q %AppName%
mkdir %AppName%

xcopy /q %AppRoot%images\* %AppName%\images\
xcopy /q %AppRoot%app.json %AppName%\
xcopy /q %AppRoot%index.js %AppName%\

call react-native bundle --platform windows --entry-file %AppName%\index.js --bundle-output %AppRoot%ReactAssets\index.windows.bundle --assets-dest %AppRoot%ReactAssets

rmdir /s /q %AppName%

popd

endlocal
