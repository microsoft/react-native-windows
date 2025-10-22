# SDL Policy: /GS Stack Buffer Overrun Crash Monitoring

## Overview

This document describes the process for monitoring, analyzing, and fixing /GS (stack buffer overrun) crashes in React Native Windows, in accordance with Microsoft's Security Development Lifecycle (SDL) policy requirements.

## What is a /GS Crash?

A /GS crash occurs when the `/GS` compiler flag detects stack buffer corruption. The `/GS` flag adds security checks (stack cookies) to detect buffer overruns. When a mismatch is detected, the application is terminated with exception code **0xc0000409** (`STATUS_STACK_BUFFER_OVERRUN`).

## SDL Policy Requirements

Per Microsoft SDL policy:

1. **All high-confidence /GS crashes must be analyzed and fixed when appropriate**
2. **A monitoring plan must be in place to analyze future /GS failures within 60 days** of being reported to Watson
3. **Teams must investigate all crashes that are not false-positives** (`GS_FALSE_POSITIVE_*`)
4. **Fix identified stack buffer overruns** to prevent potential security vulnerabilities

## /GS Crash Classifications

Watson automatically classifies /GS crashes using the `!gs` debugger extension:

- **GS_POSITIVE**: High-confidence stack buffer overrun. **MUST be fixed immediately.**
- **GS_SUSPECT**: Suspected stack buffer overrun. Should be investigated.
- **GS_FALSE_POSITIVE**: False positive due to process corruption. Can be safely ignored.

## Monitoring Process

### 1. Finding /GS Crashes in Watson

1. Go to [Watson](https://aka.ms/watson)
2. Select "Buffer Overruns" under "Enter ANY of the following to see matching BUCKET(s)"
3. Enter the application name (szAppName) or DLL name (szModName)
4. Click "Show Crash/Hang Buckets"
5. Filter for crashes with exception code `0xc0000409`

### 2. Analyzing /GS Crashes

Use the provided `Analyze-Crash.ps1` script to analyze crash dumps:

```powershell
# Analyze a specific dump file
.\vnext\Scripts\Analyze-Crash.ps1 -DumpFilePath "path\to\crash.dmp"

# Or configure automatic dump collection for an exe
.\vnext\Scripts\Analyze-Crash.ps1 -ExeName "YourApp"
```

The script will:
- Detect if the crash is a /GS crash (0xc0000409)
- Run the `!gs` debugger extension for detailed analysis
- Classify the crash (GS_POSITIVE, GS_SUSPECT, or GS_FALSE_POSITIVE)
- Provide actionable guidance based on the classification

### 3. Triage and Response Timeline

| Classification | Priority | Action Required | Timeline |
|---------------|----------|----------------|----------|
| GS_POSITIVE | Critical | Analyze, fix, and test | Within 60 days of Watson report |
| GS_SUSPECT | High | Investigate and determine if real | Within 90 days |
| GS_FALSE_POSITIVE | Low | No action required | N/A |

### 4. Fixing /GS Crashes

When a real stack buffer overrun is identified:

1. **Review the crash analysis**
   - Check the `!gs` output in the analysis log
   - Identify the function with the buffer overrun
   - Locate the source of the overrun (strcpy, sprintf, etc.)

2. **Implement the fix**
   - Replace unsafe functions with safe alternatives:
     - `strcpy` → `strcpy_s`
     - `strcat` → `strcat_s`
     - `sprintf` → `sprintf_s` or `snprintf`
     - `gets` → `fgets` or `gets_s`
   - Add bounds checking to array operations
   - Validate input sizes before copying

3. **Test the fix**
   - Reproduce the original crash scenario
   - Verify the crash no longer occurs
   - Run existing tests to ensure no regressions

4. **Document the fix**
   - Include CVE information if applicable
   - Document the root cause and fix in commit message
   - Update security documentation if needed

## Common Vulnerable Patterns

Watch out for these common patterns that can lead to stack buffer overruns:

```cpp
// UNSAFE: No bounds checking
char buffer[100];
strcpy(buffer, userInput);  // ❌

// SAFE: Use safe alternative with size
char buffer[100];
strcpy_s(buffer, sizeof(buffer), userInput);  // ✅

// UNSAFE: sprintf without size limit
char buffer[50];
sprintf(buffer, "Value: %s", userInput);  // ❌

// SAFE: Use snprintf with size limit
char buffer[50];
snprintf(buffer, sizeof(buffer), "Value: %s", userInput);  // ✅

// UNSAFE: Array access without bounds check
void ProcessArray(int* data, int count) {
  int localArray[10];
  for (int i = 0; i < count; i++) {  // ❌ count could be > 10
    localArray[i] = data[i];
  }
}

// SAFE: Add bounds checking
void ProcessArray(int* data, int count) {
  int localArray[10];
  int safeCount = std::min(count, 10);
  for (int i = 0; i < safeCount; i++) {  // ✅
    localArray[i] = data[i];
  }
}
```

## Reporting Security Issues

If a /GS crash is determined to be a security vulnerability:

1. **Do NOT create a public GitHub issue**
2. Report to Microsoft Security Response Center (MSRC):
   - Online: https://msrc.microsoft.com/create-report
   - Email: secure@microsoft.com
3. Include:
   - Type of issue (stack buffer overrun)
   - Full paths of affected source files
   - Steps to reproduce
   - Crash dump and analysis (if safe to share)
   - Potential security impact

See [.github/security.md](../.github/security.md) for full security reporting guidelines.

## Tools and Resources

- **Analyze-Crash.ps1**: Automated crash analysis script with /GS detection
- **Watson**: https://aka.ms/watson - Microsoft crash reporting system
- **!gs debugger extension**: Built into Windows debuggers, analyzes /GS crashes
- **!analyze -v**: Verbose crash analysis including !gs output

## Regular Monitoring Schedule

To comply with SDL requirements:

1. **Weekly**: Check Watson for new /GS crashes
2. **Monthly**: Review and triage all open /GS issues
3. **Quarterly**: Audit codebase for unsafe string/buffer operations
4. **Before Release**: Ensure no open GS_POSITIVE crashes remain

## Questions?

For questions about /GS crashes or SDL policy:
- Internal: Contact your team's security champion
- External: File an issue in the repository (for non-security questions)
- Security issues: Follow the reporting process in [.github/security.md](../.github/security.md)
