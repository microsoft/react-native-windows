param(
    [switch] $SkipLockDeletion
)

[string] $RepoRoot = Resolve-Path "$PSScriptRoot\..\.."

$StartingLocation = Get-Location
Set-Location -Path $RepoRoot

try {
    if (-not $SkipLockDeletion) {
        # Delete existing lock files
        $existingLockFiles = (Get-ChildItem -File -Recurse -Path $RepoRoot -Filter *.lock.json)
        $existingLockFiles | Foreach-Object {
            Write-Host Deleting $_.FullName
            Remove-Item $_.FullName
        }
    }

    $packagesSolutions = (Get-ChildItem -File -Recurse -Path $RepoRoot\packages -Filter *.sln ) | Where-Object { !$_.FullName.Contains('node_modules') -and !$_.FullName.Contains('e2etest') }
    $vnextSolutions = (Get-ChildItem -File -Path $RepoRoot\vnext -Filter *.sln)

    # Run all solutions for each platform to ensure all projects are restored
    # (some projects are only configured for specific platforms)
    $platforms = @("x64","x86","ARM64")
    $($packagesSolutions; $vnextSolutions) | Foreach-Object {
        foreach ($platform in $platforms) {
            Write-Host Restoring $_.FullName with Platform=$platform
            & msbuild /t:Restore /p:RestoreForceEvaluate=true /p:Platform=$platform $_.FullName
        }
    }

    # Re-run solutions that build with UseExperimentalWinUI3
    $experimentalSolutions = @("playground-composition.sln", "Microsoft.ReactNative.NewArch.sln", "ReactWindows-Desktop.sln");
    $($packagesSolutions; $vnextSolutions) | Where-Object { $experimentalSolutions -contains $_.Name } | Foreach-Object {
        foreach ($platform in $platforms) {
            Write-Host Restoring $_.FullName with UseExperimentalWinUI3=true Platform=$platform
            & msbuild /t:Restore /p:RestoreForceEvaluate=true /p:UseExperimentalWinUI3=true /p:Platform=$platform $_.FullName
        }
    }
}
finally {
    Set-Location -Path "$StartingLocation"
}