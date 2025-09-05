/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import {SetupModuleWindows} from '../setupModuleWindows';
import type {SetupModuleWindowsOptions} from '../setupModuleWindowsOptions';

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
  };

  beforeEach(() => {
    jest.clearAllMocks();
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

      const setup = new SetupModuleWindows(root, options);

      expect(setup.root).toBe(root);
      expect(setup.options).toBe(options);
    });
  });

  describe('getFinalModuleName', () => {
    it('should return correct module name from package name', async () => {
      const fs = require('@react-native-windows/fs');
      fs.readFile.mockResolvedValue(
        JSON.stringify({name: 'react-native-webview'}),
      );
      fs.exists.mockResolvedValue(true);

      const setup = new SetupModuleWindows('/test', mockOptions);
      const moduleName = await setup.getFinalModuleName();

      expect(moduleName).toBe('ReactNativeWebview');
    });

    it('should handle scoped package names', async () => {
      const fs = require('@react-native-windows/fs');
      fs.readFile.mockResolvedValue(
        JSON.stringify({name: '@react-native-community/slider'}),
      );
      fs.exists.mockResolvedValue(true);

      const setup = new SetupModuleWindows('/test', mockOptions);
      const moduleName = await setup.getFinalModuleName();

      expect(moduleName).toBe('ReactNativeCommunitySlider');
    });

    it('should return default name when package.json is invalid', async () => {
      const fs = require('@react-native-windows/fs');
      fs.readFile.mockRejectedValue(new Error('File not found'));

      const setup = new SetupModuleWindows('/test', mockOptions);
      const moduleName = await setup.getFinalModuleName();

      expect(moduleName).toBe('SampleModule');
    });
  });

  describe('getActualProjectPaths', () => {
    it('should return correct project paths', () => {
      const setup = new SetupModuleWindows('/test', mockOptions);
      // Set internal actualProjectPath for testing
      (setup as any).actualProjectPath = 'windows/TestModule/TestModule';

      const paths = setup.getActualProjectPaths();

      expect(paths.headerPath).toBe('windows/TestModule/TestModule.h');
      expect(paths.cppPath).toBe('windows/TestModule/TestModule.cpp');
    });

    it('should return fallback paths when actualProjectPath is not set', () => {
      const setup = new SetupModuleWindows('/test', mockOptions);

      const paths = setup.getActualProjectPaths();

      expect(paths.headerPath).toBe('windows/SampleModule/SampleModule.h');
      expect(paths.cppPath).toBe('windows/SampleModule/SampleModule.cpp');
    });
  });

  describe('Hello World spec generation', () => {
    it('should generate correct Hello World spec content', () => {
      const setup = new SetupModuleWindows('/test', mockOptions);
      const generateMethod = (
        setup as any
      ).generateHelloWorldSpecFileContent.bind(setup);

      const content = generateMethod('TestModule');

      expect(content).toContain('export interface Spec extends TurboModule');
      expect(content).toContain('sayHello(name: string): Promise<string>');
      expect(content).toContain(
        'multiply(a: number, b: number): Promise<number>',
      );
      expect(content).toContain('getRandomNumber(): Promise<number>');
      expect(content).toContain('isModuleReady(): Promise<boolean>');
      expect(content).toContain(
        "TurboModuleRegistry.getEnforcing<Spec>('TestModule')",
      );
      expect(content).toContain('// Hello World method');
      expect(content).toContain('// Reference multiply function');
    });
  });

  describe('type mapping', () => {
    it('should map TypeScript types to C++ types correctly', () => {
      const setup = new SetupModuleWindows('/test', mockOptions);
      const mapTsToCpp = (setup as any).mapTSToCppType.bind(setup);

      expect(mapTsToCpp('string')).toBe('std::string');
      expect(mapTsToCpp('number')).toBe('double');
      expect(mapTsToCpp('boolean')).toBe('bool');
      expect(mapTsToCpp('object')).toBe('React::JSValue');
      expect(mapTsToCpp('any')).toBe('React::JSValue');
    });

    it('should map TypeScript return types to C++ types correctly', () => {
      const setup = new SetupModuleWindows('/test', mockOptions);
      const mapTsReturnToCpp = (setup as any).mapTSReturnTypeToCpp.bind(setup);

      expect(mapTsReturnToCpp('Promise<string>')).toBe('std::string');
      expect(mapTsReturnToCpp('Promise<number>')).toBe('double');
      expect(mapTsReturnToCpp('Promise<boolean>')).toBe('bool');
      expect(mapTsReturnToCpp('string')).toBe('std::string');
    });
  });

  describe('method implementation generation', () => {
    it('should generate correct Hello World implementation', () => {
      const setup = new SetupModuleWindows('/test', mockOptions);
      const generateImpl = (setup as any).generateMethodImplementation.bind(
        setup,
      );

      const helloImpl = generateImpl({
        name: 'sayHello',
        returnType: 'Promise<string>',
        parameters: [{name: 'name', type: 'string'}],
      });

      expect(helloImpl).toContain('std::string greeting = "Hello, " + name');
      expect(helloImpl).toContain('promise.Resolve(greeting)');
      expect(helloImpl).toContain(
        'promise.Reject(React::ReactError{e.what()})',
      );
    });

    it('should generate correct multiply implementation', () => {
      const setup = new SetupModuleWindows('/test', mockOptions);
      const generateImpl = (setup as any).generateMethodImplementation.bind(
        setup,
      );

      const multiplyImpl = generateImpl({
        name: 'multiply',
        returnType: 'Promise<number>',
        parameters: [
          {name: 'a', type: 'number'},
          {name: 'b', type: 'number'},
        ],
      });

      expect(multiplyImpl).toContain('double result = a * b');
      expect(multiplyImpl).toContain('promise.Resolve(result)');
      expect(multiplyImpl).toContain('// Reference multiply function');
    });
  });
});
