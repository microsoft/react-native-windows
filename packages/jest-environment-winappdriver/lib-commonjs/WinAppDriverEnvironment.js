"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
Object.defineProperty(exports, "__esModule", { value: true });
const child_process_1 = require("child_process");
const fs = require("fs");
const NodeEnvironment = require("jest-environment-node");
const webdriverio = require("webdriverio");
class WinAppDriverEnvironment extends NodeEnvironment {
    constructor(config) {
        super(config);
        const passedOptions = config.testEnvironmentOptions;
        if (!passedOptions.app) {
            throw new Error('"app" must be specified in testEnvironmentOptions');
        }
        this.winappdriverBin =
            passedOptions.winAppDriverBin ||
                'C:\\Program Files (x86)\\Windows Application Driver\\WinAppDriver.exe';
        const baseOptions = {
            port: 4723,
            capabilities: {
                app: passedOptions.app,
                // @ts-ignore
                'ms:experimental-webdriver': true,
            },
        };
        this.options = Object.assign({}, baseOptions, passedOptions.webdriverOptions);
    }
    async setup() {
        await super.setup();
        this.winAppDriverProcess = await spawnWinAppDriver(this.winappdriverBin);
        this.browser = await webdriverio.remote(this.options);
        this.global.remote = webdriverio.remote;
        this.global.browser = this.browser;
        this.global.$ = this.browser.$.bind(this.browser);
        this.global.$$ = this.browser.$$.bind(this.browser);
    }
    async teardown() {
        var _a;
        if (this.browser) {
            await this.browser.deleteSession();
        }
        (_a = this.winAppDriverProcess) === null || _a === void 0 ? void 0 : _a.kill('SIGINT');
        await super.teardown();
    }
}
/**
 * Starts a WinAppdriver process and resolves a promise with the process once
 * it is ready to accept commands
 *
 * Inspired-by/stolen from https://github.com/licanhua/wdio-winappdriver-service
 */
async function spawnWinAppDriver(winappdriverBin) {
    if (!fs.existsSync(winappdriverBin)) {
        throw new Error(`Could not locate WinAppDriver binary at "${winappdriverBin}"`);
    }
    return new Promise((resolve, reject) => {
        const process = child_process_1.spawn(winappdriverBin, { stdio: 'pipe' });
        process.stdout.on('data', data => {
            const s = data.toString('utf16le');
            if (s.includes('Press ENTER to exit.')) {
                resolve(process);
            }
            else if (s.includes('Failed to initialize')) {
                reject(new Error('Failed to start WinAppDriver: ' + s));
            }
        });
        process.stderr.once('data', err => {
            console.warn(err);
        });
        process.once('exit', exitCode => {
            reject(new Error(`WinAppDriver CLI exited before timeout (exit code: ${exitCode})`));
        });
    });
}
module.exports = WinAppDriverEnvironment;
//# sourceMappingURL=WinAppDriverEnvironment.js.map