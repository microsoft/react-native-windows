# Attack Surface Analyzer (ASA) Integration

## Overview

Attack Surface Analyzer (ASA) is integrated into the React Native Windows CI pipeline to meet **Secure Development Lifecycle (SDL)** compliance requirements. ASA validates that installers or other high-privilege programs do not weaken the security configuration of the operating system.

## What is ASA?

When installing software on an operating system, elevated privileges are often required. Since installers typically run with 'Administrator' or 'root' privileges, they can easily change the security configuration of the operating system, potentially leaving it in a weakened state after installation is complete.

Attack Surface Analyzer is a Microsoft tool that helps determine the changes made to an operating system during software installation by:
1. Taking a "before" snapshot of the system state
2. Performing the installation or build process
3. Taking an "after" snapshot of the system state
4. Comparing the two snapshots to identify security-relevant changes

## How ASA Runs in CI

ASA is integrated into the Build stage of the PR pipeline and runs automatically on secure pull requests (SecurePullRequest builds). The job performs the following steps:

### 1. Installation
```yaml
dotnet tool install --global Microsoft.CST.AttackSurfaceAnalyzer.CLI
```

### 2. Before Snapshot
Captures the initial system state before any build operations:
```bash
asa collect -r before --verbose
```

### 3. Build Process
Builds the React Native Windows solution (simulating package installation):
- Compiles Microsoft.ReactNative.sln (x64, Release)
- This step represents the "installation" that ASA analyzes

### 4. After Snapshot
Captures the system state after build operations:
```bash
asa collect -r after --verbose
```

### 5. Comparison & Analysis
Exports and analyzes the differences:
```bash
asa export-collect -f before after -o asa-comparison.json
asa export-collect -f before after -o asa-comparison.html
```

### 6. Results Publication
Results are published as build artifacts for review:
- JSON format: `asa-comparison.json` (machine-readable)
- HTML format: `asa-comparison.html` (human-readable)

## What ASA Checks For

ASA monitors for security-relevant changes including:

- **Registry Changes**: Permission modifications, new keys, or value changes
- **File System Changes**: Permission modifications, new files, or attribute changes
- **Service Installations**: New services or service configuration changes
- **Firewall Rules**: New rules or modifications to existing rules
- **Certificate Store**: New certificates or modifications to certificate stores
- **User/Group Changes**: New users, groups, or permission changes
- **Port/Network Changes**: New listening ports or network configuration changes

## Reviewing ASA Results

### In Azure DevOps

1. Go to the failed/completed build
2. Navigate to the "Artifacts" section
3. Download the `ASA_Results` artifact
4. Open `asa-comparison.html` in a browser for a visual overview
5. Review `asa-comparison.json` for detailed, programmatic analysis

### Understanding Results

**Expected Changes:**
- Build artifacts in the build directory
- Temporary files in system temp directories
- MSBuild cache updates
- Visual Studio temporary files

**Unexpected Changes (Require Review):**
- Registry permission changes in system areas
- Service installations
- Firewall rule additions
- Certificate store modifications
- Changes to Program Files or System32 directories

## SDL Compliance

ASA integration satisfies the SDL requirement:

> **Microsoft.Security.AccessControl.10011**: Use Attack Surface Analyzer (ASA) to validate that installers or other high-privilege programs do not weaken the security configuration of the operating system.

All issues identified by ASA must be **fixed** or **justified** before merging. If ASA detects security regressions:

1. Review the specific changes in the ASA report
2. Determine if the changes are intentional and necessary
3. If unintentional, fix the installation/build process to avoid the change
4. If intentional, document the justification in the PR
5. Security team may need to approve certain changes

## Pipeline Configuration

ASA runs as part of the Build stage in `.ado/stages.yml`:

```yaml
- template: jobs/attack-surface-analyzer.yml
  parameters:
    buildEnvironment: ${{ parameters.buildEnvironment }}
    AgentPool: ${{ parameters.AgentPool }}
    complianceWarnOnly: true
```

### Parameters

- **buildEnvironment**: Currently configured to run only for SecurePullRequest builds
- **AgentPool**: Defines the build agent pool (Medium tier recommended)
- **complianceWarnOnly**: When `true`, ASA failures won't block PR (default: `true`)

ASA is configured to run only for SecurePullRequest builds to ensure thorough security scanning in controlled environments. It uses `complianceWarnOnly: true` to allow gradual adoption. This can be changed to `false` to enforce blocking on security regressions.

## Local Testing

Developers can run ASA locally to test before submitting a PR:

### Prerequisites
```powershell
# Install ASA CLI
dotnet tool install --global Microsoft.CST.AttackSurfaceAnalyzer.CLI

# Verify installation
asa --version
```

### Running ASA Locally

```powershell
# 1. Take before snapshot
asa collect -r before

# 2. Perform your build/installation
cd vnext
msbuild Microsoft.ReactNative.sln /p:Configuration=Release /p:Platform=x64

# 3. Take after snapshot
asa collect -r after

# 4. Export comparison
asa export-collect -f before after -o .\asa-comparison.html

# 5. Review results
start .\asa-comparison.html
```

### GUI Mode (Alternative)

ASA also provides a browser-based GUI:

```powershell
asa gui
```

Then navigate to `http://localhost:5000` in your browser.

## Troubleshooting

### ASA Installation Fails

**Issue**: `dotnet tool install` fails
**Solution**: Ensure .NET SDK 8.0 or later is installed:
```powershell
dotnet --version
```

### Large Number of Changes Detected

**Issue**: ASA reports many changes
**Solution**: 
- Filter out expected build artifacts
- Check if antivirus or other background processes are making changes
- Ensure a clean build environment

### ASA Job Times Out

**Issue**: ASA job exceeds 60-minute timeout
**Solution**: 
- Check for hung processes during build
- Review snapshot collection performance
- Consider increasing timeout in `.ado/jobs/attack-surface-analyzer.yml`

## References

- [Microsoft Attack Surface Analyzer GitHub](https://github.com/microsoft/AttackSurfaceAnalyzer)
- [ASA Documentation](https://github.com/microsoft/AttackSurfaceAnalyzer/wiki)
- [SDL Requirements](https://liquid.microsoft.com/Web/Object/Read/MS.Security/Requirements/Microsoft.Security.AccessControl.10011)
- [ASA CLI Usage](https://github.com/microsoft/AttackSurfaceAnalyzer/wiki/Command-Line-Usage)

## Support

For issues or questions about ASA integration:
- Check the [ASA GitHub Issues](https://github.com/microsoft/AttackSurfaceAnalyzer/issues)
- Contact the React Native Windows security team
- Review the SDL compliance documentation in Service Tree
