/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import fs from '@react-native-windows/fs';
import path from 'path';
import chalk from 'chalk';
import {glob as globFunc} from 'glob';
import _ from 'lodash';
import {performance} from 'perf_hooks';
import {Ora} from 'ora';
import util from 'util';

const glob = util.promisify(globFunc);

import {Command, Config} from '@react-native-community/cli-types';
import {CodedError, Telemetry} from '@react-native-windows/telemetry';

import {
  newSpinner,
  setExitProcessWithError,
} from '../../utils/commandWithProgress';
import * as pathHelpers from '../../utils/pathHelpers';
import {
  getDefaultOptions,
  startTelemetrySession,
  endTelemetrySession,
} from '../../utils/telemetryHelpers';
import {copyAndReplaceWithChangedCallback} from '../../generator-common';
import * as nameHelpers from '../../utils/nameHelpers';
import {InitOptions, initOptions} from './initWindowsOptions';

export interface TemplateFileMapping {
  from: string;
  to: string;
  replacements?: Record<string, any>;
}

export interface InitWindowsTemplateConfig {
  name: string;
  description: string;
  isDefault?: boolean;
  preInstall?: (config: Config, options: InitOptions) => Promise<void>;
  getFileMappings?: (
    config: Config,
    options: InitOptions,
  ) => Promise<TemplateFileMapping[]>;
  postInstall?: (config: Config, options: InitOptions) => Promise<void>;
}

export class InitWindows {
  protected readonly rnwPath: string;
  protected readonly templates: Map<string, InitWindowsTemplateConfig> =
    new Map();

  constructor(readonly config: Config, readonly options: InitOptions) {
    this.rnwPath = pathHelpers.resolveRnwRoot(this.config.root);
  }

  protected verboseMessage(message: any) {
    verboseMessage(message, !!this.options.logging);
  }

  protected async loadTemplates() {
    const templatesRoot = path.join(this.rnwPath, 'templates');
    for (const file of await glob('**/template.config.js', {
      cwd: templatesRoot,
    })) {
      const templateName = path.dirname(file).replace(/[\\]/g, '/');
      const templateConfig: InitWindowsTemplateConfig = require(path.join(
        templatesRoot,
        file,
      ));
      this.templates.set(templateName, templateConfig);
    }
    if (this.templates.size === 0) {
      throw new CodedError(
        'NoTemplatesFound',
        `No templates were found in ${templatesRoot}.`,
      );
    }
  }

  protected getDefaultTemplateName(): string {
    for (const [name, config] of this.templates) {
      if (config.isDefault) {
        return name;
      }
    }
    throw new CodedError(
      'NoDefaultTemplate',
      'No template specified and no default template found.',
    );
  }

  protected getReactNativeProjectName(projectDir: string): string {
    this.verboseMessage('Looking for project name in package.json...');
    const pkgJsonPath = path.join(projectDir, 'package.json');
    if (!fs.existsSync(pkgJsonPath)) {
      throw new CodedError(
        'NoPackageJson',
        'Unable to find package.json. This should be run from within an existing react-native project.',
      );
    }
    type PackageJson = {name: string};

    let name = fs.readJsonFileSync<PackageJson>(pkgJsonPath).name;
    if (!name) {
      const appJsonPath = path.join(projectDir, 'app.json');
      if (fs.existsSync(appJsonPath)) {
        this.verboseMessage('Looking for project name in app.json...');
        name = fs.readJsonFileSync<PackageJson>(pkgJsonPath).name;
      }
    }

    if (!name) {
      throw new CodedError(
        'NoProjectName',
        'Please specify name in package.json or app.json',
      );
    }

    return name;
  }

  protected printTemplateList() {
    if (this.templates.size === 0) {
      console.log('\nNo templates found.\n');
      return;
    }

    for (const [key, value] of this.templates.entries()) {
      const defaultLabel = value.isDefault ? chalk.yellow('[Default] ') : '';
      console.log(
        `\n${key} - ${value.name}\n    ${defaultLabel}${value.description}`,
      );
    }
    console.log(`\n`);
  }

  public async run(spinner: Ora) {
    await this.loadTemplates();

    spinner.info();

    if (this.options.list) {
      this.printTemplateList();
      return;
    }

    this.options.template ??= this.getDefaultTemplateName();

    spinner.info(`Using template '${this.options.template}'...`);
    if (!this.templates.has(this.options.template.replace(/[\\]/g, '/'))) {
      throw new CodedError(
        'InvalidTemplateName',
        `Unable to find template '${this.options.template}'.`,
      );
    }
    const templateConfig = this.templates.get(this.options.template)!;

    // Check if there's a passed-in project name and if it's valid
    if (
      this.options.name &&
      !nameHelpers.isValidProjectName(this.options.name)
    ) {
      throw new CodedError(
        'InvalidProjectName',
        `The specified name '${this.options.name}' is not a valid identifier`,
      );
    }

    // If no project name is provided, calculate the name and clean if necessary
    if (!this.options.name) {
      const projectName = this.getReactNativeProjectName(this.config.root);
      this.options.name = nameHelpers.isValidProjectName(projectName)
        ? projectName
        : nameHelpers.cleanName(projectName);
    }

    // Final check that the project name is valid
    if (!nameHelpers.isValidProjectName(this.options.name)) {
      throw new CodedError(
        'InvalidProjectName',
        `The name '${this.options.name}' is not a valid identifier`,
      );
    }

    // Check if there's a passed-in project namespace and if it's valid
    if (
      this.options.namespace &&
      !nameHelpers.isValidProjectNamespace(this.options.namespace)
    ) {
      throw new CodedError(
        'InvalidProjectNamespace',
        `The specified namespace '${this.options.namespace}' is not a valid identifier`,
      );
    }

    // If no project namespace is provided, use the project name and clean if necessary
    if (!this.options.namespace) {
      const namespace = this.options.name;
      this.options.namespace = nameHelpers.isValidProjectNamespace(namespace)
        ? namespace
        : nameHelpers.cleanNamespace(namespace);
    }

    // Final check that the project namespace is valid
    if (!nameHelpers.isValidProjectNamespace(this.options.namespace)) {
      throw new CodedError(
        'InvalidProjectNamespace',
        `The namespace '${this.options.namespace}' is not a valid identifier`,
      );
    }

    if (templateConfig.preInstall) {
      spinner.info(`Running ${this.options.template} preInstall()...`);
      await templateConfig.preInstall(this.config, this.options);
    }

    // Get template files to copy and copy if available
    if (templateConfig.getFileMappings) {
      const fileMappings = await templateConfig.getFileMappings(
        this.config,
        this.options,
      );

      for (const fileMapping of fileMappings) {
        const targetDir = path.join(
          this.config.root,
          path.dirname(fileMapping.to),
        );

        if (!(await fs.exists(targetDir))) {
          await fs.mkdir(targetDir, {recursive: true});
        }

        await copyAndReplaceWithChangedCallback(
          fileMapping.from,
          this.config.root,
          fileMapping.to,
          fileMapping.replacements,
          this.options.overwrite,
        );
      }
    }

    if (templateConfig.postInstall) {
      spinner.info(`Running ${this.options.template} postInstall()...`);
      await templateConfig.postInstall(this.config, this.options);
    }

    spinner.succeed();
  }
}

/**
 * Logs the given message if verbose is True.
 * @param message The message to log.
 * @param verbose Whether or not verbose logging is enabled.
 */
function verboseMessage(message: any, verbose?: boolean) {
  if (verbose) {
    console.log(message);
  }
}

/**
 * Sanitizes the given option for telemetry.
 * @param key The key of the option.
 * @param value The unsanitized value of the option.
 * @returns The sanitized value of the option.
 */
function optionSanitizer(key: keyof InitOptions, value: any): any {
  // Do not add a default case here.
  // Strings risking PII should just return true if present, false otherwise.
  // All others should return the value (or false if undefined).
  switch (key) {
    case 'name':
    case 'namespace':
      return value === undefined ? false : true; // Strip PII
    case 'logging':
    case 'template':
    case 'overwrite':
    case 'telemetry':
    case 'list':
      return value === undefined ? false : value; // Return value
  }
}

/**
 * Get the extra props to add to the `init-windows` telemetry event.
 * @returns The extra props.
 */
async function getExtraProps(): Promise<Record<string, any>> {
  const extraProps: Record<string, any> = {};
  return extraProps;
}

/**
 * The function run when calling `npx @react-native-community/cli init-windows`.
 * @param args Unprocessed args passed from react-native CLI.
 * @param config Config passed from react-native CLI.
 * @param options Options passed from react-native CLI.
 */
async function initWindows(
  args: string[],
  config: Config,
  options: InitOptions,
) {
  await startTelemetrySession(
    'init-windows',
    config,
    options,
    getDefaultOptions(config, initOptions),
    optionSanitizer,
  );

  let initWindowsError: Error | undefined;
  try {
    await initWindowsInternal(args, config, options);
  } catch (ex) {
    initWindowsError =
      ex instanceof Error ? (ex as Error) : new Error(String(ex));
    Telemetry.trackException(initWindowsError);
  }

  await endTelemetrySession(initWindowsError, getExtraProps);
  setExitProcessWithError(options.logging, initWindowsError);
}

/**
 * Initializes a new RNW project from a given template.
 * @param args Unprocessed args passed from react-native CLI.
 * @param config Config passed from react-native CLI.
 * @param options Options passed from react-native CLI.
 */
export async function initWindowsInternal(
  args: string[],
  config: Config,
  options: InitOptions,
) {
  const startTime = performance.now();
  const spinner = newSpinner('Running init-windows...');
  try {
    const codegen = new InitWindows(config, options);
    await codegen.run(spinner);
    const endTime = performance.now();

    console.log(
      `${chalk.green('Success:')} init-windows completed. (${Math.round(
        endTime - startTime,
      )}ms)`,
    );
  } catch (e) {
    spinner.fail();
    const endTime = performance.now();
    console.log(
      `${chalk.red('Error:')} ${(e as any).toString()}. (${Math.round(
        endTime - startTime,
      )}ms)`,
    );
    throw e;
  }
}

/**
 * Initializes a new RNW project from a given template.
 */
export const initCommand: Command = {
  name: 'init-windows',
  description: 'Initializes a new RNW project from a given template',
  func: initWindows,
  options: initOptions,
};
