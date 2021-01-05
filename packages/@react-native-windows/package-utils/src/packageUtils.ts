/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as _ from 'lodash';
import * as fs from 'fs';
import * as path from 'path';
import findRepoRoot from '@react-native-windows/find-repo-root';

const getMonorepoPackages: (
  root: string,
) => Array<{
  location: string;
  package: any;
}> = require('get-monorepo-packages');

/**
 * Represents an NPM package
 */
export class NpmPackage {
  private readonly pkgPath: string;
  protected pkgJson: any;

  /**
   * Create a NpmPackage from a given path to a package
   */
  static async fromPath(pkgPath: string): Promise<NpmPackage | null> {
    return WritableNpmPackage.fromPath(pkgPath);
  }

  constructor(pkgPath: string, pkgJson: any) {
    this.pkgPath = pkgPath;
    this.pkgJson = pkgJson;
  }

  get path(): string {
    return this.pkgPath;
  }

  get json(): any {
    return _.cloneDeep(this.pkgJson);
  }
}

/**
 * Represents an NPM package with methods to modify its on-disk package.json
 */
export class WritableNpmPackage extends NpmPackage {
  /**
   * Create a WritableNpmPackage from a given path to a package
   */
  static async fromPath(pkgPath: string): Promise<WritableNpmPackage | null> {
    const jsonPath = path.join(pkgPath, 'package.json');
    try {
      const jsonBuffer = await fs.promises.readFile(jsonPath);
      return new WritableNpmPackage(pkgPath, JSON.parse(jsonBuffer.toString()));
    } catch (ex) {
      if (ex.code === 'ENOENT') {
        return null;
      }

      throw ex;
    }
  }

  /**
   * Assign properties to the package. Uses lodash merge semantics to assign
   * properties (i.e. deep merge instead of shallow)
   *
   * @param props key/values to merge into the package.json
   */
  async mergeProps(props: Record<string, any>) {
    await this.setJson(_.merge(this.json, props));
  }

  /**
   * Assign properties to the package. Uses Object.assign semantics to assign
   * properties (i.e. shallow merge)
   *
   * @param props key/values to merge into the package.json
   */
  async assignProps(props: Record<string, any>) {
    await this.setJson(Object.assign(this.json, props));
  }

  /**
   * Set new JSON for the package from the given object
   */
  async setJson(jsonObj: any) {
    this.pkgJson = jsonObj;

    await fs.promises.writeFile(
      path.join(this.path, 'package.json'),
      JSON.stringify(this.json, null /*replacer*/, 2 /*space*/) + '\n',
    );
  }
}

/**
 * Finds monorepo-local packages matching a given predicate. The root package
 * is not included.
 *
 * @param pred predicate describing whether to match a package
 */
export async function enumerateRepoPackages(
  pred: (pkg: NpmPackage) => Promise<boolean> = async () => true,
): Promise<WritableNpmPackage[]> {
  const repoRoot = await findRepoRoot();
  const allPackges = getMonorepoPackages(repoRoot).map(
    pkg => new WritableNpmPackage(pkg.location, pkg.package),
  );

  const filteredPackages: WritableNpmPackage[] = [];
  for (const pkg of allPackges) {
    if (await pred(pkg)) {
      filteredPackages.push(pkg);
    }
  }

  return filteredPackages;
}

/**
 * Finds a package with a given name (local or dependency)
 */
export async function findPackage(
  name: string,
  opts: {searchPath?: string} = {},
): Promise<NpmPackage | null> {
  const resolvePaths = require.resolve.paths(`${name}/package.json`)!;

  let pkgJsonPath: string;
  try {
    pkgJsonPath = require.resolve(`${name}/package.json`, {
      paths: [opts.searchPath || process.cwd(), ...resolvePaths],
    });
  } catch (ex) {
    if (ex.code === 'MODULE_NOT_FOUND') {
      return null;
    } else {
      throw ex;
    }
  }

  return new NpmPackage(
    path.dirname(pkgJsonPath),
    JSON.parse((await fs.promises.readFile(pkgJsonPath)!).toString()),
  );
}

/**
 * Finds a monorepo-local package with a given name
 */
export async function findRepoPackage(
  name: string,
): Promise<WritableNpmPackage | null> {
  const packages = await enumerateRepoPackages(async p => p.json.name === name);

  if (packages.length === 0) {
    return null;
  } else {
    return packages[0];
  }
}
