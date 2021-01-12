"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */
Object.defineProperty(exports, "__esModule", { value: true });
exports.getDiskFreeSpace = exports.isMSFTInternal = exports.sanitizeEnvelope = exports.tryGetErrorCode = exports.sanitizeFrame = exports.sanitizeMessage = exports.Telemetry = void 0;
const path = require("path");
const crypto_1 = require("crypto");
const appInsights = require("applicationinsights");
const child_process_1 = require("child_process");
class Telemetry {
    static disable() {
        if (Telemetry.client) {
            Telemetry.client.config.disableAppInsights = true;
        }
        Telemetry.shouldDisable = true;
    }
    static setup() {
        if (Telemetry.isCI()) {
            this.disable();
            return;
        }
        if (Telemetry.client) {
            return;
        }
        if (!process.env.RNW_CLI_TEST) {
            appInsights.Configuration.setInternalLogging(false, false);
        }
        appInsights.setup('795006ca-cf54-40ee-8bc6-03deb91401c3');
        Telemetry.client = appInsights.defaultClient;
        if (Telemetry.shouldDisable) {
            Telemetry.disable();
        }
        if (process.env.RNW_CLI_TEST) {
            Telemetry.client.commonProperties.isTest = process.env.RNW_CLI_TEST;
        }
        if (!Telemetry.client.commonProperties.sessionId) {
            Telemetry.client.commonProperties.sessionId = crypto_1.randomBytes(16).toString('hex');
            Telemetry.client.addTelemetryProcessor(sanitizeEnvelope);
        }
    }
    static isCI() {
        return (process.env.AGENT_NAME !== undefined || // Azure DevOps
            process.env.CIRCLECI === 'true' || // CircleCI
            process.env.TRAVIS === 'true' || // Travis
            process.env.CI === 'true' // other CIs
        );
    }
}
exports.Telemetry = Telemetry;
Telemetry.client = undefined;
Telemetry.shouldDisable = false;
function getAnonymizedPath(filepath) {
    const projectRoot = process.cwd().toLowerCase();
    filepath = filepath.replace(/\//g, '\\');
    const knownPathsVars = ['AppData', 'LocalAppData', 'UserProfile'];
    if (filepath.toLowerCase().startsWith(projectRoot)) {
        const ext = path.extname(filepath);
        const rest = filepath.slice(projectRoot.length);
        const node_modules = '\\node_modules\\';
        // this is in the project dir but not under node_modules
        if (rest.toLowerCase().startsWith('\\windows\\')) {
            return `[windows]\\???${ext}(${filepath.length})`;
        }
        else if (rest.toLowerCase().startsWith(node_modules)) {
            return 'node_modules' + rest.slice(node_modules.length - 1);
        }
        else {
            return `[project_dir]\\???${ext}(${filepath.length})`;
        }
    }
    else {
        for (const knownPath of knownPathsVars) {
            if (process.env[knownPath] &&
                filepath.toLowerCase().startsWith(process.env[knownPath].toLowerCase())) {
                return `[${knownPath}]\\???(${filepath.length})`;
            }
        }
    }
    return '[path]';
}
/**
 * Sanitize any paths that appear between quotes (''), brackets ([]), or double quotes ("").
 * @param msg the string to sanitize
 */
function sanitizeMessage(msg) {
    const cpuThreadId = /^\d+(:\d+)?>/g;
    msg = msg.replace(cpuThreadId, '');
    const parts = msg.split(/['[\]"]/g);
    const clean = [];
    const pathRegEx = /([A-Za-z]:|\\)[\\/]([^<>:;,?"*\t\r\n|/\\]+[\\/])+([^<>:;,?"*\t\r\n|]+\/?)/gi;
    for (const part of parts) {
        if (pathRegEx.test(part)) {
            pathRegEx.lastIndex = -1;
            let matches;
            let noPath = '';
            let last = 0;
            while ((matches = pathRegEx.exec(part))) {
                noPath +=
                    part.substr(last, matches.index - last) +
                        getAnonymizedPath(matches[0]);
                last = matches.index + matches[0].length;
            }
            clean.push(noPath);
        }
        else if (part !== '') {
            clean.push(part);
        }
    }
    return clean.join(' ').trim();
}
exports.sanitizeMessage = sanitizeMessage;
function sanitizeFrame(frame) {
    const parens = frame.method.indexOf('(');
    if (parens !== -1) {
        // case 1: method === 'methodName (rootOfThePath'
        frame.method = frame.method.substr(0, parens).trim();
    }
    else {
        // case 2: method === <no_method> or something without '(', fileName is full path
    }
    // preserve only the last_directory/filename
    frame.fileName = path.join(path.basename(path.dirname(frame.fileName)), path.basename(frame.fileName));
    frame.assembly = '';
}
exports.sanitizeFrame = sanitizeFrame;
function tryGetErrorCode(msg) {
    const errorRegEx = /error (\w+\d+)[\s:]/gi;
    const m = errorRegEx.exec(msg);
    return m ? m[1] : undefined;
}
exports.tryGetErrorCode = tryGetErrorCode;
/**
 * Remove PII from exceptions' stack traces and messages
 * @param envelope the telemetry envelope. Provided by AppInsights.
 */
function sanitizeEnvelope(envelope /*context: any*/) {
    if (envelope.data.baseType === 'ExceptionData') {
        const data = envelope.data.baseData;
        for (const exception of data.exceptions || []) {
            for (const frame of exception.parsedStack) {
                sanitizeFrame(frame);
            }
            const errorCode = tryGetErrorCode(exception.message);
            data.properties.errorCode = errorCode;
            exception.message = sanitizeMessage(exception.message);
        }
    }
    delete envelope.tags['ai.cloud.roleInstance'];
    return true;
}
exports.sanitizeEnvelope = sanitizeEnvelope;
function isMSFTInternal() {
    return (process.env.UserDNSDomain !== undefined &&
        process.env.UserDNSDomain.toLowerCase().endsWith('.microsoft.com'));
}
exports.isMSFTInternal = isMSFTInternal;
function getDiskFreeSpace(drivePath) {
    const out = child_process_1.execSync(`dir /-C ${drivePath}`)
        .toString()
        .split('\r\n');
    const line = out[out.length - 2];
    const result = line.match(/(\d+) [^\d]+(\d+) /);
    if (result && result.length > 2) {
        return Number(result[2]);
    }
    return -1;
}
exports.getDiskFreeSpace = getDiskFreeSpace;
//# sourceMappingURL=telemetry.js.map