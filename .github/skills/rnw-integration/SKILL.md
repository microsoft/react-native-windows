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
