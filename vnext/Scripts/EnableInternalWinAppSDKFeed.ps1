param(
)

[string] $RepoRoot = Resolve-Path "$PSScriptRoot\..\.."

$StartingLocation = Get-Location
Set-Location -Path $RepoRoot

try
{
    [xml] $nugetConfig = Get-Content "NuGet.Config"
    $feedKey = "Project.Reunion.nuget.internal"
    $newFeed = $nugetConfig.CreateElement("add")
    $newFeed.SetAttribute("key", $feedKey)
    $newFeed.SetAttribute("value", "https://microsoft.pkgs.visualstudio.com/ProjectReunion/_packaging/Project.Reunion.nuget.internal/nuget/v3/index.json")
    [void] $nugetConfig.configuration.packageSources.AppendChild($newFeed)
    $mapping = $nugetConfig.CreateElement("packageSource")
    $mapping.SetAttribute("key", $feedKey)
    $pattern = $nugetConfig.CreateElement("package")
    $pattern.SetAttribute("pattern", "*")
    [void] $mapping.AppendChild($pattern)
    [void] $nugetConfig.configuration.packageSourceMapping.AppendChild($mapping)
    $nugetConfig.Save("$RepoRoot\NuGet.Config")
}
finally
{
    Set-Location -Path "$StartingLocation"
}