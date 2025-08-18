/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import {promptForArchitectureChoice} from '../utils/architecturePrompt';

// Mock prompts module
jest.mock('prompts', () => {
  return jest.fn();
});

import prompts from 'prompts';
const mockPrompts = prompts as jest.MockedFunction<typeof prompts>;

describe('architecturePrompt', () => {
  beforeEach(() => {
    jest.clearAllMocks();
  });

  test('returns true when user chooses Y', async () => {
    mockPrompts.mockResolvedValue({choice: 'y'});

    const result = await promptForArchitectureChoice();

    expect(result.shouldContinueWithOldArch).toBe(true);
    expect(result.userCancelled).toBe(false);
  });

  test('returns true when user chooses Y (uppercase)', async () => {
    mockPrompts.mockResolvedValue({choice: 'Y'});

    const result = await promptForArchitectureChoice();

    expect(result.shouldContinueWithOldArch).toBe(true);
    expect(result.userCancelled).toBe(false);
  });

  test('returns false when user chooses N', async () => {
    mockPrompts.mockResolvedValue({choice: 'n'});

    const result = await promptForArchitectureChoice();

    expect(result.shouldContinueWithOldArch).toBe(false);
    expect(result.userCancelled).toBe(false);
  });

  test('returns false when user chooses N (uppercase)', async () => {
    mockPrompts.mockResolvedValue({choice: 'N'});

    const result = await promptForArchitectureChoice();

    expect(result.shouldContinueWithOldArch).toBe(false);
    expect(result.userCancelled).toBe(false);
  });

  test('returns true with userCancelled when user cancels with no input', async () => {
    mockPrompts.mockResolvedValue({});

    const result = await promptForArchitectureChoice();

    expect(result.shouldContinueWithOldArch).toBe(true);
    expect(result.userCancelled).toBe(true);
  });

  test('returns true with userCancelled when prompts throws cancellation error', async () => {
    mockPrompts.mockRejectedValue(new Error('User cancelled'));

    const result = await promptForArchitectureChoice();

    expect(result.shouldContinueWithOldArch).toBe(true);
    expect(result.userCancelled).toBe(true);
  });

  test('returns true and not cancelled on other errors', async () => {
    mockPrompts.mockRejectedValue(new Error('Some other error'));

    const result = await promptForArchitectureChoice();

    expect(result.shouldContinueWithOldArch).toBe(true);
    expect(result.userCancelled).toBe(false);
  });
});
