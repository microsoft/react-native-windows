param(
  [Parameter(Mandatory = $true)]
  [ValidateSet('Strict', 'Midgard', 'MidgardNpx')]
  [string] $Installer,

  [string] $WorkingDirectory = '.',

  [string] $Scope
)

$resolvedWorkingDirectory = (Resolve-Path $WorkingDirectory).Path
$lockFile = Join-Path $resolvedWorkingDirectory 'yarn.lock'
$originalLockFile = $null

if ($env:BUILD_REASON -eq 'PullRequest') {
  if (-not (Test-Path -LiteralPath $lockFile -PathType Leaf)) {
    throw "Could not find yarn.lock in '$resolvedWorkingDirectory'."
  }

  $registry = $env:NPM_CONFIG_REGISTRY
  if ([string]::IsNullOrWhiteSpace($registry)) {
    throw 'NPM_CONFIG_REGISTRY must be set for the network-isolated PR install.'
  }

  $registry = $registry.TrimEnd('/') + '/'
  $originalLockFile = [System.IO.File]::ReadAllBytes($lockFile)
  $lockFileText = [System.Text.Encoding]::UTF8.GetString($originalLockFile)
  $remappedLockFileText = $lockFileText.
    Replace('https://registry.yarnpkg.com/', $registry).
    Replace('https://registry.npmjs.org/', $registry)

  Write-Host "Temporarily routing yarn.lock package URLs through $registry"
  [System.IO.File]::WriteAllText(
    $lockFile,
    $remappedLockFileText,
    [System.Text.UTF8Encoding]::new($false)
  )
}

try {
  switch ($Installer) {
    'Strict' {
      $arguments = @('--yes', 'midgard-yarn-strict@1.2.4')
      if (-not [string]::IsNullOrWhiteSpace($Scope)) {
        $arguments += $Scope
      }
      & npx @arguments
    }
    'Midgard' {
      & midgard-yarn --ignore-scripts --frozen-lockfile --cwd $resolvedWorkingDirectory
    }
    'MidgardNpx' {
      & npx --yes midgard-yarn@1.23.34 --ignore-scripts --frozen-lockfile --cwd $resolvedWorkingDirectory
    }
  }

  if ($LASTEXITCODE -ne 0) {
    throw "$Installer dependency installation exited with code $LASTEXITCODE."
  }
}
finally {
  if ($null -ne $originalLockFile) {
    [System.IO.File]::WriteAllBytes($lockFile, $originalLockFile)
    Write-Host 'Restored the original yarn.lock.'
  }
}
