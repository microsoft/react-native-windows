# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.
#
# Eval-MsBuildProperties.ps1
#
# This script lets you determine the final values of MSBUILD properties for a
# given solution and project file. Simply pass in a comma delimited list of
# property names and you'll get a JSON blob of evaluated values.
#
# For example, from the root of the repo:
#
# .\packages\@react-native-windows\cli\powershell\Eval-MsBuildProperties.ps1 -SolutionFile 'vnext\Microsoft.ReactNative.sln' -ProjectFile 'vnext\Microsoft.ReactNative\Microsoft.ReactNative.vcxproj' -PropertyNames 'ProjectGUID,ProjectName'
#
# will output:
#
# {
#     "ProjectGuid":  "{f7d32bd0-2749-483e-9a0d-1635ef7e3136}",
#     "ProjectName":  "Microsoft.ReactNative"
# }
#

param(
    [Parameter(Mandatory = $true)]
    [String]$SolutionFile,
    [Parameter(Mandatory = $true)]
    [String]$ProjectFile,
    [Parameter()]
    [String]$PropertyNames = "",
    [Parameter()]
    [String]$MSBuildPath,
    [Parameter()]
    [String]$ExtraMSBuildProps
)

function Get-MSBuildPath {
    $vsWhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
    if (!(Test-Path $vsWhere)) {
        throw "Unable to find vswhere.exe."
    }
    $vsPath = & $vsWhere -version 16.5 -property installationPath;
    return "$vsPath\MSBuild\Current\Bin";
}

function Get-MSBuildProperties {
    param (
        [Parameter(Mandatory = $true)]
        [String]$MSBuildPath,
        [Parameter(Mandatory = $true)]
        [String]$SolutionPath,
        [Parameter(Mandatory = $true)]
        [String]$ProjectPath,
        [Parameter()]
        [String[]]$PropertyNames = @(),
        [Parameter()]
        [String]$ExtraMSBuildProps
    )

    if (!(Test-Path (Join-Path $MSBuildPath "MSBuild.exe"))) {
        throw "Unable to find MSBuild.exe in $MSBuildPath"
    }

    if (!(Test-Path (Join-Path $MSBuildPath "MSBuild.exe"))) {
        throw "Unable to find Microsoft.Build.dll in $MSBuildPath"
    }

    # Method to intercept resolution of assemblies to add MSBuild's path
    $onAssemblyResolveEventHandler = [System.ResolveEventHandler] {
        param($s, $e)

        # Figure out which assembly to look for
        $assemblyName = $e.Name.Substring(0, $e.Name.IndexOf(", "));
        $assemblyPath = "$MSBuildPath\$assemblyName.dll"

        # Search for the assembly in the MSBuild directory
        if (Test-Path $assemblyPath) {
            # Found the assembly!
            return [System.Reflection.Assembly]::LoadFrom("$MSBuildPath\$assemblyName.dll");
        }

        # Unable to find the assembly
        return $null
    }

    # Wire-up assembly resolution event handler
    [System.AppDomain]::CurrentDomain.add_AssemblyResolve($onAssemblyResolveEventHandler)

    # Load Microsoft.Build.dll into script
    Add-Type -Path "$MSBuildPath\Microsoft.Build.dll" | Out-Null

    # Build a local project collection
    $projectCollection = [Microsoft.Build.Evaluation.ProjectCollection]::new()

    try {
        # Build a temporary "metaproj" of the solution file so it can be processed
        ${env:MSBUILDEMITSOLUTION} = 1
        & $MSBuildPath\MSBuild.exe $SolutionPath | Out-Null

        # Process solution
        $solution = [Microsoft.Build.Evaluation.Project]::new("$SolutionPath.metaproj", $null, "Current", $projectCollection)
    }
    finally {
        # Clean up "metaproj" files
        ${env:MSBUILDEMITSOLUTION} = 0
        Remove-Item -Path @("$SolutionPath.metaproj", "$SolutionPath.metaproj.tmp") | Out-Null
        Get-ChildItem * -Include *.metaproj -Recurse | Remove-Item | Out-Null
    }

    # Evaluate all of the Solution* properties and save into a collection
    $globalProps = New-Object 'System.Collections.Generic.Dictionary[String,String]'
    $solution.Properties | ForEach-Object -Process {
        if ($_.Name.StartsWith("Solution")) {
            $globalProps.Add($_.Name, $_.EvaluatedValue)
        }
    }

    # Evaluate all extra build props and save into the collection
    $extraPropsTable = ConvertFrom-StringData -StringData $ExtraMSBuildProps.Replace(',', "`n")
    $extraPropsTable.Keys | ForEach-Object -Process {
        $globalProps[$_] = $extraPropsTable[$_]
    }

    # Process the project file (with the Solution* properties we calculated before)
    $project = [Microsoft.Build.Evaluation.Project]::new("$ProjectPath", $globalProps, "Current", $projectCollection)

    # Create object to hold evaluated property key value pairs
    $evaluatedProps = @{}

    # Look for the specified PropertyNames and evaluate them
    $project.Properties | ForEach-Object -Process {
        if (($PropertyNames.Length -eq 0) -or ($PropertyNames -contains $_.Name)) {
            $evaluatedProps[$_.Name] = $_.EvaluatedValue;
        }
    }

    # Output as JSON
    Write-Output (ConvertTo-Json $evaluatedProps)
}

# Main

if ($MSBuildPath -and (Test-Path $MSBuildPath)) {
    if (Test-Path $MSBuildPath -PathType Leaf) {
        # It's a file (probably msbuild.exe), just get the folder path
        $MSBuildPath = [System.IO.Path]::GetDirectoryName($MSBuildPath)
    }
}
else {
    # Use simple fallback logic in this script to find MSBuild path
    $MSBuildPath = Get-MSBuildPath
}

# Get the full absolute paths to the solution and projects
$SolutionPath = Convert-Path $SolutionFile
$ProjectPath = Convert-Path $ProjectFile

Get-MSBuildProperties -MSBuildPath $MSBuildPath -SolutionPath $SolutionPath -ProjectPath $ProjectPath -PropertyNames $PropertyNames.Split(',', [System.StringSplitOptions]::RemoveEmptyEntries) -ExtraMSBuildProps $ExtraMSBuildProps