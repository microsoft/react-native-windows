[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string] $PackageJsonPath,

    [Parameter(Mandatory = $true)]
    [string] $ReactNativeVersion,

    [Parameter(Mandatory = $true)]
    [string] $ReactNativeCliVersion
)

$ErrorActionPreference = 'Stop'

$packageJson = Get-Content -LiteralPath $PackageJsonPath -Raw | ConvertFrom-Json
$dependencySections = @(
    'dependencies',
    'devDependencies',
    'optionalDependencies',
    'peerDependencies',
    'resolutions'
)

foreach ($sectionName in $dependencySections) {
    $section = $packageJson.PSObject.Properties[$sectionName]
    if ($null -eq $section) {
        continue
    }

    foreach ($dependency in $section.Value.PSObject.Properties) {
        $name = $dependency.Name
        if ($name -eq 'react-native' -and $dependency.Value -notmatch '\*') {
            $dependency.Value = $ReactNativeVersion
        }
        elseif ($name -eq '@react-native/new-app-screen') {
            $dependency.Value = $ReactNativeVersion
        }
        elseif ($name -match '^@react-native/.+-(config|preset)$') {
            $dependency.Value = $ReactNativeVersion
        }
        elseif ($name -match '^@react-native-community/cli(?:-platform-(?:ios|android))?$') {
            $dependency.Value = $ReactNativeCliVersion
        }
    }
}

$packageJson | ConvertTo-Json -Depth 100 | Set-Content -LiteralPath $PackageJsonPath -Encoding utf8NoBOM