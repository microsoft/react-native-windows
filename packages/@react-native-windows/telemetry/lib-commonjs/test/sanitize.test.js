"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
Object.defineProperty(exports, "__esModule", { value: true });
const telemetry_1 = require("../telemetry");
const path_1 = require("path");
delete process.env.AGENT_NAME; // allow this test to run in Azure DevOps / GHA
telemetry_1.Telemetry.setup();
telemetry_1.Telemetry.client.config.disableAppInsights = true;
test('Sanitize message, no-op', () => {
    // do stuff
    expect(telemetry_1.sanitizeMessage('')).toEqual('');
    expect(telemetry_1.sanitizeMessage('some text')).toEqual('some text');
});
test('Sanitize message, project_dir', () => {
    expect(telemetry_1.sanitizeMessage(`this is the cwd: '${process.cwd()}'`)).toEqual(`this is the cwd:  [project_dir]\\???(${process.cwd().length})`);
    expect(telemetry_1.sanitizeMessage(`uppercase: '${process.cwd().toUpperCase()}'`)).toEqual(`uppercase:  [project_dir]\\???(${process.cwd().length})`);
    expect(telemetry_1.sanitizeMessage(`lowercase: '${process.cwd().toLowerCase()}'`)).toEqual(`lowercase:  [project_dir]\\???(${process.cwd().length})`);
    expect(telemetry_1.sanitizeMessage(`this is the cwd: '${process.cwd()}' and something else`)).toEqual(`this is the cwd:  [project_dir]\\???(${process.cwd().length})  and something else`);
    expect(telemetry_1.sanitizeMessage(`this is the cwd: ${process.cwd()} and something else`)).toEqual(`this is the cwd: [project_dir]\\???(${(process.cwd() + ' and something else').length})`);
});
test('Sanitize message, node_modules', () => {
    expect(telemetry_1.sanitizeMessage(`this is the cwd: '${process.cwd()}\\node_modules'`)).toEqual(`this is the cwd:  [project_dir]\\???(${(process.cwd() + '\\node_modules').length})`);
    expect(telemetry_1.sanitizeMessage(`this is the cwd: '${process.cwd()}\\node_modules\\foo'`)).toEqual('this is the cwd:  node_modules\\foo');
    expect(telemetry_1.sanitizeMessage(`uppercase: '${process.cwd().toUpperCase()}\\NODE_MODULES\\foo'`)).toEqual('uppercase:  node_modules\\foo');
    expect(telemetry_1.sanitizeMessage(`lowercase: '${process.cwd().toLowerCase()}\\NODE_MODULES\\'`)).toEqual('lowercase:  node_modules\\');
    expect(telemetry_1.sanitizeMessage(`trailing: '${process.cwd()}\\node_modules\\' and something else`)).toEqual('trailing:  node_modules\\  and something else');
    expect(telemetry_1.sanitizeMessage(`this is the cwd: ${process.cwd()}\\node_modules and something else that could be part of the path`)).toEqual(`this is the cwd: [project_dir]\\???(${(process.cwd() +
        '\\node_modules and something else that could be part of the path').length})`);
    expect(telemetry_1.sanitizeMessage(`this is the cwd: ${process.cwd()}\\node_modules\\ a file under nm`)).toEqual(`this is the cwd: node_modules\\ a file under nm`);
});
test('Sanitize message, other path', () => {
    expect(telemetry_1.sanitizeMessage(`this is another path: 'A:\\foo\\bar\\baz'`)).toEqual(`this is another path:  [path]`);
    expect(telemetry_1.sanitizeMessage(`this is another path: A:\\foo\\bar\\baz`)).toEqual(`this is another path: [path]`);
    expect(telemetry_1.sanitizeMessage(`Cannot find module 'react-native/package.json'
      Require stack:
      - ${process.env.AppData}\\npm-cache\\_npx\\1384\\node_modules\\react-native-windows-init\\lib-commonjs\\Cli.js
      - ${process.env.AppData}\\npm-cache\\_npx\\1384\\node_modules\\react-native-windows-init\\bin.js`)).toEqual(`Cannot find module  react-native/package.json 
      Require stack:
      - [AppData]\\???(${(process.env.AppData +
        '\\npm-cache\\_npx\\1384\\node_modules\\react-native-windows-init\\lib-commonjs\\Cli.js').length})
      - [AppData]\\???(${(process.env.AppData +
        '\\npm-cache\\_npx\\1384\\node_modules\\react-native-windows-init\\bin.js').length})`);
});
test('Sanitize message, forward slashes', () => {
    expect(telemetry_1.sanitizeMessage(`EPERM: operation not permitted, scandir  ${process.env.UserProfile.replace(/\\/g, '/')}/source/repos/rn2/wintest/windows/packages/boost.1.72.0.0/lib/native/include`)).toEqual(`EPERM: operation not permitted, scandir  [UserProfile]\\???(${(process.env.UserProfile +
        '/source/repos/rn2/wintest/windows/packages/boost.1.72.0.0/lib/native/include').length})`);
});
test('Sanitize message, file share path', () => {
    expect(telemetry_1.sanitizeMessage(`\\\\server\\share`)).toEqual('[path]');
});
test('Sanitize message, with cpu/thread id', () => {
    expect(telemetry_1.sanitizeMessage('5>This is an error')).toEqual('This is an error');
    expect(telemetry_1.sanitizeMessage('5:42>This is an error')).toEqual('This is an error');
});
test('Error code', () => {
    expect(telemetry_1.tryGetErrorCode('foo bar error FOO2020: the thing')).toEqual('FOO2020');
});
test('Sanitize stack frame', () => {
    const emptyFrame = {
        level: 0,
        method: '',
        fileName: '',
        assembly: 'asdf',
        line: 0,
    };
    telemetry_1.sanitizeFrame(emptyFrame);
    expect(emptyFrame).toEqual({
        level: 0,
        assembly: '',
        fileName: '.',
        method: '',
        line: 0,
    });
    const frame1 = {
        method: '',
        fileName: `${process.cwd()}\\foo.js`,
        assembly: 'asdf',
        level: 0,
        line: 0,
    };
    telemetry_1.sanitizeFrame(frame1);
    expect(frame1).toEqual({
        assembly: '',
        fileName: 'telemetry\\foo.js',
        method: '',
        level: 0,
        line: 0,
    });
    const frame2 = {
        method: `myMethod (something ${process.cwd()}`,
        fileName: `${process.cwd()}\\foo.js`,
        assembly: 'asdf',
        level: 1,
        line: 42,
    };
    telemetry_1.sanitizeFrame(frame2);
    expect(frame2).toEqual({
        assembly: '',
        fileName: 'telemetry\\foo.js',
        method: 'myMethod',
        level: 1,
        line: 42,
    });
});
test('basic setup', () => {
    expect(telemetry_1.Telemetry.client.commonProperties.sessionId).toBeDefined();
    expect(telemetry_1.Telemetry.client.commonProperties.sessionId.length).toBeGreaterThanOrEqual(32);
    expect(telemetry_1.Telemetry.client.commonProperties.isTest).toEqual('true');
});
function b(s) {
    throw new Error('hello ' + s);
}
function a(s) {
    b(s);
}
test('thrown exception a->b, hello world', done => {
    let pass = false;
    telemetry_1.Telemetry.client.addTelemetryProcessor((envelope, _) => {
        if (envelope.data.baseType === 'ExceptionData') {
            const data = envelope.data.baseData;
            expect(data.exceptions).toBeDefined();
            expect(data.exceptions.length).toEqual(1);
            expect(data.exceptions[0].message).toEqual('hello world');
            const stack = data.exceptions[0].parsedStack;
            expect(stack).toBeDefined();
            expect(stack.length).toBeGreaterThan(5);
            const filename = path_1.basename(__filename);
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
    }
    catch (e) {
        telemetry_1.Telemetry.client.trackException({ exception: e });
    }
    telemetry_1.Telemetry.client.flush();
    expect(pass).toBeTruthy();
    telemetry_1.Telemetry.client.clearTelemetryProcessors();
    telemetry_1.Telemetry.client.addTelemetryProcessor(telemetry_1.sanitizeEnvelope);
    done();
});
test('throw exception with error code', done => {
    telemetry_1.Telemetry.client.addTelemetryProcessor((envelope, _) => {
        const data = envelope.data.baseData;
        expect(data.properties.errorCode).toEqual('FOO2020');
        return true;
    });
    try {
        throw new Error('hello from an error FOO2020: the error string');
    }
    catch (e) {
        telemetry_1.Telemetry.client.trackException({ exception: e });
        telemetry_1.Telemetry.client.flush();
    }
    telemetry_1.Telemetry.client.clearTelemetryProcessors();
    telemetry_1.Telemetry.client.addTelemetryProcessor(telemetry_1.sanitizeEnvelope);
    done();
});
test('thrown exception a->b, hello path', done => {
    let pass = false;
    telemetry_1.Telemetry.client.addTelemetryProcessor((envelope, _) => {
        if (envelope.data.baseType === 'ExceptionData') {
            const data = envelope.data.baseData;
            expect(data.exceptions).toBeDefined();
            expect(data.exceptions.length).toEqual(1);
            expect(data.exceptions[0].message).toEqual(`hello [project_dir]\\???(${process.cwd().length})`);
            const stack = data.exceptions[0].parsedStack;
            expect(stack).toBeDefined();
            expect(stack.length).toBeGreaterThan(5);
            const filename = path_1.basename(__filename);
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
    }
    catch (e) {
        telemetry_1.Telemetry.client.trackException({ exception: e });
    }
    telemetry_1.Telemetry.client.flush();
    expect(pass).toBeTruthy();
    expect(pass).toBeTruthy();
    telemetry_1.Telemetry.client.clearTelemetryProcessors();
    telemetry_1.Telemetry.client.addTelemetryProcessor(telemetry_1.sanitizeEnvelope);
    done();
});
test('trackEvent should not identify roleInstance', () => {
    telemetry_1.Telemetry.client.addTelemetryProcessor((envelope, _) => {
        expect(envelope.tags['ai.cloud.roleInstance']).toBeUndefined();
        return true;
    });
    telemetry_1.Telemetry.client.trackEvent({
        name: 'test',
        properties: {},
    });
    telemetry_1.Telemetry.client.flush();
    telemetry_1.Telemetry.client.clearTelemetryProcessors();
    telemetry_1.Telemetry.client.addTelemetryProcessor(telemetry_1.sanitizeEnvelope);
});
//# sourceMappingURL=sanitize.test.js.map