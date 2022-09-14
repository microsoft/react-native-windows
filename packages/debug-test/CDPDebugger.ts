/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @file Contains code to mimic a CDP debugger front end.
 *
 * @format
 */

import * as lodash from 'lodash';
import * as http from 'http';
import {testLog} from './TestLog';
import {abbreviate, sleep} from './Utilities';
import WebSocket from 'ws';

/**
 * Container for information about CDP debug targets.
 */
export class DebugTarget {
  id: string = '';
  title: string = '';
  faviconUrl: string = '';
  devtoolsFrontendUrl: string = '';
  type: string = '';
  webSocketDebuggerUrl: string = '';
  vm: string = '';
}

/**
 * Gets debug target information.
 * @param port Port number of CDP debugger backend (or proxy).
 * @returns List of debug targets.
 */
export async function getDebugTargets(
  port: number = 8081,
): Promise<DebugTarget[]> {
  return new Promise((resolve, _reject) => {
    const options: http.RequestOptions = {
      hostname: 'localhost',
      port: port,
      path: '/json',
      method: 'GET',
    };

    const request = http.request(options, response => {
      response.on('data', data => {
        const debugTargets = JSON.parse(data.toString());
        resolve(debugTargets);
      });
    });

    request.end();
  });
}

/**
 * Names of events that can get raised by CDPDebugger instances.
 */
export type EventName =
  // Event indicating that debugger backend is paused.
  //
  // The backend raises this event in response to the Debugger.pause command.
  // See https://chromedevtools.github.io/devtools-protocol/tot/Debugger/#event-paused
  | 'Debugger.paused'

  // Event indicating that debugger backend has resumed script execution.
  //
  // The backend raises this event in response to the Debugger.resume command.
  // See https://chromedevtools.github.io/devtools-protocol/tot/Debugger/#event-resumed
  | 'Debugger.resumed'

  // Event indicating that debugger backend has parsed a script.
  // See https://chromedevtools.github.io/devtools-protocol/tot/Debugger/#event-scriptParsed
  | 'Debugger.scriptParsed'

  // Event indicating that debugger backend has created an execution context.
  //
  // The backend raises this event in response to the Runtime.enable command.
  // See https://chromedevtools.github.io/devtools-protocol/tot/Runtime/#event-executionContextCreated
  | 'Runtime.executionContextCreated';

/**
 * Signature of handlers for events raised by CDPDebugger instances.
 */
export type EventHandler = (...any: any[]) => void;

/**
 * Information about successfully set breakpoints.
 */
export type BreakpointInfo = {
  breakpointId: string;
  locations: Array<
    [scriptId: string, lineNumber: number, columnNumber: number]
  >;
};

type CDPMessageHandler = (data: any) => boolean;

/**
 * Mimics a CDP debugger frontend.
 */
export class CDPDebugger {
  /**
   * Establishes connection to debugger backend.
   * @param url WebSocket URL for debugger backend (can be obtained from getDebugTargets return value).
   */
  constructor(url: string) {
    this.ws = new WebSocket(url);

    this.wsOpened = new Promise((resolve, _reject) => {
      this.ws.on('open', () => {
        resolve();
      });
    });

    this.ws.on('message', data => {
      const obj = JSON.parse(data as string);

      let i = 0;
      for (; i < this.expectedResponses.length; ++i) {
        const [description, isExpectedResponse] = this.expectedResponses[i];
        if (isExpectedResponse(obj)) {
          testLog.message(`received ${description}`);
          break;
        }
      }

      if (i < this.expectedResponses.length) {
        // found a handler, remove it
        this.expectedResponses.splice(i, 1);
      } else {
        this.logUnexpectedMessage(abbreviate(data as string, 500));
      }
    });
  }

  /**
   * Closes connection to debugger backend.
   */
  public close() {
    this.ws.close();
  }

  /**
   * Registers an event handler.
   * @param eventName Name of event to register handler for.
   * @param eventHandler Event handler.
   */
  public on(eventName: EventName, eventHandler: EventHandler): void {
    const handlers = this.eventHandlers.get(eventName);
    if (handlers) {
      handlers.push(eventHandler);
    } else {
      this.eventHandlers.set(eventName, [eventHandler]);
    }
  }

  private raise(eventName: string, getArgs: () => any) {
    const handlers = this.eventHandlers.get(eventName);
    if (handlers && handlers.length > 0) {
      testLog.message(`dispatching '${eventName}' event`);
      // builds args now that we know we have at least one handler
      const args = getArgs();
      for (const handler of handlers) {
        handler(args);
      }
    } else {
      testLog.message(`received '${eventName}' event with no handlers`);
    }
  }

  /**
   * Gets a promise that can be used to await an event.
   * @param eventName Name of expected event.
   * @returns Promise that can be used to await the event.
   */
  public expectEvent(eventName: EventName): Promise<any> {
    return new Promise((resolve, _reject) => {
      this.on(eventName, eventArgs => resolve(eventArgs));
    });
  }

  /**
   * Enables the debugger.
   *
   * Sends a Debugger.enable CDP message (see https://chromedevtools.github.io/devtools-protocol/tot/Debugger/#method-enable).
   * @param maxScriptsCacheSize Maximum size in bytes of collected scripts the debugger can hold.
   * @returns {Promise<void>} Promise indicating that the debugger backend has received the command.
   */
  public async debuggerEnable(
    maxScriptsCacheSize: number = 10000000,
  ): Promise<void> {
    const commandId = this.nextCommandId++;

    // the actual CDP message
    const command = JSON.stringify({
      id: commandId,
      method: 'Debugger.enable',
      params: {
        maxScriptsCacheSize: maxScriptsCacheSize,
      },
    });

    // a short description for logging
    const commandDescription = `Debugger.enable (id: ${commandId})`;

    const resultPromise = new Promise<void>((resolve, _reject) => {
      const expectedResponse = {id: commandId, result: {}};
      this.expectedResponses.push([
        `result response for ${commandDescription}`,
        response => {
          // relying on object comparison to be independent of key order differences
          if (lodash.isEqual(response, expectedResponse)) {
            resolve();
            return true;
          }
          return false;
        },
      ]);
    });

    this.expectedResponses.push([
      `Debugger.scriptParsed response for ${commandDescription}`,
      response => {
        if (
          response.hasOwnProperty('method') &&
          response.method === 'Debugger.scriptParsed'
        ) {
          this.raise('Debugger.scriptParsed', () => response.params);
          return true;
        }
        return false;
      },
    ]);

    await this.wsOpened;
    testLog.message(`sending ${commandDescription}`);
    this.ws.send(command);
    this.lastCommand = commandDescription;

    return resultPromise;
  }

  /**
   * Disables the debugger.
   *
   * Sends a Debugger.disable CDP message (see https://chromedevtools.github.io/devtools-protocol/tot/Debugger/#method-disable).
   * @returns {Promise<void>} Promise indicating that the debugger backend has received the command.
   */
  public async debuggerDisable(): Promise<void> {
    const commandId = this.nextCommandId++;

    // the actual CDP message
    const command = JSON.stringify({
      id: commandId,
      method: 'Debugger.disable',
      params: {},
    });

    // a short description for logging
    const commandDescription = `Debugger.disable (id: ${commandId})`;

    const expectedResponse = {id: commandId, result: {}};
    const resultPromise = new Promise<void>((resolve, _reject) => {
      this.expectedResponses.push([
        `result response for ${commandDescription}`,
        actualResponse => {
          // relying on object comparison to be independent of key order differences
          if (lodash.isEqual(expectedResponse, actualResponse)) {
            resolve();
            return true;
          }
          return false;
        },
      ]);
    });

    await this.wsOpened;
    testLog.message(`sending ${commandDescription}`);
    this.ws.send(command);
    this.lastCommand = commandDescription;
    return resultPromise;
  }

  /**
   * Stops JavaScript execution on the next statement.
   *
   * Sends a Debugger.pause CDP message (see https://chromedevtools.github.io/devtools-protocol/tot/Debugger/#method-pause).
   * @returns {Promise<void>} Promise indicating that the debugger backend has received the command.
   * @fires Debugger.paused event indicating that the debugger backend has reached the 'paused' state.
   */
  public async debuggerPause(): Promise<void> {
    const commandId = this.nextCommandId++;

    // the actual CDP message
    const command = JSON.stringify({
      id: commandId,
      method: 'Debugger.pause',
      params: {},
    });

    // a short description for logging
    const commandDescription = `Debugger.pause (id: ${commandId})`;

    const resultPromise = new Promise<void>((resolve, _reject) => {
      const resultResponse = {id: commandId, result: {}};
      this.expectedResponses.push([
        `result response for ${commandDescription}`,
        response => {
          // relying on object comparison to be independent of key order differences
          if (lodash.isEqual(response, resultResponse)) {
            resolve();
            return true;
          }
          return false;
        },
      ]);
    });

    this.expectedResponses.push([
      `Debugger.paused response for ${commandDescription}`,
      response => {
        if (
          response.hasOwnProperty('method') &&
          response.method === 'Debugger.paused'
        ) {
          this.raise('Debugger.paused', () => response.params);
          return true;
        }
        return false;
      },
    ]);

    await this.wsOpened;
    testLog.message(`sending ${commandDescription}`);
    this.ws.send(command);
    this.lastCommand = commandDescription;
    return resultPromise;
  }

  /**
   * Resumes JavaScript execution.
   *
   * Sends a Debugger.resume CDP message (see https://chromedevtools.github.io/devtools-protocol/tot/Debugger/#method-resume)
   * @returns {Promise<void>} Promise indicating that the debugger backend has received the command.
   * @fires Debugger.resumed event indicating that the debugger backend has resumed execution.
   */
  public async debuggerResume(): Promise<void> {
    const commandId = this.nextCommandId++;

    // the actual CDP message
    const command = JSON.stringify({
      id: commandId,
      method: 'Debugger.resume',
      params: {
        terminateOnResume: false,
      },
    });

    // a short description for logging
    const commandDescription = `Debugger.resume (id: ${commandId})`;

    const resultPromise = new Promise<void>((resolve, _reject) => {
      const resultResponse = {id: commandId, result: {}};
      this.expectedResponses.push([
        `result response for ${commandDescription}`,
        response => {
          // relying on object comparison to be independent of key order differences
          if (lodash.isEqual(response, resultResponse)) {
            resolve();
            return true;
          }
          return false;
        },
      ]);
    });

    this.expectedResponses.push([
      `Debugger.resumed response for ${commandDescription}`,
      response => {
        if (
          response.hasOwnProperty('method') &&
          response.method === 'Debugger.resumed'
        ) {
          this.raise('Debugger.resumed', () => {});
          return true;
        }
        return false;
      },
    ]);

    await this.wsOpened;
    testLog.message(`sending ${commandDescription}`);
    this.ws.send(command);
    this.lastCommand = commandDescription;
    return resultPromise;
  }

  /**
   * Activates / deactivates all breakpoints on the page.
   *
   * Sends Debugger.setBreakpointsActive CDP message (see https://chromedevtools.github.io/devtools-protocol/tot/Debugger/#method-setBreakpointsActive).
   * @param active True to activate, false to deactivate all breakpoints on the page.
   * @returns {Promise<void>} Promise indicating that the debugger backend has received the command.
   */
  public async debuggerSetBreakpointIsActive(active: boolean): Promise<void> {
    const commandId = this.nextCommandId++;

    // the actual CDP message
    const command = JSON.stringify({
      id: commandId,
      method: 'Debugger.setBreakpointIsActive',
      params: {
        active: active,
      },
    });

    // a short description for logging
    const commandDescription = `Debugger.setBreakpointIsActive (id: ${commandId})`;

    const resultPromise = new Promise<void>((resolve, _reject) => {
      const resultResponse = {id: commandId, result: {}};
      this.expectedResponses.push([
        `result response for ${commandDescription}`,
        response => {
          // relying on object comparison to be independent of key order differences
          if (lodash.isEqual(response, resultResponse)) {
            resolve();
            return true;
          }
          return false;
        },
      ]);
    });

    await this.wsOpened;
    testLog.message(`sending ${commandDescription}`);
    this.ws.send(command);
    this.lastCommand = commandDescription;
    return resultPromise;
  }

  /**
   * Sets JavaScript breakpoint at given location specified either by URL or URL regex. Once this command is issued,
   * all existing parsed scripts will have breakpoints resolved and returned in locations property. Further matching
   * script parsing will result in subsequent breakpointResolved events issued. This logical breakpoint will survive
   * page reloads.
   *
   * Sends Debugger.setBreakpointByUrl CDP message (see https://chromedevtools.github.io/devtools-protocol/tot/Debugger/#method-setBreakpointByUrl).
   * @param url URL of the resources to set breakpoint on.
   * @param urlRegex Regex pattern for the URLs of the resources to set breakpoints on. Either url or urlRegex must be specified.
   * @param lineNumber Line to set breakpoint on.
   * @param columnNumber Column to set breakpoint on.
   * @param scriptHash Script hash of the resources to set breakpoint on.
   * @param condition Expression to use as a breakpoint condition.
   * @returns {Promise<BreakpointInfo>} Information about the breakpoint.
   */
  public async debuggerSetBreakpointByUrl(
    url: string,
    urlRegex: string,
    lineNumber: number,
    columnNumber: number,
    scriptHash: string,
    condition: string = '',
  ): Promise<BreakpointInfo> {
    const commandId = this.nextCommandId++;

    // the actual CDP message
    // TODO: pass on urlRegex, scriptHash etc.
    const command = JSON.stringify({
      id: commandId,
      method: 'Debugger.setBreakpointByUrl',
      params: {
        url: url,
        lineNumber: lineNumber,
        columnNumber: columnNumber,
        condition: condition,
      },
    });

    // a short description for logging
    const commandDescription = `Debugger.setBreakpointByUrl (id: ${commandId})`;

    const resultPromise = new Promise<BreakpointInfo>((resolve, _reject) => {
      this.expectedResponses.push([
        `result response for ${commandDescription}`,
        response => {
          if (response.hasOwnProperty('id') && response.id === commandId) {
            resolve(response.result);
            return true;
          }
          return false;
        },
      ]);
    });

    await this.wsOpened;
    testLog.message(`sending ${commandDescription}`);
    this.ws.send(command);
    this.lastCommand = commandDescription;
    return resultPromise;
  }

  /**
   * Removes a breakpoint.
   *
   * Sends a Debugger.removeBreakpoint CDP message (see https://chromedevtools.github.io/devtools-protocol/tot/Debugger/#method-removeBreakpoint).
   * @param breakpointId Identifier of the breakpoint to remove (e.g. result of debuggerSetBreakpointByUrl)
   * @returns {Promise<void>} Promise indicating that the debugger backend has received the command.
   */
  public async debuggerRemoveBreakpoint(breakpointId: string): Promise<void> {
    const commandId = this.nextCommandId++;

    // the actual CDP message
    const command = JSON.stringify({
      id: commandId,
      method: 'Debugger.removeBreakpoint',
      params: {
        breakpointId: breakpointId,
      },
    });

    // a short description for logging
    const commandDescription = `Debugger.removeBreakpoint (id: ${commandId})`;

    const expectedResponse1 = {id: commandId, result: {}};

    const resultPromise = new Promise<void>((resolve, _reject) => {
      this.expectedResponses.push([
        `result response for ${commandDescription}`,
        actualResponse => {
          // relying on object comparison to be independent of key order differences
          if (lodash.isEqual(expectedResponse1, actualResponse)) {
            resolve();
            return true;
          }
          return false;
        },
      ]);
    });

    await this.wsOpened;
    testLog.message(`sending ${commandDescription}`);
    this.ws.send(command);
    this.lastCommand = commandDescription;
    return resultPromise;
  }

  /**
   * Enables reporting of execution context creation via executionContextCreated events.
   *
   * Sends a Runtime.enable CDP message (see https://chromedevtools.github.io/devtools-protocol/tot/Runtime/#method-enable).
   * @returns {Promise<void>} Promise indicating that the debugger backend has received the command.
   * @fires Runtime.executionContextCreated event indicating that an execution context has been created.
   */
  public async runtimeEnable(): Promise<void> {
    const commandId = this.nextCommandId++;

    // the actual CDP message
    const command = JSON.stringify({
      id: commandId,
      method: 'Runtime.enable',
    });

    // a short description for logging
    const commandDescription = `Runtime.enable (id: ${commandId})`;

    const resultPromise = new Promise<void>((resolve, _reject) => {
      const expectedResponse = {id: commandId, result: {}};
      this.expectedResponses.push([
        `result response for ${commandDescription}`,
        response => {
          // relying on object comparison to be independent of key order differences
          if (lodash.isEqual(response, expectedResponse)) {
            resolve();
            return true;
          }
          return false;
        },
      ]);
    });

    this.expectedResponses.push([
      `Runtime.executionContextCreated response for ${commandDescription}`,
      response => {
        if (
          response.hasOwnProperty('method') &&
          response.method === 'Runtime.executionContextCreated'
        ) {
          this.raise('Runtime.executionContextCreated', () => response.params);
          return true;
        }
        return false;
      },
    ]);

    await this.wsOpened;
    testLog.message(`sending ${commandDescription}`);
    this.ws.send(command);
    this.lastCommand = commandDescription;

    return resultPromise;
  }

  /**
   * Checks whether all expected responses to previous commands have been retrieved.
   *
   * If there are outstanding responses at the end of the timeout period, information
   * about these responses is written to the log file and a summary exception is thrown.
   * @param timeout Timeout, in milliseconds, to spend on checking outstanding responses.
   */
  public async checkOutstandingResponses(timeout: number) {
    const pollPeriod = 500;
    const retryCount = Math.ceil(timeout / pollPeriod);
    const startTime = new Date();

    let i = 0;
    do {
      if (this.expectedResponses.length === 0) {
        testLog.message('received all expected responses');
        return;
      }

      if (new Date().getTime() - startTime.getTime() >= timeout) break;

      await sleep(pollPeriod);
    } while (++i < retryCount);

    for (const [description] of this.expectedResponses) {
      testLog.message(`missing ${description}`);
    }
    throw new Error('missing expected responses (see log file for details)');
  }

  private logUnexpectedMessage(message: string) {
    testLog.warning(
      `received unexpected message '${message}'` +
        (this.lastCommand
          ? `, perhaps in response to '${this.lastCommand}'`
          : ''),
    );
  }

  private readonly eventHandlers: Map<string, Array<EventHandler>> = new Map();
  private readonly wsOpened: Promise<void>;
  private readonly ws: WebSocket;
  private nextCommandId: number = 0;
  private lastCommand: string = '';
  private readonly expectedResponses: Array<[string, CDPMessageHandler]> = [];
}
