/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import chalk from 'chalk';

/**
 * Displays a warning message about the Old Architecture template.
 */
export function showOldArchitectureWarning(): void {
  console.log(
    chalk.yellow(
      `⚠️ The old architecture (Paper) is not supported in RNW version v0.82. Please enable your projects using new architecture (Fabric).`,
    ),
  );
  console.log();
  console.log(
    chalk.blue(
      '🔗 Learn more: https://microsoft.github.io/react-native-windows/docs/new-architecture',
    ),
  );
  console.log();
  console.log(
    chalk.blue(
      '🔗 View the list of properties not yet supported in Fabric: https://microsoft.github.io/react-native-windows/docs/new-arch-missingProps',
    ),
  );
  console.log();
  console.log(
    chalk.blue(
      '💬 If you encounter missing properties, please open an issue: https://github.com/microsoft/react-native-windows/issues',
    ),
  );
  console.log();
}
