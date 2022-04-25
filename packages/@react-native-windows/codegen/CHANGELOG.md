# Change Log - @react-native-windows/codegen

This log was last generated on Mon, 25 Apr 2022 22:50:30 GMT and should not be manually modified.

<!-- Start content -->

## 0.0.0-canary.31

Mon, 25 Apr 2022 22:50:30 GMT

### Changes

- Update globby of @react-native-windows/codegen to remove glob-parent 3.x dependency (dannyvv@microsoft.com)

## 0.0.0-canary.30

Tue, 12 Apr 2022 05:10:38 GMT

### Changes

- Fix Overrides (34109996+chiaramooney@users.noreply.github.com)
- Bump @react-native-windows/fs to v0.0.0-canary.2

## 0.0.0-canary.29

Fri, 01 Apr 2022 05:09:56 GMT

### Changes

- Fix publish after promotion to preview (ngerlem@microsoft.com)
- Bump @react-native-windows/fs to v0.0.0-canary.1

## 0.0.0-canary.28

Wed, 09 Mar 2022 06:09:07 GMT

### Changes

- Fix turbo module codegen to generate correct import statement and registration (53799235+ZihanChen-MSFT@users.noreply.github.com)

## 0.0.0-canary.27

Fri, 04 Mar 2022 06:09:13 GMT

### Changes

- Generate fabric eventemitter.cpp (acoates@microsoft.com)

## 0.0.0-canary.26

Thu, 24 Feb 2022 06:07:47 GMT

### Changes

- Add TypeScript turbo module definition as new codegen traget (53799235+ZihanChen-MSFT@users.noreply.github.com)

## 0.0.0-canary.25

Wed, 09 Feb 2022 06:09:36 GMT

### Changes

- Bump minimum Node version to 14 (jthysell@microsoft.com)
- Bump @react-native-windows/fs to v1.0.2
- Bump @rnw-scripts/eslint-config to v1.1.11
- Bump @rnw-scripts/jest-unittest-config to v1.2.6
- Bump @rnw-scripts/just-task to v2.2.3
- Bump @rnw-scripts/ts-config to v2.0.2

## 0.0.0-canary.24

Tue, 08 Feb 2022 18:21:23 GMT

### Changes

- Fix TurboModule codegen document (53799235+ZihanChen-MSFT@users.noreply.github.com)

## 0.0.0-canary.23

Sat, 29 Jan 2022 06:08:58 GMT

### Changes

- Add document for turbo module codegen (53799235+ZihanChen-MSFT@users.noreply.github.com)

## 0.0.0-canary.22

Fri, 03 Dec 2021 06:12:10 GMT

### Changes

- Integrate 11/11 (34109996+chiaramooney@users.noreply.github.com)
- Bump @react-native-windows/fs to v1.0.1
- Bump @rnw-scripts/eslint-config to v1.1.10
- Bump @rnw-scripts/jest-unittest-config to v1.2.5
- Bump @rnw-scripts/just-task to v2.2.2

## 0.0.0-canary.21

Thu, 18 Nov 2021 06:11:20 GMT

### Changes

- Use '@react-native-windows/fs' (ngerlem@microsoft.com)
- Bump @react-native-windows/fs to v1.0.0
- Bump @rnw-scripts/eslint-config to v1.1.9

## 0.0.0-canary.20

Thu, 04 Nov 2021 05:07:29 GMT

### Changes

- Codegen prints `::React::JSValue` instead of `React::JSValue` (53799235+ZihanChen-MSFT@users.noreply.github.com)

## 0.0.0-canary.20

Thu, 04 Nov 2021 05:07:08 GMT

### Changes

- Codegen prints `::React::JSValue` instead of `React::JSValue` (53799235+ZihanChen-MSFT@users.noreply.github.com)

## 0.0.0-canary.19

Sat, 23 Oct 2021 05:06:49 GMT

### Changes

- Only generate component files if a component is specified (30809111+acoates-ms@users.noreply.github.com)

## 0.0.0-canary.18

Fri, 22 Oct 2021 05:06:46 GMT

### Changes

- Built files not included in published package (30809111+acoates-ms@users.noreply.github.com)

## 0.0.0-canary.17

Thu, 21 Oct 2021 05:08:01 GMT

### Changes

- Update to typescript 4 (30809111+acoates-ms@users.noreply.github.com)

## 0.0.0-canary.16

Wed, 29 Sep 2021 05:06:54 GMT

### Changes

- Check constants in generated turbo module specs (53799235+ZihanChen-MSFT@users.noreply.github.com)

## 0.0.0-canary.15

Wed, 15 Sep 2021 05:07:04 GMT

### Changes

- Generate anonymous objects in correct order for turbo module (53799235+ZihanChen-MSFT@users.noreply.github.com)

## 0.0.0-canary.14

Fri, 10 Sep 2021 05:07:31 GMT

### Changes

- Generate `std::vector<T>` for array types in turbo modules (53799235+ZihanChen-MSFT@users.noreply.github.com)

## 0.0.0-canary.13

Thu, 09 Sep 2021 05:07:46 GMT

### Changes

- Bump to 0.66-based react-native-tscodegen (ngerlem@microsoft.com)
- Generate nullable type for turbo module (53799235+ZihanChen-MSFT@users.noreply.github.com)

## 0.0.0-canary.12

Wed, 08 Sep 2021 05:08:53 GMT

### Patches

- Bump @rnw-scripts/jest-unittest-config to v1.2.4 (ngerlem@microsoft.com)
- Bump @rnw-scripts/just-task to v2.2.1 (ngerlem@microsoft.com)
- Bump @rnw-scripts/ts-config to v2.0.1 (ngerlem@microsoft.com)
- Bump @rnw-scripts/eslint-config to v1.1.8 (ngerlem@microsoft.com)

### Changes

- Set consistent node requirements on our packages (ngerlem@microsoft.com)
- Match Alert module with NativeDialogManagerWindowsSpec.h (53799235+ZihanChen-MSFT@users.noreply.github.com)

## 0.0.0-canary.11

Fri, 03 Sep 2021 05:06:04 GMT

### Changes

- Generate aliased struct for turbo module (53799235+ZihanChen-MSFT@users.noreply.github.com)

## 0.0.0-canary.10

Thu, 05 Aug 2021 05:07:20 GMT

### Changes

- Do not write codegen files if the contents have not changed, as this breaks incremental builds (30809111+acoates-ms@users.noreply.github.com)

## 0.0.0-canary.9

Fri, 11 Jun 2021 05:08:55 GMT

### Patches

- Bump @rnw-scripts/just-task to v2.2.0 (ngerlem@microsoft.com)

## 0.0.0-canary.8

Wed, 09 Jun 2021 05:10:07 GMT

### Patches

- Bump @rnw-scripts/just-task to v2.1.3 (igklemen@microsoft.com)
- Bump @rnw-scripts/jest-unittest-config to v1.2.3 (igklemen@microsoft.com)

## 0.0.0-canary.7

Thu, 03 Jun 2021 05:09:47 GMT

### Patches

- Bump @rnw-scripts/just-task to v2.1.2 (ngerlem@microsoft.com)
- Bump @rnw-scripts/ts-config to v2.0.0 (ngerlem@microsoft.com)
- Bump @rnw-scripts/eslint-config to v1.1.7 (ngerlem@microsoft.com)
- Bump @rnw-scripts/jest-unittest-config to v1.2.2 (ngerlem@microsoft.com)

### Changes

- Enable esModuleInterop Repo Wide (ngerlem@microsoft.com)

## 0.0.0-canary.6

Fri, 21 May 2021 05:17:18 GMT

### Patches

- Bump @rnw-scripts/just-task to v2.1.1 (ngerlem@microsoft.com)
- Bump @rnw-scripts/jest-unittest-config to v1.2.1 (ngerlem@microsoft.com)

### Changes

- Integrate Apr 28 (base of 0.65) Nightly RN Build (ngerlem@microsoft.com)

## 0.0.0-canary.5

Thu, 13 May 2021 05:11:48 GMT

### Changes

- Hook up view component codegen (30809111+acoates-ms@users.noreply.github.com)

## 0.0.0-canary.4

Thu, 04 Mar 2021 05:05:53 GMT

### Changes

- Update yargs version used and patch y18n version 4.0.0 to 4.0.1 as used by detox of transitive dependencies to address https://cve.mitre.org/cgi-bin/cvename.cgi?name=CVE-2020-7774 (dannyvv@microsoft.com)

## 0.0.0-canary.3

Thu, 11 Feb 2021 05:08:29 GMT

### Patches

- Bump @rnw-scripts/just-task to v2.1.0 (ngerlem@microsoft.com)
- Bump @rnw-scripts/jest-unittest-config to v1.2.0 (ngerlem@microsoft.com)

## 0.0.0-canary.2

Tue, 09 Feb 2021 05:07:37 GMT

### Patches

- Bump @rnw-scripts/just-task to v2.0.2 (dannyvv@microsoft.com)

### Changes

- Bump just-scripts from 1.3.2 to 1.3.3 (dannyvv@microsoft.com)

## 0.0.0-canary.1

Wed, 03 Feb 2021 05:05:12 GMT

### Changes

- Move `react-native-windows-codegen` to `@react-native-windows/codegen` (ngerlem@microsoft.com)

## 1.1.14

Thu, 28 Jan 2021 05:06:47 GMT

### Patches

- Leverage default config support in Just library and factor out common tasks (dannyvv@microsoft.com)

## 1.1.13

Wed, 27 Jan 2021 05:06:36 GMT

### Patches

- Show TS Stack Traces on Uncaught CLI Exception (ngerlem@microsoft.com)

## 1.1.12

Tue, 26 Jan 2021 05:06:39 GMT

### Patches

- Update @types package versions in package.json from dependabot updates to yarn.lock (dannyvv@microsoft.com)

## 0.1.11

Tue, 08 Dec 2020 03:20:37 GMT

### Patches

- Bump yargs to 15.4.1 (ngerlem@microsoft.com)

## 0.1.10

Tue, 24 Nov 2020 05:08:17 GMT

### Patches

- Add more eslint rules (ngerlem@microsoft.com)

## 0.1.8

Sat, 07 Nov 2020 05:04:58 GMT

### Patches

- Create codegen directory if not present (ngerlem@microsoft.com)

## 0.1.7

Thu, 01 Oct 2020 05:08:11 GMT

### Patches

- ESLint Fixup (ngerlem@microsoft.com)

## 0.1.6

Thu, 24 Sep 2020 05:06:21 GMT

### Patches

- Zero-Config Tests for CLI, react-native-windows-init, react-native-windows-codegen (ngerlem@microsoft.com)

## 0.1.5

Mon, 21 Sep 2020 05:06:22 GMT

### Patches

- Fixup eslint config (ngerlem@microsoft.com)

## 0.1.4

Mon, 14 Sep 2020 22:03:37 GMT

### Patches

- Reconcile package versions (ngerlem@microsoft.com)

## 0.1.0

Fri, 14 Aug 2020 05:05:34 GMT

### Minor changes

- Integrate new version of codegen that has changes from react-native (acoates-ms@noreply.github.com)

## 0.0.8

Wed, 01 Jul 2020 05:06:19 GMT

### Patches

- Share eslint and Typescript configs across packages (ngerlem@microsoft.com)

## 0.0.7

Fri, 26 Jun 2020 23:59:27 GMT

### Patches

- Upgrade React Native to 3/22 Nightly Build (ngerlem@microsoft.com)

## 0.0.6

Sun, 07 Jun 2020 00:05:23 GMT

### Patches

- Update template gen to use mustache (dannyvv@microsoft.com)

## 0.0.5

Sat, 23 May 2020 00:04:45 GMT

### Patches

- Include React namespace in more places (acoates@microsoft.com)

## 0.0.4

Thu, 30 Apr 2020 00:04:27 GMT

### Patches

- Update tscodegen to version based on 0.62 (acoates@microsoft.com)

## 0.0.3

Fri, 24 Apr 2020 00:04:41 GMT

### Patches

- Add files option (acoates@microsoft.com)

## 0.0.2

Thu, 23 Apr 2020 00:04:37 GMT

### Patches

- Initial react-native-windows-codegen (acoates@microsoft.com)
