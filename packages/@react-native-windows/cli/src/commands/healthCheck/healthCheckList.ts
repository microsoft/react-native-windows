/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

// Store list of health checks here to avoid having to launch PowerShell on every react-native config call
export const HealthCheckList = [
  [false, 'FreeSpace', 'Free space on current drive > 15 GB'],
  [false, 'InstalledMemory', 'Installed memory >= 16 GB'],
  [true, 'WindowsVersion', 'Windows version >= 10.0.17763.0'],
  [true, 'DeveloperMode', 'Developer mode is on'],
  [true, 'LongPath', 'Long path support is enabled'],
  [true, 'VSUWP', 'Visual Studio 2022 (>= 17.11.0) & req. components'],
  [true, 'Node', 'Node.js (LTS, >= 18.0)'],
  [true, 'Yarn', 'Yarn'],
  [true, 'DotNetCore', '.NET SDK (LTS, = 8.0)'],
];
