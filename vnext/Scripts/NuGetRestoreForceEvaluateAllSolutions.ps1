param(
    [switch] $SkipLockDeletion
)

[string] $RepoRoot = Resolve-Path "$PSScriptRoot\..\.."

$StartingLocation = Get-Location
Set-Location -Path $RepoRoot

$failedRestores = [System.Collections.Generic.List[string]]::new()

function Restore-Solution([string] $solution, [string[]] $extraArgs) {
    Write-Host "Restoring $solution $($extraArgs -join ' ')"
    & msbuild /t:Restore /p:RestoreForceEvaluate=true @extraArgs $solution
    if ($LASTEXITCODE -ne 0) {
        $failedRestores.Add(("$solution $($extraArgs -join ' ')").Trim())
    }
}

try {
    # Some node_modules packages ship a NuGet.Config that re-adds the public nuget.org feed. That
    # breaks the repo's single-ADO-feed compliance and trips corporate network blocks on api.nuget.org.
    # Strip any nuget.org source so restore only uses the repo's ADO feed.
    Get-ChildItem -File -Recurse -Path $RepoRoot -Filter NuGet.Config -ErrorAction SilentlyContinue |
        Where-Object { $_.FullName -match '\\node_modules\\' } | ForEach-Object {
            $configPath = $_.FullName
            try { [xml]$doc = Get-Content -LiteralPath $configPath -Raw } catch { return }
            $sources = $doc.SelectSingleNode('/configuration/packageSources')
            if (-not $sources) { return }
            $badSources = @($sources.SelectNodes('add') | Where-Object { $_.GetAttribute('value') -match 'nuget\.org' })
            if ($badSources.Count -eq 0) { return }
            $badSources | ForEach-Object { [void]$sources.RemoveChild($_) }
            $doc.Save($configPath)
            Write-Host "Removed nuget.org feed from $configPath"
        }

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

    # Run all solutions with their defaults
    $($packagesSolutions; $vnextSolutions) | Foreach-Object {
        Restore-Solution $_.FullName @()
    }

    # Re-run solutions that build with UseExperimentalWinUI3
    $experimentalSolutions = @("playground-composition.sln", "Microsoft.ReactNative.sln", "Microsoft.ReactNative.NewArch.sln", "ReactWindows-Desktop.sln");
    $($packagesSolutions; $vnextSolutions) | Where-Object { $experimentalSolutions -contains $_.Name } | Foreach-Object {
        Restore-Solution $_.FullName @('/p:UseExperimentalWinUI3=true')
    }

    # Re-run solutions that build with Chakra (UseHermes=false)
    $chakraSolutions = @("ReactUWPTestApp.sln", "integrationtest.sln");
    $($packagesSolutions; $vnextSolutions) | Where-Object { $chakraSolutions -contains $_.Name } | Foreach-Object {
        Restore-Solution $_.FullName @('/p:UseHermes=false')
    }
}
finally {
    Set-Location -Path "$StartingLocation"
}

# Restore failures used to be ignored (msbuild's exit code was never checked), so a broken run
# looked successful while leaving lock files deleted. Fail loudly on both failure modes.
if ($failedRestores.Count -gt 0) {
    Write-Host ''
    Write-Host "ERROR: NuGet restore returned a non-zero exit code for $($failedRestores.Count) solution pass(es):" -ForegroundColor Red
    $failedRestores | ForEach-Object { Write-Host "    $_" -ForegroundColor Red }
}

# A committed lock that was deleted but never came back means a project failed or was skipped during restore.
$missingTrackedLocks = @(git -C $RepoRoot ls-files -- '*.lock.json' | Where-Object { -not (Test-Path (Join-Path $RepoRoot $_)) })
if ($missingTrackedLocks.Count -gt 0) {
    Write-Host ''
    Write-Host "ERROR: $($missingTrackedLocks.Count) committed lock file(s) were deleted but not regenerated:" -ForegroundColor Red
    $missingTrackedLocks | ForEach-Object { Write-Host "    $_" -ForegroundColor Red }
}

if ($failedRestores.Count -gt 0 -or $missingTrackedLocks.Count -gt 0) {
    Write-Host ''
    Write-Host 'Lock file regeneration is INCOMPLETE - do not commit. Fix the errors above and re-run.' -ForegroundColor Red
    exit 1
}

Write-Host ''
Write-Host 'All solutions restored and every committed lock file was regenerated.' -ForegroundColor Green