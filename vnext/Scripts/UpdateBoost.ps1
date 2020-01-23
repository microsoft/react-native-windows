param(
    [Parameter(Mandatory=$true)]
    [version] $Version,

    [System.IO.DirectoryInfo] $SubmodulePath = "$($PSScriptRoot | Split-Path)\Modules\boostorg"
)

Push-Location

Get-ChildItem $SubmodulePath | ForEach-Object {
    Set-Location $_.FullName
    git fetch origin --tags
    git checkout "boost-$Version"
}

Pop-Location