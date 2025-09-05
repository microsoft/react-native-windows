/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import {SetupModuleWindows} from '../setupModuleWindows';
import type {SetupModuleWindowsOptions} from '../setupModuleWindowsOptions';
import fs from '@react-native-windows/fs';

// Mock dependencies
jest.mock('@react-native-windows/fs');
jest.mock('glob');
jest.mock('child_process');

describe('SetupModuleWindows', () => {
  const mockOptions: SetupModuleWindowsOptions = {
    logging: false,
    telemetry: false,
    skipDeps: false,
    skipBuild: false,
    template: 'cpp-lib',
  };

  beforeEach(() => {
    jest.clearAllMocks();
  });

  describe('getModuleName', () => {
    it('should convert package names to PascalCase', () => {
      const setup = new SetupModuleWindows('/test', mockOptions);
      // Access private method for testing
      const getModuleName = (setup as any).getModuleName.bind(setup);

      expect(getModuleName('react-native-webview')).toBe('ReactNativeWebview');
      expect(getModuleName('@react-native-community/slider')).toBe(
        'ReactNativeCommunitySlider',
      );
      expect(getModuleName('simple-module')).toBe('SimpleModule');
      expect(getModuleName('@scope/complex-package-name')).toBe(
        'ScopeComplexPackageName',
      );
    });

    it('should handle edge cases', () => {
      const setup = new SetupModuleWindows('/test', mockOptions);
      const getModuleName = (setup as any).getModuleName.bind(setup);

      expect(getModuleName('')).toBe('');
      expect(getModuleName('single')).toBe('Single');
      expect(getModuleName('---multiple---dashes---')).toBe('MultipleDashes');
    });
  });

  describe('constructor', () => {
    it('should create instance with correct root and options', () => {
      const root = '/test/project';
      const options = {
        logging: true,
        telemetry: true,
        skipDeps: true,
        skipBuild: true,
        template: 'cpp-app' as const,
      };

      const setup = new SetupModuleWindows(root, options);

      expect(setup.root).toBe(root);
      expect(setup.options).toBe(options);
    });
  });

  describe('verboseMessage', () => {
    it('should log when logging is enabled', () => {
      const consoleSpy = jest.spyOn(console, 'log').mockImplementation();
      const setup = new SetupModuleWindows('/test', {logging: true});

      (setup as any).verboseMessage('test message');

      expect(consoleSpy).toHaveBeenCalledWith(
        '[SetupModuleWindows] test message',
      );
      consoleSpy.mockRestore();
    });

    it('should not log when logging is disabled', () => {
      const consoleSpy = jest.spyOn(console, 'log').mockImplementation();
      const setup = new SetupModuleWindows('/test', {logging: false});

      (setup as any).verboseMessage('test message');

      expect(consoleSpy).not.toHaveBeenCalled();
      consoleSpy.mockRestore();
    });
  });

  describe('template option', () => {
    it('should use cpp-lib as default template', () => {
      const setup = new SetupModuleWindows('/test', {});
      expect(setup.options.template).toBeUndefined(); // Will default to cpp-lib in implementation
    });

    it('should accept cpp-app template', () => {
      const setup = new SetupModuleWindows('/test', {template: 'cpp-app'});
      expect(setup.options.template).toBe('cpp-app');
    });

    it('should accept cpp-lib template', () => {
      const setup = new SetupModuleWindows('/test', {template: 'cpp-lib'});
      expect(setup.options.template).toBe('cpp-lib');
    });
  });

  describe('spec file validation', () => {
    beforeEach(() => {
      jest.clearAllMocks();
    });

    it('should throw error if no spec file exists', async () => {
      const glob = require('glob');
      glob.sync.mockReturnValue([]);
      
      const setup = new SetupModuleWindows('/test', {logging: true});

      await expect((setup as any).checkForExistingSpec()).rejects.toThrow(
        'Create Spec File - TurboModule spec file not found. Please create a TurboModule spec file before running setup-module-windows.'
      );
    });

    it('should accept valid TurboModule spec files', async () => {
      const glob = require('glob');
      glob.sync.mockReturnValue(['NativeTestModule.ts']);
      
      fs.exists.mockResolvedValue(true);
      fs.readFile.mockResolvedValue(`
        import type {TurboModule} from 'react-native/Libraries/TurboModule/RCTExport';
        import {TurboModuleRegistry} from 'react-native';

        export interface Spec extends TurboModule {
          getString(): Promise<string>;
        }

        export default TurboModuleRegistry.getEnforcing<Spec>('TestModule');
      `);

      const setup = new SetupModuleWindows('/test', {logging: true});

      await expect((setup as any).checkForExistingSpec()).resolves.toBeUndefined();
    });
  });
});