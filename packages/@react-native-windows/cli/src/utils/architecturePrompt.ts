/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import prompts from 'prompts';
import chalk from 'chalk';
export interface ArchitecturePromptResult {
  shouldContinueWithOldArch: boolean;
  userCancelled: boolean;
}

export async function promptForArchitectureChoice(): Promise<ArchitecturePromptResult> {
  try {
    const response = await prompts(
      {
        type: 'text',
        name: 'choice',
        message: 'Would you like to continue using the Old Architecture? (Y/N)',
        validate: (value: string) => {
          const normalized = value.trim().toLowerCase();
          if (normalized === 'y' || normalized === 'n') {
            return true;
          }
          return "Invalid input. Please enter 'Y' for Yes or 'N' for No.";
        },
      },
      {
        onCancel: () => {
          throw new Error('User cancelled');
        },
      },
    );

    if (!response.choice) {
      return {shouldContinueWithOldArch: true, userCancelled: true};
    }

    const normalizedChoice = response.choice.trim().toLowerCase();
    if (normalizedChoice === 'y') {
      console.log(
        chalk.yellow(
          'Proceeding with Old Architecture. You can migrate later using our migration guide: https://microsoft.github.io/react-native-windows/docs/new-architecture',
        ),
      );
      return {shouldContinueWithOldArch: true, userCancelled: false};
    } else {
      console.log(
        chalk.green(
          'Great choice! Setting up the project with New Architecture support.',
        ),
      );
      return {shouldContinueWithOldArch: false, userCancelled: false};
    }
  } catch (error) {
    if ((error as Error).message === 'User cancelled') {
      return {shouldContinueWithOldArch: true, userCancelled: true};
    }
    console.log(
      chalk.yellow(
        'Proceeding with Old Architecture. You can migrate later using our migration guide: https://microsoft.github.io/react-native-windows/docs/new-architecture',
      ),
    );
    return {shouldContinueWithOldArch: true, userCancelled: false};
  }
}
