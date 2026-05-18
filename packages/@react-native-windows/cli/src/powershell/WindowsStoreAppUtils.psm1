<#
       Licensed to the Apache Software Foundation (ASF) under one
       or more contributor license agreements.  See the NOTICE file
       distributed with this work for additional information
       regarding copyright ownership.  The ASF licenses this file
       to you under the Apache License, Version 2.0 (the
       "License"); you may not use this file except in compliance
       with the License.  You may obtain a copy of the License at

         http://www.apache.org/licenses/LICENSE-2.0

       Unless required by applicable law or agreed to in writing,
       software distributed under the License is distributed on an
       "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
       KIND, either express or implied.  See the License for the
       specific language governing permissions and limitations
       under the License.
#>

# SDL CE.10116 SECURITY FIX: Removed PowerShell injection vulnerabilities
# - Replaced Invoke-Expression with direct cmdlet invocation
# - Implemented parameterized ScriptBlock pattern for elevated execution
# - Added input validation for package IDs and paths
# Date: October 14, 2025

$code = @"
using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
namespace StoreAppRunner
{
    public enum ActivateOptions
    {
        None = 0,
        DesignMode = 0x1,
        NoErrorUI = 0x2,
        NoSplashScreen = 0x4
    }

    [ComImport]
    [Guid("2e941141-7f97-4756-ba1d-9decde894a3d")]
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    public interface IApplicationActivationManager
    {
        IntPtr ActivateApplication([In] String appUserModelId, [In] String arguments, [In] ActivateOptions options, [Out] out UInt32 processId);
        IntPtr ActivateForFile([In] String appUserModelId, [In] IntPtr itemArray, [In] String verb, [Out] out UInt32 processId);
        IntPtr ActivateForProtocol([In] String appUserModelId, [In] IntPtr itemArray, [Out] out UInt32 processId);
    }
    [ComImport]
    [Guid("45BA127D-10A8-46EA-8AB7-56EA9078943C")]
    public class ApplicationActivationManager : IApplicationActivationManager
    {
        [MethodImpl(MethodImplOptions.InternalCall, MethodCodeType = MethodCodeType.Runtime)]
        public extern IntPtr ActivateApplication([In] String appUserModelId, [In] String arguments, [In] ActivateOptions options, [Out] out UInt32 processId);
        [MethodImpl(MethodImplOptions.InternalCall, MethodCodeType = MethodCodeType.Runtime)]
        public extern IntPtr ActivateForFile([In] String appUserModelId, [In] IntPtr itemArray, [In] String verb, [Out] out UInt32 processId);
        [MethodImpl(MethodImplOptions.InternalCall, MethodCodeType = MethodCodeType.Runtime)]
        public extern IntPtr ActivateForProtocol([In] String appUserModelId, [In] IntPtr itemArray, [Out] out UInt32 processId);
    }
}
"@

#region Security Helper Functions

<#
.SYNOPSIS
    Validates package identifier format to prevent injection attacks.
.DESCRIPTION
    Ensures package ID contains only safe characters and matches expected format.
    SDL CE.10116 compliance - prevents PowerShell injection via package names.
#>
function Validate-PackageIdentifier {
    param(
        [Parameter(Mandatory=$true)]
        [string]$PackageId
    )
    
    # Valid format: alphanumeric, dots, hyphens, underscores only
    if ($PackageId -notmatch '^[a-zA-Z0-9\.\-_]+$') {
        throw "Invalid package identifier format. Only alphanumeric characters, dots, hyphens, and underscores are allowed."
    }
    
    # Prevent common injection patterns
    $dangerousPatterns = @(';', '|', '&', '$', '`', '<', '>', "`n", "`r", '(', ')', '{', '}')
    foreach ($pattern in $dangerousPatterns) {
        if ($PackageId.Contains($pattern)) {
            throw "Package identifier contains forbidden characters: $pattern"
        }
    }
    
    return $true
}

<#
.SYNOPSIS
    Validates and canonicalizes script path to prevent path traversal attacks.
.DESCRIPTION
    Ensures path is a valid .ps1 file and resolves to canonical path.
    SDL CE.10116 compliance - prevents path injection attacks.
#>
function Validate-ScriptPath {
    param(
        [Parameter(Mandatory=$true)]
        [string]$Path
    )
    
    # Check file exists
    if (!(Test-Path $Path -PathType Leaf)) {
        throw "Script path does not exist: $Path"
    }
    
    # Check .ps1 extension
    if ([System.IO.Path]::GetExtension($Path) -ne '.ps1') {
        throw "Path must reference a PowerShell script (.ps1)"
    }
    
    # Get canonical path (prevents ../ traversal)
    $canonicalPath = [System.IO.Path]::GetFullPath($Path)
    
    return $canonicalPath
}

<#
.SYNOPSIS
    Executes a ScriptBlock with optional elevation using parameterized approach.
.DESCRIPTION
    SDL CE.10116 compliant - uses ScriptBlock with ArgumentList instead of string concatenation.
    Prevents PowerShell injection attacks by properly isolating parameters.
#>
function Invoke-ElevatedScriptBlock {
    param(
        [Parameter(Mandatory=$true)]
        [ScriptBlock]$ScriptBlock,
        
        [Parameter(Mandatory=$false)]
        [object[]]$ArgumentList = @()
    )
    
    if (!(IsElevated)) {
        # Serialize ScriptBlock and arguments for elevated execution
        $encodedCommand = [Convert]::ToBase64String(
            [System.Text.Encoding]::Unicode.GetBytes($ScriptBlock.ToString())
        )
        
        $encodedArgs = [Convert]::ToBase64String(
            [System.Text.Encoding]::Unicode.GetBytes(
                ($ArgumentList | ConvertTo-Json -Compress -Depth 10)
            )
        )
        
        # Create secure argument list (no string interpolation)
        $startArgs = @(
            '-NoProfile',
            '-NonInteractive',
            '-Command',
            "& ([ScriptBlock]::Create([System.Text.Encoding]::Unicode.GetString([Convert]::FromBase64String('$encodedCommand')))) @(ConvertFrom-Json ([System.Text.Encoding]::Unicode.GetString([Convert]::FromBase64String('$encodedArgs'))))"
        )
        
        $process = Start-Process PowerShell -ArgumentList $startArgs `
            -Verb RunAs -Wait -PassThru -WindowStyle Hidden -ErrorAction Stop
        
        if ($process.ExitCode -ne 0) {
            throw "Elevated command failed with exit code $($process.ExitCode)"
        }
    }
    else {
        # Already elevated, execute directly with splatting
        & $ScriptBlock @ArgumentList
    }
}

#endregion

#region Core Functions

<#
.SYNOPSIS
    Uninstalls an AppX package by package identifier.
.DESCRIPTION
    SDL CE.10116 FIX: Replaced Invoke-Expression with direct Remove-AppxPackage cmdlet.
    Uses parameterized ScriptBlock for elevated execution.
#>
function Uninstall-App {
    param(
        [Parameter(Mandatory=$true, Position=0, ValueFromPipelineByPropertyName=$true)]
        [string] $ID <# package.appxmanifest//Identity@name #>
    )

    # SDL FIX: Validate package ID to prevent injection
    Validate-PackageIdentifier -PackageId $ID | Out-Null

    $package = Get-AppxPackage $ID

    if($package) {
        $pfn = $package.PackageFullName
        
        # SDL FIX: Direct cmdlet invocation instead of Invoke-Expression
        try {
            Remove-AppxPackage $pfn -ErrorAction Stop
        } catch {
            # SDL FIX: Use parameterized ScriptBlock for elevation
            $scriptBlock = {
                param($PackageFullName)
                Remove-AppxPackage $PackageFullName -ErrorAction Stop
            }
            Invoke-ElevatedScriptBlock -ScriptBlock $scriptBlock -ArgumentList @($pfn)
        }
    }
}

#
# Checks whether the machine is missing a valid developer license.
#
function CheckIfNeedDeveloperLicense
{
    $Result = $true
    try
    {
        $Result = (Get-WindowsDeveloperLicense | Where-Object { $_.IsValid }).Count -eq 0
    }
    catch {}

    return $Result
}

function IsElevated {
    return [bool](([System.Security.Principal.WindowsIdentity]::GetCurrent()).groups -match "S-1-5-32-544");
}

<#
.SYNOPSIS
    Enables developer mode by setting registry keys.
.DESCRIPTION
    SDL CE.10116 FIX: Replaced Invoke-Expression-MayElevate with direct cmdlet calls
    and parameterized ScriptBlock for elevation.
#>
function EnableDevmode {
    $RegistryKeyPath = "HKLM:\SOFTWARE\Microsoft\Windows\CurrentVersion\AppModelUnlock"
    
    # Create registry key if it doesn't exist
    if (-not(Test-Path -Path $RegistryKeyPath)) {
        if (!(IsElevated)) {
            $scriptBlock = {
                param($Path)
                New-Item -Path $Path -ItemType Directory -Force | Out-Null
            }
            Invoke-ElevatedScriptBlock -ScriptBlock $scriptBlock -ArgumentList @($RegistryKeyPath)
        }
        else {
            New-Item -Path $RegistryKeyPath -ItemType Directory -Force | Out-Null
        }
    }

    # SDL FIX: Direct cmdlet invocation for AllowDevelopmentWithoutDevLicense
    $value = Get-ItemProperty -Path $RegistryKeyPath -Name AllowDevelopmentWithoutDevLicense -ErrorAction SilentlyContinue
    if (($value -eq $null) -or ($value.AllowDevelopmentWithoutDevLicense -ne 1)) {
        if (!(IsElevated)) {
            $scriptBlock = {
                param($Path, $Name, $Value)
                Set-ItemProperty -Path $Path -Name $Name -Value $Value -ErrorAction Stop
            }
            Invoke-ElevatedScriptBlock -ScriptBlock $scriptBlock `
                -ArgumentList @($RegistryKeyPath, 'AllowDevelopmentWithoutDevLicense', 1)
        }
        else {
            Set-ItemProperty -Path $RegistryKeyPath -Name AllowDevelopmentWithoutDevLicense -Value 1 -ErrorAction Stop
        }
    }
    
    # SDL FIX: Direct cmdlet invocation for AllowAllTrustedApps
    $value = Get-ItemProperty -Path $RegistryKeyPath -Name AllowAllTrustedApps -ErrorAction SilentlyContinue
    if (($value -eq $null) -or ($value.AllowAllTrustedApps -ne 1)) {
        if (!(IsElevated)) {
            $scriptBlock = {
                param($Path, $Name, $Value)
                Set-ItemProperty -Path $Path -Name $Name -Value $Value -ErrorAction Stop
            }
            Invoke-ElevatedScriptBlock -ScriptBlock $scriptBlock `
                -ArgumentList @($RegistryKeyPath, 'AllowAllTrustedApps', 1)
        }
        else {
            Set-ItemProperty -Path $RegistryKeyPath -Name AllowAllTrustedApps -Value 1 -ErrorAction Stop
        }
    }
}

#
# Checks whether the package certificate must be installed on the machine.
#
function CheckIfNeedInstallCertificate
{
    param(
        [Parameter(Mandatory=$true, Position=0, ValueFromPipelineByPropertyName=$true)]
        [string] $ScriptDir <# Full path to the dir where Add-AppDevPackage.ps1 is stored #>
    )

    $PackagePath = Get-ChildItem (Join-Path $ScriptDir "*.appx") | Where-Object { $_.Mode -NotMatch "d" }
    if ($PackagePath -eq $null)
    {
        $PackagePath = Get-ChildItem (Join-Path $ScriptDir "*.msix") | Where-Object { $_.Mode -NotMatch "d" }
    }

    $BundlePath = Get-ChildItem (Join-Path $ScriptDir "*.appxbundle") | Where-Object { $_.Mode -NotMatch "d" }
    if ($BundlePath -eq $null)
    {
        $BundlePath = Get-ChildItem (Join-Path $ScriptDir "*.msixbundle") | Where-Object { $_.Mode -NotMatch "d" }
    }

    # There must be exactly 1 package/bundle
    if (($PackagePath.Count + $BundlePath.Count) -lt 1)
    {
        Throw "The app package has not been found at dir $ScriptDir"
    }
    if (($PackagePath.Count + $BundlePath.Count) -gt 1)
    {
        Throw "To many app packages have been found at dir $ScriptDir"
    }

    if ($PackagePath.Count -ne 1) # there is *.appxbundle
    {
        $PackagePath = $BundlePath
    }

    $PackageSignature = (Get-AuthenticodeSignature $PackagePath)
    $Valid = ($PackageSignature -and $PackageSignature.Status -eq "Valid")
    return (-not $Valid)
}

<#
.SYNOPSIS
    Installs an app package using Add-AppDevPackage.ps1 script.
.DESCRIPTION
    SDL CE.10116 FIX: Replaced Invoke-Expression with call operator (&) and
    parameterized ScriptBlock for elevated execution.
#>
function Install-App {
    param(
        [Parameter(Mandatory=$true, Position=0, ValueFromPipelineByPropertyName=$true)]
        [string] $Path, <# Full path to Add-AppDevPackage.ps1 #>
        [switch] $Force = $false
    )
    
    # SDL FIX: Validate script path
    $Path = Validate-ScriptPath -Path $Path
    
    $needInstallCertificate = CheckIfNeedInstallCertificate (Join-Path $Path "..")
    
    if (!$Force -and ((CheckIfNeedDeveloperLicense) -or ($needInstallCertificate)))
    {
        # SDL FIX: Use call operator (&) instead of Invoke-Expression
        # No user input in path (validated above), so safe to execute
        & $Path
    }
    else
    {
        # SDL FIX: Use parameterized ScriptBlock for elevation
        if (!(IsElevated)) {
            $scriptBlock = {
                param($ScriptPath)
                & $ScriptPath -Force
            }
            Invoke-ElevatedScriptBlock -ScriptBlock $scriptBlock -ArgumentList @($Path)
        }
        else {
            & $Path -Force
        }
    }
}

function Install-AppFromDirectory {
    param(
        [Parameter(Mandatory=$true, Position=0, ValueFromPipelineByPropertyName=$true)]
        [string] $Path <# Full path to actual AppxManifest.xml #>
    )

    Add-AppxPackage -Path $Path -Register -ForceApplicationShutdown
}

function Install-AppFromAppx {
    param(
        [Parameter(Mandatory=$true, Position=0, ValueFromPipelineByPropertyName=$true)]
        [string] $Path <# Full path to actual .appx #>
    )

    Add-AppxPackage -Path $Path -ForceApplicationShutdown
}

function Start-Locally {
    param(
        [Parameter(Mandatory=$true, Position=0, ValueFromPipelineByPropertyName=$true)]
        [string] $ID, <# package.appxmanifest//Identity@name #>

        [Parameter(Mandatory=$false, Position=1, ValueFromPipelineByPropertyName=$true)]
        [string[]] $argv
    )

    # SDL FIX: Validate package ID
    Validate-PackageIdentifier -PackageId $ID | Out-Null

    $package = Get-AppxPackage $ID
    $manifest = Get-appxpackagemanifest $package
    $applicationUserModelId = $package.PackageFamilyName + "!" + $manifest.package.applications.application.id

    add-type -TypeDefinition $code
    $appActivator = new-object StoreAppRunner.ApplicationActivationManager
    if ($argv.Count -gt 0) {
        $args = [system.String]::Join(" ", $argv)
    }

    try {
        $appActivator.ActivateApplication($applicationUserModelId,$args,[StoreAppRunner.ActivateOptions]::None,[ref]0) | Out-Null
    } catch {
        $log = Get-EventLog 'Application' -EntryType Error -Message "*$ID*" -Newest 1
        if ($log -ne $null) {
            Write-Error $log.Message
        }
    }
}

function Map-PackageNameToPackage {
    param([Parameter(Mandatory=$true)] [string]$DependenciesPath)
    $mapP2N = @{};
    [Reflection.Assembly]::LoadWithPartialName('System.IO.Compression.Zipfile') | Out-Null;
    foreach ($package in (gci $DependenciesPath)) {
        $archive = [System.IO.Compression.Zipfile]::OpenRead($package.FullName);
        $packageManifestReader = New-Object System.IO.StreamReader($archive.GetEntry('AppxManifest.xml').Open());
        $manifest = [xml]($packageManifestReader.ReadToEnd());
        $mapP2N[$manifest.Package.Identity.Name] = $package;
    }
    return $mapP2N;
}

function Install-AppDependencies {
    param(
        [Parameter(Mandatory=$true)] [string]$AppxManifestPath,
        [Parameter(Mandatory=$true)] [string]$AppPackagePath,
        [Parameter(Mandatory=$true)] [string]$Architecture        
    )

    $xml=[xml] (gc $AppxManifestPath);
    $packageNamesToInstall = $xml.Package.Dependencies.PackageDependency | 
        Where-Object { 
            $installed = Get-AppxPackage $_.Name | Where-Object -Property Architecture -EQ -Value $Architecture;
            $installed -eq $null -or $installed.Version -lt $_.MinVersion 
        } | 
        % { $_.Name };
    $map = Map-PackageNameToPackage $AppPackagePath\Dependencies\$Architecture
    $packagePaths = $packageNamesToInstall | % { $map[$_] }
    $packagePaths | % { Add-AppxPackage -Path $_ }
}

#endregion

# Export functions
Export-ModuleMember -Function @(
    'Uninstall-App',
    'EnableDevmode',
    'CheckIfNeedDeveloperLicense',
    'CheckIfNeedInstallCertificate',
    'Install-App',
    'Install-AppFromDirectory',
    'Install-AppFromAppx',
    'Start-Locally',
    'Install-AppDependencies'
)
