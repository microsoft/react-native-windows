param(
)

[string] $RepoRoot = Resolve-Path "$PSScriptRoot\..\.."

$StartingLocation = Get-Location
Set-Location -Path $RepoRoot

try
{
    [xml] $nugetConfig = Get-Content "NuGet.Config"
    $newFeed = $nugetConfig.configuration.packageSources.add.clone()
    $newFeed.key = "Project.Reunion.nuget.internal";
    $newFeed.value = "https://microsoft.pkgs.visualstudio.com/ProjectReunion/_packaging/Project.Reunion.nuget.internal/nuget/v3/index.json";
    $nugetConfig.configuration.packageSources.AppendChild($newFeed)
    $nugetConfig.Save("NuGet.Config")
}
finally
{
    Set-Location -Path "$StartingLocation"
}