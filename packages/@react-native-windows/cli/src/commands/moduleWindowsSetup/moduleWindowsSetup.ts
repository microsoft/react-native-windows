/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import path from 'path';
import chalk from 'chalk';
import {performance} from 'perf_hooks';
import {Ora} from 'ora';
import {execSync} from 'child_process';
import glob from 'glob';

import fs from '@react-native-windows/fs';
import type {Command, Config} from '@react-native-community/cli-types';
import {Telemetry, CodedError} from '@react-native-windows/telemetry';

import {
  newSpinner,
  setExitProcessWithError,
} from '../../utils/commandWithProgress';
import {
  getDefaultOptions,
  startTelemetrySession,
  endTelemetrySession,
} from '../../utils/telemetryHelpers';
import {initWindowsInternal} from '../initWindows/initWindows';
import {codegenWindowsInternal} from '../codegenWindows/codegenWindows';
import type {ModuleWindowsSetupOptions} from './moduleWindowsSetupOptions';
import {moduleWindowsSetupOptions} from './moduleWindowsSetupOptions';

interface Parameter {
  name: string;
  type: string;
}

interface MethodSignature {
  name: string;
  returnType: string;
  parameters: Parameter[];
}

export class ModuleWindowsSetup {
  private actualModuleName?: string;
  private discoveredSpecFiles: string[] = [];
  public root: string;
  public options: ModuleWindowsSetupOptions;

  constructor(root: string, options: ModuleWindowsSetupOptions) {
    this.root = root;
    this.options = options;
  }

  private async validateEnvironment(): Promise<void> {
    this.verboseMessage('Validating environment...');

    // Check if package.json exists
    const packageJsonPath = path.join(this.root, 'package.json');
    if (!(await fs.exists(packageJsonPath))) {
      throw new CodedError(
        'NoPackageJson',
        'No package.json found. Make sure you are in a React Native project directory.',
      );
    }

    // Check if it's a valid npm package
    try {
      const pkgJson = JSON.parse(await fs.readFile(packageJsonPath, 'utf8'));
      if (!pkgJson.name) {
        throw new CodedError(
          'NoProjectName',
          'package.json must have a "name" field.',
        );
      }
      this.verboseMessage(`Project name: ${pkgJson.name}`);
    } catch (error: any) {
      if (error.code === 'NoProjectName') {
        throw error;
      }
      throw new CodedError('NoPackageJson', 'package.json is not valid JSON.');
    }

    // Check if yarn is available
    try {
      execSync('yarn --version', {stdio: 'ignore'});
      this.verboseMessage('Yarn found');
    } catch {
      throw new CodedError(
        'Unknown',
        'Yarn is required but not found. Please install Yarn first.',
      );
    }
  }

  private async extractModuleNameFromExistingSpec(
    specFilePath: string,
  ): Promise<void> {
    try {
      const fullPath = path.join(this.root, specFilePath);
      const content = await fs.readFile(fullPath, 'utf8');

      // Extract the module name from TurboModuleRegistry.getEnforcing<Spec>('ModuleName')
      const exportMatch = content.match(
        /TurboModuleRegistry\.getEnforcing<Spec>\(['"`]([^'"`]+)['"`]\)/,
      );
      if (exportMatch) {
        this.actualModuleName = exportMatch[1];
        this.verboseMessage(
          `Extracted actual module name: ${this.actualModuleName}`,
        );
      } else {
        this.verboseMessage(
          'Could not extract module name from spec file, using package name conversion',
        );
      }
    } catch (error) {
      this.verboseMessage(`Error reading spec file: ${error}`);
    }
  }

  private getActualModuleName(packageName: string): string {
    // If we extracted the actual module name from an existing spec, use that
    if (this.actualModuleName) {
      return this.actualModuleName;
    }

    // Otherwise, fall back to the package name conversion
    return this.getModuleName(packageName);
  }

  public async getFinalModuleName(): Promise<string> {
    try {
      const packageJsonPath = path.join(this.root, 'package.json');
      const pkgJson = JSON.parse(await fs.readFile(packageJsonPath, 'utf8'));
      return this.getActualModuleName(pkgJson.name || 'SampleModule');
    } catch {
      return 'SampleModule';
    }
  }

  private verboseMessage(message: any) {
    if (this.options.logging) {
      console.log(`[ModuleWindowsSetup] ${message}`);
    }
  }

  private async removeDirectoryRecursively(dirPath: string): Promise<void> {
    try {
      const stats = await fs.stat(dirPath);
      if (stats.isDirectory()) {
        const files = await fs.readdir(dirPath);
        await Promise.all(
          files.map(async file => {
            const filePath = path.join(dirPath, file);
            await this.removeDirectoryRecursively(filePath);
          }),
        );
        await fs.rmdir(dirPath);
      } else {
        await fs.unlink(dirPath);
      }
    } catch (error: any) {
      // Ignore errors if file/directory doesn't exist
      if (error.code !== 'ENOENT') {
        throw error;
      }
    }
  }

  private getModuleName(packageName: string): string {
    // Convert package name to PascalCase module name
    // e.g., "react-native-webview" -> "ReactNativeWebview"
    // e.g., "@react-native-community/slider" -> "ReactNativeCommunitySlider"
    return packageName
      .replace(/[@/-]/g, ' ')
      .split(' ')
      .filter(word => word.length > 0)
      .map(word => word.charAt(0).toUpperCase() + word.slice(1).toLowerCase())
      .join('');
  }

  private async checkAndCreateSpecFile(): Promise<void> {
    this.verboseMessage('Checking for TurboModule spec file...');

    // Look for spec files in common locations, excluding node_modules
    const specPatterns = [
      'Native*.[jt]s',
      'src/**/Native*.[jt]s',
      'lib/**/Native*.[jt]s',
      'js/**/Native*.[jt]s',
      'ts/**/Native*.[jt]s',
    ];

    const specFiles: string[] = [];
    for (const pattern of specPatterns) {
      const matches = glob.sync(pattern, {
        cwd: this.root,
        ignore: ['**/node_modules/**', '**/build/**', '**/dist/**'],
      });
      specFiles.push(...matches);
    }

    // Remove duplicates and filter for actual TurboModule specs
    const uniqueSpecFiles = Array.from(new Set(specFiles));
    const validSpecFiles = await this.filterValidSpecFiles(uniqueSpecFiles);

    if (validSpecFiles.length === 0) {
      this.verboseMessage(
        'No valid TurboModule spec file found, analyzing existing APIs...',
      );
      await this.analyzeAndCreateSpecFile();
    } else {
      this.verboseMessage(
        `Found valid spec file(s): ${validSpecFiles.join(', ')}`,
      );
      // Store the discovered spec files for later use
      this.discoveredSpecFiles = validSpecFiles;
      // Extract the actual module name from the existing spec file
      await this.extractModuleNameFromExistingSpec(validSpecFiles[0]);
    }
  }

  private async filterValidSpecFiles(specFiles: string[]): Promise<string[]> {
    const validFiles: string[] = [];

    for (const file of specFiles) {
      try {
        const filePath = path.join(this.root, file);
        if (await fs.exists(filePath)) {
          const content = await fs.readFile(filePath, 'utf8');

          // Check if it's a valid TurboModule spec file
          if (this.isValidTurboModuleSpec(content)) {
            validFiles.push(file);
          }
        }
      } catch (error) {
        this.verboseMessage(`Could not read spec file ${file}: ${error}`);
      }
    }

    return validFiles;
  }

  private isValidTurboModuleSpec(content: string): boolean {
    // Check for TurboModule indicators
    return (
      content.includes('TurboModule') &&
      (content.includes('export interface Spec') ||
        content.includes('extends TurboModule') ||
        content.includes('TurboModuleRegistry'))
    );
  }

  private async analyzeAndCreateSpecFile(): Promise<void> {
    const pkgJson = JSON.parse(
      await fs.readFile(path.join(this.root, 'package.json'), 'utf8'),
    );
    const moduleName = this.getActualModuleName(pkgJson.name || 'SampleModule');

    // Try to analyze existing API from multiple sources
    const apiMethods = await this.discoverApiMethods();

    const specContent = this.generateSpecFileContent(moduleName, apiMethods);
    const specPath = path.join(this.root, `Native${moduleName}.ts`);
    await fs.writeFile(specPath, specContent);
    this.verboseMessage(`Created spec file: ${specPath}`);
  }

  private async discoverApiMethods(): Promise<MethodSignature[]> {
    const methods: MethodSignature[] = [];

    // 1. Check for existing JavaScript/TypeScript API files
    methods.push(...(await this.analyzeJavaScriptApi()));

    // 2. Check Android native implementation for reference
    methods.push(...(await this.analyzeAndroidApi()));

    // 3. Check iOS native implementation for reference
    methods.push(...(await this.analyzeIosApi()));

    // 4. Check README for documented API
    methods.push(...(await this.analyzeReadmeApi()));

    // Deduplicate methods by name
    const uniqueMethods = methods.reduce((acc, method) => {
      if (!acc.find(m => m.name === method.name)) {
        acc.push(method);
      }
      return acc;
    }, [] as MethodSignature[]);

    this.verboseMessage(
      `Discovered ${uniqueMethods.length} API methods from various sources`,
    );
    return uniqueMethods;
  }

  private async analyzeJavaScriptApi(): Promise<MethodSignature[]> {
    const methods: MethodSignature[] = [];

    try {
      // Look for index.js, index.ts, or main entry point
      const packageJson = JSON.parse(
        await fs.readFile(path.join(this.root, 'package.json'), 'utf8'),
      );
      const mainFile = packageJson.main || 'index.js';

      const possibleFiles = [
        mainFile,
        'index.js',
        'index.ts',
        'src/index.js',
        'src/index.ts',
      ];

      for (const file of possibleFiles) {
        const filePath = path.join(this.root, file);
        if (await fs.exists(filePath)) {
          const content = await fs.readFile(filePath, 'utf8');
          methods.push(...this.parseJavaScriptMethods(content));
          this.verboseMessage(`Analyzed JavaScript API from ${file}`);
          break;
        }
      }
    } catch (error) {
      this.verboseMessage(`Could not analyze JavaScript API: ${error}`);
    }

    return methods;
  }

  private async analyzeAndroidApi(): Promise<MethodSignature[]> {
    const methods: MethodSignature[] = [];

    try {
      const androidDir = path.join(this.root, 'android');
      if (await fs.exists(androidDir)) {
        const javaFiles = glob.sync('**/*.java', {cwd: androidDir});
        const kotlinFiles = glob.sync('**/*.kt', {cwd: androidDir});

        for (const file of [...javaFiles, ...kotlinFiles]) {
          const content = await fs.readFile(
            path.join(androidDir, file),
            'utf8',
          );
          if (content.includes('@ReactMethod')) {
            methods.push(...this.parseAndroidMethods(content));
            this.verboseMessage(`Analyzed Android API from ${file}`);
          }
        }
      }
    } catch (error) {
      this.verboseMessage(`Could not analyze Android API: ${error}`);
    }

    return methods;
  }

  private async analyzeIosApi(): Promise<MethodSignature[]> {
    const methods: MethodSignature[] = [];

    try {
      const iosDir = path.join(this.root, 'ios');
      if (await fs.exists(iosDir)) {
        const objcFiles = glob.sync('**/*.{m,mm}', {cwd: iosDir});

        for (const file of objcFiles) {
          const content = await fs.readFile(path.join(iosDir, file), 'utf8');
          if (content.includes('RCT_EXPORT_METHOD')) {
            methods.push(...this.parseIosMethods(content));
            this.verboseMessage(`Analyzed iOS API from ${file}`);
          }
        }
      }
    } catch (error) {
      this.verboseMessage(`Could not analyze iOS API: ${error}`);
    }

    return methods;
  }

  private async analyzeReadmeApi(): Promise<MethodSignature[]> {
    const methods: MethodSignature[] = [];

    try {
      const readmeFiles = ['README.md', 'readme.md', 'README.txt'];
      for (const file of readmeFiles) {
        const filePath = path.join(this.root, file);
        if (await fs.exists(filePath)) {
          const content = await fs.readFile(filePath, 'utf8');
          methods.push(...this.parseReadmeMethods(content));
          this.verboseMessage(`Analyzed API documentation from ${file}`);
          break;
        }
      }
    } catch (error) {
      this.verboseMessage(`Could not analyze README API: ${error}`);
    }

    return methods;
  }

  private parseJavaScriptMethods(content: string): MethodSignature[] {
    const methods: MethodSignature[] = [];

    // Look for method exports and function definitions
    const patterns = [
      /export\s+(?:const|function)\s+(\w+)\s*[:=]\s*(?:async\s+)?\([^)]*\)(?:\s*:\s*([^{;]+))?/g,
      /(\w+)\s*:\s*(?:async\s+)?\([^)]*\)(?:\s*=>\s*([^,}]+))?/g,
      /function\s+(\w+)\s*\([^)]*\)(?:\s*:\s*([^{]+))?/g,
    ];

    patterns.forEach(pattern => {
      let match;
      while ((match = pattern.exec(content)) !== null) {
        methods.push({
          name: match[1],
          returnType: this.parseReturnType(match[2] || 'void'),
          parameters: this.parseParameters(match[0]),
        });
      }
    });

    return methods;
  }

  private parseAndroidMethods(content: string): MethodSignature[] {
    const methods: MethodSignature[] = [];

    // Look for @ReactMethod annotations
    const reactMethodPattern =
      /@ReactMethod[\s\S]*?(?:public|private)\s+(\w+)\s+(\w+)\s*\([^)]*\)/g;
    let match;

    while ((match = reactMethodPattern.exec(content)) !== null) {
      methods.push({
        name: match[2],
        returnType: this.mapJavaTypeToTS(match[1]),
        parameters: [],
      });
    }

    return methods;
  }

  private parseIosMethods(content: string): MethodSignature[] {
    const methods: MethodSignature[] = [];

    // Look for RCT_EXPORT_METHOD
    const exportMethodPattern = /RCT_EXPORT_METHOD\s*\(\s*(\w+)/g;
    let match;

    while ((match = exportMethodPattern.exec(content)) !== null) {
      methods.push({
        name: match[1],
        returnType: 'void',
        parameters: [],
      });
    }

    return methods;
  }

  private parseReadmeMethods(content: string): MethodSignature[] {
    const methods: MethodSignature[] = [];

    // Look for method signatures in markdown code blocks
    const codeBlockPattern = /```[\w]*\n([\s\S]*?)\n```/g;
    let match;

    while ((match = codeBlockPattern.exec(content)) !== null) {
      const code = match[1];
      // Look for function-like patterns
      const functionPattern = /(\w+)\s*\([^)]*\)/g;
      let funcMatch;

      while ((funcMatch = functionPattern.exec(code)) !== null) {
        if (
          !funcMatch[1].includes('import') &&
          !funcMatch[1].includes('require')
        ) {
          methods.push({
            name: funcMatch[1],
            returnType: 'Promise<any>',
            parameters: [],
          });
        }
      }
    }

    return methods;
  }

  private parseReturnType(type: string): string {
    if (!type || type === 'void') return 'void';
    if (type.includes('Promise')) return type;
    return `Promise<${type}>`;
  }

  private parseParameters(methodSignature: string): Parameter[] {
    // Extract parameters from method signature
    const paramMatch = methodSignature.match(/\(([^)]*)\)/);
    if (!paramMatch) return [];

    const params = paramMatch[1]
      .split(',')
      .map(p => p.trim())
      .filter(p => p);
    return params.map(param => {
      const [name, type] = param.split(':').map(s => s.trim());
      return {
        name: name || 'param',
        type: type || 'any',
      };
    });
  }

  private mapJavaTypeToTS(javaType: string): string {
    const typeMap: {[key: string]: string} = {
      void: 'void',
      boolean: 'boolean',
      int: 'number',
      float: 'number',
      double: 'number',
      String: 'string',
      ReadableMap: 'object',
      ReadableArray: 'any[]',
    };

    return typeMap[javaType] || 'any';
  }

  private generateSpecFileContent(
    moduleName: string,
    methods: MethodSignature[],
  ): string {
    const methodSignatures = methods
      .map(method => {
        const params = method.parameters
          .map(p => `${p.name}: ${p.type}`)
          .join(', ');
        return `  ${method.name}(${params}): ${method.returnType};`;
      })
      .join('\n');

    const defaultMethods =
      methods.length === 0
        ? `  // Add your module methods here
  // Example:
  // getString(value: string): Promise<string>;
  // getNumber(value: number): Promise<number>;
  // getBoolean(value: boolean): Promise<boolean>;`
        : methodSignatures;

    return `/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import type {TurboModule} from 'react-native/Libraries/TurboModule/RCTExport';
import {TurboModuleRegistry} from 'react-native';

export interface Spec extends TurboModule {
${defaultMethods}
}

export default TurboModuleRegistry.getEnforcing<Spec>('${moduleName}');
`;
  }

  private async updatePackageJsonCodegen(): Promise<void> {
    this.verboseMessage(
      'Checking and updating package.json codegen configuration...',
    );

    const packageJsonPath = path.join(this.root, 'package.json');
    const pkgJson = JSON.parse(await fs.readFile(packageJsonPath, 'utf8'));

    if (!pkgJson.codegenConfig) {
      const moduleName = this.getActualModuleName(
        pkgJson.name || 'SampleModule',
      );

      pkgJson.codegenConfig = {
        name: moduleName,
        type: 'modules',
        jsSrcsDir: '.',
        windows: {
          namespace: `${moduleName}Specs`,
          outputDirectory: 'codegen',
          generators: ['modulesWindows'],
        },
      };

      await fs.writeFile(packageJsonPath, JSON.stringify(pkgJson, null, 2));
      this.verboseMessage(
        `Added codegenConfig to package.json with module name: ${moduleName}`,
      );
    } else {
      this.verboseMessage('codegenConfig already exists in package.json');
    }
  }

  private async upgradeDependencies(): Promise<void> {
    if (this.options.skipDeps) {
      this.verboseMessage('Skipping dependency upgrades');
      return;
    }

    // this.verboseMessage(
    //   'Upgrading React Native and React Native Windows to latest versions...',
    // );

    try {
      // Suppress deprecation warnings to clean up output
      const env = {
        ...process.env,
        NODE_NO_WARNINGS: '1',
      };
      // Get latest versions with timeout to avoid hanging
      const rnLatest = execSync('npm view react-native version', {
        encoding: 'utf8',
        timeout: 30000,
        env,
      }).trim();
      const rnwLatest = execSync('npm view react-native-windows version', {
        encoding: 'utf8',
        timeout: 30000,
        env,
      }).trim();

      this.verboseMessage(`Latest RN version: ${rnLatest}`);
      this.verboseMessage(`Latest RNW version: ${rnwLatest}`);

      // Update package.json
    //   const packageJsonPath = path.join(this.root, 'package.json');
    //   const pkgJson = JSON.parse(await fs.readFile(packageJsonPath, 'utf8'));

    //   if (!pkgJson.peerDependencies) {
    //     pkgJson.peerDependencies = {};
    //   }
    //   if (!pkgJson.devDependencies) {
    //     pkgJson.devDependencies = {};
    //   }

    //   pkgJson.peerDependencies['react-native'] = `^${rnLatest}`;
    //   pkgJson.devDependencies['react-native-windows'] = `^${rnwLatest}`;

    //   await fs.writeFile(packageJsonPath, JSON.stringify(pkgJson, null, 2));
    //   this.verboseMessage('Updated dependency versions in package.json');

    //   // Install updated dependencies with timeout
    //   execSync('yarn install', {
    //     cwd: this.root,
    //     stdio: 'inherit',
    //     timeout: 120000,
    //   });
    } catch (error: any) {
      this.verboseMessage(
        `Warning: Could not upgrade dependencies: ${error.message}`,
      );
      // Don't fail the entire process if dependency upgrade fails
    }
  }

  private async handleExistingWindowsDirectory(): Promise<void> {
    const windowsDir = path.join(this.root, 'windows');
    const windowsOldDir = path.join(this.root, 'windows_old');

    if (await fs.exists(windowsDir)) {
      this.verboseMessage(
        'Found existing windows directory, renaming to windows_old...',
      );

      try {
        // If windows_old already exists, remove it first
        if (await fs.exists(windowsOldDir)) {
          this.verboseMessage('Removing existing windows_old directory...');
          await this.removeDirectoryRecursively(windowsOldDir);
        }

        // Rename windows to windows_old
        await fs.rename(windowsDir, windowsOldDir);
        this.verboseMessage(
          'Successfully renamed windows directory to windows_old',
        );
      } catch (error: any) {
        this.verboseMessage(
          `Warning: Could not rename windows directory: ${error.message}`,
        );
        // Don't fail the entire process if we can't rename the directory
        // The overwrite flag in init-windows should handle most conflicts
      }
    }
  }

  private async runInitWindows(config: Config): Promise<void> {
    this.verboseMessage('Running init-windows with cpp-lib template...');

    try {
      // Create a modified config with the correct root directory
      // The original config might point to an example directory instead of the package root
      const modifiedConfig: Config = {
        ...config,
        root: this.root, // Use our validated package root instead of config.root
      };

      await initWindowsInternal([], modifiedConfig, {
        template: 'cpp-lib',
        overwrite: true,
        logging: this.options.logging,
        telemetry: this.options.telemetry,
      });
      this.verboseMessage('init-windows completed successfully');
    } catch (error: any) {
      // Check if windows directory was created even with errors
      const windowsDir = path.join(this.root, 'windows');
      if (await fs.exists(windowsDir)) {
        this.verboseMessage(
          'Windows directory exists, continuing despite init-windows warnings',
        );
      } else {
        throw new CodedError(
          'Unknown',
          `Failed to run init-windows: ${error.message}`,
        );
      }
    }
  }

  private async runCodegenWindows(config: Config): Promise<void> {
    this.verboseMessage('Running codegen-windows...');

    try {
      // Create a modified config with the correct root directory
      // The original config might point to an example directory instead of the package root
      const modifiedConfig: Config = {
        ...config,
        root: this.root, // Use our validated package root instead of config.root
      };

      await codegenWindowsInternal([], modifiedConfig, {
        logging: this.options.logging,
        telemetry: this.options.telemetry,
      });
      this.verboseMessage('codegen-windows completed successfully');
    } catch (error: any) {
      // Check if codegen directory was created even with errors
      const codegenDir = path.join(this.root, 'codegen');
      if (await fs.exists(codegenDir)) {
        this.verboseMessage(
          'Codegen directory exists, continuing despite codegen-windows warnings',
        );
      } else {
        throw new CodedError(
          'InvalidCodegenConfig',
          `Failed to run codegen-windows: ${error.message}`,
        );
      }
    }
  }

  private async generateStubFiles(): Promise<void> {
    this.verboseMessage('Generating C++ stub files...');

    // Look for codegen directory in multiple possible locations
    let codegenDir = path.join(this.root, 'codegen');
    let codegenLocation = 'root';
    
    if (!(await fs.exists(codegenDir))) {
      // Try looking in windows directory
      const windowsDir = path.join(this.root, 'windows');
      if (await fs.exists(windowsDir)) {
        const windowsSubdirs = await fs.readdir(windowsDir);
        // Look for subdirectories that might contain codegen
        for (const subdir of windowsSubdirs) {
          const subdirPath = path.join(windowsDir, subdir);
          const stats = await fs.stat(subdirPath);
          if (stats.isDirectory()) {
            const possibleCodegenDir = path.join(subdirPath, 'codegen');
            if (await fs.exists(possibleCodegenDir)) {
              codegenDir = possibleCodegenDir;
              codegenLocation = `windows/${subdir}`;
              this.verboseMessage(`Found codegen directory at: ${codegenDir}`);
              break;
            }
          }
        }
      }
    } else {
      this.verboseMessage(`Found codegen directory at: ${codegenDir}`);
    }

    if (!(await fs.exists(codegenDir))) {
      this.verboseMessage(
        'No codegen directory found in root or windows subdirectories, skipping stub generation',
      );
      return;
    }

    const files = await fs.readdir(codegenDir);
    const specFiles = files.filter(file => file.endsWith('Spec.g.h'));

    if (specFiles.length === 0) {
      this.verboseMessage(
        'No Spec.g.h files found in codegen directory. This typically means the TurboModule spec file needs to be created or codegen needs to be run.',
      );
      return;
    }

    this.verboseMessage(`Found ${specFiles.length} codegen spec file(s) in ${codegenLocation}: ${specFiles.join(', ')}`);

    for (const specFile of specFiles) {
      const specName = specFile.replace('Spec.g.h', '');
      const windowsDir = path.join(this.root, 'windows');
      const headerPath = path.join(windowsDir, `${specName}.h`);
      const cppPath = path.join(windowsDir, `${specName}.cpp`);

      if (!(await fs.exists(windowsDir))) {
        await fs.mkdir(windowsDir, {recursive: true});
      }

      // Parse method signatures from codegen files first, then fallback to TypeScript spec files
      const methods = await this.parseSpecFileForMethods(specName, codegenDir);

      if (methods.length === 0) {
        this.verboseMessage(
          `No methods found for ${specName}. The generated files will contain empty stubs with TODO comments.`,
        );
      } else {
        this.verboseMessage(
          `Found ${methods.length} methods from ${specName}: ${methods.map(m => m.name).join(', ')}`,
        );
      }

      // Generate header file with parsed methods
      const headerContent = await this.generateHeaderStub(specName, methods);
      // Always write the header file to ensure it has the correct methods from the spec
      await fs.writeFile(headerPath, headerContent);
      this.verboseMessage(
        `Generated header stub: ${headerPath} with ${methods.length} methods`,
      );

      // Generate cpp file with parsed methods
      const cppContent = await this.generateCppStub(specName, methods);
      // Always write the cpp file to ensure it has the correct methods from the spec
      await fs.writeFile(cppPath, cppContent);
      this.verboseMessage(
        `Generated cpp stub: ${cppPath} with ${methods.length} methods`,
      );
    }
  }

  private async parseSpecFileForMethods(
    moduleName: string,
    codegenDir?: string,
  ): Promise<MethodSignature[]> {
    try {
      // First, try to read from codegen C++ header files
      const actualCodegenDir = codegenDir || path.join(this.root, 'codegen');
      if (await fs.exists(actualCodegenDir)) {
        const methods = await this.parseCodegenHeaderFiles(actualCodegenDir, moduleName);
        if (methods.length > 0) {
          this.verboseMessage(
            `Extracted ${methods.length} methods from codegen files: ${methods
              .map(m => m.name)
              .join(', ')}`,
          );
          return methods;
        }
      }

      // Fallback to TypeScript spec files if no codegen files found
      let specFiles = this.discoveredSpecFiles;

      // If no discovered spec files, try to find them again with broader patterns
      if (specFiles.length === 0) {
        this.verboseMessage(
          `Searching for spec files for module: ${moduleName}`,
        );

        // Try multiple patterns to find the spec file
        const patterns = [
          `**/Native${moduleName}.[jt]s`,
          `**/Native*${moduleName}*.[jt]s`,
          `**/Native*.[jt]s`,
          `src/**/Native*.[jt]s`,
          `lib/**/Native*.[jt]s`,
        ];

        for (const pattern of patterns) {
          const matches = glob.sync(pattern, {
            cwd: this.root,
            ignore: ['**/node_modules/**', '**/build/**', '**/dist/**'],
          });
          if (matches.length > 0) {
            specFiles = await this.filterValidSpecFiles(matches);
            if (specFiles.length > 0) {
              this.verboseMessage(
                `Found spec files with pattern "${pattern}": ${specFiles.join(
                  ', ',
                )}`,
              );
              break;
            }
          }
        }
      }

      if (specFiles.length > 0) {
        // Use the first valid spec file
        const specPath = path.join(this.root, specFiles[0]);
        this.verboseMessage(`Reading spec file: ${specPath}`);
        const specContent = await fs.readFile(specPath, 'utf8');

        // Parse method signatures from the Spec interface
        const methods = this.extractMethodsFromSpecInterface(specContent);
        if (methods.length > 0) {
          this.verboseMessage(
            `Extracted ${methods.length} methods from spec file: ${methods
              .map(m => m.name)
              .join(', ')}`,
          );
          return methods;
        }
      }

      // If no methods found from any source, return empty array
      this.verboseMessage(
        `No spec file found for ${moduleName}, no methods will be generated`,
      );
      return [];
    } catch (error) {
      this.verboseMessage(
        `Could not parse spec file for ${moduleName}: ${error}`,
      );
      return [];
    }
  }

  private async parseCodegenHeaderFiles(
    codegenDir: string, 
    moduleName: string
  ): Promise<MethodSignature[]> {
    try {
      this.verboseMessage(`Looking for codegen files in: ${codegenDir}`);
      
      const files = await fs.readdir(codegenDir);
      const specFiles = files.filter(file => file.endsWith('Spec.g.h'));
      
      this.verboseMessage(`Found codegen spec files: ${specFiles.join(', ')}`);
      
      for (const specFile of specFiles) {
        const specPath = path.join(codegenDir, specFile);
        const content = await fs.readFile(specPath, 'utf8');
        
        // Extract methods from the codegen C++ header
        const methods = this.extractMethodsFromCodegenHeader(content);
        if (methods.length > 0) {
          this.verboseMessage(`Parsed ${methods.length} methods from ${specFile}`);
          return methods;
        }
      }
      
      this.verboseMessage('No methods found in codegen files');
      return [];
    } catch (error) {
      this.verboseMessage(`Error parsing codegen files: ${error}`);
      return [];
    }
  }

  private extractMethodsFromCodegenHeader(content: string): MethodSignature[] {
    const methods: MethodSignature[] = [];

    // Parse from REACT_SHOW_METHOD_SPEC_ERRORS sections which contain the exact method signatures
    const errorSectionPattern = /REACT_SHOW_METHOD_SPEC_ERRORS\s*\(\s*\d+,\s*"([^"]+)",\s*"[^"]*REACT_METHOD\(([^)]+)\)\s+(?:static\s+)?void\s+(\w+)\s*\(([^)]*)\)[^"]*"/g;
    
    let match;
    while ((match = errorSectionPattern.exec(content)) !== null) {
      const methodName = match[1]; // Method name from first parameter
      const parameters = this.parseCodegenParameters(match[4]); // Parameters from method signature
      
      methods.push({
        name: methodName,
        returnType: 'void', // Codegen methods are typically void with callbacks or promises
        parameters: parameters,
      });
    }

    // Also try to parse from the methods tuple for additional methods
    if (methods.length === 0) {
      const methodsTuplePattern = /Method<([^>]+)>\{\s*(\d+),\s*L"([^"]+)"\s*\}/g;
      
      while ((match = methodsTuplePattern.exec(content)) !== null) {
        const signature = match[1]; // Method signature type
        const methodName = match[3]; // Method name
        const parameters = this.parseCodegenSignature(signature);
        
        methods.push({
          name: methodName,
          returnType: 'void',
          parameters: parameters,
        });
      }
    }

    // Try parsing directly from C++ method declarations in the header
    if (methods.length === 0) {
      this.verboseMessage('Trying to parse C++ method declarations directly...');
      
      // Look for virtual method declarations like:
      // virtual void MethodName(parameters) = 0;
      const virtualMethodPattern = /virtual\s+(\w+(?:\s*\*)?)\s+(\w+)\s*\(([^)]*)\)\s*(?:const\s*)?=\s*0\s*;/g;
      
      while ((match = virtualMethodPattern.exec(content)) !== null) {
        const returnType = match[1].trim();
        const methodName = match[2];
        const paramString = match[3];
        const parameters = this.parseCodegenParameters(paramString);
        
        methods.push({
          name: methodName,
          returnType: returnType === 'void' ? 'void' : returnType,
          parameters: parameters,
        });
      }
    }

    // Try parsing from JSI method declarations
    if (methods.length === 0) {
      this.verboseMessage('Trying to parse JSI method declarations...');
      
      // Look for JSI-style method declarations
      const jsiMethodPattern = /static\s+jsi::Value\s+__hostFunction_(\w+)\s*\([^)]*\)\s*{/g;
      
      while ((match = jsiMethodPattern.exec(content)) !== null) {
        const methodName = match[1];
        
        methods.push({
          name: methodName,
          returnType: 'void',
          parameters: [], // JSI methods will be parsed separately for parameters
        });
      }
    }

    // Try parsing from struct member methods
    if (methods.length === 0) {
      this.verboseMessage('Trying to parse struct member methods...');
      
      // Look for struct methods like:
      // bool MessagingEnabled() const;
      // void MessagingEnabled(bool enabled);
      const structMethodPattern = /^\s*(?:virtual\s+)?(\w+(?:\s*&)?)\s+(\w+)\s*\(([^)]*)\)\s*(?:const\s*)?(?:noexcept\s*)?(?:=\s*0\s*)?;/gm;
      
      while ((match = structMethodPattern.exec(content)) !== null) {
        const returnType = match[1].trim();
        const methodName = match[2];
        const paramString = match[3];
        const parameters = this.parseCodegenParameters(paramString);
        
        // Skip common non-API methods
        if (!methodName.startsWith('~') && 
            !methodName.includes('Destructor') && 
            !methodName.includes('Constructor') &&
            methodName !== 'getContext' &&
            methodName !== 'invalidate') {
          
          methods.push({
            name: methodName,
            returnType: returnType === 'void' ? 'void' : returnType,
            parameters: parameters,
          });
        }
      }
    }

    this.verboseMessage(`Extracted ${methods.length} methods from codegen header using multiple parsing strategies`);
    return methods;
  }

  private parseCodegenParameters(paramString: string): Parameter[] {
    if (!paramString || paramString.trim() === '') {
      return [];
    }

    const params: Parameter[] = [];
    
    // Split parameters carefully, handling nested templates like std::function<void(bool)>
    const cleanParamString = paramString.trim();
    let current = '';
    let depth = 0;
    let inString = false;
    
    for (let i = 0; i < cleanParamString.length; i++) {
      const char = cleanParamString[i];
      
      if (char === '"' && cleanParamString[i-1] !== '\\') {
        inString = !inString;
      } else if (!inString) {
        if (char === '<' || char === '(') {
          depth++;
        } else if (char === '>' || char === ')') {
          depth--;
        } else if (char === ',' && depth === 0) {
          if (current.trim()) {
            params.push(this.parseCodegenParameter(current.trim()));
          }
          current = '';
          continue;
        }
      }
      
      current += char;
    }
    
    if (current.trim()) {
      params.push(this.parseCodegenParameter(current.trim()));
    }
    
    return params;
  }

  private parseCodegenParameter(param: string): Parameter {
    // Handle common codegen parameter patterns
    param = param.trim();
    
    // std::function<void(type)> const & callback -> callback parameter
    if (param.includes('std::function')) {
      if (param.includes('onSuccess') || param.includes('callback')) {
        return { name: 'callback', type: 'function' };
      } else if (param.includes('onError')) {
        return { name: 'onError', type: 'function' };
      } else {
        return { name: 'callback', type: 'function' };
      }
    }
    
    // Extract parameter name from the end
    const parts = param.split(/\s+/);
    let name = parts[parts.length - 1].replace(/[&*]/g, ''); // Remove references/pointers
    
    // Handle winrt types and const references
    if (name.includes('const')) {
      name = parts[parts.length - 2] || 'param';
    }
    
    // Map common codegen types
    let type = 'any';
    if (param.includes('std::string') || param.includes('winrt::hstring')) {
      type = 'string';
    } else if (param.includes('double') || param.includes('float')) {
      type = 'number';
    } else if (param.includes('bool')) {
      type = 'boolean';
    } else if (param.includes('int32_t') || param.includes('int64_t') || param.includes('int')) {
      type = 'number';
    } else if (param.includes('JSValue')) {
      type = 'any';
    } else if (param.includes('winrt::')) {
      type = 'string'; // Most winrt types are strings or can be treated as such
    }
    
    return { name: name || 'param', type };
  }

  private parseCodegenSignature(signature: string): Parameter[] {
    // Parse Method<void(...)> signature to extract parameters
    const paramMatch = signature.match(/void\s*\(([^)]*)\)/);
    if (!paramMatch) {
      return [];
    }
    
    return this.parseCodegenParameters(paramMatch[1]);
  }

  private extractMethodsFromSpecInterface(content: string): MethodSignature[] {
    const methods: MethodSignature[] = [];

    // Find the Spec interface definition
    const interfaceMatch = content.match(
      /export\s+interface\s+Spec\s+extends\s+TurboModule\s*\{([\s\S]*?)\}/,
    );
    if (!interfaceMatch) {
      return methods;
    }

    const interfaceBody = interfaceMatch[1];

    // Parse method signatures from the interface
    const methodPattern = /(\w+)\s*\(\s*([^)]*)\s*\)\s*:\s*([^;]+);/g;
    let match;

    while ((match = methodPattern.exec(interfaceBody)) !== null) {
      const methodName = match[1];
      const paramString = match[2].trim();
      const returnType = match[3].trim();

      // Skip comments and empty lines
      if (methodName.startsWith('//') || !methodName) {
        continue;
      }

      const parameters = this.parseParameterString(paramString);

      methods.push({
        name: methodName,
        returnType: returnType,
        parameters: parameters,
      });
    }

    return methods;
  }

  private parseParameterString(paramString: string): Parameter[] {
    if (!paramString || paramString.trim() === '') {
      return [];
    }

    const params = paramString
      .split(',')
      .map(p => p.trim())
      .filter(p => p);
    return params.map(param => {
      const colonIndex = param.lastIndexOf(':');
      if (colonIndex === -1) {
        return {name: param, type: 'any'};
      }

      const name = param.substring(0, colonIndex).trim();
      const type = param.substring(colonIndex + 1).trim();

      return {name, type};
    });
  }

  private async getNamespaceInfo(): Promise<{
    namespace: string;
    codegenNamespace: string;
  }> {
    try {
      const packageJsonPath = path.join(this.root, 'package.json');
      const pkgJson = JSON.parse(await fs.readFile(packageJsonPath, 'utf8'));
      const actualModuleName = this.getActualModuleName(
        pkgJson.name || 'SampleModule',
      );

      // Create reasonable namespace from package name
      const namespace = this.getModuleName(pkgJson.name || 'SampleModule');
      const codegenNamespace = `${actualModuleName}Codegen`;

      return {namespace, codegenNamespace};
    } catch (error) {
      // Fallback
      return {
        namespace: 'ReactNativeWebview',
        codegenNamespace: 'ReactNativeWebviewCodegen',
      };
    }
  }

  private async generateHeaderStub(
    moduleName: string,
    methods: MethodSignature[],
  ): Promise<string> {
    const {namespace, codegenNamespace} = await this.getNamespaceInfo();
    const methodDeclarations = methods
      .map(method => {
        const cppParams = method.parameters
          .map(p => {
            if (p.type === 'function') {
              // Handle callback functions from codegen
              if (p.name.includes('onSuccess') || p.name === 'callback') {
                return `std::function<void()> const & ${p.name}`;
              } else if (p.name.includes('onError')) {
                return `std::function<void(::React::JSValue const &)> const & ${p.name}`;
              } else {
                return `std::function<void()> const & ${p.name}`;
              }
            } else {
              let cppType = this.mapTSToCppType(p.type);
              // Use winrt::hstring for string parameters to match Windows conventions
              if (p.type === 'string') {
                cppType = 'winrt::hstring const&';
              }
              return `${cppType} ${p.name}`;
            }
          })
          .join(', ');

        // Determine if this is a getter method (no parameters and non-void return type)
        const isGetter = method.parameters.length === 0 && method.returnType !== 'void';
        const returnType = isGetter ? this.mapTSToCppType(method.returnType) : 'void';
        const constModifier = isGetter ? ' const' : '';

        return `  REACT_METHOD(${method.name})
  ${returnType} ${method.name}(${cppParams}) noexcept${constModifier};`;
      })
      .join('\n\n');

    const defaultMethods =
      methods.length === 0
        ? `  // TODO: Add your method implementations here
  // Example:
  // REACT_METHOD(getString)
  // void getString(std::string value, std::function<void(std::string)> const & callback) noexcept;`
        : methodDeclarations;

    return `#pragma once

#include "pch.h"
#include "resource.h"

#if __has_include("codegen/Native${moduleName}DataTypes.g.h")
  #include "codegen/Native${moduleName}DataTypes.g.h"
#endif
#include "codegen/Native${moduleName}Spec.g.h"

#include "NativeModules.h"

namespace winrt::${namespace}
{

// See https://microsoft.github.io/react-native-windows/docs/native-platform for help writing native modules

REACT_MODULE(${moduleName})
struct ${moduleName}
{
  using ModuleSpec = ${codegenNamespace}::${moduleName}Spec;

  REACT_INIT(Initialize)
  void Initialize(React::ReactContext const &reactContext) noexcept;

${defaultMethods}

private:
  React::ReactContext m_context;
};

} // namespace winrt::${namespace}`;
  }

  private async generateCppStub(
    moduleName: string,
    methods: MethodSignature[],
  ): Promise<string> {
    const {namespace} = await this.getNamespaceInfo();
    const methodImplementations = methods
      .map(method => {
        const cppParams = method.parameters
          .map(p => {
            if (p.type === 'function') {
              // Handle callback functions from codegen
              if (p.name.includes('onSuccess') || p.name === 'callback') {
                return `std::function<void()> const & ${p.name}`;
              } else if (p.name.includes('onError')) {
                return `std::function<void(::React::JSValue const &)> const & ${p.name}`;
              } else {
                return `std::function<void()> const & ${p.name}`;
              }
            } else {
              let cppType = this.mapTSToCppType(p.type);
              // Use winrt::hstring for string parameters to match Windows conventions
              if (p.type === 'string') {
                cppType = 'winrt::hstring const&';
              }
              return `${cppType} ${p.name}`;
            }
          })
          .join(', ');

        // Determine if this is a getter method (no parameters and non-void return type)
        const isGetter = method.parameters.length === 0 && method.returnType !== 'void';
        const returnType = isGetter ? this.mapTSToCppType(method.returnType) : 'void';
        const constModifier = isGetter ? ' const' : '';

        // Generate implementation based on method type
        const hasCallback = method.parameters.some(p => p.type === 'function' && (p.name.includes('onSuccess') || p.name === 'callback'));
        const hasErrorCallback = method.parameters.some(p => p.type === 'function' && p.name.includes('onError'));
        
        let implementation = `  // TODO: Implement ${method.name}`;
        
        if (isGetter) {
          // Getter method - return default value
          const defaultValue = this.generateDefaultValue(method.returnType);
          implementation += `\n  return ${defaultValue};`;
        } else if (hasCallback && hasErrorCallback) {
          // Method with success and error callbacks
          implementation += `\n  // Example: callback(); // Call on success\n  // Example: onError(React::JSValue{"Error message"}); // Call on error`;
        } else if (hasCallback) {
          // Method with just callback
          implementation += `\n  // Example: callback(); // Call when complete`;
        }

        return `${returnType} ${moduleName}::${method.name}(${cppParams}) noexcept${constModifier} {
${implementation}
}`;
      })
      .join('\n\n');

    const defaultImplementations =
      methods.length === 0
        ? `// TODO: Implement your methods here
// Example:
// void ${moduleName}::getString(std::string value, std::function<void(std::string)> const & callback) noexcept {
//   callback("Hello " + value);
// }`
        : methodImplementations;

    return `#include "${moduleName}.h"

namespace winrt::${namespace} {

void ${moduleName}::Initialize(React::ReactContext const &reactContext) noexcept {
  m_context = reactContext;
}

${defaultImplementations}

} // namespace winrt::${namespace}
`;
  }

  private mapTSToCppType(tsType: string): string {
    const typeMap: {[key: string]: string} = {
      string: 'std::string',
      number: 'double',
      boolean: 'bool',
      object: 'React::JSValue',
      any: 'React::JSValue',
      'any[]': 'React::JSValueArray',
      void: 'void',
      Double: 'double', // React Native codegen type
      Int32: 'int32_t', // React Native codegen type
      Float: 'float', // React Native codegen type
    };

    // Handle array types
    if (tsType.endsWith('[]')) {
      const baseType = tsType.slice(0, -2);
      const cppBaseType = typeMap[baseType] || 'React::JSValue';
      return `std::vector<${cppBaseType}>`;
    }

    return typeMap[tsType] || 'React::JSValue';
  }



  private generateDefaultValue(returnType: string): string {
    if (returnType === 'string') {
      return '"example"';
    } else if (returnType === 'number') {
      return '0';
    } else if (returnType === 'boolean') {
      return 'false';
    } else {
      return 'React::JSValue{}';
    }
  }

  private async verifyBuild(): Promise<void> {
    if (this.options.skipBuild) {
      this.verboseMessage('Skipping build verification');
      return;
    }

    this.verboseMessage('Verifying build...');

    const windowsDir = path.join(this.root, 'windows');
    if (!(await fs.exists(windowsDir))) {
      this.verboseMessage(
        'No windows directory found, skipping build verification',
      );
      return;
    }

    const files = await fs.readdir(windowsDir);
    const slnFiles = files.filter(file => file.endsWith('.sln'));
    if (slnFiles.length === 0) {
      this.verboseMessage('No .sln file found, skipping build verification');
      return;
    }

    try {
      // Just check if MSBuild is available, don't actually build to keep setup fast
      execSync('where msbuild', {stdio: 'ignore'});
      this.verboseMessage('MSBuild found, project should be buildable');
    } catch {
      this.verboseMessage(
        'Warning: MSBuild not found, cannot verify build capability',
      );
    }
  }

  private async cleanAndInstallDeps(): Promise<void> {
    this.verboseMessage('Installing dependencies...');

    // Skip node_modules cleaning as it can cause permission issues on Windows
    // and yarn install will handle dependency updates anyway
    try {
      // Suppress deprecation warnings to clean up output
      const env = {
        ...process.env,
        NODE_NO_WARNINGS: '1',
      };
      execSync('yarn install', {cwd: this.root, stdio: 'inherit', env});
      this.verboseMessage('Dependencies installed');
    } catch (error: any) {
      throw new CodedError(
        'Unknown',
        `Failed to install dependencies: ${error.message}`,
      );
    }
  }

  public async run(spinner: Ora, config: Config): Promise<void> {
    await this.validateEnvironment();
    spinner.text = 'Checking and creating spec file...';

    await this.checkAndCreateSpecFile();
    spinner.text = 'Updating package.json...';

    await this.updatePackageJsonCodegen();
    spinner.text = 'Installing dependencies...';

    await this.cleanAndInstallDeps();
    spinner.text = 'Upgrading dependencies...';

    await this.upgradeDependencies();
    spinner.text = 'Handling existing Windows directory...';

    await this.handleExistingWindowsDirectory();
    spinner.text = 'Setting up Windows library...';

    await this.runInitWindows(config);
    spinner.text = 'Running Windows codegen...';

    await this.runCodegenWindows(config);
    spinner.text = 'Generating C++ stub files...';

    await this.generateStubFiles();
    spinner.text = 'Verifying build setup...';

    await this.verifyBuild();

    spinner.succeed();
  }
}

/**
 * Sanitizes the given option for telemetry.
 * @param key The key of the option.
 * @param value The unsanitized value of the option.
 * @returns The sanitized value of the option.
 */
function optionSanitizer(
  key: keyof ModuleWindowsSetupOptions,
  value: any,
): any {
  switch (key) {
    case 'logging':
    case 'telemetry':
    case 'skipDeps':
    case 'skipBuild':
      return value === undefined ? false : value;
  }
}

/**
 * Get the extra props to add to the `module-windows-setup` telemetry event.
 * @returns The extra props.
 */
async function getExtraProps(): Promise<Record<string, any>> {
  const extraProps: Record<string, any> = {};
  return extraProps;
}

/**
 * The function run when calling `npx @react-native-community/cli module-windows-setup`.
 * @param args Unprocessed args passed from react-native CLI.
 * @param config Config passed from react-native CLI.
 * @param options Options passed from react-native CLI.
 */
async function moduleWindowsSetup(
  args: string[],
  config: Config,
  options: ModuleWindowsSetupOptions,
) {
  await startTelemetrySession(
    'module-windows-setup',
    config,
    options,
    getDefaultOptions(config, moduleWindowsSetupOptions),
    optionSanitizer,
  );

  let moduleWindowsSetupError: Error | undefined;
  try {
    await moduleWindowsSetupInternal(args, config, options);
  } catch (ex) {
    moduleWindowsSetupError =
      ex instanceof Error ? (ex as Error) : new Error(String(ex));
    Telemetry.trackException(moduleWindowsSetupError);
  }

  await endTelemetrySession(moduleWindowsSetupError, getExtraProps);
  setExitProcessWithError(options.logging, moduleWindowsSetupError);
}

/**
 * Sets up Windows support for React Native community modules.
 * @param args Unprocessed args passed from react-native CLI.
 * @param config Config passed from react-native CLI.
 * @param options Options passed from react-native CLI.
 */
export async function moduleWindowsSetupInternal(
  args: string[],
  config: Config,
  options: ModuleWindowsSetupOptions,
) {
  // Suppress Node.js deprecation warnings to clean up output
  const originalNoWarnings = process.env.NODE_NO_WARNINGS;
  if (!options.logging) {
    process.env.NODE_NO_WARNINGS = '1';
  }

  const startTime = performance.now();
  const spinner = newSpinner(
    'Setting up Windows support for React Native module...',
  );

  try {
    const setup = new ModuleWindowsSetup(config.root, options);
    await setup.run(spinner, config);
    const endTime = performance.now();

    // Get the actual module name for display
    const moduleName = await setup.getFinalModuleName();

    console.log(
      `${chalk.green('Success:')} Windows module setup completed! (${Math.round(
        endTime - startTime,
      )}ms)`,
    );
    console.log('');
    console.log(
      chalk.bold('🎉 Your React Native module now supports Windows!'),
    );
    console.log('');
    console.log(chalk.bold('Files created/updated:'));
    console.log(`📄 package.json - Added codegen configuration`);
    console.log(
      `🏗️  Native${moduleName}.ts - TurboModule spec file (edit with your API)`,
    );
    console.log(
      `💻 windows/${moduleName}.h - C++ header file (implement your methods here)`,
    );
    console.log(
      `⚙️  windows/${moduleName}.cpp - C++ implementation file (add your logic here)`,
    );
    console.log('');
    console.log(chalk.bold('Next steps:'));
    console.log(
      "1. 📝 Update the generated spec file with your module's interface",
    );
    console.log('2. 🔧 Implement the methods in the generated C++ stub files');
    console.log('3. 🏗️  Build your project to verify everything works');
    console.log(
      '4. 📚 See the documentation for more details on TurboModule development',
    );
    console.log('');
    console.log(
      chalk.dim(
        'For help, visit: https://microsoft.github.io/react-native-windows/',
      ),
    );
    console.log('');
  } catch (e) {
    spinner.fail();
    const endTime = performance.now();
    console.log(
      `${chalk.red('Error:')} ${(e as any).toString()}. (${Math.round(
        endTime - startTime,
      )}ms)`,
    );
    throw e;
  } finally {
    // Restore original NODE_NO_WARNINGS setting
    if (originalNoWarnings !== undefined) {
      process.env.NODE_NO_WARNINGS = originalNoWarnings;
    } else {
      delete process.env.NODE_NO_WARNINGS;
    }
  }
}

/**
 * Sets up Windows support for React Native community modules.
 */
export const moduleWindowsSetupCommand: Command = {
  name: 'module-windows-setup',
  description:
    'Streamlined setup of Windows support for React Native community modules',
  func: moduleWindowsSetup,
  options: moduleWindowsSetupOptions,
};
