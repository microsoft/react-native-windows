#!/usr/bin/env pwsh
<#
.SYNOPSIS
  Warm the ms/react-native-public Azure Artifacts feed with the npm and NuGet
  packages the React Native Windows CI and CLI-init tests need.

.DESCRIPTION
  Public PR builds read the feed anonymously, and an Azure Artifacts upstream feed
  only serves a version once it has been *saved* (pulled through by an authenticated
  request). Any not-yet-saved transitive dependency therefore 404s on a PR build,
  e.g. the CLI lib job failing on 'is-unc-path'.

  The script warms two ways, both with your credentials:
  - npm: it reproduces the base project generations the CLI-init tests run (a
    create-react-native-library lib and a community-CLI app) and installs them, which
    pulls the whole toolchain closure into the feed.
  - NuGet: it downloads every package in the repo's packages.lock.json files (the
    full resolved closure, incl. transitives) via the feed's flat2 endpoint.

  Saving is idempotent, so re-warming is a no-op.

  Auth (in order): -Pat / $env:ADO_PAT / $env:AZURE_DEVOPS_EXT_PAT, else an AAD token
  from `az account get-access-token` (requires `az login` locally, or an AzureCLI@2
  task with a managed identity in a pipeline).

  Run it manually from a clone, or on a schedule from the ADO warm-up pipeline. It
  does not touch your local checkout: all work happens in a throwaway work dir.

.PARAMETER NpmRegistry
  The feed npm registry to warm. Defaults to ms/react-native-public.

.PARAMETER NuGetIndex
  The feed NuGet v3 index.json to warm. Defaults to ms/react-native-public.

.PARAMETER Pat
  Optional ADO PAT (Packaging: Read on the feed's org). Falls back to env vars, then az.

.PARAMETER WorkDir
  Directory for the throwaway generated projects. Defaults to a new temp folder.

.PARAMETER ReactNativeVersion
  react-native version to generate against. Defaults to vnext/package.json.

.PARAMETER CliVersion
  @react-native-community/cli version. Defaults to vnext/package.json.

.PARAMETER SkipLib
  Skip the create-react-native-library warm pass.

.PARAMETER SkipApp
  Skip the community-CLI app warm pass.

.PARAMETER SkipNuGet
  Skip the NuGet warm pass.

.PARAMETER SkipRnwPackages
  Skip warming the repo's own already-published workspace packages into the feed.

.PARAMETER KeepWorkDir
  Keep the work dir instead of deleting it (for debugging).

.EXAMPLE
  ./Warm-RnwFeedCache.ps1

.EXAMPLE
  # Warm against explicit versions and keep the generated projects to inspect:
  ./Warm-RnwFeedCache.ps1 -ReactNativeVersion 0.85.0-nightly-20260303-c26dbe286 -KeepWorkDir
#>
[CmdletBinding()]
param(
  [string]$NpmRegistry = 'https://pkgs.dev.azure.com/ms/react-native/_packaging/react-native-public/npm/registry/',
  [string]$Pat,
  [string]$WorkDir,
  [string]$ReactNativeVersion,
  [string]$CliVersion,
  # CODESYNC: keep in step with vnext/Scripts/creaternwlib.cmd and creaternwapp.cmd,
  # so the warm run reproduces the same generator/template versions the CLI-init tests use.
  [string]$CreateLibraryVersion = '0.48.9',
  [string]$TemplateVersion = '@react-native-community/template@0.84.1',
  [string]$NuGetIndex = 'https://pkgs.dev.azure.com/ms/react-native/_packaging/react-native-public/nuget/v3/index.json',
  [switch]$SkipLib,
  [switch]$SkipApp,
  [switch]$SkipNuGet,
  [switch]$SkipRnwPackages,
  [switch]$KeepWorkDir
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

# Azure DevOps resource id for AAD access tokens.
$AdoResourceId = '499b84ac-1321-427f-aa17-267ca6975798'
$RepoRoot = (Resolve-Path "$PSScriptRoot\..\..").Path
$UpdateNightly = Join-Path $RepoRoot 'vnext\Scripts\UpdateNightlyDependencies.ps1'

function Get-FeedToken {
  param([string]$Pat)
  $token = $Pat
  if (-not $token) { $token = $env:ADO_PAT }
  if (-not $token) { $token = $env:AZURE_DEVOPS_EXT_PAT }
  if ($token) { return [pscustomobject]@{ Token = $token; Kind = 'pat' } }
  $az = Get-Command az -CommandType Application -ErrorAction SilentlyContinue
  if ($az) {
    $aad = & az account get-access-token --resource $AdoResourceId --query accessToken -o tsv 2>$null
    if ($LASTEXITCODE -eq 0 -and $aad) { return [pscustomobject]@{ Token = $aad.Trim(); Kind = 'aad' } }
  }
  throw "No Azure DevOps auth available. Run 'az login', or pass -Pat / set `$env:ADO_PAT."
}

function Get-RepoVersion {
  param([object]$Pkg, [string]$Section, [string]$Name, [string]$Override)
  if ($Override) { return $Override }
  $deps = $Pkg.$Section
  if ($deps) {
    $prop = $deps.PSObject.Properties | Where-Object { $_.Name -eq $Name } | Select-Object -First 1
    if ($prop) { return [string]$prop.Value }
  }
  throw "Could not find '$Name' in vnext/package.json '$Section'. Pass it explicitly."
}

function Invoke-Checked {
  param([scriptblock]$Script, [string]$What)
  & $Script
  if ($LASTEXITCODE -ne 0) { throw "$What failed (exit $LASTEXITCODE)." }
}

# --- auth + versions ----------------------------------------------------------

$token = Get-FeedToken -Pat $Pat
$pkg = Get-Content (Join-Path $RepoRoot 'vnext\package.json') -Raw | ConvertFrom-Json
$rnVersion = Get-RepoVersion -Pkg $pkg -Section 'devDependencies' -Name 'react-native' -Override $ReactNativeVersion
$cliVersion = Get-RepoVersion -Pkg $pkg -Section 'dependencies' -Name '@react-native-community/cli' -Override $CliVersion
$isNightly = $rnVersion -match 'nightly'

if (-not $WorkDir) { $WorkDir = Join-Path ([IO.Path]::GetTempPath()) "rnw-warm-$(Get-Random)" }
# Refuse a pre-existing directory: cleanup deletes $WorkDir recursively, so it must be one we created.
if (Test-Path -LiteralPath $WorkDir) {
  throw "WorkDir '$WorkDir' already exists. Pass a path that does not exist so cleanup only deletes what this script creates."
}
New-Item -ItemType Directory -Path $WorkDir | Out-Null

Write-Host "Feed:          $NpmRegistry" -ForegroundColor Cyan
Write-Host "react-native:  $rnVersion" -ForegroundColor Cyan
Write-Host "cli:           $cliVersion" -ForegroundColor Cyan
Write-Host "Work dir:      $WorkDir" -ForegroundColor Cyan

# Point npm/npx and Yarn (classic + berry) at the feed with our token, via a
# work-dir-local npm config so the caller's ~/.npmrc is untouched. $npmrc and
# $savedEnv are declared before the try so the finally can always undo them.
$npmrc = Join-Path $WorkDir '.npmrc'
$savedEnv = @{}
try {
$registryKey = ($NpmRegistry -replace '^https?:', '')
Set-Content -LiteralPath $npmrc -Encoding ascii -Value @(
  "registry=$NpmRegistry"
  "${registryKey}:_authToken=$($token.Token)"
  'always-auth=true'
)

foreach ($kv in @{
    NPM_CONFIG_USERCONFIG          = $npmrc
    NPM_CONFIG_REGISTRY            = $NpmRegistry
    YARN_NPM_REGISTRY_SERVER       = $NpmRegistry
    YARN_NPM_ALWAYS_AUTH           = 'true'
    YARN_NPM_AUTH_TOKEN            = $token.Token
    YARN_ENABLE_IMMUTABLE_INSTALLS = 'false'
  }.GetEnumerator()) {
  $savedEnv[$kv.Key] = [Environment]::GetEnvironmentVariable($kv.Key)
  Set-Item "env:$($kv.Key)" $kv.Value
}

function Update-NightlyPackageJson {
  param([string]$PackageJsonPath)
  # UpdateNightlyDependencies.ps1 throws on error (ErrorActionPreference=Stop); no exit code to check.
  if (-not $isNightly) { return }
  & $UpdateNightly -PackageJsonPath $PackageJsonPath -ReactNativeVersion $rnVersion -ReactNativeCliVersion $cliVersion
}

# --- warm passes --------------------------------------------------------------

function Warm-Lib {
  Push-Location $WorkDir
  try {
    Invoke-Checked -What 'create-react-native-library' -Script {
      & npx --yes "create-react-native-library@$CreateLibraryVersion" `
        --slug warmlib --description warmlib `
        --author-name 'React-Native-Windows Bot' `
        --author-email '53619745+rnbot@users.noreply.github.com' `
        --author-url 'http://example.com' --repo-url 'http://example.com' `
        --languages kotlin-objc --local false --type turbo-module `
        --react-native-version $rnVersion --example vanilla warmlib
    }
    Push-Location (Join-Path $WorkDir 'warmlib')
    try {
      Update-NightlyPackageJson -PackageJsonPath 'package.json'
      if (Test-Path 'example\package.json') { Update-NightlyPackageJson -PackageJsonPath 'example\package.json' }
      Invoke-Checked -What 'yarn install (lib)' -Script { & yarn install }
    }
    finally { Pop-Location }
  }
  finally { Pop-Location }
}

function Warm-App {
  Push-Location $WorkDir
  try {
    Invoke-Checked -What 'react-native init (app)' -Script {
      $templateArgs = if ($isNightly) { @('--template', $TemplateVersion) } else { @() }
      & npx --yes "@react-native-community/cli@$cliVersion" init warmapp `
        --version $rnVersion @templateArgs --skip-install --install-pods false --skip-git-init true
    }
    Push-Location (Join-Path $WorkDir 'warmapp')
    try {
      Update-NightlyPackageJson -PackageJsonPath 'package.json'
      Invoke-Checked -What 'yarn install (app)' -Script { & yarn install }
    }
    finally { Pop-Location }
  }
  finally { Pop-Location }
}

# NuGet warm: save the resolved closure (packages.lock.json, incl. transitives)
# from upstream into the feed via authenticated flat2 downloads. No VS/msbuild needed.
function Get-NuGetAuthHeader {
  param([object]$Token)
  if ($Token.Kind -eq 'pat') {
    $basic = [Convert]::ToBase64String([Text.Encoding]::ASCII.GetBytes(":$($Token.Token)"))
    return @{ Authorization = "Basic $basic" }
  }
  return @{ Authorization = "Bearer $($Token.Token)" }
}

function Get-Flat2Base {
  param([string]$IndexUrl, [hashtable]$Headers)
  $index = Invoke-RestMethod -Uri $IndexUrl -Headers $Headers
  $res = $index.resources | Where-Object { $_.'@type' -match '^PackageBaseAddress' } | Select-Object -First 1
  if (-not $res) { throw "NuGet index has no PackageBaseAddress (flat2) resource: $IndexUrl" }
  return ($res.'@id').TrimEnd('/')
}

function Get-NuGetRefsFromLockFiles {
  $refs = @{}
  $lockFiles = Get-ChildItem -Recurse -File -Filter packages.lock.json -Path $RepoRoot -ErrorAction SilentlyContinue |
    Where-Object { $_.FullName -notmatch '[\\/]node_modules[\\/]' }
  foreach ($lf in $lockFiles) {
    $json = Get-Content -LiteralPath $lf.FullName -Raw | ConvertFrom-Json
    if (-not $json.PSObject.Properties['dependencies']) { continue }
    foreach ($framework in $json.dependencies.PSObject.Properties) {
      foreach ($dep in $framework.Value.PSObject.Properties) {
        $info = $dep.Value
        if ($info.type -eq 'Project') { continue }
        if (-not $info.PSObject.Properties['resolved']) { continue }
        $refs["$($dep.Name)|$($info.resolved)"] = [pscustomobject]@{ Id = $dep.Name; Version = [string]$info.resolved }
      }
    }
  }
  return @($refs.Values | Sort-Object Id, Version)
}

function Save-UpstreamNupkg {
  param([string]$Id, [string]$Version, [string]$Flat2Base, [hashtable]$Headers, [int]$MaxAttempts = 4)
  $idLower = $Id.ToLowerInvariant(); $verLower = $Version.ToLowerInvariant()
  $url = "$Flat2Base/$idLower/$verLower/$idLower.$verLower.nupkg"
  $tmp = Join-Path $WorkDir "nuget-$idLower.$verLower.nupkg"
  $lastError = 'unknown error'
  for ($attempt = 1; $attempt -le $MaxAttempts; $attempt++) {
    $retryable = $true
    try {
      # -PassThru returns the response even with -OutFile, so a 202 is visible here
      # (a 2xx never throws, so the old catch reported a still-pending 202 as saved).
      $resp = Invoke-WebRequest -Uri $url -Headers $Headers -OutFile $tmp -PassThru -ErrorAction Stop
      Remove-Item $tmp -Force -ErrorAction SilentlyContinue
      # 202 = Azure Artifacts is still saving the upstream package; not available yet, so retry.
      if ([int]$resp.StatusCode -ne 202) { return }
      $lastError = 'HTTP 202 (upstream save still pending)'
    }
    catch {
      $code = $null; try { $code = [int]$_.Exception.Response.StatusCode } catch { }
      $lastError = if ($code) { "HTTP $code" } else { $_.Exception.Message }
      $retryable = ($null -eq $code -or $code -in 404, 408, 429, 500, 502, 503, 504)
    }
    if (-not $retryable) { break }
    if ($attempt -lt $MaxAttempts) { Start-Sleep -Seconds ([Math]::Min(2 * $attempt, 10)) }
  }
  throw "$Id $Version ($lastError)"
}

function Warm-NuGet {
  $refs = Get-NuGetRefsFromLockFiles
  if ($refs.Count -eq 0) { Write-Host 'No packages.lock.json entries found.' -ForegroundColor Yellow; return }
  $headers = Get-NuGetAuthHeader -Token $token
  $flat2 = Get-Flat2Base -IndexUrl $NuGetIndex -Headers $headers
  Write-Host "Saving $($refs.Count) NuGet package(s) via $flat2" -ForegroundColor Cyan
  $failures = @()
  foreach ($r in $refs) {
    try { Save-UpstreamNupkg -Id $r.Id -Version $r.Version -Flat2Base $flat2 -Headers $headers }
    catch { $failures += $_.Exception.Message }
  }
  if ($failures.Count) { throw "$($failures.Count)/$($refs.Count) NuGet package(s) failed: $($failures -join '; ')" }
  Write-Host "Saved $($refs.Count) NuGet package(s)." -ForegroundColor Green
}

# The CLI-init tests install react-native-windows, whose closure pulls the repo's own already-published
# workspace packages (e.g. @react-native-windows/package-utils). Verdaccio publishes only the *changed*
# packages locally and proxies the rest to the feed, so anonymous PR reads 404/500 unless those
# published versions are cached. CODESYNC: enumerates the same workspaces npmPack.js packs; the subset
# it strips with --check-npm (already on npmjs) is exactly what a PR build must read from the feed.
function Warm-RnwPackages {
  $dir = Join-Path $WorkDir 'rnwpkgs'
  New-Item -ItemType Directory -Path $dir | Out-Null

  $rootPkg = Get-Content (Join-Path $RepoRoot 'package.json') -Raw | ConvertFrom-Json
  $seen = [System.Collections.Generic.HashSet[string]]::new()
  $specs = [System.Collections.Generic.List[string]]::new()
  foreach ($pattern in $rootPkg.workspaces.packages) {
    $pkgDirs = @()
    if ($pattern.EndsWith('/*')) {
      $base = Join-Path $RepoRoot ($pattern.Substring(0, $pattern.Length - 2))
      if (Test-Path -LiteralPath $base) { $pkgDirs = @((Get-ChildItem -LiteralPath $base -Directory).FullName) }
    }
    else { $pkgDirs = @(Join-Path $RepoRoot $pattern) }
    foreach ($d in $pkgDirs) {
      $pj = Join-Path $d 'package.json'
      if (-not (Test-Path -LiteralPath $pj)) { continue }
      $p = Get-Content -LiteralPath $pj -Raw | ConvertFrom-Json
      $props = $p.PSObject.Properties
      if (($props['private'] -and $p.private -eq $true) -or -not $props['name'] -or -not $props['version']) { continue }
      $spec = "$($p.name)@$($p.version)"
      if ($seen.Add($spec)) { $specs.Add($spec) }
    }
  }

  Push-Location $dir
  try {
    # Skip versions not yet on the feed's upstream: the build's freshly bumped packages aren't published
    # and the CLI test gets those from verdaccio locally, so a miss here is expected (not a failure).
    $published = foreach ($spec in $specs) {
      try { & npm view $spec version *> $null; if ($LASTEXITCODE -eq 0) { $spec } } catch { }
    }
    $published = @($published)
    if ($published.Count -eq 0) {
      Write-Host 'No already-published workspace packages to warm.' -ForegroundColor Yellow
      return
    }
    Write-Host "Warming $($published.Count)/$($specs.Count) workspace package(s) into the feed." -ForegroundColor Cyan
    Invoke-Checked -What 'warm RNW packages' -Script {
      & npm install --ignore-scripts @published
    }
  }
  finally { Pop-Location }
}

$passes = [ordered]@{}
if (-not $SkipLib) { $passes['lib'] = ${function:Warm-Lib} }
if (-not $SkipApp) { $passes['app'] = ${function:Warm-App} }
if (-not $SkipNuGet) { $passes['nuget'] = ${function:Warm-NuGet} }
if (-not $SkipRnwPackages) { $passes['rnwpackages'] = ${function:Warm-RnwPackages} }

# Run each pass "bare" (no pipe/capture) so npm/yarn inherits the console: real TTY -> live progress, UTF-8 -> no mojibake.
$results = [System.Collections.Generic.List[object]]::new()
foreach ($name in $passes.Keys) {
  Write-Host "`n=== Warming '$name' ===" -ForegroundColor Green
  try { & $passes[$name]; $results.Add([pscustomobject]@{ Pass = $name; Status = 'OK' }) }
  catch { Write-Host "##[error]$($_.Exception.Message)" -ForegroundColor Red; $results.Add([pscustomobject]@{ Pass = $name; Status = "FAILED: $($_.Exception.Message)" }) }
}

# --- cleanup + summary --------------------------------------------------------
}
finally {
  # Runs even on Ctrl+C or an uncaught error, so the token file and env overrides never linger.
  foreach ($k in $savedEnv.Keys) {
    if ($null -eq $savedEnv[$k]) { Remove-Item "env:$k" -ErrorAction SilentlyContinue }
    else { Set-Item "env:$k" $savedEnv[$k] }
  }
  Remove-Item -LiteralPath $npmrc -Force -ErrorAction SilentlyContinue
  if (-not $KeepWorkDir) { Remove-Item -Recurse -Force $WorkDir -ErrorAction SilentlyContinue }
  else { Write-Host "`nKept work dir: $WorkDir" -ForegroundColor Yellow }
}

$results = @($results)
Write-Host ''
if ($results.Count -eq 0) { Write-Host 'Nothing warmed (all passes skipped).' -ForegroundColor Yellow; exit 0 }
$results | Format-Table -AutoSize Pass, Status | Out-Host

$failed = @($results | Where-Object { $_.Status -ne 'OK' })
if ($failed.Count) {
  Write-Host "$($failed.Count) of $($results.Count) warm pass(es) failed. A failed authenticated install usually means a package is missing upstream or the feed is unhealthy." -ForegroundColor Red
  exit 1
}
Write-Host "Feed warmed. Anonymous PR builds should now restore these closures." -ForegroundColor Green
