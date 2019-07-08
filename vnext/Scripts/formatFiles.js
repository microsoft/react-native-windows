/**
 * @format
 * @ts-check
 */
'use strict';

const fs = require('fs');
const path = require('path');
const spawn = require('child_process').spawn;
const glob = require('glob');
const async = require('async');
const {getNativeBinary} = require('clang-format');

function main() {
  // Run clang-format.
  try {
    // Pass all arguments to clang-format, including e.g. -version etc.
    spawnClangFormat(process.argv.slice(2), process.exit, 'inherit');
  } catch (e) {
    process.stdout.write(e.message);
    process.exit(1);
  }
}

function errorFromExitCode(exitCode) {
  return new Error(`clang-format exited with exit code ${exitCode}.`);
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

  // extract glob, if present
  //const filesGlob = getGlobArg(args);
  const allFilesGlobs = ['**/*.cpp', '**/*.h'];

  const ignoreGlobs = fs
    .readFileSync(path.resolve(__dirname, '../../.clang-format-ignore'))
    .toString()
    .split('\n');

  let files = [];
  allFilesGlobs.forEach(filesGlob => {
    files = files.concat(glob.sync(filesGlob, {ignore: ignoreGlobs}));
  });

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
