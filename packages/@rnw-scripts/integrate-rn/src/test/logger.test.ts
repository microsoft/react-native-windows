/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {PassThrough} from 'stream';
import {MarkdownLogger, ConsoleLogger} from '../Logger';

let outStream: NodeJS.WritableStream;
let loggerOuptput: string;

beforeEach(() => {
  loggerOuptput = '';
  outStream = new PassThrough();
  outStream.on('data', chunk => {
    loggerOuptput += chunk;
  });
});

test('Markdown Output', () => {
  const logger = new MarkdownLogger(outStream);

  logger.info('Commits: foo');
  logger.newTask('Updating dependencies');
  logger.success('Dependencies updated');
  logger.newTask('Updating overrides');
  logger.warn('Out of date overrides', '- A\n- B');
  logger.newTask('yarn build');
  logger.error('yarn build');

  logger.close();

  expect(loggerOuptput).toBe(`### ℹ Commits: foo
### ✅ Dependencies updated
### ⚠ Out of date overrides
\`\`\`
- A
- B
\`\`\`
### ❌ yarn build\n`);
});

beforeEach(() => {
  loggerOuptput = '';
});

test('Console Output', () => {
  const logger = new ConsoleLogger(outStream);

  logger.info('Commits: foo');
  logger.newTask('Updating dependencies');
  logger.success('Dependencies updated');
  logger.newTask('Updating overrides');
  logger.warn('Out of date overrides', '- A\n- B');
  logger.newTask('yarn build');
  logger.error('yarn build');

  logger.close();

  expect(stripAnsiColors(loggerOuptput)).toBe(
    `i Commits: foo
- Updating dependencies
√ Dependencies updated
- Updating overrides
‼ Out of date overrides
- A
- B
- yarn build
× yarn build
`,
  );
});

function stripAnsiColors(str: string) {
  // Taken from https://github.com/chalk/ansi-regex/blob/master/index.js
  const pattern = [
    '[\\u001B\\u009B][[\\]()#;?]*(?:(?:(?:[a-zA-Z\\d]*(?:;[-a-zA-Z\\d\\/#&.:=?%@~_]*)*)?\\u0007)',
    '(?:(?:\\d{1,4}(?:;\\d{0,4})*)?[\\dA-PR-TZcf-ntqry=><~]))',
  ].join('|');

  return str.replace(new RegExp(pattern, 'g'), '');
}
