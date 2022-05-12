import * as lodash from 'lodash';
import {testLog} from './TestLog'
import {
	shortenIfNeeded,
	sleep
} from './TestUtilities';
import * as WebSocket from 'ws';

type CDPMessageHandler = (data: any) => boolean;
type EventHandler = (...any) => void;

export type BreakpointInfo = [
	breakpointId: string,
	locations: Array<[scriptId: string, lineNumber: number, columnNumber: number]>
];

export class CDPDebugger {

	constructor(url: string) {
		this.ws = new WebSocket(url);

		this.wsOpened = new Promise((resolve, reject) => {
			this.ws.on('open', () => { resolve(); });
		});

		this.ws.on('message', data => {
			const obj = JSON.parse(data as string);

			let i = 0;
			for (; i < this.expectedResponses.length; ++i) {
				const [description, isExpectedResponse] = this.expectedResponses[i];
				if (isExpectedResponse(obj)){
					testLog.message(`received ${description}`);
					break;
				};
			}

			if (i < this.expectedResponses.length) {
				// found a handler, remove it
				this.expectedResponses.splice(i, 1);
			} else {
				this.logUnexpectedMessage(shortenIfNeeded(data as string, 500));
			}
		});
	}

	public close() {
		this.ws.close();
	}

	public on(eventName: string, eventHandler: EventHandler): void {
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
			testLog.message(`dispatching '${eventName}' event`)
			// builds args now that we know we have at least one handler
			const args = getArgs();
			for (const handler of handlers) {
				handler(args);
			}
		} else {
			testLog.message(`received '${eventName}' event with no handlers`);
		}
	}

	public expectEvent(eventName: string) : Promise<any> {
		return new Promise((resolve, reject) => {
			this.on(eventName, eventArgs => resolve(eventArgs));
		});
	}

	/**
	 * Sends a Debugger.enable CDP message (see https://chromedevtools.github.io/devtools-protocol/tot/Debugger/#method-enable).
	 */
	public async DebuggerEnable(maxScriptsCacheSize: number = 10000000): Promise<void> {
		const commandId = this.nextCommandId++;
		
		// the actual CDP message
		const command = `{ "id":${commandId}, "method":"Debugger.enable", "params":{ "maxScriptsCacheSize":${maxScriptsCacheSize} } }`;

		// a short description for logging
		const commandDescription = `Debugger.enable (id: ${commandId})`;

		const resultPromise = new Promise<void>(async (resolve, reject) => {
			const expectedResponse = {"id": commandId, "result": {}};
			this.expectedResponses.push([
				`result response for ${commandDescription}`,
				response => {
					// relying on object comparison to be independent of key order differences
					if (lodash.isEqual(response, expectedResponse)) {
						resolve();
						return true;
					}
					return false;
				}
			]);
		});

		await this.wsOpened;
		testLog.message(`sending ${commandDescription}`);
		this.ws.send(command);
		this.lastCommand = commandDescription;
	
		return resultPromise;
	}

	/**
	 * Sends a Debugger.disable CDP message (see https://chromedevtools.github.io/devtools-protocol/tot/Debugger/#method-disable).
	 */
	public async DebuggerDisable(): Promise<void> {
		const commandId = this.nextCommandId++;
		
		// the actual CDP message
		const command = `{ "id":${commandId}, "method":"Debugger.disable", "params":{}}`;

		// a short description for logging
		const commandDescription = `Debugger.disable (id: ${commandId})`;

		const expectedResponse = {"id": commandId, "result": {}};
		const resultPromise = new Promise<void>((resolve, reject) => {
			this.expectedResponses.push([
				`result response for ${commandDescription}`,
				actualResponse => {
					// relying on object comparison to be independent of key order differences
					if (lodash.isEqual(expectedResponse, actualResponse)) {
						resolve();
						return true;
					}
					return false;
				}
			]);
		});

		await this.wsOpened;
		testLog.message(`sending ${commandDescription}`);
		this.ws.send(command);
		this.lastCommand = commandDescription;
		return resultPromise;
	}

	/**
	 * Sends a Debugger.pause CDP message (see https://chromedevtools.github.io/devtools-protocol/tot/Debugger/#method-pause).
	 */
	public async DebuggerPause() : Promise<void> {
		const commandId = this.nextCommandId++;
		
		// the actual CDP message
		const command = `{"id":${commandId}, "method":"Debugger.pause", "params":{}}`;

		// a short description for logging
		const commandDescription = `Debugger.pause (id: ${commandId})`;

		const resultPromise = new Promise<void>((resolve, reject) => {
			const resultResponse = {"id": commandId, "result": {}};
			this.expectedResponses.push([
				`result response for ${commandDescription}`,
				response => {
					// relying on object comparison to be independent of key order differences
					if (lodash.isEqual(response, resultResponse)) {
						resolve();
						return true;
					}
					return false;
				}
			]);
		});

		this.expectedResponses.push([
			`Debugger.paused response for ${commandDescription}`,
			response => {
				if (response.hasOwnProperty("method") && response.method == "Debugger.paused") {
					this.raise('paused', () => response.params);
					return true;
				}
				return false;
			}
		]);

		await this.wsOpened;
		testLog.message(`sending ${commandDescription}`);
		this.ws.send(command);
		this.lastCommand = commandDescription;
		return resultPromise;
	}

	/**
	 * Sends a Debugger.resume CDP message (see https://chromedevtools.github.io/devtools-protocol/tot/Debugger/#method-resume)
	 */
	public async DebuggerResume(): Promise<void> {
		const commandId = this.nextCommandId++;

		// the actual CDP message
		const command = `{"id":${commandId}, "method":"Debugger.resume", "params":{"terminateOnResume": false}}`;

		// a short description for logging
		const commandDescription = `Debugger.resume (id: ${commandId})`;

		const resultPromise = new Promise<void>((resolve, reject) => {
			const resultResponse = {"id": commandId, "result": {}};
			this.expectedResponses.push([
				`result response for ${commandDescription}`,
				response => {
					// relying on object comparison to be independent of key order differences
					if (lodash.isEqual(response, resultResponse)) {
						resolve();
						return true;
					}
					return false;
				}
			]);
		});

		this.expectedResponses.push([
			`Debugger.resumed response for ${commandDescription}`,
			response => {
				if (response.hasOwnProperty("method") && response.method == "Debugger.resumed") {
					this.raise('resumed', () => {});
					return true;
				}
				return false;
			}
		]);

		await this.wsOpened;
		testLog.message(`sending ${commandDescription}`);
		this.ws.send(command);
		this.lastCommand = commandDescription;
		return resultPromise;
	}

	/**
	 * Sends Debugger.setBreakpointsActive CDP message (see https://chromedevtools.github.io/devtools-protocol/tot/Debugger/#method-setBreakpointsActive).
	 * Activates / deactivates all breakpoints on the page.
	 * @param active 
	 */
	 public async DebuggerSetBreakpointIsActive(active: boolean): Promise<void> {
		const commandId = this.nextCommandId++;
		
		// the actual CDP message
		const command = `{"id":${commandId}, "method":"Debugger.setBreakpointIsActive", "params":{"active":${active}}}`;

		// a short description for logging
		const commandDescription = `Debugger.setBreakpointIsActive (id: ${commandId})`;

		const resultPromise = new Promise<void>((resolve, reject) => {
			const resultResponse = {"id": commandId, "result": {}};
			this.expectedResponses.push([
				`result response for ${commandDescription}`,
				response => {
					// relying on object comparison to be independent of key order differences
					if (lodash.isEqual(response, resultResponse)) {
						resolve();
						return true;
					}
					return false;
				}
			]);
		});

		await this.wsOpened;
		testLog.message(`sending ${commandDescription}`);
		this.ws.send(command);
		this.lastCommand = commandDescription;
		return resultPromise;
	}

	/**
	 * Sends D CDP message (see https://chromedevtools.github.io/devtools-protocol/tot/Debugger/#method-setBreakpointByUrl).
	 * 
	 * Sets JavaScript breakpoint at given location specified either by URL or URL regex. Once this command is issued,
	 * all existing parsed scripts will have breakpoints resolved and returned in locations property. Further matching
	 * script parsing will result in subsequent breakpointResolved events issued. This logical breakpoint will survive
	 * page reloads.
	 * @param active 
	 */
	 public async DebuggerSetBreakpointByUrl(
		url: string,
		urlRegex: string,
		lineNumber: number,
		columnNumber: number,
		scriptHash: string,
		condition: string = "") : Promise<BreakpointInfo> {
		const commandId = this.nextCommandId++;
		
		// the actual CDP message
		// TODO: pass on urlRegex. scriptHash
		const command = `{"id":${commandId}, "method":"Debugger.setBreakpointByUrl", "params":{
			"url":${url},
			"lineNumber":${lineNumber},
			"columnNumber":${columnNumber},
			"condition": ${condition}
		}}`;

		// a short description for logging
		const commandDescription = `Debugger.setBreakpointByUrl (id: ${commandId})`;

		const expectedResponse1 = {"id": commandId, "result": {}};

		const resultPromise = new Promise<BreakpointInfo>((resolve, reject) => {
			this.expectedResponses.push([
				`result response for ${commandDescription}`,
				response => {
					if (response.hasOwnProperty("id") && response.id == commandId) {
						resolve(response.result);
						return true;
					}
					return false;
				}
			]);
		});

		await this.wsOpened;
		testLog.message(`sending ${commandDescription}`);
		this.ws.send(command);
		this.lastCommand = commandDescription;
		return resultPromise;
	}

	/**
	 * Sends a Debugger.removeBreakpoint CDP message (see https://chromedevtools.github.io/devtools-protocol/tot/Debugger/#method-removeBreakpoint).
	 * @param breakpointId Breakpoint identifier (e.g. result of DebuggerSetBreakpointByUrl)
	 */
	public async DebuggerRemoveBreakpoint(breakpointId: string): Promise<void> {
		const commandId = this.nextCommandId++;
		
		// the actual CDP message
		const command = `{"id":${commandId}, "method":"Debugger.removeBreakpoint", "params":{"breakpointId":"${breakpointId}"}}`;

		// a short description for logging
		const commandDescription = `Debugger.removeBreakpoint (id: ${commandId})`;

		const expectedResponse1 = {"id": commandId, "result": {}};

		const resultPromise = new Promise<void>((resolve, reject) => {
			this.expectedResponses.push([
				`result response for ${commandDescription}`,
				actualResponse => {
					// relying on object comparison to be independent of key order differences
					if (lodash.isEqual(expectedResponse1, actualResponse)) {
						resolve();
						return true;
					}
					return false;
				}
			]);
		});

		await this.wsOpened;
		testLog.message(`sending ${commandDescription}`);
		this.ws.send(command);
		this.lastCommand = commandDescription;
		return resultPromise;
	}

	public async checkOutstandingResponses(timeout: number) {
		return new Promise<void>(async (resolve, reject) => {
			const pollPeriod = 500;
			const retryCount =  Math.ceil(timeout / pollPeriod);
			const startTime = new Date();

			let i = 0;
			do {
				if (this.expectedResponses.length == 0) {
					testLog.message("received all expected responses");
					resolve();
					return;
				}

				if ((new Date()).getTime() - startTime.getTime() >= timeout) break;

				await sleep(pollPeriod);
			 } while (++i < retryCount);

			for (const [description, unused] of this.expectedResponses) {
				testLog.message(`missing ${description}`);
			}
			reject();
		});
	}

	private logUnexpectedMessage(message: string) {
		testLog.warning(`received unexpected message '${message}'` + (this.lastCommand ? `, perhaps in response to '${this.lastCommand}'` : ""));
	}

	private eventHandlers: Map<string, Array<EventHandler>> = new Map();
	private wsOpened: Promise<void>;
	private ws: WebSocket;
	private nextCommandId: number = 0;
	private lastCommand: string;
	private expectedResponses: Array<[string, CDPMessageHandler]> = [];

}
