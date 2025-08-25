/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import {ModuleWindowsSetup} from '../moduleWindowsSetup';
import type {ModuleWindowsSetupOptions} from '../moduleWindowsSetupOptions';

// Mock dependencies
jest.mock('@react-native-windows/fs');
jest.mock('glob');
jest.mock('child_process');

describe('ModuleWindowsSetup', () => {
  const mockOptions: ModuleWindowsSetupOptions = {
    logging: false,
    telemetry: false,
    skipDeps: false,
    skipBuild: false,
  };

  beforeEach(() => {
    jest.clearAllMocks();
  });

  describe('getModuleName', () => {
    it('should convert package names to PascalCase', () => {
      const setup = new ModuleWindowsSetup('/test', mockOptions);
      // Access private method for testing
      const getModuleName = (setup as any).getModuleName.bind(setup);
      
      expect(getModuleName('react-native-webview')).toBe('ReactNativeWebview');
      expect(getModuleName('@react-native-community/slider')).toBe('ReactNativeCommunitySlider');
      expect(getModuleName('simple-module')).toBe('SimpleModule');
      expect(getModuleName('@scope/complex-package-name')).toBe('ScopeComplexPackageName');
    });

    it('should handle edge cases', () => {
      const setup = new ModuleWindowsSetup('/test', mockOptions);
      const getModuleName = (setup as any).getModuleName.bind(setup);
      
      expect(getModuleName('')).toBe('');
      expect(getModuleName('single')).toBe('Single');
      expect(getModuleName('---multiple---dashes---')).toBe('MultipleDashes');
    });
  });

  describe('constructor', () => {
    it('should create instance with correct root and options', () => {
      const root = '/test/project';
      const options = {logging: true, telemetry: true, skipDeps: true, skipBuild: true};
      
      const setup = new ModuleWindowsSetup(root, options);
      
      expect(setup.root).toBe(root);
      expect(setup.options).toBe(options);
    });
  });

  describe('verboseMessage', () => {
    it('should log when logging is enabled', () => {
      const consoleSpy = jest.spyOn(console, 'log').mockImplementation();
      const setup = new ModuleWindowsSetup('/test', {logging: true});
      
      (setup as any).verboseMessage('test message');
      
      expect(consoleSpy).toHaveBeenCalledWith('[ModuleWindowsSetup] test message');
      consoleSpy.mockRestore();
    });

    it('should not log when logging is disabled', () => {
      const consoleSpy = jest.spyOn(console, 'log').mockImplementation();
      const setup = new ModuleWindowsSetup('/test', {logging: false});
      
      (setup as any).verboseMessage('test message');
      
      expect(consoleSpy).not.toHaveBeenCalled();
      consoleSpy.mockRestore();
    });
  });
});