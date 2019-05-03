:: Copyright (c) Microsoft Corporation. All rights reserved.
:: Licensed under the MIT License.

@echo off
setlocal

pushd %~dp0\..\

echo Bundling Bootstrap
call react-native bundle --platform uwp --entry-file Universal.SampleApp\index.uwp.js --bundle-output Universal.SampleApp\Bundle\Universal.SampleApp\index.uwp.bundle --assets-dest Universal.SampleApp\Bundle\Universal.SampleApp

popd

endlocal

exit /b