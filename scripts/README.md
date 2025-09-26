# RNTester Sync Script

A PowerShell script that syncs React Native Windows files with upstream React Native commits. Fixes `yarn validate-overrides` errors and keeps IntegrationTests up to date.

## 🎯 What It Does

**Syncs TWO things from upstream React Native:**
1. **RNTester files** → `packages/@react-native/tester/js/` (fixes yarn validate-overrides)
2. **IntegrationTests** → `vnext/ReactCopies/IntegrationTests/` (keeps tests current)

**Uses the commit hash YOU provide** - so you control exactly which React Native version to sync from.

## 🚀 Quick Start

```powershell
# Preview what will change (recommended first)
.\scripts\sync-rn-tester.ps1 -CommitHash '1a6d466f1' -DryRun

# Apply the changes
.\scripts\sync-rn-tester.ps1 -CommitHash '1a6d466f1'

# Verify it worked
yarn validate-overrides
```

## 📖 Usage

### Basic Command
```powershell
.\sync-rn-tester.ps1 -CommitHash <your-commit-hash>
```

### Options
- `-CommitHash` (required) - React Native commit to sync from
- `-DryRun` - Preview changes without modifying files
- `-ForceClone` - Force fresh download (if cache issues)

## 🔍 Finding the Right Commit Hash

### From GitHub Compare URL
```
https://github.com/facebook/react-native/compare/old...NEW
Use the NEW commit hash (after the ...)
Example: 1a6d466f1
```

### From Command Line
```bash
git ls-remote https://github.com/facebook/react-native.git main
```

### From GitHub Web
Visit: https://github.com/facebook/react-native/commits/main

## ⚡ Performance

- **First run**: Downloads React Native (~30-60 seconds)
- **Later runs**: Uses cache (~5-10 seconds)
- **Cache location**: `[YourRepoParent]/.rn-cache/`

## 🛡️ Safety

- **Preview mode**: Use `-DryRun` to see changes first
- **Git tracked**: All changes show up in `git diff`
- **Limited scope**: Only touches RNTester and IntegrationTests
- **Auto-detection**: Finds your repo automatically

## 💡 Common Workflow

```powershell
# 1. Save your work
git add . && git commit -m "checkpoint before sync"

# 2. Preview the sync
.\scripts\sync-rn-tester.ps1 -CommitHash 'TARGET_COMMIT' -DryRun

# 3. Apply the sync  
.\scripts\sync-rn-tester.ps1 -CommitHash 'TARGET_COMMIT'

# 4. Verify and commit
yarn validate-overrides
git add . && git commit -m "sync: RNTester with TARGET_COMMIT"
```

## 🔧 Troubleshooting

**"Could not auto-detect repository root"**
→ Run from inside react-native-windows directory

**"Failed to checkout commit"**  
→ Try: `.\sync-rn-tester.ps1 -CommitHash 'abc123' -ForceClone`

**yarn validate-overrides still fails**
→ Double-check the commit hash is correct

## 📁 What Gets Modified

✅ **Modified**: RNTester examples and IntegrationTests only  
❌ **Never touched**: Your app code, Windows native code, build files

---

**That's it!** Simple script, one mode, syncs both areas you need for React Native Windows integration work.