#!/usr/bin/env node
// @ts-check

// Groups packed npm tarballs into tag-specific folders so ESRP can publish with the
// correct productstate value per tag.

const fs = require('fs');
const path = require('path');

/**
 * @typedef {Object} PackageJsonBeachball
 * @property {string | undefined} [defaultNpmTag]
 */

/**
 * @typedef {Object} PackageJson
 * @property {string | undefined} [name]
 * @property {string | undefined} [version]
 * @property {boolean | undefined} [private]
 * @property {PackageJsonBeachball | undefined} [beachball]
 */

/**
 * @returns {{packRootArg: string, customRootArg: string, latestRootArg: string}}
 */
function ensureArgs() {
  const [, , packRootArg, customRootArg, latestRootArg] = process.argv;
  if (!packRootArg || !customRootArg || !latestRootArg) {
    console.error('Usage: node npmGroupByTag.js <packRoot> <customRoot> <latestRoot>');
    process.exit(1);
  }
  return {packRootArg, customRootArg, latestRootArg};
}

/**
 * @param {string} filePath
 * @returns {unknown}
 */
function readJson(filePath) {
  return JSON.parse(fs.readFileSync(filePath, 'utf8'));
}

/**
 * @param {string} pkgName
 * @param {string} version
 * @returns {string}
 */
function sanitizedTarballName(pkgName, version) {
  const prefix = pkgName.startsWith('@')
    ? pkgName.slice(1).replace(/\//g, '-').replace(/@/g, '-')
    : pkgName.replace(/@/g, '-');
  return `${prefix}-${version}.tgz`;
}

/**
 * @param {string} tarballName
 * @returns {string}
 */
function normalizePackedTarballName(tarballName) {
  // beachball prefixes packed tarballs with a monotonically increasing number to avoid collisions
  // when multiple packages share the same filename. Strip that prefix (single or repeated) for comparison.
  return tarballName.replace(/^(?:\d+[._-])+/u, '');
}

/**
 * @param {string} root
 * @returns {string[]}
 */
function findPackageJsons(root) {
  /** @type {string[]} */
  const results = [];
  /** @type {string[]} */
  const stack = [root];

  while (stack.length) {
    const current = stack.pop();
    if (!current) {
      continue;
    }
    /** @type {fs.Stats | undefined} */
    let stats;
    try {
      stats = fs.statSync(current);
    } catch (e) {
      continue;
    }

    if (!stats.isDirectory()) {
      continue;
    }

    const entries = fs.readdirSync(current, {withFileTypes: true});
    for (const entry of entries) {
      if (entry.name === 'node_modules' || entry.name === '.git') {
        continue;
      }
      const entryPath = path.join(current, entry.name);
      if (entry.isDirectory()) {
        stack.push(entryPath);
      } else if (entry.isFile() && entry.name === 'package.json') {
        results.push(entryPath);
      }
    }
  }

  return results;
}

/**
 * @param {string} name
 * @param {string} value
 */
function setPipelineVariable(name, value) {
  console.log(`##vso[task.setvariable variable=${name}]${value}`);
}

(function main() {
  const {packRootArg, customRootArg, latestRootArg} = ensureArgs();

  const repoRoot = process.env.BUILD_SOURCESDIRECTORY || process.cwd();
  const packRoot = path.resolve(packRootArg);
  const customRoot = path.resolve(customRootArg);
  const latestRoot = path.resolve(latestRootArg);

  fs.mkdirSync(customRoot, {recursive: true});
  fs.mkdirSync(latestRoot, {recursive: true});

  /** @type {string | null} */
  let customTag = null;
  try {
    const vnextPackageJson = /** @type {PackageJson} */ (
      readJson(path.join(repoRoot, 'vnext', 'package.json'))
    );
    const tagFromVnext = vnextPackageJson?.beachball?.defaultNpmTag;
    if (tagFromVnext && tagFromVnext !== 'latest') {
      customTag = tagFromVnext;
    }
  } catch (e) {
    console.warn('Unable to read vnext/package.json to determine custom tag.');
  }

  /** @type {string[]} */
  const tarballs = fs.existsSync(packRoot)
    ? fs.readdirSync(packRoot).filter(file => file.endsWith('.tgz'))
    : [];

  if (!tarballs.length) {
    setPipelineVariable('NpmCustomTag', customTag || '');
    setPipelineVariable('NpmCustomFolder', customRoot);
    setPipelineVariable('NpmCustomFolderHasContent', 'false');
    setPipelineVariable('NpmLatestFolder', latestRoot);
    setPipelineVariable('NpmLatestFolderHasContent', 'false');
    return;
  }

  /** @type {Set<string>} */
  const customTarballs = new Set();

  if (customTag) {
    for (const packageJsonPath of findPackageJsons(repoRoot)) {
      /** @type {PackageJson | undefined} */
      let pkg;
      try {
        pkg = /** @type {PackageJson} */ (readJson(packageJsonPath));
      } catch (e) {
        continue;
      }

      if (!pkg?.name || !pkg?.version) {
        continue;
      }

      const pkgTag = pkg?.beachball?.defaultNpmTag;
      if (pkgTag === customTag && pkg.private !== true) {
        customTarballs.add(sanitizedTarballName(pkg.name, pkg.version));
      }
    }
  }

  let customCount = 0;
  let latestCount = 0;

  for (const tarball of tarballs) {
    const sourcePath = path.join(packRoot, tarball);
    const normalizedName = normalizePackedTarballName(tarball);
    const destinationRoot = customTag && customTarballs.has(normalizedName) ? customRoot : latestRoot;
    const destinationPath = path.join(destinationRoot, tarball);
    fs.mkdirSync(path.dirname(destinationPath), {recursive: true});
    fs.renameSync(sourcePath, destinationPath);
    if (destinationRoot === customRoot) {
      customCount++;
    } else {
      latestCount++;
    }
  }

  setPipelineVariable('NpmCustomTag', customTag || '');
  setPipelineVariable('NpmCustomFolder', customRoot);
  setPipelineVariable('NpmCustomFolderHasContent', customCount ? 'true' : 'false');
  setPipelineVariable('NpmLatestFolder', latestRoot);
  setPipelineVariable('NpmLatestFolderHasContent', latestCount ? 'true' : 'false');
})();
