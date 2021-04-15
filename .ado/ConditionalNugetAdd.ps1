$sources = nuget sources list
$found? = $sources -Match "https://api.nuget.org/v3/index.json"
 If (!$found?){
   nuget sources add -Name nuget.org -Source https://api.nuget.org/v3/index.json -ConfigFile $env:APPDATA\NuGet\NuGet.Config
 }
