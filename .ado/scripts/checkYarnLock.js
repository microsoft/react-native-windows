// @ts-check
/**
 * Keeps yarn.lock free of package download URLs.
 *
 * Yarn records a download URL (`__archiveUrl`) when a registry serves a package
 * from another host, as PackageFeedProxy does with its ms-feed-* hosts.
 * Pipelines don't need these URLs: without them, Yarn downloads each package
 * from its configured registry. Dependabot can't reach those hosts, so a single
 * recorded URL fails its update jobs.
 *
 * Usage:
 *   node .ado/scripts/checkYarnLock.js        Fail if yarn.lock records a URL.
 *   node .ado/scripts/checkYarnLock.js --fix  Remove the recorded URLs and
 *                                             merge the entries they split.
 */
const {execSync} = require('child_process');
const fs = require('fs');
const path = require('path');

const repoRoot = path.resolve(__dirname, '../..');
const lockfilePath = path.join(repoRoot, 'yarn.lock');

// `name@npm:1.2.3::__archiveUrl=<url>`, or URL-encoded inside a patch locator.
const archiveUrlPattern =
  /::__archiveUrl=[^"&#\s]+|%3A%3A__archiveUrl=[^"&#\s]+/g;

// A binding followed by other bindings would need a different rewrite.
const unsupportedPattern = /__archiveUrl=[^"#\s]*(?:&|%26)/;

/**
 * @param {string} lockfile
 * @returns {string[]} names of the packages with a recorded download URL
 */
function findPackagesWithUrls(lockfile) {
  const names = new Set();
  for (const line of lockfile.split('\n')) {
    if (line.includes('__archiveUrl')) {
      const match = /resolution: "((?:@[^/"]+\/)?[^@"]+)@/.exec(line);
      names.add(match ? match[1] : line.trim());
    }
  }
  return [...names];
}

function main() {
  const fix = process.argv.includes('--fix');
  const lockfile = fs.readFileSync(lockfilePath, 'utf8');
  const packages = findPackagesWithUrls(lockfile);

  if (packages.length === 0) {
    console.log('yarn.lock records no download URLs.');
    return;
  }

  if (!fix) {
    const examples = packages.slice(0, 10).join(', ');
    const more = packages.length > 10 ? ', ...' : '';
    const errorPrefix = process.env.TF_BUILD
      ? '##vso[task.logissue type=error]'
      : '';
    console.error(
      `${errorPrefix}yarn.lock records download URLs for ${packages.length} ` +
        `packages (${examples}${more}). Dependabot can't reach those hosts. ` +
        'Run `node .ado/scripts/checkYarnLock.js --fix` and commit yarn.lock.',
    );
    process.exitCode = 1;
    return;
  }

  if (unsupportedPattern.test(lockfile)) {
    console.error(
      'yarn.lock has a download URL combined with other bindings; ' +
        'remove it manually.',
    );
    process.exitCode = 1;
    return;
  }

  const fixed = lockfile.replace(archiveUrlPattern, '');
  const unremoved = findPackagesWithUrls(fixed);
  if (unremoved.length > 0) {
    console.error(
      `Could not remove the download URLs for: ${unremoved.join(', ')}.`,
    );
    process.exitCode = 1;
    return;
  }
  fs.writeFileSync(lockfilePath, fixed);

  // Entries that differed only by their URL now share a resolution; Yarn merges
  // them. This mode skips the link step, so node_modules stays untouched.
  try {
    execSync('yarn install --mode=update-lockfile', {
      cwd: repoRoot,
      stdio: 'inherit',
    });
  } catch {
    console.error(
      'URLs removed, but `yarn install --mode=update-lockfile` failed. ' +
        'Run it again to merge the duplicate yarn.lock entries.',
    );
    process.exitCode = 1;
    return;
  }

  const readded = findPackagesWithUrls(fs.readFileSync(lockfilePath, 'utf8'));
  if (readded.length > 0) {
    console.error(
      `Yarn recorded download URLs again for: ${readded.join(', ')}.`,
    );
    process.exitCode = 1;
    return;
  }

  console.log(
    `Removed download URLs for ${packages.length} packages from yarn.lock.`,
  );
}

main();
