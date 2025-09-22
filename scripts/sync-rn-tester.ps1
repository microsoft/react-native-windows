param(
    [Parameter(Mandatory=$true)]
    [string]$CommitHash,
    [string]$ReactNativeRepo = "https://github.com/facebook/react-native.git",
    [string]$WindowsRepoRoot = $null,     # Auto-detect current repo root
    [string]$TesterPath = $null,          # Auto-detect tester path  
    [string]$IntegrationTestsPath = $null, # Auto-detect vnext\ReactCopies\IntegrationTests path
    [string]$CacheDir = $null,            # Auto-detect cache location
    [switch]$DryRun = $false,
    [switch]$ForceClone = $false
)

# Colors for output
function Write-Success { param($Message) Write-Host $Message -ForegroundColor Green }
function Write-Warning { param($Message) Write-Host $Message -ForegroundColor Yellow }
function Write-Error { param($Message) Write-Host $Message -ForegroundColor Red }
function Write-Info { param($Message) Write-Host $Message -ForegroundColor Cyan }

# Help function
function Show-Help {
    Write-Host @"
RNTester Sync Script - Sync React Native Tester files with upstream commits

USAGE:
    .\sync-rn-tester.ps1 -CommitHash <hash> [options]

REQUIRED:
    -CommitHash <string>    The React Native commit hash to sync from

OPTIONS:
    -DryRun                Preview changes without actually modifying files
    -ForceClone            Force fresh clone (ignore cache)

EXAMPLES:
    # Sync RNTester + IntegrationTests (main use case)
    .\sync-rn-tester.ps1 -CommitHash "1a6d466f1"

    # Preview changes first (recommended)
    .\sync-rn-tester.ps1 -CommitHash "1a6d466f1" -DryRun

    # Force fresh clone and sync
    .\sync-rn-tester.ps1 -CommitHash "1a6d466f1" -ForceClone

    # Force fresh clone and sync
    .\sync-rn-tester.ps1 -CommitHash "1a6d466f1" -ForceClone

CACHE:
    Repository is automatically cached in parent directory of your repo:
    [YourRepoParent]\.rn-cache\react-native
    
    You can override with: -CacheDir "C:\your\custom\path"
    Use -ForceClone to refresh cache if needed

AUTO-DETECTION:
    Script auto-detects your repository root and paths
    Override with: -WindowsRepoRoot "C:\path\to\react-native-windows"
"@ -ForegroundColor White
}

# Auto-detect paths if not provided
if (-not $WindowsRepoRoot) {
    # Find the react-native-windows root by looking for package.json with "react-native-windows" in name
    $CurrentDir = Get-Location
    $SearchDir = $CurrentDir
    
    while ($SearchDir) {
        $PackageJson = Join-Path $SearchDir "package.json"
        if (Test-Path $PackageJson) {
            try {
                $PackageContent = Get-Content $PackageJson -Raw | ConvertFrom-Json
                if ($PackageContent.name -eq "react-native-windows" -or 
                    $PackageContent.name -eq "react-native-windows-repo" -or 
                    $SearchDir.Name -eq "react-native-windows") {
                    $WindowsRepoRoot = $SearchDir.Path
                    break
                }
            } catch {
                # Ignore JSON parsing errors and continue searching
            }
        }
        $SearchDir = $SearchDir.Parent
    }
    
    if (-not $WindowsRepoRoot) {
        Write-Error "Could not auto-detect react-native-windows repository root. Please run from within the repo or specify -WindowsRepoRoot"
        exit 1
    }
}

if (-not $TesterPath) {
    $TesterPath = Join-Path $WindowsRepoRoot "packages\@react-native\tester"
}

if (-not $IntegrationTestsPath) {
    $IntegrationTestsPath = Join-Path $WindowsRepoRoot "vnext\ReactCopies\IntegrationTests"
}

if (-not $CacheDir) {
    # Use parent directory of WindowsRepoRoot for cache
    $ParentDir = Split-Path $WindowsRepoRoot -Parent
    $CacheDir = Join-Path $ParentDir ".rn-cache"
}

# List of directories that should be exact copies (from yarn validate-overrides)
$OverrideDirectories = @(
    "js\components",
    "js\examples\Accessibility",
    "js\examples\ActionSheetIOS",
    "js\examples\ActivityIndicator",
    "js\examples\Alert",
    "js\examples\Animated",
    "js\examples\AnimatedGratuitousApp",
    "js\examples\Appearance",
    "js\examples\AppState",
    "js\examples\Border",
    "js\examples\Button",
    "js\examples\ContentURLAndroid",
    "js\examples\Crash",
    "js\examples\DevSettings",
    "js\examples\Dimensions",
    "js\examples\DisplayContents",
    "js\examples\DrawerLayoutAndroid",
    "js\examples\Experimental",
    "js\examples\Filter",
    "js\examples\FlatList",
    "js\examples\Image",
    "js\examples\InputAccessoryView",
    "js\examples\InvalidProps",
    "js\examples\JSResponderHandlerExample",
    "js\examples\Keyboard",
    "js\examples\KeyboardAvoidingView",
    "js\examples\Layout",
    "js\examples\LinearGradient",
    "js\examples\MixBlendMode",
    "js\examples\Modal",
    "js\examples\NativeAnimation",
    "js\examples\NewAppScreen",
    "js\examples\NewArchitecture",
    "js\examples\OrientationChange",
    "js\examples\OSSLibraryExample",
    "js\examples\PanResponder",
    "js\examples\Performance",
    "js\examples\PermissionsAndroid",
    "js\examples\PixelRatio",
    "js\examples\PlatformColor",
    "js\examples\Playground",
    "js\examples\PointerEvents",
    "js\examples\PopupMenuAndroid",
    "js\examples\Pressable",
    "js\examples\RCTRootView",
    "js\examples\RootViewSizeFlexibilityExample",
    "js\examples\RTL",
    "js\examples\ScrollView",
    "js\examples\SectionList",
    "js\examples\SetPropertiesExample",
    "js\examples\Share",
    "js\examples\Snapshot",
    "js\examples\StatusBar",
    "js\examples\Switch",
    "js\examples\Text",
    "js\examples\Timer",
    "js\examples\ToastAndroid",
    "js\examples\Touchable",
    "js\examples\Transform",
    "js\examples\TransparentHitTest",
    "js\examples\TurboModule",
    "js\examples\Vibration",
    "js\examples\View",
    "js\examples\XHR",
    "js\types"
)

# Determine operation mode
$Mode = "RNTester Override Directories + IntegrationTests"

# Show help if requested
if ($args -contains "--help" -or $args -contains "-h") {
    Show-Help
    exit 0
}

Write-Info "=== RNTester Sync Script ==="
Write-Info "Auto-detected paths:"
Write-Info "  Windows Repo: $WindowsRepoRoot"
Write-Info "  Tester Path:  $TesterPath"
Write-Info "  Integration Tests: $IntegrationTestsPath"
Write-Info "  Cache Dir:    $CacheDir"
Write-Info "Mode: $Mode"
Write-Info "Commit: $CommitHash"
if ($DryRun) { Write-Warning "DRY RUN MODE - No files will be modified" }

# Check if git is available
if (-not (Get-Command git -ErrorAction SilentlyContinue)) {
    Write-Error "Git is not installed or not in PATH"
    exit 1
}

# Setup repository cache
$RepoDir = Join-Path $CacheDir "react-native"
$UpdatedFiles = @()
$FailedFiles = @()

try {
    # Create cache directory if it doesn't exist
    if (-not (Test-Path $CacheDir)) {
        Write-Info "Creating cache directory: $CacheDir"
        New-Item -ItemType Directory -Path $CacheDir -Force | Out-Null
    }

    # Check if repo already exists
    if ((Test-Path $RepoDir) -and (-not $ForceClone)) {
        Write-Info "Using cached React Native repository at: $RepoDir"
        Set-Location $RepoDir
        
        # Fetch latest changes
        Write-Info "Fetching latest changes..."
        git fetch origin 2>&1 | Out-Host
        
        # Check if commit exists locally
        $CommitExists = git rev-parse --verify "$CommitHash^{commit}" 2>$null
        if ($LASTEXITCODE -ne 0) {
            Write-Warning "Commit $CommitHash not found locally, fetching more history..."
            git fetch --depth 1000 origin 2>&1 | Out-Host
        }
    } else {
        if (Test-Path $RepoDir) {
            Write-Warning "Removing existing repository (ForceClone=$ForceClone)"
            Remove-Item $RepoDir -Recurse -Force
        }
        
        Write-Info "Cloning React Native repository to cache..."
        git clone --depth 500 $ReactNativeRepo $RepoDir 2>&1 | Out-Host
        if ($LASTEXITCODE -ne 0) {
            throw "Failed to clone React Native repository"
        }
        Set-Location $RepoDir
    }

    # Checkout the specific commit
    Write-Info "Checking out commit $CommitHash..."
    git checkout $CommitHash --quiet 2>$null
    if ($LASTEXITCODE -ne 0) {
        Write-Warning "Failed to checkout $CommitHash, trying deeper fetch..."
        git fetch --depth 2000 origin 2>&1 | Out-Host
        git checkout $CommitHash --quiet 2>$null
        if ($LASTEXITCODE -ne 0) {
            throw "Failed to checkout commit $CommitHash even after deeper fetch"
        }
    }

    # Show commit info
    $CommitInfo = git log -1 --oneline $CommitHash
    Write-Success "Using commit: $CommitInfo"

    $UpstreamTesterPath = Join-Path $RepoDir "packages\rn-tester"
    if (-not (Test-Path $UpstreamTesterPath)) {
        throw "Upstream rn-tester directory not found at: $UpstreamTesterPath"
    }

    # Sync RNTester override directories
    Write-Info "Processing $($OverrideDirectories.Count) override directories..."

    foreach ($RelativeDir in $OverrideDirectories) {
            $SourceDir = Join-Path $UpstreamTesterPath $RelativeDir
            $TargetDir = Join-Path $TesterPath $RelativeDir
            
            Write-Info "Processing: $RelativeDir"

            if (-not (Test-Path $SourceDir)) {
                Write-Warning "Source directory not found: $RelativeDir (skipping)"
                continue
            }

            if (-not (Test-Path $TargetDir)) {
                if (-not $DryRun) {
                    New-Item -ItemType Directory -Path $TargetDir -Force | Out-Null
                    Write-Info "Created directory: $TargetDir"
                } else {
                    Write-Info "[DRY RUN] Would create directory: $TargetDir"
                }
            }

            $SourceFiles = Get-ChildItem -Path $SourceDir -Recurse -File
            foreach ($SourceFile in $SourceFiles) {
                $RelativeFilePath = $SourceFile.FullName.Substring($SourceDir.Length + 1)
                $TargetFile = Join-Path $TargetDir $RelativeFilePath
                $TargetFileDir = Split-Path $TargetFile -Parent

                if (-not (Test-Path $TargetFileDir)) {
                    if (-not $DryRun) {
                        New-Item -ItemType Directory -Path $TargetFileDir -Force | Out-Null
                    } else {
                        Write-Info "[DRY RUN] Would create directory: $TargetFileDir"
                    }
                }

                try {
                    if (-not $DryRun) {
                        Copy-Item -Path $SourceFile.FullName -Destination $TargetFile -Force
                        $UpdatedFiles += $TargetFile
                        Write-Success "Updated: $($RelativeDir)\$($RelativeFilePath)"
                    } else {
                        Write-Info "[DRY RUN] Would update: $($RelativeDir)\$($RelativeFilePath)"
                    }
                } catch {
                    Write-Error "Failed to copy $($RelativeDir)\$($RelativeFilePath)`: $_"
                    $FailedFiles += "$($RelativeDir)\$($RelativeFilePath)"
                }
            }

            # Remove obsolete files
            if (Test-Path $TargetDir) {
                $TargetFiles = Get-ChildItem -Path $TargetDir -Recurse -File
                foreach ($TargetFile in $TargetFiles) {
                    $RelativeFilePath = $TargetFile.FullName.Substring($TargetDir.Length + 1)
                    $SourceFile = Join-Path $SourceDir $RelativeFilePath
                    
                    if (-not (Test-Path $SourceFile)) {
                        if (-not $DryRun) {
                            Remove-Item -Path $TargetFile.FullName -Force
                            Write-Warning "Removed obsolete file: $($RelativeDir)\$($RelativeFilePath)"
                        } else {
                            Write-Info "[DRY RUN] Would remove obsolete file: $($RelativeDir)\$($RelativeFilePath)"
                        }
                    }
                }
            }
        }

    # Sync IntegrationTests
    Write-Info "Syncing IntegrationTests directory..."
    
    $SourceIntegrationTestsDir = Join-Path $RepoDir "packages\rn-tester\IntegrationTests"
    
    if (Test-Path $SourceIntegrationTestsDir) {
        # Create target directory if it doesn't exist
        if (-not (Test-Path $IntegrationTestsPath)) {
            if (-not $DryRun) {
                New-Item -ItemType Directory -Path $IntegrationTestsPath -Force | Out-Null
                Write-Info "Created IntegrationTests directory: $IntegrationTestsPath"
            } else {
                Write-Info "[DRY RUN] Would create directory: $IntegrationTestsPath"
            }
        }
        
        # Get all files in source IntegrationTests directory
        $IntegrationSourceFiles = Get-ChildItem -Path $SourceIntegrationTestsDir -Recurse -File
        
        foreach ($SourceFile in $IntegrationSourceFiles) {
            $RelativeFilePath = $SourceFile.FullName.Substring($SourceIntegrationTestsDir.Length + 1)
            $TargetFile = Join-Path $IntegrationTestsPath $RelativeFilePath
            $TargetFileDir = Split-Path $TargetFile -Parent

            if (-not (Test-Path $TargetFileDir)) {
                if (-not $DryRun) {
                    New-Item -ItemType Directory -Path $TargetFileDir -Force | Out-Null
                } else {
                    Write-Info "[DRY RUN] Would create directory: $TargetFileDir"
                }
            }

            try {
                if (-not $DryRun) {
                    Copy-Item -Path $SourceFile.FullName -Destination $TargetFile -Force
                    $UpdatedFiles += $TargetFile
                    Write-Success "Updated IntegrationTests: $RelativeFilePath"
                } else {
                    Write-Info "[DRY RUN] Would update IntegrationTests: $RelativeFilePath"
                }
            } catch {
                Write-Error "Failed to copy IntegrationTests file $RelativeFilePath`: $_"
                $FailedFiles += "IntegrationTests\$RelativeFilePath"
            }
        }
        
        # Remove obsolete files in IntegrationTests
        if (Test-Path $IntegrationTestsPath) {
            $TargetIntegrationFiles = Get-ChildItem -Path $IntegrationTestsPath -Recurse -File
            foreach ($TargetFile in $TargetIntegrationFiles) {
                $RelativeFilePath = $TargetFile.FullName.Substring($IntegrationTestsPath.Length + 1)
                $SourceFile = Join-Path $SourceIntegrationTestsDir $RelativeFilePath
                
                if (-not (Test-Path $SourceFile)) {
                    if (-not $DryRun) {
                        Remove-Item -Path $TargetFile.FullName -Force
                        Write-Warning "Removed obsolete IntegrationTests file: $RelativeFilePath"
                    } else {
                        Write-Info "[DRY RUN] Would remove obsolete IntegrationTests file: $RelativeFilePath"
                    }
                }
            }
        }
    } else {
        Write-Warning "IntegrationTests directory not found in upstream commit: $SourceIntegrationTestsDir"
    }

} catch {
    Write-Error "Error: $_"
    exit 1
} finally {
    Write-Info "Repository cached at: $RepoDir"
}

# Summary
Write-Info "`n=== SUMMARY ==="
Write-Info "Mode: $Mode"
Write-Info "Commit: $CommitHash"
Write-Success "Updated files: $($UpdatedFiles.Count)"
if ($FailedFiles.Count -gt 0) {
    Write-Error "Failed files: $($FailedFiles.Count)"
    $FailedFiles | ForEach-Object { Write-Host "  - $_" -ForegroundColor Red }
}

if (-not $DryRun) {
    Write-Success "`nRNTester and IntegrationTests synced!"
} else {
    Write-Info "`n[DRY RUN] Use without -DryRun to actually perform the sync."
}