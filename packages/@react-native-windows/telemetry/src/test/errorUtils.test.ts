/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as appInsights from 'applicationinsights';

import * as errorUtils from '../utils/errorUtils';

test('tryGetErrorCode() with valid error code', () => {
  expect(errorUtils.tryGetErrorCode('foo bar error FOO2020: the thing')).toBe(
    'FOO2020',
  );
});

test('tryGetErrorCode() without valid error code', () => {
  expect(errorUtils.tryGetErrorCode('foo bar the thing')).toBeUndefined();
});

test('tryGetErrorCode() with word error but no code', () => {
  expect(errorUtils.tryGetErrorCode('test error')).toBeUndefined();
});

test('sanitizeErrorMessage() no-op on empty string', () => {
  expect(errorUtils.sanitizeErrorMessage('')).toBe('');
});

test('sanitizeErrorMessage() no-op on test string', () => {
  expect(errorUtils.sanitizeErrorMessage('some text')).toBe('some text');
});

test("sanitizeErrorMessage() 'project_dir'", () => {
  expect(
    errorUtils.sanitizeErrorMessage(`this is the cwd: '${process.cwd()}'`),
  ).toBe(`this is the cwd: [project_dir]\\???(${process.cwd().length})`);
});

test("sanitizeErrorMessage() 'project_dir' uppercase", () => {
  expect(
    errorUtils.sanitizeErrorMessage(
      `uppercase: '${process.cwd().toUpperCase()}'`,
    ),
  ).toBe(`uppercase: [project_dir]\\???(${process.cwd().length})`);
});

test("sanitizeErrorMessage() 'project_dir' lowercase", () => {
  expect(
    errorUtils.sanitizeErrorMessage(
      `lowercase: '${process.cwd().toLowerCase()}'`,
    ),
  ).toBe(`lowercase: [project_dir]\\???(${process.cwd().length})`);
});

test("sanitizeErrorMessage() 'project_dir' and something else", () => {
  expect(
    errorUtils.sanitizeErrorMessage(
      `this is the cwd: '${process.cwd()}' and something else`,
    ),
  ).toBe(
    `this is the cwd: [project_dir]\\???(${
      process.cwd().length
    }) and something else`,
  );
});

test('sanitizeErrorMessage() project_dir and something else', () => {
  expect(
    errorUtils.sanitizeErrorMessage(
      `this is the cwd: ${process.cwd()} and something else`,
    ),
  ).toBe(
    `this is the cwd: [project_dir]\\???(${
      (process.cwd() + ' and something else').length
    })`,
  );
});

test("sanitizeErrorMessage() 'node_modules'", () => {
  expect(
    errorUtils.sanitizeErrorMessage(
      `this is the cwd: '${process.cwd()}\\node_modules'`,
    ),
  ).toBe(
    `this is the cwd: [project_dir]\\???(${
      (process.cwd() + '\\node_modules').length
    })`,
  );
});

test("sanitizeErrorMessage() 'node_modules\\foo'", () => {
  expect(
    errorUtils.sanitizeErrorMessage(
      `this is the cwd: '${process.cwd()}\\node_modules\\foo'`,
    ),
  ).toBe(
    `this is the cwd: [node_modules]\\???(${
      (process.cwd() + '\\node_modules\\foo').length
    })`,
  );
});

test("sanitizeErrorMessage() 'node_modules\\foo' uppercase", () => {
  expect(
    errorUtils.sanitizeErrorMessage(
      `uppercase: '${process.cwd().toUpperCase()}\\NODE_MODULES\\foo'`,
    ),
  ).toBe(
    `uppercase: [node_modules]\\???(${
      (process.cwd() + '\\NODE_MODULES\\foo').length
    })`,
  );
});

test("sanitizeErrorMessage() 'node_modules\\foo' lowercase", () => {
  expect(
    errorUtils.sanitizeErrorMessage(
      `lowercase: '${process.cwd().toLowerCase()}\\NODE_MODULES\\'`,
    ),
  ).toBe(
    `lowercase: [node_modules]\\???(${
      (process.cwd() + '\\NODE_MODULES\\').length
    })`,
  );
});

test("sanitizeErrorMessage() 'node_modules\\' and something else", () => {
  expect(
    errorUtils.sanitizeErrorMessage(
      `trailing: '${process.cwd()}\\node_modules\\' and something else`,
    ),
  ).toBe(
    `trailing: [node_modules]\\???(${
      (process.cwd() + '\\node_modules\\').length
    }) and something else`,
  );
});

test('sanitizeErrorMessage() node_modules and something else that could be part of the path', () => {
  expect(
    errorUtils.sanitizeErrorMessage(
      `this is the cwd: ${process.cwd()}\\node_modules and something else that could be part of the path`,
    ),
  ).toBe(
    `this is the cwd: [project_dir]\\???(${
      (
        process.cwd() +
        '\\node_modules and something else that could be part of the path'
      ).length
    })`,
  );
});

test('sanitizeErrorMessage() \\node_modules\\ a file under nm', () => {
  expect(
    errorUtils.sanitizeErrorMessage(
      `this is the cwd: ${process.cwd()}\\node_modules\\ a file under nm`,
    ),
  ).toBe(
    `this is the cwd: [node_modules]\\???(${
      (process.cwd() + '\\node_modules\\ a file under nm').length
    })`,
  );
});

test('sanitizeErrorMessage() other path', () => {
  expect(
    errorUtils.sanitizeErrorMessage(`this is another path: A:\\foo\\bar\\baz`),
  ).toBe(`this is another path: [path]`);
});

test("sanitizeErrorMessage() 'other path'", () => {
  expect(
    errorUtils.sanitizeErrorMessage(
      `this is another path: 'A:\\foo\\bar\\baz'`,
    ),
  ).toBe(`this is another path: [path]`);
});

test('sanitizeErrorMessage() multiple known paths', () => {
  expect(
    errorUtils.sanitizeErrorMessage(
      `Cannot find module 'react-native/package.json'
      Require stack:
      - ${process.env.AppData}\\npm-cache\\_npx\\1384\\node_modules\\react-native-windows-init\\lib-commonjs\\Cli.js
      - ${process.env.AppData}\\npm-cache\\_npx\\1384\\node_modules\\react-native-windows-init\\bin.js`,
    ),
  ).toBe(`Cannot find module react-native/package.json
      Require stack:
      - [AppData]\\???(${
        (
          process.env.AppData +
          '\\npm-cache\\_npx\\1384\\node_modules\\react-native-windows-init\\lib-commonjs\\Cli.js'
        ).length
      })
      - [AppData]\\???(${
        (
          process.env.AppData +
          '\\npm-cache\\_npx\\1384\\node_modules\\react-native-windows-init\\bin.js'
        ).length
      })`);
});

test('sanitizeErrorMessage() forward slashes', () => {
  expect(
    errorUtils.sanitizeErrorMessage(
      `EPERM: operation not permitted, scandir  ${process.env.UserProfile!.replace(
        /\\/g,
        '/',
      )}/source/repos/rn2/wintest/windows/packages/boost.1.76.0.0/lib/native/include`,
    ),
  ).toBe(
    `EPERM: operation not permitted, scandir  [UserProfile]\\???(${
      (
        process.env.UserProfile +
        '/source/repos/rn2/wintest/windows/packages/boost.1.76.0.0/lib/native/include'
      ).length
    })`,
  );
});

test('sanitizeErrorMessage() file share path', () => {
  expect(errorUtils.sanitizeErrorMessage(`file here: \\\\server\\share`)).toBe(
    'file here: [path]',
  );
});

test('sanitizeErrorMessage() with cpu id', () => {
  expect(errorUtils.sanitizeErrorMessage('5>This is an error')).toBe(
    'This is an error',
  );
});

test('sanitizeErrorMessage() with cpu/thread id', () => {
  expect(errorUtils.sanitizeErrorMessage('5:42>This is an error')).toEqual(
    'This is an error',
  );
});

test('sanitizeErrorStackFrame() with empty frame', () => {
  const emptyFrame: appInsights.Contracts.StackFrame = {
    level: 0,
    method: '',
    fileName: '',
    assembly: 'asdf',
    line: 0,
  };
  errorUtils.sanitizeErrorStackFrame(emptyFrame);
  expect(emptyFrame).toEqual({
    level: 0,
    assembly: '',
    fileName: '.',
    method: '',
    line: 0,
  });
});

test('sanitizeErrorStackFrame() with assembly name', () => {
  const frame1: appInsights.Contracts.StackFrame = {
    method: '',
    fileName: `${process.cwd()}\\foo.js`,
    assembly: 'asdf',
    level: 0,
    line: 0,
  };
  errorUtils.sanitizeErrorStackFrame(frame1);
  expect(frame1).toEqual({
    assembly: '',
    fileName: 'telemetry\\foo.js',
    method: '',
    level: 0,
    line: 0,
  });
});

test('sanitizeErrorStackFrame() with method name', () => {
  const frame2: appInsights.Contracts.StackFrame = {
    method: `myMethod (something ${process.cwd()}`,
    fileName: `${process.cwd()}\\foo.js`,
    assembly: 'asdf',
    level: 1,
    line: 42,
  };
  errorUtils.sanitizeErrorStackFrame(frame2);
  expect(frame2).toEqual({
    assembly: '',
    fileName: 'telemetry\\foo.js',
    method: 'myMethod',
    level: 1,
    line: 42,
  });
});
