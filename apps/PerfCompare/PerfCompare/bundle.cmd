@echo off
setlocal

set AppName=PerfCompareRN

set AppRoot=%~dp0

echo Building index.windows.bundle

set CurrentRoot=%AppRoot%..\..\..\current

pushd %CurrentRoot%

rmdir /s /q %AppName%
mkdir %AppName%

xcopy /q %AppRoot%images\* %AppName%\images\
xcopy /q %AppRoot%app.json %AppName%\
xcopy /q %AppRoot%index.js %AppName%\

call react-native bundle --platform windows --entry-file %AppName%\index.js --bundle-output %AppRoot%ReactAssets\index.windows.bundle --assets-dest %AppRoot%ReactAssets

rmdir /s /q %AppName%

popd

echo Building index.uwp.bundle

set VNextRoot=%AppRoot%..\..\..\vnext

pushd %VNextRoot%

rmdir /s /q %AppName%
mkdir %AppName%

xcopy /q %AppRoot%images\* %AppName%\images\
xcopy /q %AppRoot%app.json %AppName%\
xcopy /q %AppRoot%index.js %AppName%\

call react-native bundle  --platform uwp --entry-file %AppName%\index.js --bundle-output %AppRoot%Bundle\index.uwp.bundle --assets-dest %AppRoot%Bundle

rmdir /s /q %AppName%

popd

endlocal
