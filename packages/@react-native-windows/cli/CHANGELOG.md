# Change Log - @react-native-windows/cli

This log was last generated on Thu, 28 Jan 2021 05:06:47 GMT and should not be manually modified.

<!-- Start content -->

## 0.0.0-canary.55

Thu, 28 Jan 2021 05:06:47 GMT

### Patches

- Bump @rnw-scripts/just-task to v2.0.1 (dannyvv@microsoft.com)

### Changes

- Leverage default config support in Just library and factor out common tasks (dannyvv@microsoft.com)

## 0.0.0-canary.54

Tue, 26 Jan 2021 05:06:39 GMT

### Patches

- Bump @rnw-scripts/just-task to v2.0.0 (ngerlem@microsoft.com)

### Changes

- Update @types package versions in package.json from dependabot updates to yarn.lock (dannyvv@microsoft.com)

## 0.0.0-canary.53

Thu, 21 Jan 2021 18:54:54 GMT

### Changes

- Update V8 and Hermes package versions (tudor.mihai@microsoft.com)

## 0.0.0-canary.52

Fri, 15 Jan 2021 05:07:01 GMT

### Changes

- Bump @react-native-windows/telemetry to v0.0.0-canary.13 (dannyvv@microsoft.com)

## 0.0.0-canary.51

Wed, 06 Jan 2021 05:06:51 GMT

### Changes

- Print cwd if generator cannot detect project package (ngerlem@microsoft.com)
- Move some packages from @rnw-scripts to @react-native-windows (ngerlem@microsoft.com)

## 0.0.0-canary.50

Tue, 05 Jan 2021 05:06:42 GMT

### Changes

- Rework error reporting/telemetry to only log the minimal amount of data (asklar@microsoft.com)

## 0.0.0-canary.49

Tue, 29 Dec 2020 05:06:55 GMT

### Changes

- Fix newline issues in new cli project generated files (jthysell@microsoft.com)

## 0.0.0-canary.48

Wed, 23 Dec 2020 05:07:22 GMT

### Changes

- Check whether dependencies have been installed (asklar@winse.microsoft.com)

## 0.0.0-canary.47

Sat, 19 Dec 2020 05:06:39 GMT

### Changes

- Print error message when missing vswhere/msbuild (asklar@winse.microsoft.com)

## 0.0.0-canary.46

Wed, 09 Dec 2020 05:07:19 GMT

### Changes

- Deploy using VS tool (asklar@microsoft.com)

## 0.0.0-canary.45

Tue, 08 Dec 2020 03:20:37 GMT

### Changes

- Change from Manually Entered Version to Reference to WinUI.props (34109996+chiaramooney@users.noreply.github.com)
- Ensure windows script added on init (ngerlem@microsoft.com)

## 0.0.0-canary.43

Thu, 03 Dec 2020 21:43:48 GMT

### Changes

- Update cli telemetry defaults (jthysell@microsoft.com)

## 0.0.0-canary.42

Thu, 03 Dec 2020 05:06:17 GMT

### Changes

- Remove the timeout from self-signed certificate generating (Bartosz.Klonowski@callstack.com)

## 0.0.0-canary.41

Tue, 24 Nov 2020 05:08:17 GMT

### Changes

- Add more eslint rules (ngerlem@microsoft.com)

## 0.0.0-canary.39

Wed, 18 Nov 2020 03:10:51 GMT

### Changes

- Use MSBuild Builtin Support for NuGet Restore (ngerlem@microsoft.com)

## 0.0.0-canary.38

Tue, 17 Nov 2020 05:05:00 GMT

### Changes

- Collect anonymized project name (ngerlem@microsoft.com)

## 0.0.0-canary.36

Sat, 14 Nov 2020 05:04:43 GMT

### Changes

- print out sessionId when erroring out in run-windows so people can choose to let us know when filing bugs (asklar@microsoft.com)

## 0.0.0-canary.35

Fri, 13 Nov 2020 05:05:57 GMT

### Changes

- Update nuget version in preparation of VS16.8 which has a breaking change compatibility issue with earlier nuget versoins (dannyvv@microsoft.com)
- Update nuget version in preparation of VS16.8 which has a breaking change compatibility issue with earlier nuget versoins (dannyvv@microsoft.com)

## 0.0.0-canary.34

Thu, 12 Nov 2020 05:05:57 GMT

### Changes

- Disable telemetry in other CI systems (asklar@microsoft.com)

## 0.0.0-canary.33

Wed, 11 Nov 2020 05:05:29 GMT

### Changes

- Don't initialize AppInsights when passing no-telemetry (asklar@microsoft.com)

## 0.0.0-canary.32

Sat, 07 Nov 2020 05:04:58 GMT

### Changes

- Fix Bundling With "react-native-windows-init --useDevMode" (ngerlem@microsoft.com)

## 0.0.0-canary.29

Thu, 05 Nov 2020 05:05:45 GMT

### Changes

- fix(cli): Also look for .msix* extensions in CheckIfNeedInstallCertificate (tonguye@microsoft.com)
- Add telemetry to react-native-windows CLI (asklar@microsoft.com)

## 0.0.0-canary.28

Wed, 04 Nov 2020 05:10:08 GMT

### Changes

- Autolinking fixes for C++ / C# interop (jthysell@microsoft.com)

## 0.0.0-canary.27

Sun, 01 Nov 2020 05:05:46 GMT

### Changes

- Update V8 and Hermes package versions (tudor.mihai@microsoft.com)

## 0.0.0-canary.26

Sat, 31 Oct 2020 05:06:50 GMT

### Changes

- Fix Generator Failing to Overwrite Template Files (ngerlem@microsoft.com)

## 0.0.0-canary.25

Fri, 30 Oct 2020 05:07:51 GMT

### Changes

- Enable Hermes Bytecode Compilation and SourceMap Creation as Part of Build (ngerlem@microsoft.com)

## 0.0.0-canary.24

Thu, 29 Oct 2020 05:05:49 GMT

### Changes

- Allow Hermes to be used in C++ projects without modifying source code (ngerlem@microsoft.com)

## 0.0.0-canary.23

Sat, 17 Oct 2020 05:06:20 GMT

### Changes

- C# apps have x86 platform=x86 but C++ apps have Win32 (asklar@microsoft.com)
- Revert "Fix project config and package deploy to support the windows code in a different location (#6232)" (dannyvv@microsoft.com)

## 0.0.0-canary.22

Fri, 16 Oct 2020 05:04:19 GMT

### Changes

- Fix config and app publish to allow for the windows native source code to live in a different location (dannyvv@microsoft.com)

## 0.0.0-canary.21

Thu, 01 Oct 2020 05:08:11 GMT

### Changes

- ESLint Fixup (ngerlem@microsoft.com)

## 0.0.0-canary.20

Tue, 29 Sep 2020 05:05:53 GMT

### Changes

- Fix run-windows Warnings Using Node 14 (ngerlem@microsoft.com)

## 0.0.0-canary.19

Fri, 25 Sep 2020 05:05:40 GMT

### Changes

- Fix casing rules for new projects names for `react-native-windows-init` (jthysell@microsoft.com)
- Add -NoProfile to powershell invocations to ensure deterministic script execution where user profiles do not have the ability to break or change the build outputs (dannyvv@microsoft.com)

## 0.0.0-canary.18

Thu, 24 Sep 2020 05:06:21 GMT

### Changes

- Zero-Config Tests for CLI, react-native-windows-init, react-native-windows-codegen (ngerlem@microsoft.com)

## 0.0.0-canary.17

Mon, 21 Sep 2020 05:06:22 GMT

### Changes

- Fixup eslint config (ngerlem@microsoft.com)

## 0.0.0-canary.16

Sat, 19 Sep 2020 05:05:08 GMT

### Changes

- Adding new native module template support to react-native-windows-init (jthysell@microsoft.com)

## 0.0.0-canary.15

Wed, 16 Sep 2020 05:05:28 GMT

### Changes

- Enable trying Previews of VS as a fallback (asklar@winse.microsoft.com)
- Yarn on Windows breaks apps that read from the environment variables (asklar@winse.microsoft.com)

## 0.0.0-canary.14

Tue, 15 Sep 2020 05:05:15 GMT

### Changes

- fix ts warning (asklar@microsoft.com)

## 0.0.0-canary.13

Mon, 14 Sep 2020 22:03:37 GMT

### Changes

- Reconcile package versions (ngerlem@microsoft.com)

## 0.0.0-canary.11

Wed, 09 Sep 2020 05:06:35 GMT

### Changes

- Enable @typescript-eslint/no-floating-promises (ngerlem@microsoft.com)

## 0.0.0-canary.10

Wed, 02 Sep 2020 05:05:16 GMT

### Changes

- Keep Templates in react-native-windows Package (ngerlem@microsoft.com)

## 0.0.0-canary.9

Fri, 28 Aug 2020 05:06:54 GMT

### Changes

- Fixed templates so C++ apps can consume C# native modules (jthysell@microsoft.com)

## 0.0.0-canary.8

Wed, 26 Aug 2020 05:07:06 GMT

### Changes

- Fix deploy bug where a framework package is installed for one arch but not the one that we are building for (asklar@winse.microsoft.com)

## 0.0.0-canary.7

Tue, 25 Aug 2020 05:05:08 GMT

### Changes

- Default run-windows to disabling parallel MSBuild When Machine Has < 16GB of Memory (ngerlem@microsoft.com)

## 0.0.0-canary.5

Thu, 20 Aug 2020 05:05:23 GMT

### Changes

- Add --singleproc to run-windows (asklar@microsoft.com)

## 0.0.0-canary.4

Wed, 19 Aug 2020 05:04:41 GMT

### Changes

- Fixing config and autolinking (jthysell@microsoft.com)

## 0.0.0-canary.3

Fri, 14 Aug 2020 05:05:34 GMT

### Changes

- Disable autolinking check in msbuild when using `react-native run-windows` (jthysell@microsoft.com)

## 0.0.0-canary.2

Tue, 11 Aug 2020 07:36:05 GMT

### Changes

- Make C# CodeGen the default (dannyvv@microsoft.com)

## 0.0.0-canary.1

Tue, 28 Jul 2020 05:06:13 GMT

### Changes

- Separate local-cli into its own package (ngerlem@microsoft.com)
