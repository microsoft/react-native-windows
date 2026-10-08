# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.

$ErrorActionPreference = 'Stop'

& yarn config set npmMinimalAgeGate 8d
if ($LASTEXITCODE -ne 0) {
    throw 'Failed to configure the Yarn package age gate.'
}

$yarnConfigPath = Join-Path (Get-Location) '.yarnrc.yml'
if (Select-String -LiteralPath $yarnConfigPath -Pattern '^npmPreapprovedPackages:' -Quiet) {
    throw 'The generated project already defines Yarn package preapprovals.'
}

@(
    '',
    'npmPreapprovedPackages:',
    '  - "react-native-windows"',
    '  - "@react-native-windows/*"',
    '  - "@rnw-scripts/*"',
    '  - "@office-iss/*"',
    '  - "react-native"',
    '  - "@react-native/*"'
) | Add-Content -LiteralPath $yarnConfigPath -Encoding utf8NoBOM
