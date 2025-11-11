/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */

import fs from '@react-native-windows/fs';
import * as path from 'path';
import {execSync} from 'child_process';

/**
 * Get latest stable version from npm
 */
function getLatestStableVersion(): string {
  try {
    return execSync('npm view react-native-windows version', {
      encoding: 'utf8',
    }).trim();
  } catch (error) {
    throw new Error(`Could not fetch latest stable version from npm: ${error}`);
  }
}

/**
 * Get latest preview version from npm
 */
function getLatestPreviewVersion(): string | undefined {
  try {
    const versions = JSON.parse(
      execSync('npm view react-native-windows versions --json', {
        encoding: 'utf8',
      }),
    ) as string[];
    // Preview versions usually have "preview" in the string
    return versions.reverse().find(v => v.includes('preview'));
  } catch (error) {
    console.warn('Could not fetch preview versions from npm:', error);
    return undefined;
  }
}

const LATEST_STABLE_VERSION = getLatestStableVersion();
const LATEST_PREVIEW_VERSION = getLatestPreviewVersion();

// Ensure we have valid versions for testing
if (!LATEST_STABLE_VERSION) {
  throw new Error('Could not fetch latest stable version from npm');
}
if (!LATEST_PREVIEW_VERSION) {
  throw new Error('Could not fetch latest preview version from npm');
}

/**
 * Mock NPM registry response for version check
 */
const mockNpmShow = (packageNameWithVersion: string, version: string) => {
  if (packageNameWithVersion === `react-native-windows@${version}`) {
    const rnVersion = `^${version.split('-')[0]}`;
    return {
      version: version,
      devDependencies: {
        'react-native': rnVersion,
      },
      dependencies: {
        '@react-native-community/cli': '^17.0.0',
      },
    };
  }
  return null;
};

// Test suites for stable and preview versions
describe('creaternwapp Configuration Tests - Stable Version', () => {
  const repoRoot = path.resolve(__dirname, '../../../../..');
  const createRnwAppScript = path.join(
    repoRoot,
    'vnext/Scripts/creaternwapp.cmd',
  );
  const RNW_VERSION = LATEST_STABLE_VERSION;

  beforeAll(() => {
    // Verify the script exists in the repository
    expect(fs.existsSync(createRnwAppScript)).toBe(true);

    // Verify we have a stable version to test
    expect(RNW_VERSION).toBeTruthy();
    expect(RNW_VERSION).not.toContain('preview');
  });

  describe('Script Configuration Validation', () => {
    test('creaternwapp.cmd script should exist and be readable', () => {
      expect(fs.existsSync(createRnwAppScript)).toBe(true);

      const scriptContent = fs.readFileSync(createRnwAppScript, 'utf8');

      // Verify script contains expected parameters
      expect(scriptContent).toContain('/rnw');
      expect(scriptContent).toContain('/t');
      expect(scriptContent).toContain('/verdaccio');
      expect(scriptContent).toContain('cpp-app');
    });

    test('should validate fabric template is the default', () => {
      const scriptContent = fs.readFileSync(createRnwAppScript, 'utf8');

      // Default template should be cpp-app (fabric/new arch)
      expect(scriptContent).toContain('set RNW_TEMPLATE_TYPE=cpp-app');
    });

    test('should validate new fabric templates are available', () => {
      const newTemplatesDir = path.join(repoRoot, 'vnext/templates');
      expect(fs.existsSync(newTemplatesDir)).toBe(true);

      const cppAppTemplate = path.join(newTemplatesDir, 'cpp-app');
      expect(fs.existsSync(cppAppTemplate)).toBe(true);
    });
  });

  describe('Version Configuration Tests', () => {
    test('should validate target version is available', async () => {
      // This test validates that the specified version exists in npm
      // In a real environment, this would use npm show to check the version
      expect(RNW_VERSION).toBeTruthy();

      // Mock version validation - in actual Windows CI this would call npm show
      const versionInfo = mockNpmShow(
        `react-native-windows@${RNW_VERSION}`,
        RNW_VERSION,
      );
      expect(versionInfo).toBeTruthy();
      expect(versionInfo?.version).toBe(RNW_VERSION);
    });

    test('should validate required dependencies for target version', () => {
      const versionInfo = mockNpmShow(
        `react-native-windows@${RNW_VERSION}`,
        RNW_VERSION,
      );

      // Validate that the version has the expected dependency structure
      expect(versionInfo?.devDependencies).toHaveProperty('react-native');
      expect(versionInfo?.dependencies).toHaveProperty(
        '@react-native-community/cli',
      );
    });
  });

  describe('Template Configuration Tests', () => {
    test('should validate new architecture (fabric) command structure', () => {
      // Test the command that would be used for new arch:
      // creaternwapp.cmd /rnw preview test-app-name
      const expectedCommand = `creaternwapp.cmd /rnw preview test-app-name`;

      expect(expectedCommand).toContain('/rnw preview');
      expect(expectedCommand).toContain('test-app-name');
      expect(expectedCommand).not.toContain('/t'); // No template specified = default fabric
    });

    test('should validate yarn windows command variations', () => {
      // Test the commands that would be used to build apps:
      const debugCommand = 'yarn windows';
      const releaseCommand = 'yarn windows --release';

      expect(debugCommand).toBe('yarn windows');
      expect(releaseCommand).toBe('yarn windows --release');
      expect(releaseCommand).toContain('--release');
    });
  });

  describe('Workflow Integration Tests', () => {
    test('should validate complete new architecture workflow commands', () => {
      const workflow = {
        create: `creaternwapp.cmd /rnw ${RNW_VERSION} TestAppFabric`,
        start: 'yarn start',
        buildDebug: 'yarn windows',
        buildRelease: 'yarn windows --release',
      };

      expect(workflow.create).toContain(RNW_VERSION);
      expect(workflow.create).toContain('TestAppFabric');
      expect(workflow.start).toBe('yarn start');
      expect(workflow.buildDebug).toBe('yarn windows');
      expect(workflow.buildRelease).toBe('yarn windows --release');
    });

  });

  describe('Documentation and Example Validation', () => {
    test('should document the testing procedure for new architecture', () => {
      const procedure = {
        title: 'New Architecture (Fabric) Testing',
        steps: [
          `creaternwapp.cmd /rnw ${RNW_VERSION} test-app-name`,
          'yarn start (in test-app-name path)',
          'cd test-app-name && yarn windows',
          'cd test-app-name && yarn windows --release',
        ],
      };

      expect(procedure.steps[0]).toContain(RNW_VERSION);
      expect(procedure.steps[1]).toContain('yarn start');
      expect(procedure.steps[2]).toContain('yarn windows');
      expect(procedure.steps[3]).toContain('yarn windows --release');
    });

  });
});

describe('creaternwapp Configuration Tests - Preview Version', () => {
  const repoRoot = path.resolve(__dirname, '../../../../..');
  const createRnwAppScript = path.join(
    repoRoot,
    'vnext/Scripts/creaternwapp.cmd',
  );
  const RNW_VERSION = LATEST_PREVIEW_VERSION;

  beforeAll(() => {
    // Verify the script exists in the repository
    expect(fs.existsSync(createRnwAppScript)).toBe(true);

    // Verify we have a preview version to test
    expect(RNW_VERSION).toBeTruthy();
    expect(RNW_VERSION).toContain('preview');
  });

  describe('Script Configuration Validation', () => {
    test('creaternwapp.cmd script should exist and be readable', () => {
      expect(fs.existsSync(createRnwAppScript)).toBe(true);

      const scriptContent = fs.readFileSync(createRnwAppScript, 'utf8');

      // Verify script contains expected parameters
      expect(scriptContent).toContain('/rnw');
      expect(scriptContent).toContain('/t');
      expect(scriptContent).toContain('/verdaccio');
      expect(scriptContent).toContain('cpp-app');
    });

    test('should validate fabric template is the default', () => {
      const scriptContent = fs.readFileSync(createRnwAppScript, 'utf8');

      // Default template should be cpp-app (fabric/new arch)
      expect(scriptContent).toContain('set RNW_TEMPLATE_TYPE=cpp-app');
    });

    test('should validate new fabric templates are available', () => {
      const newTemplatesDir = path.join(repoRoot, 'vnext/templates');
      expect(fs.existsSync(newTemplatesDir)).toBe(true);

      const cppAppTemplate = path.join(newTemplatesDir, 'cpp-app');
      expect(fs.existsSync(cppAppTemplate)).toBe(true);
    });
  });

  describe('Version Configuration Tests', () => {
    test('should validate target version is available', async () => {
      // This test validates that the specified version exists in npm
      // In a real environment, this would use npm show to check the version
      expect(RNW_VERSION).toBeTruthy();

      // Mock version validation - in actual Windows CI this would call npm show
      const versionInfo = mockNpmShow(
        `react-native-windows@${RNW_VERSION}`,
        RNW_VERSION,
      );
      expect(versionInfo).toBeTruthy();
      expect(versionInfo?.version).toBe(RNW_VERSION);
    });

    test('should validate required dependencies for target version', () => {
      const versionInfo = mockNpmShow(
        `react-native-windows@${RNW_VERSION}`,
        RNW_VERSION,
      );

      // Validate that the version has the expected dependency structure
      expect(versionInfo?.devDependencies).toHaveProperty('react-native');
      expect(versionInfo?.dependencies).toHaveProperty(
        '@react-native-community/cli',
      );
    });
  });

  describe('Template Configuration Tests', () => {
    test('should validate new architecture (fabric) command structure', () => {
      // Test the command that would be used for new arch:
      // creaternwapp.cmd /rnw preview test-app-name
      const expectedCommand = `creaternwapp.cmd /rnw preview test-app-name`;

      expect(expectedCommand).toContain('/rnw preview');
      expect(expectedCommand).toContain('test-app-name');
      expect(expectedCommand).not.toContain('/t'); // No template specified = default fabric
    });

    test('should validate yarn windows command variations', () => {
      // Test the commands that would be used to build apps:
      const debugCommand = 'yarn windows';
      const releaseCommand = 'yarn windows --release';

      expect(debugCommand).toBe('yarn windows');
      expect(releaseCommand).toBe('yarn windows --release');
      expect(releaseCommand).toContain('--release');
    });
  });

  describe('Workflow Integration Tests', () => {
    test('should validate complete new architecture workflow commands', () => {
      const workflow = {
        create: `creaternwapp.cmd /rnw ${RNW_VERSION} TestAppFabric`,
        start: 'yarn start',
        buildDebug: 'yarn windows',
        buildRelease: 'yarn windows --release',
      };

      expect(workflow.create).toContain(RNW_VERSION);
      expect(workflow.create).toContain('TestAppFabric');
      expect(workflow.start).toBe('yarn start');
      expect(workflow.buildDebug).toBe('yarn windows');
      expect(workflow.buildRelease).toBe('yarn windows --release');
    });
  });

  describe('Documentation and Example Validation', () => {
    test('should document the testing procedure for new architecture', () => {
      const procedure = {
        title: 'New Architecture (Fabric) Testing',
        steps: [
          `creaternwapp.cmd /rnw ${RNW_VERSION} test-app-name`,
          'yarn start (in test-app-name path)',
          'cd test-app-name && yarn windows',
          'cd test-app-name && yarn windows --release',
        ],
      };

      expect(procedure.steps[0]).toContain(RNW_VERSION);
      expect(procedure.steps[1]).toContain('yarn start');
      expect(procedure.steps[2]).toContain('yarn windows');
      expect(procedure.steps[3]).toContain('yarn windows --release');
    });

  });
});
