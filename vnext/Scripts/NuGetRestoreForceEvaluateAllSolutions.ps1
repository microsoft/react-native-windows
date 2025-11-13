param(
)

[string] $RepoRoot = Resolve-Path "$PSScriptRoot\..\.."

$StartingLocation = Get-Location
Set-Location -Path $RepoRoot

try
{
    # Delete existing lock files
    $existingLockFiles = (Get-ChildItem -File -Recurse -Path $RepoRoot -Filter *.lock.json)
    $existingLockFiles | Foreach-Object {
        Write-Host Deleting $_.FullName
        Remove-Item $_.FullName
    }

    $packagesSolutions = (Get-ChildItem -File -Recurse -Path $RepoRoot\packages -Filter *.sln )| Where-Object { !$_.FullName.Contains('node_modules') -and !$_.FullName.Contains('e2etest') }
    $vnextSolutions = (Get-ChildItem -File -Path $RepoRoot\vnext -Filter *.sln)

    # Run all solutions with their defaults
    $($packagesSolutions; $vnextSolutions) | Foreach-Object {
        Write-Host Restoring $_.FullName with defaults
        & msbuild /t:Restore /p:RestoreForceEvaluate=true $_.FullName
    }

    # Re-run solutions that build with UseExperimentalWinUI3
    $experimentalSolutions = @("playground-composition.sln", "Microsoft.ReactNative.sln", "Microsoft.ReactNative.NewArch.sln", "ReactWindows-Desktop.sln");
    $($packagesSolutions; $vnextSolutions) | Where-Object { $experimentalSolutions -contains $_.Name } | Foreach-Object {
        Write-Host Restoring $_.FullName with UseExperimentalWinUI3=true
        & msbuild /t:Restore /p:RestoreForceEvaluate=true /p:UseExperimentalWinUI3=true $_.FullName
    }

    # Re-run solutions that build with Chakra
    $chakraSolutions = @("ReactUWPTestApp.sln", "integrationtest.sln");
    $($packagesSolutions; $vnextSolutions) | Where-Object { $chakraSolutions -contains $_.Name } | Foreach-Object {
        Write-Host Restoring $_.FullName with UseHermes=false
        & msbuild /t:Restore /p:RestoreForceEvaluate=true /p:UseHermes=false $_.FullName
    }
}
finally
{
    Set-Location -Path "$StartingLocation"
}