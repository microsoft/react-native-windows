param(
)

[string] $RepoRoot = Resolve-Path "$PSScriptRoot\..\.."

$StartingLocation = Get-Location
Set-Location -Path $RepoRoot

try
{
    $packagesSolutions = Get-ChildItem -File -Recurse -Path packages *.sln | Where-Object { !$_.FullName.Contains('node_modules') -and !$_.FullName.Contains('e2etest') }
    $vnextSolutions = Get-ChildItem -File -Path vnext *.sln

    # Run all solutions with their defaults
    $($packagesSolutions; $vnextSolutions) | Foreach {
        Write-Host Restoring $_.FullName with defaults
        & msbuild /t:Restore /p:RestoreForceEvaluate=true $_.FullName
    }

    # Re-run solutions that we also build a Fabric variant
    @("$RepoRoot\vnext\ReactWindows-Desktop.sln", "$RepoRoot\vnext\Microsoft.ReactNative.CppOnly.slnf") | Foreach {
        Write-Host Restoring $_ with Fabric
        & msbuild /t:Restore /p:RestoreForceEvaluate=true /p:RnwNewArch=true $_
    }
}
finally
{
    Set-Location -Path "$StartingLocation"
}