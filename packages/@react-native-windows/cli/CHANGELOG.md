# Change Log - @react-native-windows/cli

This log was last generated on Thu, 30 Sep 2021 23:54:23 GMT and should not be manually modified.

<!-- Start content -->

## 0.63.14

Thu, 30 Sep 2021 23:54:23 GMT

### Patches

- Bump the version of xmldom used by @react-native-windows/cli to 0.7.0. (yicyao@microsoft.com)

## 0.63.13

Thu, 26 Aug 2021 14:12:28 GMT

### Patches

- [0.63] Consume Node-Api V8 JSI Runtime (julio.rocha@microsoft.com)

## 0.63.12

Mon, 24 May 2021 15:06:11 GMT

### Patches

- Fix Typo in Autolinking Source (34109996+chiaramooney@users.noreply.github.com)

## 0.63.11

Fri, 26 Mar 2021 22:43:28 GMT

### Patches

- Bump xmldom to 0.5.0. (yicyao@microsoft.com)
- [0.63] Fix app template to use app.json's name for main component name (#7464) (jthysell@microsoft.com)
- [0.63] Autolinking doesn't add a project marked as a non-direct dependency (asklar@microsoft.com)
- Promote 0.63 to legacy (ngerlem@microsoft.com)

## 0.63.10

Mon, 15 Feb 2021 15:05:25 GMT

### Patches

- Fixed typo in template which breaks C# autolinking (jthysell@microsoft.com)

## 0.63.9

Mon, 04 Jan 2021 15:05:20 GMT

### Patches

- [0.63] Fix newline issues in new cli project generated files (#6768) (jthysell@microsoft.com)

## 0.63.8

Mon, 21 Dec 2020 15:06:05 GMT

### Patches

- Print error message when vswhere is missing (asklar@winse.microsoft.com)
- [0.63] Make package config more strict (dannyvv@microsoft.com)

## 0.63.7

Tue, 08 Dec 2020 04:33:16 GMT

### Patches

- [0.63] Autolinking fixes for C++ / C# interop (jthysell@microsoft.com)

## 0.63.6

Fri, 04 Dec 2020 00:20:35 GMT

### Patches

- [0.63] Update cli telemetry defaults (jthysell@microsoft.com)

## 0.63.5

Wed, 18 Nov 2020 01:36:10 GMT

### Patches

- Use NuGet 5.8.0 (ngerlem@microsoft.com)
- Collect anonymized project name (ngerlem@microsoft.com)

## 0.63.4

Mon, 16 Nov 2020 15:06:51 GMT

### Patches

- port nuget 5.8 fix to 0.63 (asklar@microsoft.com)
- Fix run-windows Warnings Using Node 14 (ngerlem@microsoft.com)
- Disable telemetry in other CI systems (asklar@microsoft.com)

## 0.63.3

Tue, 10 Nov 2020 16:50:27 GMT

### Patches

- Don't initialize the telemetry client if running in no-telemetry mode (asklar@microsoft.com)

## 0.63.2

Mon, 09 Nov 2020 17:01:11 GMT

### Patches

- CLI telemetry (asklar@microsoft.com)

## 0.63.1

Mon, 02 Nov 2020 15:05:04 GMT

### Patches

- Fix Generator Failing to Overwrite Template Files (ngerlem@microsoft.com)

## 0.63.0

Mon, 31 Aug 2020 15:05:16 GMT

### Patches

- Promote 0.63 to latest (ngerlem@microsoft.com)

## 0.63.0-preview.4

Fri, 28 Aug 2020 01:35:52 GMT

### Changes

- [0.63] Fix deploy bug where a framework package is installed for one arch but not the one that we are building for (asklar@winse.microsoft.com)
- Fixed templates so C++ apps can consume C# native modules (jthysell@microsoft.com)

## 0.63.0-preview.3

Mon, 24 Aug 2020 15:05:26 GMT

### Changes

- Fixing config and autolinking (jthysell@microsoft.com)

## 0.63.0-preview.2

Mon, 17 Aug 2020 15:05:03 GMT

### Changes

- Make C# CodeGen the default (dannyvv@microsoft.com)
- Disable autolinking check in msbuild when using `react-native run-windows` (jthysell@microsoft.com)

## 0.63.0-preview.1

Fri, 31 Jul 2020 23:32:31 GMT

### Changes

- Promote 0.63 to preview (ngerlem@microsoft.com)

## 0.0.0-canary.1

Tue, 28 Jul 2020 05:06:13 GMT

### Changes

- Separate local-cli into its own package (ngerlem@microsoft.com)
