#!/usr/bin/env node
// @ts-check

/**
 * npmPack.js - Pack all non-private workspace packages to tgz files
 *
 * Usage:
 *   node npmPack.js [targetDir] [--clean]
 *
 * Arguments:
 *   targetDir - Target directory for .tgz files (default: npm-pkgs in repo root)
 *   --clean   - Clean target directory if it's not empty
 */

const fs = require('fs');
const path = require('path');
const { execSync } = require('child_process');

/**
 * Find the enlistment root by going up two directories from script location
 * @returns {string} Repository root path
 */
function findEnlistmentRoot() {
  const scriptDir = __dirname;
  const repoRoot = path.resolve(scriptDir, '..', '..');

  // Verify this is the repo root by checking for package.json
  const packageJsonPath = path.join(repoRoot, 'package.json');
  if (!fs.existsSync(packageJsonPath)) {
    throw new Error(`Could not find package.json at ${packageJsonPath}`);
  }

  return repoRoot;
}

/**
 * Get workspace package paths from root package.json
 * @param {string} repoRoot - Repository root directory
 * @returns {string[]} Array of workspace patterns
 */
function getWorkspacePackages(repoRoot) {
  const packageJsonPath = path.join(repoRoot, 'package.json');
  const packageJson = JSON.parse(fs.readFileSync(packageJsonPath, 'utf8'));

  if (!packageJson.workspaces || !packageJson.workspaces.packages) {
    throw new Error('No workspaces.packages found in root package.json');
  }

  return packageJson.workspaces.packages;
}

/**
 * Recursively find all package.json files in a directory
 * @param {string} dir - Directory to search
 * @param {string[]} results - Accumulated results
 * @returns {string[]} Array of package.json file paths
 */
function findPackageJsonsRecursive(dir, results = []) {
  if (!fs.existsSync(dir)) {
    return results;
  }

  const entries = fs.readdirSync(dir, { withFileTypes: true });

  for (const entry of entries) {
    const fullPath = path.join(dir, entry.name);

    if (entry.isDirectory()) {
      // Skip node_modules directories
      if (entry.name === 'node_modules') {
        continue;
      }
      findPackageJsonsRecursive(fullPath, results);
    } else if (entry.isFile() && entry.name === 'package.json') {
      results.push(fullPath);
    }
  }

  return results;
}

/**
 * Match a pattern against a path
 * Supports patterns like "packages/*" or "packages/@react-native-windows/*"
 * @param {string} pattern - Workspace pattern to match
 * @param {string} basePath - Base path to resolve pattern from
 * @returns {string[]} Array of matching package.json paths
 */
function matchPattern(pattern, basePath) {
  // Remove trailing /* if present
  const cleanPattern = pattern.replace(/\/\*$/, '');
  const patternPath = path.join(basePath, cleanPattern);

  const results = [];

  // Check if pattern ends with /*
  if (pattern.endsWith('/*')) {
    // Pattern like "packages/*" - find all direct subdirectories
    if (fs.existsSync(patternPath)) {
      const entries = fs.readdirSync(patternPath, { withFileTypes: true });
      for (const entry of entries) {
        if (entry.isDirectory()) {
          const packageJsonPath = path.join(patternPath, entry.name, 'package.json');
          if (fs.existsSync(packageJsonPath)) {
            results.push(packageJsonPath);
          }
        }
      }
    }
  } else {
    // Exact path - check if package.json exists
    const packageJsonPath = path.join(patternPath, 'package.json');
    if (fs.existsSync(packageJsonPath)) {
      results.push(packageJsonPath);
    }
  }

  return results;
}

/**
 * Find all package.json files matching workspace patterns
 * @param {string} repoRoot - Repository root directory
 * @param {string[]} workspacePatterns - Array of workspace patterns
 * @returns {string[]} Array of package.json file paths
 */
function findWorkspacePackageJsons(repoRoot, workspacePatterns) {
  const packageJsonPaths = [];

  for (const pattern of workspacePatterns) {
    const matches = matchPattern(pattern, repoRoot);
    packageJsonPaths.push(...matches);
  }

  return packageJsonPaths;
}

/**
 * Check if a package is private
 * @param {string} packageJsonPath - Path to package.json file
 * @returns {boolean} True if package is private
 */
function isPrivatePackage(packageJsonPath) {
  const packageJson = JSON.parse(fs.readFileSync(packageJsonPath, 'utf8'));
  return packageJson.private === true;
}

/**
 * Pack a package using npm pack
 * @param {string} packageDir - Directory containing the package
 * @param {string} targetDir - Directory to output .tgz file
 * @returns {boolean} True if packing succeeded
 */
function packPackage(packageDir, targetDir) {
  const packageJsonPath = path.join(packageDir, 'package.json');
  const packageJson = JSON.parse(fs.readFileSync(packageJsonPath, 'utf8'));
  const packageName = packageJson.name;

  console.log(`Packing ${packageName}...`);

  try {
    // Run npm pack in the package directory, output to target directory
    const output = execSync(`npm pack --pack-destination "${targetDir}"`, {
      cwd: packageDir,
      encoding: 'utf8',
      stdio: ['pipe', 'pipe', 'pipe']
    });

    const tgzFileName = output.trim().split('\n').pop();
    console.log(`  ✓ Created ${tgzFileName}`);
    return true;
  } catch (error) {
    const message = error instanceof Error ? error.message : String(error);
    console.error(`  ✗ Failed to pack ${packageName}: ${message}`);
    return false;
  }
}

/**
 * Main function
 */
function main() {
  const args = process.argv.slice(2);
  const cleanFlag = args.includes('--clean');
  const targetDirArg = args.find(arg => !arg.startsWith('--'));

  try {
    // Find repo root
    const repoRoot = findEnlistmentRoot();
    console.log(`Repository root: ${repoRoot}`);

    // Determine target directory
    const targetDir = targetDirArg
      ? path.resolve(repoRoot, targetDirArg)
      : path.join(repoRoot, 'npm-pkgs');

    console.log(`Target directory: ${targetDir}`);

    // Handle target directory
    if (fs.existsSync(targetDir)) {
      const files = fs.readdirSync(targetDir);
      if (files.length > 0) {
        if (!cleanFlag) {
          console.error(`Error: Target directory is not empty: ${targetDir}`);
          console.error('Use --clean flag to clean the directory before packing');
          process.exit(1);
        }

        console.log('Cleaning target directory...');
        for (const file of files) {
          fs.rmSync(path.join(targetDir, file), { recursive: true, force: true });
        }
      }
    } else {
      console.log('Creating target directory...');
      fs.mkdirSync(targetDir, { recursive: true });
    }

    // Get workspace packages
    const workspacePatterns = getWorkspacePackages(repoRoot);
    console.log(`Workspace patterns: ${workspacePatterns.join(', ')}`);

    // Find all package.json files
    const packageJsonPaths = findWorkspacePackageJsons(repoRoot, workspacePatterns);
    console.log(`Found ${packageJsonPaths.length} workspace packages\n`);

    // Pack non-private packages
    let packedCount = 0;
    let skippedCount = 0;
    let failedCount = 0;

    for (const packageJsonPath of packageJsonPaths) {
      const packageDir = path.dirname(packageJsonPath);

      if (isPrivatePackage(packageJsonPath)) {
        const packageJson = JSON.parse(fs.readFileSync(packageJsonPath, 'utf8'));
        console.log(`Skipping private package: ${packageJson.name}`);
        skippedCount++;
        continue;
      }

      const success = packPackage(packageDir, targetDir);
      if (success) {
        packedCount++;
      } else {
        failedCount++;
      }
    }

    console.log(`\n✓ Packing complete:`);
    console.log(`  Packed: ${packedCount}`);
    console.log(`  Skipped (private): ${skippedCount}`);
    console.log(`  Failed: ${failedCount}`);
    console.log(`  Target: ${targetDir}`);

    if (failedCount > 0) {
      process.exit(1);
    }
  } catch (error) {
    const message = error instanceof Error ? error.message : String(error);
    console.error(`Error: ${message}`);
    process.exit(1);
  }
}

// Run main function
main();
