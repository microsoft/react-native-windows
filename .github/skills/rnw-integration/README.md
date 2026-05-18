# RNW Integration Skill

Automates upgrading React Native Windows to newer upstream React Native nightly builds.

## What This Skill Does

This skill guides GitHub Copilot through the full RNW integration workflow — from finding the next nightly version to building and deploying the Playground app. It handles version updates, override upgrades, merge conflict resolution, and build validation.

## Prerequisites

Before running this skill, ensure you have:

- **Node.js** >= 22 installed
- **Yarn** package manager (the repo uses Yarn Berry / v4)
- **Visual Studio 2022** with:
  - C++ Desktop development workload
  - Windows App SDK
  - Windows 10/11 SDK
- **Git** configured with access to:
  - Your RNW fork (origin)
  - `https://github.com/Microsoft/react-native-windows` (upstream)
- **Developer Mode** enabled in Windows Settings (for app sideloading)

## How to Use

### Option 1: Run the Full Integration

Type in VS Code Copilot Chat:
```
/rnw-integration run all steps
```

### Option 2: Run Specific Steps

```
/rnw-integration run step 1 to 5
/rnw-integration run step 11
/rnw-integration run step 12
```

### Option 3: Just Find the Next Version

```
/rnw-integration find next nightly version
```

## Steps Overview

| Step | Description | Automated? |
|------|-------------|-----------|
| 1-5 | Find current version, calculate target date, query npm | Yes |
| 6 | Run `yarn integrate-rn <version>` | Yes |
| 7 | Update remaining package.json files | Yes |
| 8 | Clean node_modules and reinstall dependencies | Yes |
| 9 | Run `npx react-native-platform-override upgrade` | Interactive |
| 10 | Commit changes | Yes |
| 11 | Replace failing overrides from upstream | Yes |
| 12 | Resolve merge conflicts (human-in-the-loop) | Per-hunk approval |
| 13 | Update yarn.lock, build Playground composition app | Yes (long build) |
| 14 | Deploy and launch Playground | Yes |

## Key Concepts

### Nightly Versions
React Native publishes nightly builds with the format:
```
0.XX.0-nightly-YYYYMMDD-<commit-hash>
```
RNW integrates every ~7 days by advancing to a newer nightly.

### Overrides
RNW maintains platform-specific copies of upstream files (`.windows.js`, `.win32.js`). The override system tracks which upstream files are copied and detects when they change.

### Merge Conflicts
When upstream changes conflict with RNW's Windows-specific modifications, conflicts appear as `<<<<<<< Upstream` / `>>>>>>> Override` markers. Step 11 resolves these with human approval.

### Conflict Resolution Patterns
Historical patterns are documented in `references/conflict-patterns.md`. The skill uses these to propose resolutions with confidence levels (Low/Medium/High).

## File Structure

```
.github/skills/rnw-integration/
├── README.md                          # This file
├── SKILL.md                           # Main skill instructions
├── references/
│   └── conflict-patterns.md           # Historical conflict resolution patterns
└── scripts/                           # (future automation scripts)
```

## Common Issues

### Build fails with "Yarn packages are out of date"
Run `yarn install` in the repo root before building.

### Build fails with missing Boost headers
Run NuGet restore:
```powershell
msbuild <solution.sln> /t:Restore /p:Configuration=Debug /p:Platform=x64
```

### MSIX install fails with "No signature was present" (0x800B0100)
Debug builds are not signed. Use loose layout registration instead:
```powershell
Add-AppxPackage -Register <path>\AppxManifest.xml -ForceApplicationShutdown
```

### MSBuild file locking errors (midl.exe)
A previous build process is still running. Clean the build directory:
```powershell
Remove-Item -Recurse -Force vnext\build -ErrorAction SilentlyContinue
```

### Override validation fails after integration
Run Step 10 to replace failing overrides from upstream, then Step 11 to resolve remaining conflicts.

## Tips

- The build (Step 12) takes 15-30 minutes for a clean build
- When resolving conflicts, respond "Yes to all remaining" if all hunks are High confidence
- Always check the upstream compare URL before accepting conflict resolutions
- The major version (e.g., 0.84 → 0.85) may change between nightlies
