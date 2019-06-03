:: Copyright (c) Microsoft Corporation. All rights reserved.
:: Licensed under the MIT License.

@echo off
setlocal

pushd %~dp0\..\

set BundlePath=Universal.SampleApp\Bundle\Universal.SampleApp

echo Bundling Bootstrap
call react-native bundle --platform uwp --entry-file Universal.SampleApp\index.uwp.js --bundle-output %BundlePath%\index.uwp.bundle --assets-dest %BundlePath%

popd

endlocal

exit /b