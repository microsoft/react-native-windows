/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
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
    // Mock console methods to prevent output during tests
    jest.spyOn(console, 'log').mockImplementation(() => {});
  });

  afterEach(() => {
    jest.restoreAllMocks();
  });

  test('returns true when user chooses Y', async () => {
    mockPrompts.mockResolvedValue({choice: 'y'});

    const result = await promptForArchitectureChoice('old/uwp-cpp-app');

    expect(result.shouldContinueWithOldArch).toBe(true);
    expect(result.userCancelled).toBe(false);
  });

  test('returns true when user chooses Y (uppercase)', async () => {
    mockPrompts.mockResolvedValue({choice: 'Y'});

    const result = await promptForArchitectureChoice('old/uwp-cpp-app');

    expect(result.shouldContinueWithOldArch).toBe(true);
    expect(result.userCancelled).toBe(false);
  });

  test('returns false when user chooses N', async () => {
    mockPrompts.mockResolvedValue({choice: 'n'});

    const result = await promptForArchitectureChoice('old/uwp-cpp-app');

    expect(result.shouldContinueWithOldArch).toBe(false);
    expect(result.userCancelled).toBe(false);
  });

  test('returns false when user chooses N (uppercase)', async () => {
    mockPrompts.mockResolvedValue({choice: 'N'});

    const result = await promptForArchitectureChoice('old/uwp-cpp-app');

    expect(result.shouldContinueWithOldArch).toBe(false);
    expect(result.userCancelled).toBe(false);
  });

  test('returns true with userCancelled when user cancels', async () => {
    mockPrompts.mockResolvedValue({});

    const result = await promptForArchitectureChoice('old/uwp-cpp-app');

    expect(result.shouldContinueWithOldArch).toBe(true);
    expect(result.userCancelled).toBe(true);
  });

  test('returns true with userCancelled when prompts throws cancellation error', async () => {
    mockPrompts.mockRejectedValue(new Error('User cancelled'));

    const result = await promptForArchitectureChoice('old/uwp-cpp-app');

    expect(result.shouldContinueWithOldArch).toBe(true);
    expect(result.userCancelled).toBe(true);
  });

  test('handles max retries for invalid input', async () => {
    // First two calls return invalid responses, third call succeeds
    mockPrompts
      .mockRejectedValueOnce(new Error('Invalid input'))
      .mockRejectedValueOnce(new Error('Invalid input'))
      .mockResolvedValueOnce({choice: 'y'});

    const result = await promptForArchitectureChoice('old/uwp-cpp-app', 3);

    expect(result.shouldContinueWithOldArch).toBe(true);
    expect(result.userCancelled).toBe(false);
    expect(mockPrompts).toHaveBeenCalledTimes(3);
  });

  test('returns true after max retries exceeded', async () => {
    // All calls return invalid responses
    mockPrompts.mockRejectedValue(new Error('Invalid input'));

    const result = await promptForArchitectureChoice('old/uwp-cpp-app', 2);

    expect(result.shouldContinueWithOldArch).toBe(true);
    expect(result.userCancelled).toBe(false);
    expect(mockPrompts).toHaveBeenCalledTimes(2);
  });
});
