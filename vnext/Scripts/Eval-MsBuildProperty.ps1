# Evaluate an MSBuild property name
param(
    [Parameter()]
    [String]$MSBuildPath,
    [Parameter(Mandatory=$true)]
    [String]$SolutionFile,
    [Parameter(Mandatory=$true)]
    [String]$ProjectFile,
    [Parameter(Mandatory=$true)]
    [String]$PropertyName
)

function Get-MSBuildPath {
    $vsWhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
    if (!(Test-Path $vsWhere)) {
        throw "Unable to find vswhere.exe."
    }
    $vsPath = & $vsWhere -version 16.5 -property installationPath;
    return "$vsPath\MSBuild\Current\Bin";
}

function Get-MSBuildProperty {
    param (
        [Parameter(Mandatory=$true)]
        [String]$MSBuildPath,
        [Parameter(Mandatory=$true)]
        [String]$SolutionPath,
        [Parameter(Mandatory=$true)]
        [String]$ProjectPath,
        [Parameter(Mandatory=$true)]
        [String]$PropertyName
    )

    if (!(Test-Path (Join-Path $MSBuildPath "MSBuild.exe"))) {
        throw "Unable to find MSBuild.exe in $MSBuildPath"
    }

    if (!(Test-Path (Join-Path $MSBuildPath "MSBuild.exe"))) {
        throw "Unable to find Microsoft.Build.dll in $MSBuildPath"
    }

    # Load Microsoft.Build.dll into script
    Add-Type -Path "$MSBuildPath\Microsoft.Build.dll" | Out-Null

    # Build a local project collection
    $projectCollection =[Microsoft.Build.Evaluation.ProjectCollection]::new()

    try {
        # Build a temporary "metaproj" of the solution file so it can be processed
        ${env:MSBUILDEMITSOLUTION} = 1
        & $MSBuildPath\MSBuild.exe $SolutionPath | Out-Null

        # Process solution
        $solution = [Microsoft.Build.Evaluation.Project]::new("$SolutionPath.metaproj", $null, "Current", $projectCollection)
    } finally {
        # Clean up "metaproj" files
        Remove-Item "$SolutionPath.metaproj" | Out-Null
        Remove-Item "$SolutionPath.metaproj.tmp" | Out-Null
    }

    # Evaluate all of the Solution* properties and save into a collection
    $globalProps = New-Object 'System.Collections.Generic.Dictionary[String,String]'
    $solution.Properties | ForEach-Object -Process {
        if ($_.Name.StartsWith("Solution")) {
            $globalProps.Add($_.Name, $_.EvaluatedValue)
        }
    }

    # Process the project file (with the Solution* properties we calculated before)
    $project = [Microsoft.Build.Evaluation.Project]::new("$ProjectPath", $globalProps, "Current", $projectCollection)

    # Look for the specified PropertyName and evaluate it
    $project.Properties | ForEach-Object -Process {
        if ($_.Name -eq $PropertyName) {
            Write-Host $_.EvaluatedValue
            break
        }
    }
}

# Main

if (Test-Path $MSBuildPath) {
    if (Test-Path $MSBuildPath -PathType Leaf) {
        # It's a file (probably msbuild.exe), just get the folder path
        $MSBuildPath = [System.IO.Path]::GetDirectoryName($MSBuildPath)
    }
} else {
    # Use simple fallback logic in this script to find MSBuild path
    $MSBuildPath = Get-MSBuildPath
}

# Get the full absolute paths to the solution and projects
$SolutionPath = [System.IO.Path]::GetFullPath((Join-Path $pwd $SolutionFile))
$ProjectPath = [System.IO.Path]::GetFullPath((Join-Path $pwd $ProjectFile))

Get-MSBuildProperty -MSBuildPath $MSBuildPath -SolutionPath $SolutionPath -ProjectPath $ProjectPath -PropertyName $PropertyName
