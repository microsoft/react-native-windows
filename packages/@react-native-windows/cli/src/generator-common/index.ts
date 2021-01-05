/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import * as fs from 'fs';
import * as chalk from 'chalk';
import * as inquirer from 'inquirer';
import * as path from 'path';
import * as mustache from 'mustache';
import {CodedError} from '@react-native-windows/telemetry';

/**
 * Text to replace, + config options
 */
export type Replacements = {
  useMustache?: boolean;
  regExpPatternsToRemove?: RegExp[];
  [key: string]: any;
};

function walk(current: string): string[] {
  if (!fs.lstatSync(current).isDirectory()) {
    return [current];
  }

  const files = fs
    .readdirSync(current)
    .map(child => walk(path.join(current, child)));
  const result: string[] = [];
  return result.concat.apply([current], files);
}

/**
 * Get a source file and replace parts of its contents.
 * @param srcPath Path to the source file.
 * @param replacements e.g. {'TextToBeReplaced': 'Replacement'}
 * @return The contents of the file with the replacements applied.
 */
export function resolveContents(
  srcPath: string,
  replacements: Replacements,
): string {
  let content = fs.readFileSync(srcPath, 'utf8');

  if (content.includes('\r\n')) {
    // CRLF file, make sure multiline replacements are also CRLF
    for (const key of Object.keys(replacements)) {
      if (typeof replacements[key] === 'string') {
        replacements[key] = replacements[key].replace(/(?<!\r)\n/g, '\r\n');
      }
    }
  } else {
    // LF file, make sure multiline replacements are also LF
    for (const key of Object.keys(replacements)) {
      if (typeof replacements[key] === 'string') {
        replacements[key] = replacements[key].replace(/\r\n/g, '\n');
      }
    }
  }

  if (replacements.useMustache) {
    content = mustache.render(content, replacements);
    (replacements.regExpPatternsToRemove || []).forEach(regexPattern => {
      content = content.replace(new RegExp(regexPattern, 'g'), '');
    });
  } else {
    Object.keys(replacements).forEach(regex => {
      content = content.replace(new RegExp(regex, 'g'), replacements[regex]);
    });
  }
  return content;
}

// Binary files, don't process these (avoid decoding as utf8)
const binaryExtensions = ['.png', '.jar', '.keystore'];

type ContentChangedCallbackOption = 'identical' | 'changed' | 'new' | null;

type ContentChangedCallback = (
  path: string,
  option: ContentChangedCallbackOption,
) => Promise<'keep' | 'overwrite'>;

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
export async function copyAndReplace(
  srcPath: string,
  destPath: string,
  replacements: Replacements,
  contentChangedCallback: ContentChangedCallback | null,
) {
  if (fs.lstatSync(srcPath).isDirectory()) {
    if (!fs.existsSync(destPath)) {
      fs.mkdirSync(destPath);
    }
    // Not recursive
    return;
  }

  const extension = path.extname(srcPath);
  if (binaryExtensions.includes(extension)) {
    // Binary file
    let shouldOverwrite = 'overwrite';
    if (contentChangedCallback) {
      const newContentBuffer = fs.readFileSync(srcPath);
      let contentChanged: ContentChangedCallbackOption = 'identical';
      try {
        const origContentBuffer = fs.readFileSync(destPath);
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
      shouldOverwrite = await contentChangedCallback(destPath, contentChanged);
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
    const content = resolveContents(srcPath, replacements);

    let shouldOverwrite = 'overwrite';
    if (contentChangedCallback) {
      // Check if contents changed and ask to overwrite
      let contentChanged: ContentChangedCallbackOption = 'identical';
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
      shouldOverwrite = await contentChangedCallback(destPath, contentChanged);
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
  srcPath: string,
  destPath: string,
  cb: (err?: Error) => void,
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
  function done(err?: Error) {
    if (!cbCalled) {
      cb(err);
      cbCalled = true;
    }
  }
}

export function createDir(destPath: string) {
  if (!fs.existsSync(destPath)) {
    fs.mkdirSync(destPath);
  }
}

export async function copyAndReplaceWithChangedCallback(
  srcPath: string,
  destRoot: string,
  relativeDestPath: string,
  replacements?: Record<string, string>,
  alwaysOverwrite?: boolean,
) {
  if (!replacements) {
    replacements = {};
  }
  const contentChangedCallback: ContentChangedCallback = alwaysOverwrite
    ? (_, contentChanged) =>
        alwaysOverwriteContentChangedCallback(
          srcPath,
          relativeDestPath,
          contentChanged,
        )
    : (_, contentChanged) =>
        upgradeFileContentChangedCallback(
          srcPath,
          relativeDestPath,
          contentChanged,
        );

  await copyAndReplace(
    srcPath,
    path.join(destRoot, relativeDestPath),
    replacements,
    contentChangedCallback,
  );
}

export async function copyAndReplaceAll(
  srcPath: string,
  destPath: string,
  relativeDestDir: string,
  replacements: Replacements,
  alwaysOverwrite: boolean,
) {
  for (const absoluteSrcFilePath of walk(srcPath)) {
    const filename = path.relative(srcPath, absoluteSrcFilePath);
    const relativeDestPath = path.join(relativeDestDir, filename);
    await copyAndReplaceWithChangedCallback(
      absoluteSrcFilePath,
      destPath,
      relativeDestPath,
      replacements,
      alwaysOverwrite,
    );
  }
}

async function alwaysOverwriteContentChangedCallback(
  absoluteSrcFilePath: string,
  relativeDestPath: string,
  contentChanged: ContentChangedCallbackOption,
): Promise<'keep' | 'overwrite'> {
  if (contentChanged === 'new') {
    console.log(`${chalk.bold('new')} ${relativeDestPath}`);
    return 'overwrite';
  }
  if (contentChanged === 'changed') {
    console.log(
      `${chalk.bold('changed')} ${relativeDestPath} ${chalk.yellow(
        '[overwriting]',
      )}`,
    );
    return 'overwrite';
  }
  if (contentChanged === 'identical') {
    return 'keep';
  }
  throw new CodedError(
    'Autolinking',
    `Unknown file changed state: ${relativeDestPath}, ${contentChanged}`,
  );
}

async function upgradeFileContentChangedCallback(
  absoluteSrcFilePath: string,
  relativeDestPath: string,
  contentChanged: ContentChangedCallbackOption,
): Promise<'keep' | 'overwrite'> {
  if (contentChanged === 'new') {
    console.log(`${chalk.bold('new')} ${relativeDestPath}`);
    return 'overwrite';
  }
  if (contentChanged === 'changed') {
    console.log(
      `${chalk.bold(relativeDestPath)} ` +
        `has changed in the new version.\nDo you want to keep your ${relativeDestPath} or replace it with the ` +
        'latest version?\nMake sure you have any changes you made to this file saved somewhere.\n' +
        `You can see the new version here: ${absoluteSrcFilePath}`,
    );

    const {shouldReplace} = await inquirer.prompt([
      {
        name: 'shouldReplace',
        type: 'confirm',
        message: `Do you want to replace ${relativeDestPath}?`,
        default: false,
      },
    ]);

    return shouldReplace ? 'overwrite' : 'keep';
  }
  if (contentChanged === 'identical') {
    return 'keep';
  }
  throw new CodedError(
    'Autolinking',
    `Unknown file changed state: ${relativeDestPath}, ${contentChanged}`,
  );
}
