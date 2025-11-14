/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import {createHash} from 'crypto';
import fs from '@react-native-windows/fs';
import path from 'path';

interface ProjectInfo {
  id: string | null;
  platforms: Array<string>;
  rnwLang: 'cpp' | 'cs' | 'cpp+cs' | null;
}

export interface DependencyProjectInfo extends ProjectInfo {}

export interface AppProjectInfo extends ProjectInfo {
  usesTS: boolean;
  usesRNConfig: boolean;
  jsEngine: string;
  rnwSource: string;
  dependencies: Array<DependencyProjectInfo>;
}

/**
 * Gets a unique, telemetry-safe project ID based on the project name.
 * @param projectName The project name.
 * @returns The telemetry-safe project ID.
 */
export function getProjectId(projectName: string): string {
  const hash = createHash('sha256');
  hash.update(projectName);
  return hash.digest('hex');
}

/**
 * Checks that a given file exits in the path specified.
 * @param fileName The file to check for.
 * @param projectRoot The root path to look in.
 * @returns Whether the file exists.
 */
async function fileExists(
  fileName: string,
  projectRoot: string,
): Promise<boolean> {
  try {
    const reactNativeConfigPath = path.resolve(projectRoot, fileName);
    const stats = await fs.stat(reactNativeConfigPath);
    return stats.isFile();
  } catch {}
  return false;
}

/**
 * Gets whether the project at the given path has a react-native.config.js file.
 * @param projectRoot The project root path to look in.
 * @returns Whether the project at the given path has a react-native.config.js file.
 */
export async function usesReactNativeConfig(
  projectRoot: string,
): Promise<boolean> {
  return fileExists('./react-native.config.js', projectRoot);
}

/**
 * Gets whether the project at the given path is using TypeScript.
 * @param projectRoot The project root path to look in.
 * @returns Whether the project at the given path is using TypeScript.
 */
export async function usesTypeScript(projectRoot: string): Promise<boolean> {
  return fileExists('./tsconfig.json', projectRoot);
}

/**
 * Get the list of keys in the object for which the value is defined.
 * @param obj The object to search.
 * @returns The list of keys.
 */
function getDefinedKeys(obj: Record<string, any>): string[] {
  return Object.keys(obj).filter(
    value => obj[value] !== undefined && obj[value] !== null && value,
  );
}

/**
 * Given a react-native CLI config, determine the language of the RNW dependency if possible.
 * @param config Dependency config passed from react-native CLI.
 * @returns The language of the RNW dependency.
 */
function getDependencyRnwLang(
  config: Record<string, any> | null | undefined,
): 'cpp' | 'cs' | 'cpp+cs' | null {
  if (config) {
    let cppCount = 0;
    let csCount = 0;
    for (const project of config.projects) {
      switch (project.projectLang) {
        case 'cpp':
          cppCount++;
          break;
        case 'cs':
          csCount++;
          break;
      }
    }
    if (cppCount > 0 && csCount > 0) {
      return 'cpp+cs';
    } else if (cppCount > 0) {
      return 'cpp';
    } else if (csCount > 0) {
      return 'cs';
    }
  }
  return null;
}

/**
 * Returns true if the item is a boolean with a value of true or a string with a value of 'true'.
 * @param item The item to parse.
 * @returns The boolean value.
 */
function parseBoolean(item: any): boolean {
  if (typeof item === 'boolean') {
    return item;
  } else if (typeof item === 'string') {
    return item.toLowerCase() === 'true';
  }
  return false;
}

/**
 * Calculate the project telemetry info from a react-native CLI config.
 * @param config Config passed from react-native CLI.
 * @returns The calculated project info.
 */
export async function configToProjectInfo(
  config: Record<string, any>,
): Promise<AppProjectInfo | DependencyProjectInfo | null> {
  try {
    const id: string = getProjectId(
      require(path.join(config.root, 'package.json')).name,
    );
    const platforms = getDefinedKeys(config.project);

    if ('windows' in config.project && config.project.windows !== null) {
      const rnwLang = config.project.windows.project?.projectLang;
      const usesTS = await usesTypeScript(config.project.windows.folder);
      const usesRNConfig = await usesReactNativeConfig(
        config.project.windows.folder,
      );
      const jsEngine = 'Hermes';
      const rnwSource = parseBoolean(
        config.project.windows.experimentalFeatures?.UseExperimentalNuget,
      )
        ? 'NuGet'
        : 'Source';

      const dependencies: DependencyProjectInfo[] = [];
      for (const dependencyName in config.dependencies) {
        if (!Object.prototype.hasOwnProperty(dependencyName)) {
          const dependencyId = getProjectId(dependencyName);
          const dependencyPlatforms = getDefinedKeys(
            config.dependencies[dependencyName].platforms,
          );

          if (dependencyPlatforms.length > 0) {
            const dependencyRnwLang = getDependencyRnwLang(
              config.dependencies[dependencyName].platforms.windows,
            );
            const dependencyInfo: DependencyProjectInfo = {
              id: dependencyId,
              platforms: dependencyPlatforms,
              rnwLang: dependencyRnwLang,
            };
            dependencies.push(dependencyInfo);
          }
        }
      }

      const appInfo: AppProjectInfo = {
        id,
        platforms,
        rnwLang,
        usesTS,
        usesRNConfig,
        jsEngine,
        rnwSource,
        dependencies,
      };

      return appInfo;
    }

    // Probably just a dependency project, return minimal info
    const dependencyInfo: DependencyProjectInfo = {
      id,
      platforms,
      rnwLang: null,
    };
    return dependencyInfo;
  } catch {}
  return null;
}

/**
 * Gets the full path to the app's native project file from a react-native CLI config.
 * @param config Config passed from react-native CLI.
 * @returns The full path to the app's native project file
 */
export function getProjectFileFromConfig(
  config: Record<string, any>,
): string | null {
  try {
    if ('windows' in config.project && config.project.windows !== null) {
      return path.join(
        config.project.windows.folder,
        config.project.windows.sourceDir,
        config.project.windows.project.projectFile,
      );
    }
  } catch {}
  return null;
}
