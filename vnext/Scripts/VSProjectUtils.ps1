# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.

# Checks that $PackagesConfigPath is to a packages.config file
# If $ExistingFile, check that the file exists, else that it doesn't exist
function Check-PackagesConfigXml {
    param(
        [string] $PackagesConfigPath,
        [bool] $ExistingFile = $True
    )

    Write-Host
    Write-Host "Checking " -NoNewLine
    Write-Host -ForegroundColor White $PackagesConfigPath -NoNewLine
    Write-Host "..." -NoNewLine

    if ((Split-Path -Path $PackagesConfigPath -Leaf) -ne 'packages.config') {
        Write-Host " error."
        throw "$PackagesConfigPath is not a packages.config file."
    }

    if ($ExistingFile -and -not(Test-Path -Path $PackagesConfigPath)) {
        Write-Host " error."
        throw "$PackagesConfigPath does not exist."
    } elseif (-not($ExistingFile) -and (Test-Path -Path $PackagesConfigPath)) {
        Write-Host " error."
        throw "$PackagesConfigPath already exists."
    }

    Write-Host " done."
}

# Creates a new packages.config file at $PackagesConfigPath
# If $CheckConfig, check the file doesn't already exist first
function Create-PackagesConfigXml {
    param(
        [string] $PackagesConfigPath,
        [bool] $CheckConfig = $True
    )

    if ($CheckConfig) {
        Check-PackagesConfigXml -PackagesConfigPath $PackagesConfigPath -ExistingFile $False
    }

    Write-Host
    Write-Host "Creating " -NoNewLine
    Write-Host -ForegroundColor White $PackagesConfigPath -NoNewLine
    Write-Host "..." -NoNewLine

    [xml]$packageXml = '<?xml version="1.0" encoding="utf-8"?>
                        <packages>
                        </packages>'

    $packageXml.Save($PackagesConfigPath)

    Write-Host " done."
}

# Loads the packages.config file at $PackagesConfigPath
# If $CheckConfig, check the file exists first
function Load-PackagesConfigXml {
    param(
        [string] $PackagesConfigPath,
        [bool] $CheckConfig = $True
    )

    if ($CheckConfig) {
        Check-PackagesConfigXml -PackagesConfigPath $PackagesConfigPath
    }

    Write-Host
    Write-Host "Loading " -NoNewLine
    Write-Host -ForegroundColor White $PackagesConfigPath -NoNewLine
    Write-Host "..." -NoNewLine

    [xml]$content = Get-Content -Path $PackagesConfigPath

    Write-Host " done."

    $content.packages.package | foreach {
            Write-Host "Found package " -NoNewLine
            Write-Host -ForegroundColor White $_.id -NoNewLine
            Write-Host " version " -NoNewLine
            Write-Host -ForegroundColor White $_.version -NoNewLine
            Write-Host "."
        }

    return $content;
}

# Update package $PackageName to version $PackageVersion in the packages.config
# file at $PackagesConfigPath
# If $CheckConfig, check the file exists first
function Update-PackagesConfigXml {
    param(
        [string] $PackagesConfigPath,
        [string] $PackageName,
        [string] $PackageVersion,
        [bool] $CheckConfig = $True
    )

    if ($CheckConfig) {
        Check-PackagesConfigXml -PackagesConfigPath $PackagesConfigPath
    }

    Write-Host
    Write-Host "Updating package " -NoNewLine
    Write-Host -ForegroundColor White $PackageName -NoNewLine
    Write-Host " to version " -NoNewLine
    Write-Host -ForegroundColor White $PackageVersion -NoNewLine
    Write-Host "..."

    & nuget.exe update "$PackagesConfigPath" -Id "$PackageName" -Version "$PackageVersion"
}

# Add package $PackageName version $PackageVersion to the packages.config
# file at $PackagesConfigPath
# If $CheckConfig, check the file exists first
function Add-PackagesConfigXml {
    param(
        [string] $PackagesConfigPath,
        [string] $PackageName,
        [string] $PackageVersion,
        [bool] $CheckConfig = $True
    )

    $packageXml = Load-PackagesConfigXml -PackagesConfigPath $PackagesConfigPath -CheckConfig $CheckConfig

    $existingPackageNode = $packageXml.selectSingleNode("packages/package[@id=""$PackageName""]")

    if ($existingPackageNode) {
        Write-Host
        Write-Host "Nothing to add, package " -NoNewLine
        Write-Host -ForegroundColor White $PackageName -NoNewLine
        Write-Host " is already present."
    } else {
        Write-Host
        Write-Host "Adding package " -NoNewLine
        Write-Host -ForegroundColor White $PackageName -NoNewLine
        Write-Host "..." -NoNewLine

        $packagesNode = $packageXml.selectSingleNode('packages')
        $newPackageNode = $packagesNode.appendChild($packageXml.CreateElement('package'))
        $newPackageNode.SetAttribute('id', $PackageName)
        $newPackageNode.SetAttribute('version', '0')

        $packageXml.Save($PackagesConfigPath)

        Write-Host " done."
    }

    Update-PackagesConfigXml -PackagesConfigPath $PackagesConfigPath -PackageName $PackageName -PackageVersion $PackageVersion -CheckConfig $False
}
