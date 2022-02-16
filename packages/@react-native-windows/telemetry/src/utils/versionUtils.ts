/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import envinfo from 'envinfo';
import fs from '@react-native-windows/fs';
import path from 'path';
import {DOMParser} from '@xmldom/xmldom';
import xpath from 'xpath';

const msbuildSelect = xpath.useNamespaces({
  msbuild: 'http://schemas.microsoft.com/developer/msbuild/2003',
});

/**
 * Gets the version of node being used.
 * @returns The version of node being used.
 */
export async function getNodeVersion(): Promise<string | null> {
  return process.version.slice(1);
}

/**
 * Gets the version of npm installed, if available.
 * @returns The version of npm installed, if available.
 */
export async function getNpmVersion(): Promise<string | null> {
  try {
    const info: any = await envinfo.helpers.getnpmInfo();
    return info[1] as string;
  } catch {}
  return null;
}

/**
 * Gets the version of yarn installed, if available.
 * @returns The version of yarn installed, if available.
 */
export async function getYarnVersion(): Promise<string | null> {
  try {
    const info: any = await envinfo.helpers.getYarnInfo();
    return info[1] as string;
  } catch {}
  return null;
}

/**
 * Gets the latest version of Visual Studio installed, if available.
 * @returns The latest version of Visual Studio installed, if available.
 */
export async function getVisualStudioVersion(): Promise<string | null> {
  try {
    const info: any = await envinfo.helpers.getVisualStudioInfo();
    const versions = info[1] as string[];
    return versions.sort().slice(-1)[0].split(' ')[0];
  } catch {}
  return null;
}

/**
 * Gets the version installed of the specified npm package.
 * @param pkgName The npm package name.
 * @returns The version installed, if available.
 */
export async function getVersionOfNpmPackage(
  pkgName: string,
): Promise<string | null> {
  try {
    const pkgJsonPath = require.resolve(`${pkgName.trim()}/package.json`, {
      paths: [process.cwd(), __dirname],
    });
    const pkgJson = await fs.readJsonFile<{name: string; version: string}>(
      pkgJsonPath,
    );
    if (pkgJson.name === pkgName) {
      return pkgJson.version;
    }
  } catch {}
  return null;
}

/**
 * Reads and parses an XML file into a Document.
 * @param filePath The path to the XML file.
 * @returns The parsed Document.
 */
async function readXmlFile(filePath: string): Promise<Document> {
  const contents = await fs.readFile(filePath, 'utf-8');
  return new DOMParser().parseFromString(contents);
}

/**
 * Gets the versions of the specified NuGet packages referenced in a packages.config file.
 * @param projectDoc The XML document of the packages.connfig file.
 * @param nugetPackages The NuGet package names to look for.
 * @returns The mapping of NuGet package names and their versions.
 */
function getVersionsFromPackagesConfig(
  packagesConfigDoc: Document,
  nugetPackages: string[],
): Record<string, string> {
  const versions: Record<string, string> = {};
  for (const pkgName of nugetPackages) {
    const version = xpath.select1(
      `//packages/package[@id='${pkgName}']/@version`,
      packagesConfigDoc,
    );

    if (version) {
      const versionValue = (version as Attr).nodeValue;
      if (versionValue !== null) {
        versions[pkgName] = versionValue;
      }
    }
  }
  return versions;
}

/**
 * Gets the versions of the specified NuGet packages referenced in a project file.
 * @param projectDoc The XML document of the project file.
 * @param nugetPackages The NuGet package names to look for.
 * @returns The mapping of NuGet package names and their versions.
 */
function getVersionsFromProjectFile(
  projectDoc: Document,
  nugetPackages: string[],
): Record<string, string> {
  const versions: Record<string, string> = {};
  for (const pkgName of nugetPackages) {
    const version = msbuildSelect(
      `//msbuild:ItemGroup/msbuild:PackageReference[@Include='${pkgName}']/msbuild:Version`,
      projectDoc,
      true,
    );

    if (version) {
      const versionValue = (version as Node).textContent;
      if (versionValue !== null) {
        versions[pkgName] = versionValue;
      }
    }
  }
  return versions;
}

/**
 * Gets the versions of the specified NuGet packages referenced in a project file.
 * @param projectFile Path to the native project file.
 * @param nugetPackages The NuGet package names to look for.
 * @returns The mapping of NuGet package names and their versions.
 */
export async function getVersionsOfNuGetPackages(
  projectFile: string,
  nugetPackages: string[],
): Promise<Record<string, string>> {
  try {
    // First check for the presence of a packages.config file
    const packagesConfigFile = path.join(
      path.dirname(projectFile),
      'packages.config',
    );
    const packagesConfigDoc = await readXmlFile(packagesConfigFile);
    return getVersionsFromPackagesConfig(packagesConfigDoc, nugetPackages);
  } catch {}

  try {
    const projectDoc = await readXmlFile(projectFile);
    return getVersionsFromProjectFile(projectDoc, nugetPackages);
  } catch {}

  return {};
}
