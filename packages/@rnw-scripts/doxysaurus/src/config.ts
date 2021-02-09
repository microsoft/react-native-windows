/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

/**
 * Configuration for the Doxysaurus tool.
 */

import path from 'path';
import {promises as fs, existsSync} from 'fs';

/** Doxysaurus project configuration. */
export interface Config {
  configDir: string;
  input: string;
  buildDir: string;
  outputDir: string;
  filePatterns?: string[];
  docIdPrefix: string;
  indexFilename?: string;
  indexTemplatePath?: string;
  projects?: string[];
  types?: string[];
  namespaceAliases?: {[key: string]: string[]};
  sections?: {[key: string]: string};
  stdTypeLinks?: {
    linkPrefix?: string;
    linkMap: {[key: string]: string};
    operatorMap?: {[key: string]: string};
  };
  idlTypeLinks?: {
    linkPrefix?: string;
    linkMap: {[key: string]: string};
    operatorMap?: {[key: string]: string};
  };
}

export type LoadedConfig = Partial<Config> & {
  configDir: string;
  buildDir: string;
};

/** Generates an async stream of project configs. */
export async function* getProjectConfigs(
  configPath: string,
  outputDir?: string,
  parentConfig?: LoadedConfig,
): AsyncGenerator<Config> {
  const config = await loadConfig(
    path.resolve(process.cwd(), configPath),
    parentConfig,
  );
  config.outputDir = outputDir;
  if (Array.isArray(config.projects) && config.projects.length > 0) {
    for (const project of config.projects) {
      const projectConfigPath = path.resolve(
        config.configDir,
        path.join(project, 'doxysaurus.json'),
      );
      if (existsSync(projectConfigPath)) {
        for await (const projectConfig of getProjectConfigs(
          projectConfigPath,
          outputDir,
          config,
        )) {
          yield projectConfig;
        }
      } else {
        yield normalizeConfig({...config, input: project});
      }
    }
  } else {
    yield normalizeConfig(config);
  }
}

/** Loads config file and merges it with parent config if it is given. */
async function loadConfig(
  configPath: string,
  parentConfig?: LoadedConfig,
): Promise<LoadedConfig> {
  const configText = await fs.readFile(configPath, 'utf-8');
  const config = JSON.parse(configText) as Partial<Config>;
  const configDir = path.dirname(configPath);
  if (parentConfig) {
    return {
      ...parentConfig,
      input: undefined,
      projects: undefined,
      ...config,
      configDir,
      buildDir: path.join(
        parentConfig.buildDir,
        config.buildDir || path.basename(configDir),
      ),
    };
  } else {
    return {
      ...config,
      configDir,
      buildDir: config.buildDir
        ? path.resolve(configDir, config.buildDir)
        : path.join(process.cwd(), 'docs'),
    };
  }
}

/** Normalizes config to set all required fields and makes all paths absolute. */
function normalizeConfig(config: LoadedConfig): Config {
  const buildDir = config.buildDir
    ? path.resolve(config.configDir, config.buildDir)
    : path.join(process.cwd(), 'docs');
  return {
    ...config,
    input: config.input
      ? path.resolve(config.configDir, config.input)
      : config.configDir,
    buildDir,
    outputDir: config.outputDir || path.join(buildDir, 'out'),
    docIdPrefix: config.docIdPrefix || '',
    indexFilename: config.indexFilename || 'index.md',
  };
}
