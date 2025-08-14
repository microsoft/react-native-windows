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
      `⚠️ This project is using the React Native (for Windows) Old Architecture. The old architecture will begin to be removed starting with react-native-windows version 0.82.`,
    ),
  );
  console.log();
  console.log(
    chalk.cyan(
      '💡 It is strongly recommended to move to the new architecture as soon as possible to take advantage of improved performance, long-term support, and modern capabilities.',
    ),
  );
  console.log();
  console.log(
    chalk.blue(
      '🔗 Learn more: https://microsoft.github.io/react-native-windows/docs/new-architecture',
    ),
  );
  console.log();
}
