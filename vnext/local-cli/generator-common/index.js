const fs = require('fs');
const chalk = require('chalk');
const path = require('path');

/*
type Options = {
  echo?: string;
  ask?: string;
  value?: string;
  autocomplete?: string[] | Function;
};*/

const term = 13; // carriage return

function prompt(
  ask/*?: string | Options*/,
  value/*?: string | Options*/,
  opts/*?: Options*/,
) {
  let insert = 0;
  opts = opts || {};

  if (typeof ask === 'object') {
    opts = ask;
    ask = opts.ask;
  } else if (typeof value === 'object') {
    opts = value;
    value = opts.value;
  }
  ask = ask || '';
  const echo = opts.echo;
  const masked = 'echo' in opts;

  let fd;
  if (process.platform === 'win32') {
    // @ts-ignore
    fd = process.stdin.fd;
  } else {
    fd = fs.openSync('/dev/tty', 'rs');
  }

  const wasRaw = process.stdin.isRaw;
  if (!wasRaw && process.stdin.setRawMode) {
    process.stdin.setRawMode(true);
  }

  // eslint-disable-next-line no-undef
  let buf = Buffer.alloc(3);
  let str = '';

  let character;

  let read;

  if (ask) {
    process.stdout.write(ask);
  }

  while (true) {
    read = fs.readSync(fd, buf, 0, 3, null);
    if (read > 1) {
      // received a control sequence
      if (buf.toString()) {
        str += buf.toString();
        str = str.replace(/\0/g, '');
        insert = str.length;
        process.stdout.write(`\u001b[2K\u001b[0G${ask}${str}`);
        process.stdout.write(`\u001b[${insert + ask.length + 1}G`);
        // eslint-disable-next-line no-undef
        buf = Buffer.alloc(3);
      }
      continue; // any other 3 character sequence is ignored
    }

    // if it is not a control character seq, assume only one character is read
    character = buf[read - 1];

    // catch a ^C and return null
    if (character === 3) {
      process.stdout.write('^C\n');
      fs.closeSync(fd);
      process.exit(130);
      if (process.stdin.setRawMode) {
        process.stdin.setRawMode/*!*/(!!wasRaw);
      }
      return null;
    }

    // catch the terminating character
    if (character === term) {
      fs.closeSync(fd);
      break;
    }

    if (
      character === 127 ||
      (process.platform === 'win32' && character === 8)
    ) {
      // backspace
      if (!insert) {
        continue;
      }
      str = str.slice(0, insert - 1) + str.slice(insert);
      insert--;
      process.stdout.write('\u001b[2D');
    } else {
      if (character < 32 || character > 126) {
        continue;
      }
      str =
        str.slice(0, insert) +
        String.fromCharCode(character) +
        str.slice(insert);
      insert++;
    }

    if (masked) {
      process.stdout.write(
        `\u001b[2K\u001b[0G${ask}${Array(str.length + 1).join(echo)}`,
      );
    } else {
      process.stdout.write('\u001b[s');
      if (insert === str.length) {
        process.stdout.write(`\u001b[2K\u001b[0G${ask}${str}`);
      } else if (ask) {
        process.stdout.write(`\u001b[2K\u001b[0G${ask}${str}`);
      } else {
        process.stdout.write(
          `\u001b[2K\u001b[0G${str}\u001b[${str.length - insert}D`,
        );
      }
      process.stdout.write('\u001b[u');
      process.stdout.write('\u001b[1C');
    }
  }

  process.stdout.write('\n');

  if (process.stdin.setRawMode) {
    process.stdin.setRawMode(!!wasRaw);
  }

  return str || value || '';
}

function walk(current/*: string*/)/*: string[]*/ {
  if (!fs.lstatSync(current).isDirectory()) {
    return [current];
  }

  const files = fs
    .readdirSync(current)
    .map(child => walk(path.join(current, child)));
  const result/*: string[]*/ = [];
  return result.concat.apply([current], files);
}

// Binary files, don't process these (avoid decoding as utf8)
const binaryExtensions = ['.png', '.jar', '.keystore'];

// type ContentChangedCallbackOption = 'identical' | 'changed' | 'new' | null;

/**
 * Copy a file to given destination, replacing parts of its contents.
 * @param srcPath Path to a file to be copied.
 * @param destPath Destination path.
 * @param replacements: e.g. {'TextToBeReplaced': 'Replacement'}
 * @param contentChangedCallback
 *        Used when upgrading projects. Based on if file contents would change
 *        when being replaced, allows the caller to specify whether the file
 *        should be replaced or not.
 *        If null, files will be overwritten.
 *        Function(path, 'identical' | 'changed' | 'new') => 'keep' | 'overwrite'
 */
function copyAndReplace(
  srcPath /*: string*/,
  destPath /*: string*/,
  replacements /*: Record<string, string>*/,
  contentChangedCallback/*:
    | ((
        path: string,
        option: ContentChangedCallbackOption,
      ) => 'keep' | 'overwrite')
    | null*/,
) {
  if (fs.lstatSync(srcPath).isDirectory()) {
    if (!fs.existsSync(destPath)) {
      fs.mkdirSync(destPath);
    }
    // Not recursive
    return;
  }

  const extension = path.extname(srcPath);
  if (binaryExtensions.indexOf(extension) !== -1) {
    // Binary file
    let shouldOverwrite = 'overwrite';
    if (contentChangedCallback) {
      const newContentBuffer = fs.readFileSync(srcPath);
      let contentChanged/*: ContentChangedCallbackOption*/ = 'identical';
      try {
        const origContentBuffer = fs.readFileSync(destPath);
        // eslint-disable-next-line no-undef
        if (Buffer.compare(origContentBuffer, newContentBuffer) !== 0) {
          contentChanged = 'changed';
        }
      } catch (err) {
        if (err.code === 'ENOENT') {
          contentChanged = 'new';
        } else {
          throw err;
        }
      }
      shouldOverwrite = contentChangedCallback(destPath, contentChanged);
    }
    if (shouldOverwrite === 'overwrite') {
      copyBinaryFile(srcPath, destPath, err => {
        if (err) {
          throw err;
        }
      });
    }
  } else {
    // Text file
    const srcPermissions = fs.statSync(srcPath).mode;
    let content = fs.readFileSync(srcPath, 'utf8');
    Object.keys(replacements).forEach(regex => {
      content = content.replace(new RegExp(regex, 'g'), replacements[regex]);
    });

    let shouldOverwrite = 'overwrite';
    if (contentChangedCallback) {
      // Check if contents changed and ask to overwrite
      let contentChanged/*: ContentChangedCallbackOption*/ = 'identical';
      try {
        const origContent = fs.readFileSync(destPath, 'utf8');
        if (content !== origContent) {
          // logger.info('Content changed: ' + destPath);
          contentChanged = 'changed';
        }
      } catch (err) {
        if (err.code === 'ENOENT') {
          contentChanged = 'new';
        } else {
          throw err;
        }
      }
      shouldOverwrite = contentChangedCallback(destPath, contentChanged);
    }
    if (shouldOverwrite === 'overwrite') {
      fs.writeFileSync(destPath, content, {
        encoding: 'utf8',
        mode: srcPermissions,
      });
    }
  }
}

/**
 * Same as 'cp' on Unix. Don't do any replacements.
 */
function copyBinaryFile(
  srcPath/*: string*/,
  destPath/*: string*/,
  cb/*: (err?: Error) => void*/,
) {
  let cbCalled = false;
  const srcPermissions = fs.statSync(srcPath).mode;
  const readStream = fs.createReadStream(srcPath);
  readStream.on('error', err => {
    done(err);
  });
  const writeStream = fs.createWriteStream(destPath, {
    mode: srcPermissions,
  });
  writeStream.on('error', err => {
    done(err);
  });
  writeStream.on('close', () => {
    done();
  });
  readStream.pipe(writeStream);
  function done(err/*?: Error*/) {
    if (!cbCalled) {
      cb(err);
      cbCalled = true;
    }
  }
}

function createDir(destPath) {
  if (!fs.existsSync(destPath)) {
    fs.mkdirSync(destPath);
  }
}

function copyAndReplaceWithChangedCallback(srcPath, destRoot, relativeDestPath, replacements, alwaysOverwrite) {
  if (!replacements) {
    replacements = {};
  }
  const contentChangedCallback = alwaysOverwrite ? (_, contentChanged) =>
  alwaysOverwriteContentChangedCallback(
    srcPath,
    relativeDestPath,
    contentChanged
  ) : (_, contentChanged) =>
    upgradeFileContentChangedCallback(
      srcPath,
      relativeDestPath,
      contentChanged
    );

  copyAndReplace(
    srcPath,
    path.join(destRoot, relativeDestPath),
    replacements,
    contentChangedCallback
  );
}

function copyAndReplaceAll(srcPath, destPath, relativeDestDir, replacements, alwaysOverwrite) {
  walk(srcPath).forEach(absoluteSrcFilePath => {
    const filename = path.relative(srcPath, absoluteSrcFilePath);
    const relativeDestPath = path.join(relativeDestDir, filename);
    copyAndReplaceWithChangedCallback(absoluteSrcFilePath, destPath, relativeDestPath, replacements, alwaysOverwrite);
  });
}

function alwaysOverwriteContentChangedCallback(  absoluteSrcFilePath,
  relativeDestPath,
  contentChanged
) {
  if (contentChanged === 'new') {
    console.log(`${chalk.bold('new')} ${relativeDestPath}`);
    return 'overwrite';
  }
  if (contentChanged === 'changed') {
    console.log(`${chalk.bold('changed')} ${relativeDestPath} ${chalk.yellow('[overwriting]')}`);
    return 'overwrite';
  }
  if (contentChanged === 'identical') {
    return 'keep';
  }
  throw new Error(
    `Unknown file changed state: ${relativeDestPath}, ${contentChanged}`
  );
}

function upgradeFileContentChangedCallback(
  absoluteSrcFilePath,
  relativeDestPath,
  contentChanged
) {
  if (contentChanged === 'new') {
    console.log(`${chalk.bold('new')} ${relativeDestPath}`);
    return 'overwrite';
  }
  if (contentChanged === 'changed') {
    console.log(
      `${chalk.bold(relativeDestPath)} ` +
      `has changed in the new version.\nDo you want to keep your ${relativeDestPath} or replace it with the ` +
      'latest version?\nMake sure you have any changes you made to this file saved somewhere.\n' +
      `You can see the new version here: ${absoluteSrcFilePath}\n` +
      `Do you want to replace ${relativeDestPath}? ` +
      'Answer y to replace, n to keep your version: '
    );
    let answer;
    while (!answer) {
       answer = prompt();
    }

    if (answer.toLowerCase() === 'y' || answer.toLowerCase() === 'yes') {
      console.log(`Replacing ${relativeDestPath}`);
      return 'overwrite';
    }
    console.log(`Keeping your ${relativeDestPath}`);
    return 'keep';
  }
  if (contentChanged === 'identical') {
    return 'keep';
  }
  throw new Error(
    `Unknown file changed state: ${relativeDestPath}, ${contentChanged}`
  );
}

module.exports = {
  createDir, copyAndReplaceWithChangedCallback, copyAndReplaceAll,
};
