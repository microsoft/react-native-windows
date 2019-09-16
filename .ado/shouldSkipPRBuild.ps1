# Determine if the current PR payload requires a build.
# We skip the build if the only files changed are .md files.

function AllChangedFilesAreSkippable
{
    Param($files)

    $skipExts = @(".md")
    $allFilesAreSkippable = $true

    foreach($file in $files)
    {
        Write-Host "Checking '$file'"
        $ext = [System.IO.Path]::GetExtension($file)
        $fileIsSkippable = $ext -in $skipExts
        Write-Host "File '$file' is skippable: '$fileIsSkippable'"

        if(!$fileIsSkippable)
        {
            $allFilesAreSkippable = $false
        }
    }

    return $allFilesAreSkippable
}

$shouldSkipBuild = $false

if($env:BUILD_REASON -eq "PullRequest")
{
    $targetBranch = "origin/$env:SYSTEM_PULLREQUEST_TARGETBRANCH"

    $gitCommandLine = "git diff $targetBranch --name-only"
    Write-Host "$gitCommandLine"

    $diffOutput = Invoke-Expression $gitCommandLine
    Write-Host $diffOutput

    $files = $diffOutput.Split([Environment]::NewLine)

    Write-Host "Files changed: $files"
    

    $shouldSkipBuild = AllChangedFilesAreSkippable($files)
}

Write-Host "shouldSkipBuild = $shouldSkipBuild"

Write-Host "##vso[task.setvariable variable=shouldSkipPRBuild;isOutput=true]$shouldSkipBuild"