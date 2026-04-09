---
name: rnw-integration
description: 'Automate React Native Windows integration with upstream React Native nightly versions. Use when: upgrading RNW to a newer React Native nightly, finding target nightly versions, preparing integration PRs, updating package dependencies.'
argument-hint: 'Describe the integration task (e.g., "find next nightly version")'
---

# RNW Integration Automation

Automates the process of integrating React Native Windows with upstream React Native nightly builds.

## When to Use

- Finding the target React Native nightly version for integration
- Upgrading RNW dependencies to a new React Native nightly
- Preparing integration pull requests
- Validating nightly version availability on npm

## Procedure

### Step 1: Get Current React Native Version

Extract the current `react-native` version from `vnext/package.json`:

```powershell
$packageJson = Get-Content -Path "vnext/package.json" | ConvertFrom-Json
$currentVersion = $packageJson.devDependencies.'react-native'
Write-Host "Current version: $currentVersion"
```

Expected format: `0.XX.0-nightly-YYYYMMDD-<commit-hash>`

### Step 2: Parse the Current Version Date

Extract the date from the nightly version string:

```powershell
# Parse version like: 0.84.0-nightly-20260107-58bc6c3e3
$versionMatch = $currentVersion -match 'nightly-(\d{8})'
$currentDate = [DateTime]::ParseExact($matches[1], 'yyyyMMdd', $null)
Write-Host "Current nightly date: $($currentDate.ToString('yyyy-MM-dd'))"
```

### Step 3: Calculate Target Date (+7 days)

```powershell
$targetDate = $currentDate.AddDays(7)
$targetDateStr = $targetDate.ToString('yyyyMMdd')
Write-Host "Target date (+7 days): $($targetDate.ToString('yyyy-MM-dd'))"
```

### Step 4: Find Nightly Version on npm

Query npm for available nightly versions around the target date:

```powershell
# Get all nightly versions for the target month
$yearMonth = $targetDate.ToString('yyyyMM')
$nightlyVersions = npm view react-native versions --json 2>$null | 
    ConvertFrom-Json | 
    Where-Object { $_ -like "*nightly-$yearMonth*" }

# Find exact match or next available
$targetVersion = $nightlyVersions | Where-Object { $_ -like "*nightly-$targetDateStr*" } | Select-Object -First 1

if (-not $targetVersion) {
    Write-Host "No nightly for $targetDateStr, finding next available..."
    $targetVersion = $nightlyVersions | Where-Object { 
        $_ -match 'nightly-(\d{8})' -and [int]$matches[1] -gt [int]$targetDateStr 
    } | Select-Object -First 1
}

Write-Host "Target nightly version: $targetVersion"
```

### Step 5: Output Summary

```powershell
Write-Host ""
Write-Host "=== Integration Summary ==="
Write-Host "Current Version: $currentVersion"
Write-Host "Current Date:    $($currentDate.ToString('yyyy-MM-dd'))"
Write-Host "Target Date:     $($targetDate.ToString('yyyy-MM-dd'))"
Write-Host "Target Version:  $targetVersion"
```

### Step 6: Run Integration Script

Run the integration script with the target nightly version to update all dependencies:

```powershell
yarn integrate-rn $targetVersion
```

This command updates all `react-native` and `@react-native/*` package references across the monorepo to the target nightly version.

### Step 7: Search and Update Remaining package.json Files

After running the integration script, search all `package.json` files for any remaining references to the previous nightly version and update them:

```powershell
# Find all package.json files containing the previous nightly version
$packageFiles = Get-ChildItem -Path . -Filter "package.json" -Recurse -File | 
    Where-Object { $_.FullName -notlike "*node_modules*" }

$updatedFiles = @()

foreach ($file in $packageFiles) {
    $content = Get-Content -Path $file.FullName -Raw
    if ($content -like "*$currentVersion*") {
        Write-Host "Found previous version in: $($file.FullName)"
        $newContent = $content -replace [regex]::Escape($currentVersion), $targetVersion
        Set-Content -Path $file.FullName -Value $newContent -NoNewline
        $updatedFiles += $file.FullName
    }
}

Write-Host ""
Write-Host "=== Updated $($updatedFiles.Count) package.json files ==="
$updatedFiles | ForEach-Object { Write-Host "  - $_" }
```

This ensures all references to the previous nightly version are updated, including:
- `dependencies`
- `devDependencies`
- `peerDependencies`
- `resolutions`

### Step 8: Upgrade Platform Overrides

Run the override upgrade tool to resolve conflicts in platform-specific override files:

```powershell
npx react-native-platform-override upgrade
```

This interactive tool helps resolve conflicts between upstream React Native changes and RNW platform overrides. It will:
- Show files with merge conflicts
- Allow you to accept upstream changes, keep local changes, or manually merge
- Update the overrides.json manifest files

After completing the upgrade, verify the overrides are valid:

```powershell
yarn validate-overrides
```

### Step 9: Commit All Changes

After completing all integration steps, commit the changes:

```powershell
git add -A
git commit -m "Integrate RN $targetVersion"
```

This creates a single commit with all the integration changes including:
- Updated package.json dependencies
- Override file upgrades
- Any auto-fixed lint issues

### Step 10: Update Failing Overrides from Upstream

Run `yarn validate-overrides` to identify which folders need to be replaced with upstream versions, then download and replace them:

```powershell
# Extract commit hash from target version (last 8 characters after final hyphen)
# Example: 0.85.0-nightly-20260114-f15985f4f -> f15985f4f
$commitHash = ($targetVersion -split '-')[-1]
Write-Host "Commit hash: $commitHash"

# Run validate-overrides and capture failing paths
$validateOutput = yarn validate-overrides 2>&1
$failingPaths = $validateOutput | Where-Object { $_ -match '^ - ' } | ForEach-Object { $_.Trim(' -') }

if ($failingPaths.Count -eq 0) {
    Write-Host "No failing overrides to update"
    return
}

Write-Host "Found $($failingPaths.Count) failing override paths to update"

# Create temp directory (outside git tracking)
$tempDir = "$env:TEMP\rn-upstream-$commitHash"
if (Test-Path $tempDir) { Remove-Item -Recurse -Force $tempDir }
New-Item -ItemType Directory -Path $tempDir | Out-Null

# Download react-native repo at the specific commit
Write-Host "Downloading react-native at commit $commitHash..."
git clone --depth 1 --filter=blob:none --sparse https://github.com/facebook/react-native.git $tempDir
Push-Location $tempDir
git sparse-checkout set packages/rn-tester
git fetch --unshallow
git checkout $commitHash
Pop-Location

# Replace each failing path with upstream version
foreach ($path in $failingPaths) {
    # Convert absolute path to relative
    $relativePath = $path -replace [regex]::Escape((Get-Location).Path + '\'), ''
    
    # Map RNW paths to upstream rn-tester paths
    $upstreamPath = $null
    
    if ($relativePath -like "packages\@react-native\tester\*") {
        # packages\@react-native\tester\X -> packages\rn-tester\X
        $subPath = $relativePath -replace '^packages\\@react-native\\tester\\', ''
        $upstreamPath = "$tempDir\packages\rn-tester\$subPath"
    }
    elseif ($relativePath -like "vnext\ReactCopies\*") {
        # vnext\ReactCopies\X -> packages\rn-tester\X
        $subPath = $relativePath -replace '^vnext\\ReactCopies\\', ''
        $upstreamPath = "$tempDir\packages\rn-tester\$subPath"
    }
    
    if ($upstreamPath -and (Test-Path $upstreamPath)) {
        Write-Host "Replacing $relativePath with upstream version..."
        Remove-Item -Recurse -Force $relativePath
        Copy-Item -Recurse -Force $upstreamPath $relativePath
    } else {
        Write-Host "WARNING: Could not find upstream path for $relativePath"
    }
}

# Cleanup temp directory
Remove-Item -Recurse -Force $tempDir
Write-Host "Overrides updated from upstream commit $commitHash"
```

Verify the overrides are valid after the update:

```powershell
yarn validate-overrides
```

## Key Files to Update

When integrating a new nightly version, these files typically need updates:

| File | Dependencies to Update |
|------|----------------------|
| `vnext/package.json` | `react-native`, `@react-native/*` packages |
| `package.json` (root) | `react-native` in resolutions |
| Various app `package.json` files | Peer dependencies |

## Notes

- The major version (e.g., 0.84 → 0.85) may change between nightlies
- Always verify the target version exists before updating dependencies
