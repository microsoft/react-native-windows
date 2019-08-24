/**
 * @format
 * @ts-check
 */
'use strict';

/// These constants control which files are formatted
const includeEndsWith = ['.h', '.cpp'];
const excludePathContains = ['cppwinrt/winrt'];
const excludePathEndsWith = ['.g.h', '.g.cpp'];

const path = require('path');
const {spawn, spawnSync, execSync} = require('child_process');
const async = require('async');
const {getNativeBinary} = require('clang-format');

const VERIFY_FLAG = '-verify';

function main() {
  // Run clang-format.
  try {
    let verify = process.argv.indexOf(VERIFY_FLAG) > 0;
    const args = process.argv.slice(2).filter(_ => _ !== VERIFY_FLAG);

    // Pass all arguments to clang-format, including e.g. -version etc.
    spawnClangFormat(args, verify ? queryNoOpenFiles : process.exit, 'inherit');
  } catch (e) {
    process.stdout.write(e.message);
    process.exit(1);
  }
}

function queryNoOpenFiles() {
  const opened = execSync('git status -s').toString();
  if (opened) {
    console.error('The following files have incorrect formatting:');
    console.error(opened);
    process.exit(2);
  }
}

function errorFromExitCode(exitCode) {
  return new Error(`clang-format exited with exit code ${exitCode}.`);
}

function git(args, options) {
  const results = spawnSync('git', args, options);

  if (results.status === 0) {
    return {
      stderr: results.stderr.toString().trim(),
      stdout: results.stdout.toString().trim(),
      success: true,
    };
  } else {
    return {
      stderr: results.stderr.toString().trim(),
      stdout: results.stdout.toString().trim(),
      success: false,
    };
  }
}

function listAllTrackedFiles(cwd) {
  const results = git(['ls-tree', '-r', '--name-only', '--full-tree', 'HEAD'], {
    cwd,
  });

  if (results.success) {
    return results.stdout.split('\n');
  }

  return [];
}

/**
 * Spawn the clang-format binary with given arguments.
 */
function spawnClangFormat(args, done, stdio) {
  // WARNING: This function's interface should stay stable across versions for the cross-version
  // loading below to work.
  let nativeBinary;

  try {
    nativeBinary = getNativeBinary();
  } catch (e) {
    setImmediate(done.bind(e));
    return;
  }

  let files = listAllTrackedFiles(path.resolve(__dirname, '../..'));

  // Apply file filters from constants
  files = files.filter(
    file =>
      includeEndsWith.some(_ => file.endsWith(_)) &&
      !excludePathContains.some(_ => file.indexOf(_) > 0) &&
      !excludePathEndsWith.some(_ => file.endsWith(_)),
  );

  // split file array into chunks of 30
  let i,
    j,
    chunks = [],
    chunkSize = 30;

  for (i = 0, j = files.length; i < j; i += chunkSize) {
    chunks.push(files.slice(i, i + chunkSize));
  }

  // launch a new process for each chunk
  async.series(
    chunks.map(function(chunk) {
      return function(callback) {
        const clangFormatProcess = spawn(nativeBinary, args.concat(chunk), {
          stdio: stdio,
        });
        clangFormatProcess.on('close', function(exit) {
          if (exit !== 0) {
            callback(errorFromExitCode(exit));
          } else {
            callback();
          }
        });
      };
    }),
    function(err) {
      if (err) {
        done(err);
        return;
      }
      console.log('\n');
      console.log(
        `ran clang-format on ${files.length} ${
          files.length === 1 ? 'file' : 'files'
        }`,
      );
      done();
    },
  );
}

if (require.main === module) {
  main();
}
