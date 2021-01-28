# Change Log - react-native-windows-init

This log was last generated on Thu, 28 Jan 2021 05:06:47 GMT and should not be manually modified.

<!-- Start content -->

## 1.1.46

Thu, 28 Jan 2021 05:06:47 GMT

### Patches

- Leverage default config support in Just library and factor out common tasks (dannyvv@microsoft.com)

### Changes

- Bump @react-native-windows/telemetry to v0.0.0-canary.15 (dannyvv@microsoft.com)
- Bump @react-native-windows/cli to v0.0.0-canary.55 (dannyvv@microsoft.com)

## 1.1.45

Wed, 27 Jan 2021 05:06:36 GMT

### Patches

- Show TS Stack Traces on Uncaught CLI Exception (ngerlem@microsoft.com)

## 1.1.44

Tue, 26 Jan 2021 05:06:39 GMT

### Patches

- Update @types package versions in package.json from dependabot updates to yarn.lock (dannyvv@microsoft.com)

### Changes

- Bump @react-native-windows/cli to v0.0.0-canary.54 (dannyvv@microsoft.com)
- Bump @react-native-windows/telemetry to v0.0.0-canary.14 (dannyvv@microsoft.com)

## 1.1.43

Thu, 21 Jan 2021 18:54:54 GMT

### Changes

- Bump @react-native-windows/cli to v0.0.0-canary.53 (tudor.mihai@microsoft.com)

## 0.1.43

Fri, 15 Jan 2021 05:07:01 GMT

### Changes

- Bump @react-native-windows/telemetry to v0.0.0-canary.13 (dannyvv@microsoft.com)
- Bump @react-native-windows/cli to v0.0.0-canary.52 (dannyvv@microsoft.com)

## 0.1.43

Tue, 05 Jan 2021 05:06:42 GMT

### Patches

- Rework error reporting/telemetry to only log the minimal amount of data (asklar@microsoft.com)

## 0.1.42

Tue, 08 Dec 2020 03:20:37 GMT

### Patches

- Allow unreleased RNW versions with react-native-windows-init --useDevMode (ngerlem@microsoft.com)

## 0.1.41

Fri, 04 Dec 2020 00:10:06 GMT

### Patches

- Force republish (ngerlem@microsoft.com)

## 0.1.40

Thu, 03 Dec 2020 21:43:48 GMT

### Patches

- Update cli telemetry defaults (jthysell@microsoft.com)

## 0.1.38

Tue, 01 Dec 2020 05:05:07 GMT

### Patches

- Use strings for telem exit codes (ngerlem@microsoft.com)

## 0.1.37

Tue, 24 Nov 2020 05:08:17 GMT

### Patches

- Add more eslint rules (ngerlem@microsoft.com)

## 0.1.35

Thu, 12 Nov 2020 05:05:57 GMT

### Patches

- Disable telemetry in other CI systems (asklar@microsoft.com)

## 0.1.34

Wed, 11 Nov 2020 05:05:29 GMT

### Patches

- Don't initialize AppInsights when passing no-telemetry (asklar@microsoft.com)

## 0.1.33

Sat, 07 Nov 2020 05:04:58 GMT

### Patches

- Fix Bundling With "react-native-windows-init --useDevMode" (ngerlem@microsoft.com)

## 0.1.32

Thu, 05 Nov 2020 05:05:45 GMT

### Patches

- Add telemetry to react-native-windows CLI (asklar@microsoft.com)

## 0.1.31

Thu, 29 Oct 2020 05:05:49 GMT

### Patches

- Allow Hermes to be used in C++ projects without modifying source code (ngerlem@microsoft.com)

## 0.1.30

Thu, 01 Oct 2020 05:08:11 GMT

### Patches

- ESLint Fixup (ngerlem@microsoft.com)

## 0.1.29

Thu, 24 Sep 2020 05:06:21 GMT

### Patches

- Zero-Config Tests for CLI, react-native-windows-init, react-native-windows-codegen (ngerlem@microsoft.com)

## 0.1.28

Mon, 21 Sep 2020 05:06:22 GMT

### Patches

- Fixup eslint config (ngerlem@microsoft.com)

## 0.1.27

Sat, 19 Sep 2020 05:05:08 GMT

### Patches

- Adding new native module template support to react-native-windows-init (jthysell@microsoft.com)

## 0.1.26

Mon, 14 Sep 2020 22:03:37 GMT

### Patches

- Reconcile package versions (ngerlem@microsoft.com)

## 0.1.22

Fri, 31 Jul 2020 05:05:10 GMT

### Patches

- Fix devmode for react-native-windows-init to add the package to packages.config as well (dannyvv@microsoft.com)

## 0.1.21

Tue, 28 Jul 2020 05:06:13 GMT

### Patches

- Separate local-cli into its own package (ngerlem@microsoft.com)

## 0.1.19

Wed, 08 Jul 2020 01:27:37 GMT

### Patches

- Rever to new experimentalNuGetDependency Naming (ngerlem@microsoft.com)

## 0.1.19-0

Tue, 07 Jul 2020 05:05:23 GMT

### Changes

- Enable experimental usage of Microsoft.ReactNative.Managed as a nuget package (dannyvv@microsoft.com)

## 0.1.17

Fri, 03 Jul 2020 05:05:03 GMT

### Patches

- Replace Custom Generator Prompt With Inquirer (ngerlem@microsoft.com)

## 0.1.16

Wed, 01 Jul 2020 05:06:19 GMT

### Patches

- Share eslint and Typescript configs across packages (ngerlem@microsoft.com)
- Migrate the local-cli to TypeScript (ngerlem@microsoft.com)
- Fix a few more casing typos (dannyvv@microsoft.com)

## 0.1.15

Sun, 28 Jun 2020 05:04:51 GMT

### Patches

- Fix casing typos in NuGet and MSBuild (dannyvv@microsoft.com)

## 0.1.14

Fri, 26 Jun 2020 23:59:27 GMT

### Patches

- Upgrade React Native to 3/22 Nightly Build (ngerlem@microsoft.com)

## 0.1.13

Thu, 25 Jun 2020 14:32:50 GMT

### Patches

- make cli strict and pass true to trailing bool param (asklar@microsoft.com)

## 0.1.12

Wed, 24 Jun 2020 11:51:11 GMT

### Patches

- Fix issue running react-native-windows-init with yarn (acoates@microsoft.com)

## 0.1.11

Wed, 24 Jun 2020 00:07:20 GMT

### Patches

- Prevent --useWinUI3 and --experimentalNugetDependency from being used at the same time in react-native-windows-init (dannyvv@microsoft.com)

## 0.1.10

Fri, 19 Jun 2020 00:07:58 GMT

### Patches

- This change is only intended for developers working on react-native-windows, it is not intended for end-developers 'using' react-native-windows. (dannyvv@microsoft.com)

## 0.1.9

Fri, 12 Jun 2020 00:33:44 GMT

### Patches

- Fix support for `--namespace` on react-native-windows init for cpp (dannyvv@microsoft.com)

## 0.1.8

Sun, 07 Jun 2020 00:05:23 GMT

### Patches

- Update template gen to use mustache (dannyvv@microsoft.com)

## 0.1.7

Fri, 15 May 2020 00:04:44 GMT

### Patches

- winui3 option for app project template (asklar@microsoft.com)

## 0.1.6

Wed, 06 May 2020 00:05:03 GMT

### Patches

- Support --version file:<path> for running init against local changes (acoates@microsoft.com)

## 0.1.5

Wed, 22 Apr 2020 00:04:29 GMT

### Patches

- Fix react-native-windows-init to work with tags (@master) (acoates@microsoft.com)

## 0.1.4

Tue, 07 Apr 2020 18:46:38 GMT

### Patches

- Show version of react-native-windows that was installed (acoates@microsoft.com)

## 0.1.3

Fri, 03 Apr 2020 22:20:34 GMT

### Patches

- Add flag for future template generation (acoates@microsoft.com)

## 0.1.2
Wed, 01 Apr 2020 05:12:32 GMT

### Patches

- Fix version parameter (acoates@microsoft.com)
## 0.1.1
Thu, 26 Mar 2020 20:40:43 GMT

### Patches

- Provide better feedback when RN version has no matching RNW version (acoates@microsoft.com)
## 0.1.0
Wed, 25 Mar 2020 22:34:17 GMT

### Minor changes

- Initial creation of react-native-windows-init package (acoates@microsoft.com)
