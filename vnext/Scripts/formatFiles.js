/**
 * @format
 * @ts-check
 */
'use strict';

const fs = require('fs');
const path = require('path');
const {spawn, execSync} = require('child_process');
const glob = require('glob');
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
