/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as telemetry from '../telemetry';
import * as appInsights from 'applicationinsights';
import {basename} from 'path';

telemetry.telClient.config.disableAppInsights = true;

test('Sanitize message, no-op', () => {
  // do stuff
  expect(telemetry.sanitizeMessage('')).toEqual('');
  expect(telemetry.sanitizeMessage('some text')).toEqual('some text');
});

test('Sanitize message, project_dir', () => {
  expect(
    telemetry.sanitizeMessage(`this is the cwd: '${process.cwd()}'`),
  ).toEqual(`this is the cwd:  [project_dir]\\???(${process.cwd().length})`);
  expect(
    telemetry.sanitizeMessage(`uppercase: '${process.cwd().toUpperCase()}'`),
  ).toEqual(`uppercase:  [project_dir]\\???(${process.cwd().length})`);
  expect(
    telemetry.sanitizeMessage(`lowercase: '${process.cwd().toLowerCase()}'`),
  ).toEqual(`lowercase:  [project_dir]\\???(${process.cwd().length})`);
  expect(
    telemetry.sanitizeMessage(
      `this is the cwd: '${process.cwd()}' and something else`,
    ),
  ).toEqual(
    `this is the cwd:  [project_dir]\\???(${
      process.cwd().length
    })  and something else`,
  );
  expect(
    telemetry.sanitizeMessage(
      `this is the cwd: ${process.cwd()} and something else`,
    ),
  ).toEqual(
    `this is the cwd: [project_dir]\\???(${
      (process.cwd() + ' and something else').length
    })`,
  );
});

test('Sanitize message, node_modules', () => {
  expect(
    telemetry.sanitizeMessage(
      `this is the cwd: '${process.cwd()}\\node_modules'`,
    ),
  ).toEqual(
    `this is the cwd:  [project_dir]\\???(${
      (process.cwd() + '\\node_modules').length
    })`,
  );
  expect(
    telemetry.sanitizeMessage(
      `this is the cwd: '${process.cwd()}\\node_modules\\foo'`,
    ),
  ).toEqual('this is the cwd:  node_modules\\foo');
  expect(
    telemetry.sanitizeMessage(
      `uppercase: '${process.cwd().toUpperCase()}\\NODE_MODULES\\foo'`,
    ),
  ).toEqual('uppercase:  node_modules\\foo');
  expect(
    telemetry.sanitizeMessage(
      `lowercase: '${process.cwd().toLowerCase()}\\NODE_MODULES\\'`,
    ),
  ).toEqual('lowercase:  node_modules\\');
  expect(
    telemetry.sanitizeMessage(
      `trailing: '${process.cwd()}\\node_modules\\' and something else`,
    ),
  ).toEqual('trailing:  node_modules\\  and something else');
  expect(
    telemetry.sanitizeMessage(
      `this is the cwd: ${process.cwd()}\\node_modules and something else that could be part of the path`,
    ),
  ).toEqual(
    `this is the cwd: [project_dir]\\???(${
      (
        process.cwd() +
        '\\node_modules and something else that could be part of the path'
      ).length
    })`,
  );
  expect(
    telemetry.sanitizeMessage(
      `this is the cwd: ${process.cwd()}\\node_modules\\ a file under nm`,
    ),
  ).toEqual(`this is the cwd: node_modules\\ a file under nm`);
});

test('Sanitize message, other path', () => {
  expect(
    telemetry.sanitizeMessage(`this is another path: 'A:\\foo\\bar\\baz'`),
  ).toEqual(`this is another path:  [path]`);

  expect(
    telemetry.sanitizeMessage(`this is another path: A:\\foo\\bar\\baz`),
  ).toEqual(`this is another path: [path]`);

  expect(
    telemetry.sanitizeMessage(
      `Cannot find module 'react-native/package.json'
      Require stack:
      - ${process.env.APPDATA}\\npm-cache\\_npx\\1384\\node_modules\\react-native-windows-init\\lib-commonjs\\Cli.js
      - ${process.env.APPDATA}\\npm-cache\\_npx\\1384\\node_modules\\react-native-windows-init\\bin.js`,
    ),
  ).toEqual(`Cannot find module  react-native/package.json 
      Require stack:
      - [appdata]\\???(${
        (
          process.env.APPDATA +
          '\\npm-cache\\_npx\\1384\\node_modules\\react-native-windows-init\\lib-commonjs\\Cli.js'
        ).length
      })
      - [appdata]\\???(${
        (
          process.env.APPDATA +
          '\\npm-cache\\_npx\\1384\\node_modules\\react-native-windows-init\\bin.js'
        ).length
      })`);
});

test('Sanitize stack frame', () => {
  const emptyFrame: appInsights.Contracts.StackFrame = {
    level: 0,
    method: '',
    fileName: '',
    assembly: 'asdf',
    line: 0,
  };
  telemetry.sanitizeFrame(emptyFrame);
  expect(emptyFrame).toEqual({
    level: 0,
    assembly: '',
    fileName: '.',
    method: '',
    line: 0,
  });

  const frame1: appInsights.Contracts.StackFrame = {
    method: '',
    fileName: `${process.cwd()}\\foo.js`,
    assembly: 'asdf',
    level: 0,
    line: 0,
  };
  telemetry.sanitizeFrame(frame1);
  expect(frame1).toEqual({
    assembly: '',
    fileName: 'telemetry\\foo.js',
    method: '',
    level: 0,
    line: 0,
  });

  const frame2: appInsights.Contracts.StackFrame = {
    method: `myMethod (something ${process.cwd()}`,
    fileName: `${process.cwd()}\\foo.js`,
    assembly: 'asdf',
    level: 1,
    line: 42,
  };
  telemetry.sanitizeFrame(frame2);
  expect(frame2).toEqual({
    assembly: '',
    fileName: 'telemetry\\foo.js',
    method: 'myMethod',
    level: 1,
    line: 42,
  });
});

test('basic setup', () => {
  expect(telemetry.telClient.commonProperties.sessionId).toBeDefined();

  expect(
    telemetry.telClient.commonProperties.sessionId.length,
  ).toBeGreaterThanOrEqual(32);

  expect(telemetry.telClient.commonProperties.isTest).toEqual('true');
});

function b(s: string) {
  throw new Error('hello ' + s);
}
function a(s: string) {
  b(s);
}

test('thrown exception a->b, hello world', done => {
  let pass = false;
  telemetry.telClient.addTelemetryProcessor((envelope, _) => {
    if (envelope.data.baseType === 'ExceptionData') {
      const data = (envelope.data as any).baseData;
      expect(data.exceptions).toBeDefined();
      expect(data.exceptions.length).toEqual(1);
      expect(data.exceptions[0].message).toEqual('hello world');

      const stack = data.exceptions[0].parsedStack;
      expect(stack).toBeDefined();
      expect(stack.length).toBeGreaterThan(5);

      const filename = basename(__filename);
      expect(stack[0].method).toEqual('b');
      expect(stack[1].method).toEqual('b');
      expect(stack[2].method).toEqual('Object.a');
      expect(stack[0].fileName).toEqual(`test\\${filename}`);
      expect(stack[1].fileName).toEqual(`test\\${filename}`);
      expect(stack[2].fileName).toEqual(`test\\${filename}`);
      pass = true;
    }
    return true;
  });
  try {
    a('world');
  } catch (e) {
    telemetry.telClient.trackException({exception: e});
  }
  telemetry.telClient.flush();

  expect(pass).toBeTruthy();
  telemetry.telClient.clearTelemetryProcessors();
  telemetry.telClient.addTelemetryProcessor(telemetry.sanitizeExceptions);
  done();
});

test('thrown exception a->b, hello path', done => {
  let pass = false;

  telemetry.telClient.addTelemetryProcessor((envelope, _) => {
    if (envelope.data.baseType === 'ExceptionData') {
      const data = (envelope.data as any).baseData;
      expect(data.exceptions).toBeDefined();
      expect(data.exceptions.length).toEqual(1);
      expect(data.exceptions[0].message).toEqual(
        `hello [project_dir]\\???(${process.cwd().length})`,
      );

      const stack = data.exceptions[0].parsedStack;
      expect(stack).toBeDefined();
      expect(stack.length).toBeGreaterThan(5);

      const filename = basename(__filename);
      expect(stack[0].method).toEqual('b');
      expect(stack[1].method).toEqual('b');
      expect(stack[2].method).toEqual('Object.a');
      expect(stack[0].fileName).toEqual(`test\\${filename}`);
      expect(stack[1].fileName).toEqual(`test\\${filename}`);
      expect(stack[2].fileName).toEqual(`test\\${filename}`);
      pass = true;
    }
    return true;
  });
  try {
    a(process.cwd());
  } catch (e) {
    telemetry.telClient.trackException({exception: e});
  }
  telemetry.telClient.flush();

  expect(pass).toBeTruthy();
  expect(pass).toBeTruthy();
  telemetry.telClient.clearTelemetryProcessors();
  telemetry.telClient.addTelemetryProcessor(telemetry.sanitizeExceptions);
  done();
});
