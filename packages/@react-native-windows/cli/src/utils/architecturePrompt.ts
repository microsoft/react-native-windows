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

/**
 * Shows an interactive prompt asking the user whether they want to continue with Old Architecture
 * or switch to New Architecture.
 * @param templateName The name of the old architecture template being used
 * @param maxRetries Maximum number of retries for invalid input (default: 3)
 * @returns Promise with the user's choice
 */
export async function promptForArchitectureChoice(
  templateName: string,
  maxRetries: number = 3,
): Promise<ArchitecturePromptResult> {
  console.log(
    chalk.yellow(
      `⚠️ The '${templateName}' template is based on the React Native Old Architecture, which will eventually be deprecated in future releases.`,
    ),
  );
  console.log();
  console.log(
    chalk.cyan(
      '💡 We recommend switching to the New Architecture to take advantage of improved performance, long-term support, and modern capabilities.',
    ),
  );
  console.log();
  console.log(
    chalk.blue(
      '🔗 Learn more: https://microsoft.github.io/react-native-windows/docs/new-architecture',
    ),
  );
  console.log();

  let attempts = 0;

  while (attempts < maxRetries) {
    try {
      const response = await prompts(
        {
          type: 'text',
          name: 'choice',
          message:
            'Would you like to continue using the Old Architecture? (Y/N)',
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
        // User cancelled or no input
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
      } else if (normalizedChoice === 'n') {
        console.log(
          chalk.green(
            'Great choice! Setting up the project with New Architecture support.',
          ),
        );
        return {shouldContinueWithOldArch: false, userCancelled: false};
      }
    } catch (error) {
      if ((error as Error).message === 'User cancelled') {
        console.log(
          chalk.yellow(
            '\nNo input received. Proceeding with Old Architecture by default. You can opt into the New Architecture later.',
          ),
        );
        return {shouldContinueWithOldArch: true, userCancelled: true};
      }

      // For other errors, continue to retry
      attempts++;
      if (attempts < maxRetries) {
        console.log(
          chalk.red("Invalid input. Please enter 'Y' for Yes or 'N' for No."),
        );
      }
    }
  }

  // Max retries reached
  console.log(
    chalk.yellow(
      `\nMax retries reached. Proceeding with Old Architecture by default. You can opt into the New Architecture later.`,
    ),
  );
  return {shouldContinueWithOldArch: true, userCancelled: false};
}
