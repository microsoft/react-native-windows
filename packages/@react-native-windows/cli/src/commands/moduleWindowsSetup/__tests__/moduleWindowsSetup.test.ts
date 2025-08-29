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
      expect(getModuleName('@react-native-community/slider')).toBe(
        'ReactNativeCommunitySlider',
      );
      expect(getModuleName('simple-module')).toBe('SimpleModule');
      expect(getModuleName('@scope/complex-package-name')).toBe(
        'ScopeComplexPackageName',
      );
    });

    it('should handle edge cases', () => {
      const setup = new ModuleWindowsSetup('/test', mockOptions);
      const getModuleName = (setup as any).getModuleName.bind(setup);

      expect(getModuleName('')).toBe('');
      expect(getModuleName('single')).toBe('Single');
      expect(getModuleName('---multiple---dashes---')).toBe('MultipleDashes');
    });
  });

  describe('extractMethodsFromSpecInterface', () => {
    it('should parse method signatures from TypeScript interface', () => {
      const setup = new ModuleWindowsSetup('/test', mockOptions);
      const extractMethods = (
        setup as any
      ).extractMethodsFromSpecInterface.bind(setup);

      const specContent = `
export interface Spec extends TurboModule {
  getString(value: string): Promise<string>;
  getNumber(count: number): Promise<number>;
  getBool(): Promise<boolean>;
  syncMethod(data: object): void;
}`;

      const methods = extractMethods(specContent);

      expect(methods).toHaveLength(4);
      expect(methods[0]).toEqual({
        name: 'getString',
        returnType: 'Promise<string>',
        parameters: [{name: 'value', type: 'string'}],
      });
      expect(methods[1]).toEqual({
        name: 'getNumber',
        returnType: 'Promise<number>',
        parameters: [{name: 'count', type: 'number'}],
      });
      expect(methods[2]).toEqual({
        name: 'getBool',
        returnType: 'Promise<boolean>',
        parameters: [],
      });
      expect(methods[3]).toEqual({
        name: 'syncMethod',
        returnType: 'void',
        parameters: [{name: 'data', type: 'object'}],
      });
    });

    it('should handle complex parameter types', () => {
      const setup = new ModuleWindowsSetup('/test', mockOptions);
      const extractMethods = (
        setup as any
      ).extractMethodsFromSpecInterface.bind(setup);

      const specContent = `
export interface Spec extends TurboModule {
  complexMethod(
    config: {
      url: string;
      timeout?: number;
    },
    callback: (result: any) => void
  ): Promise<any>;
}`;

      const methods = extractMethods(specContent);

      expect(methods).toHaveLength(1);
      expect(methods[0].name).toBe('complexMethod');
      expect(methods[0].parameters).toHaveLength(2);
    });
  });

  describe('mapTSToCppType', () => {
    it('should map TypeScript types to C++ types correctly', () => {
      const setup = new ModuleWindowsSetup('/test', mockOptions);
      const mapTsToCpp = (setup as any).mapTSToCppType.bind(setup);

      expect(mapTsToCpp('string')).toBe('std::string');
      expect(mapTsToCpp('number')).toBe('double');
      expect(mapTsToCpp('boolean')).toBe('bool');
      expect(mapTsToCpp('object')).toBe('React::JSValue');
      expect(mapTsToCpp('any')).toBe('React::JSValue');
      expect(mapTsToCpp('string[]')).toBe('std::vector<std::string>');
      expect(mapTsToCpp('number[]')).toBe('std::vector<double>');
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
      };

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

      expect(consoleSpy).toHaveBeenCalledWith(
        '[ModuleWindowsSetup] test message',
      );
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

  describe('removeOldArchWindowsDirectory', () => {
    const fs = require('@react-native-windows/fs');
    const glob = require('glob');

    beforeEach(() => {
      jest.clearAllMocks();
    });

    it('should skip removal if windows directory does not exist', async () => {
      fs.exists.mockResolvedValue(false);
      const setup = new ModuleWindowsSetup('/test', {logging: true});

      await (setup as any).removeOldArchWindowsDirectory();

      expect(fs.exists).toHaveBeenCalledWith('/test/windows');
      expect(glob.sync).not.toHaveBeenCalled();
      expect(fs.rmdir).not.toHaveBeenCalled();
    });

    it('should keep new architecture windows directory', async () => {
      fs.exists.mockResolvedValue(true);
      glob.sync.mockReturnValue([]); // No old arch files found
      const setup = new ModuleWindowsSetup('/test', {logging: true});
      const consoleSpy = jest.spyOn(console, 'log').mockImplementation();

      await (setup as any).removeOldArchWindowsDirectory();

      expect(fs.exists).toHaveBeenCalledWith('/test/windows');
      expect(glob.sync).toHaveBeenCalled();
      expect(fs.rmdir).not.toHaveBeenCalled();
      expect(consoleSpy).toHaveBeenCalledWith(
        expect.stringContaining(
          'Windows directory appears to be new architecture',
        ),
      );
      consoleSpy.mockRestore();
    });

    it('should remove old architecture windows directory', async () => {
      fs.exists.mockResolvedValue(true);
      glob.sync.mockReturnValueOnce(['ReactNativeModule.cpp']); // Found old arch file
      fs.rmdir.mockResolvedValue(undefined);
      const setup = new ModuleWindowsSetup('/test', {logging: true});
      const consoleSpy = jest.spyOn(console, 'log').mockImplementation();

      await (setup as any).removeOldArchWindowsDirectory();

      expect(fs.exists).toHaveBeenCalledWith('/test/windows');
      expect(glob.sync).toHaveBeenCalled();
      expect(fs.rmdir).toHaveBeenCalledWith('/test/windows', {recursive: true});
      expect(consoleSpy).toHaveBeenCalledWith(
        expect.stringContaining('Removing old architecture windows directory'),
      );
      expect(consoleSpy).toHaveBeenCalledWith(
        expect.stringContaining(
          'Old architecture windows directory removed successfully',
        ),
      );
      consoleSpy.mockRestore();
    });

    it('should handle removal errors gracefully', async () => {
      fs.exists.mockResolvedValue(true);
      glob.sync.mockReturnValueOnce(['pch.h']); // Found old arch file
      fs.rmdir.mockRejectedValue(new Error('Permission denied'));
      const setup = new ModuleWindowsSetup('/test', {logging: true});
      const consoleSpy = jest.spyOn(console, 'log').mockImplementation();

      await (setup as any).removeOldArchWindowsDirectory();

      expect(fs.rmdir).toHaveBeenCalledWith('/test/windows', {recursive: true});
      expect(consoleSpy).toHaveBeenCalledWith(
        expect.stringContaining(
          'Warning: Could not remove old windows directory',
        ),
      );
      consoleSpy.mockRestore();
    });
  });
});
