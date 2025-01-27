/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

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
  ).toBe(`this is the cwd: [project_dir]`);
});

test("sanitizeErrorMessage() 'project_dir' uppercase", () => {
  expect(
    errorUtils.sanitizeErrorMessage(
      `uppercase: '${process.cwd().toUpperCase()}'`,
    ),
  ).toBe(`uppercase: [project_dir]`);
});

test("sanitizeErrorMessage() 'project_dir' lowercase", () => {
  expect(
    errorUtils.sanitizeErrorMessage(
      `lowercase: '${process.cwd().toLowerCase()}'`,
    ),
  ).toBe(`lowercase: [project_dir]`);
});

test("sanitizeErrorMessage() 'project_dir' and something else", () => {
  expect(
    errorUtils.sanitizeErrorMessage(
      `this is the cwd: '${process.cwd()}' and something else`,
    ),
  ).toBe(`this is the cwd: [project_dir] and something else`);
});

test('sanitizeErrorMessage() project_dir and something else', () => {
  expect(
    errorUtils.sanitizeErrorMessage(
      `this is the cwd: ${process.cwd()} and something else`,
    ),
  ).toBe(
    `this is the cwd: [project_dir]\\???(${' and something else'.length})`,
  );
});

test("sanitizeErrorMessage() 'node_modules'", () => {
  expect(
    errorUtils.sanitizeErrorMessage(
      `this is the cwd: '${process.cwd()}\\node_modules'`,
    ),
  ).toBe(`this is the cwd: [project_dir]\\???(${'node_modules'.length})`);
});

test("sanitizeErrorMessage() 'node_modules\\foo'", () => {
  expect(
    errorUtils.sanitizeErrorMessage(
      `this is the cwd: '${process.cwd()}\\node_modules\\foo'`,
    ),
  ).toBe(`this is the cwd: [node_modules]\\???(${'foo'.length})`);
});

test("sanitizeErrorMessage() 'node_modules\\foo' uppercase", () => {
  expect(
    errorUtils.sanitizeErrorMessage(
      `uppercase: '${process.cwd().toUpperCase()}\\NODE_MODULES\\foo'`,
    ),
  ).toBe(`uppercase: [node_modules]\\???(${'foo'.length})`);
});

test("sanitizeErrorMessage() 'node_modules\\foo' lowercase", () => {
  expect(
    errorUtils.sanitizeErrorMessage(
      `lowercase: '${process.cwd().toLowerCase()}\\NODE_MODULES\\foo'`,
    ),
  ).toBe(`lowercase: [node_modules]\\???(${'foo'.length})`);
});

test("sanitizeErrorMessage() 'node_modules\\' and something else", () => {
  expect(
    errorUtils.sanitizeErrorMessage(
      `trailing: '${process.cwd()}\\node_modules\\' and something else`,
    ),
  ).toBe(`trailing: [node_modules]\\???(0) and something else`);
});

test('sanitizeErrorMessage() node_modules and something else that could be part of the path', () => {
  expect(
    errorUtils.sanitizeErrorMessage(
      `this is the cwd: ${process.cwd()}\\node_modules and something else that could be part of the path`,
    ),
  ).toBe(
    `this is the cwd: [project_dir]\\???(${
      'node_modules and something else that could be part of the path'.length
    })`,
  );
});

test('sanitizeErrorMessage() \\node_modules\\ a file under nm', () => {
  expect(
    errorUtils.sanitizeErrorMessage(
      `this is the cwd: ${process.cwd()}\\node_modules\\ a file under nm`,
    ),
  ).toBe(`this is the cwd: [node_modules]\\???(${' a file under nm'.length})`);
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

test('sanitizeErrorMessage() tracked packages in the npx cache', () => {
  expect(
    errorUtils.sanitizeErrorMessage(
      `Cannot find module 'react-native/package.json'
      Require stack:
      - ${process.env.AppData}\\npm-cache\\_npx\\1384\\node_modules\\react-native-windows-init\\lib-commonjs\\Cli.js
      - ${process.env.AppData}\\npm-cache\\_npx\\1384\\node_modules\\react-native-windows-init\\bin.js`,
    ),
  ).toBe(`Cannot find module react-native/package.json
      Require stack:
      - [node_modules]\\react-native-windows-init\\lib-commonjs\\Cli.js
      - [node_modules]\\react-native-windows-init\\bin.js`);
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
      '/source/repos/rn2/wintest/windows/packages/boost.1.76.0.0/lib/native/include'
        .length
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

test('sanitizeErrorMessage() with standard MSBuild error', () => {
  expect(
    errorUtils.sanitizeErrorMessage(
      `2:6>C:\\Program Files (x86)\\Windows Kits\\10\\bin\\10.0.22621.0\\XamlCompiler\\Microsoft.Windows.UI.Xaml.Common.targets(486,5): error MSB4181: The "CompileXaml" task returned false but did not log an error. [${process.cwd()}\\windows\\teltest68\\teltest68.csproj]`,
    ),
  ).toEqual(
    `[path](486,5): error MSB4181: The CompileXaml task returned false but did not log an error. [windows]\\???.csproj(${
      'teltest68\\teltest68.csproj'.length
    })`,
  );
});

test('sanitizeErrorStackFrame() with empty frame', () => {
  const stackFrame = {
    functionName: '',
    filePath: '',
    lineNumber: 0,
    columnNumber: 0,
  };
  errorUtils.sanitizeErrorStackFrame(stackFrame);
  expect(stackFrame).toEqual({
    functionName: '',
    filePath: '[path]',
    lineNumber: 0,
    columnNumber: 0,
  });
});

test('sanitizeErrorStackFrame() with assembly name', () => {
  const stackFrame = {
    functionName: '',
    filePath: `${process.cwd()}\\foo.js`,
    lineNumber: 10,
    columnNumber: 14,
  };
  errorUtils.sanitizeErrorStackFrame(stackFrame);
  expect(stackFrame).toEqual({
    functionName: '',
    filePath: '[project_dir]\\???.js(6)',
    lineNumber: 10,
    columnNumber: 14,
  });
});

test('sanitizeErrorStackFrame() with method name', () => {
  const stackFrame = {
    functionName: `myMethod (something ${process.cwd()}`,
    filePath: `${process.cwd()}\\telemetry\\foo.js`,
    lineNumber: 10,
    columnNumber: 14,
  };
  errorUtils.sanitizeErrorStackFrame(stackFrame);
  expect(stackFrame).toEqual({
    functionName: 'myMethod',
    filePath: '[project_dir]\\???.js(16)',
    lineNumber: 10,
    columnNumber: 14,
  });
});
