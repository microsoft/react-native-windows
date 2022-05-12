import * as http from 'http';
import { CDPDebugger } from './CDPDebugger';
import * as WebSocket from 'ws';
import { Metro } from './MetroAutomation';
import { PlaygroundDebugSettings, selectPackage } from './PlaygroundAutomation'
import { testLog } from './TestLog';
import {
	getDebugTargets,
	sleep
} from './TestUtilities';

const metro = new Metro();

beforeAll(async () => {
	metro.start();
});

afterAll(() => {
	metro.stop();
});

// beforeEach(() => {});
// afterEach(() => {});

function skip_test(msg, fn){}

test("debug target properties", async () => {
	testLog.message(`test debug target properties`);

	const settings = new PlaygroundDebugSettings({webDebugger: false, directDebugging: true, jsEngine: "Hermes"});
	await settings.initialize();
	try {
		await selectPackage("Samples\\debugTest01");

		const options : http.RequestOptions = {
			hostname: 'localhost',
			port: 8081,
			path: '/json',
			method: 'GET'
		};

		let request = null;
		const dataPromise = new Promise((resolve, reject) => {
			request = http.request(options, response => response.on('data', data => resolve(data)));
		});

		request.end();

		const data = await dataPromise;
		expect(Buffer.isBuffer(data)).toBeTruthy();
		const debugTargets = JSON.parse(data.toString());
		expect(Array.isArray(debugTargets)).toBeTruthy();
		expect(debugTargets.length).toBeGreaterThanOrEqual(1);
		for (const debugTarget of debugTargets) {
			expect(debugTarget.hasOwnProperty('id')).toBeTruthy();
			expect(debugTarget.hasOwnProperty('title')).toBeTruthy();
			expect(debugTarget.hasOwnProperty('faviconUrl')).toBeTruthy();
			expect(debugTarget.hasOwnProperty('devtoolsFrontendUrl')).toBeTruthy();
			expect(debugTarget.hasOwnProperty('type')).toBeTruthy();
			expect(debugTarget.hasOwnProperty('webSocketDebuggerUrl')).toBeTruthy();
			expect(debugTarget.hasOwnProperty('vm')).toBeTruthy();
		}
	}
	finally {
		await settings.uninitialize();
	}
});

skip_test("debugger connection", async () => {

	const settings = new PlaygroundDebugSettings({webDebugger: false, directDebugging: true, jsEngine: "Hermes"});
	await settings.initialize();
	try {
		await selectPackage("Samples\\debugTest01");

		const debugTargets = await getDebugTargets();
		expect(debugTargets.length).toBe(2);

		// testLog.message(`debugTarget0: URL: "${debugTargets[0].webSocketDebuggerUrl}", VM: "${debugTargets[0].vm}"`);
		// testLog.message(`debugTarget1: URL: "${debugTargets[1].webSocketDebuggerUrl}", VM: "${debugTargets[1].vm}"`);
		const ws = new WebSocket(debugTargets[0].webSocketDebuggerUrl);

		ws.on('open', () => {
			testLog.message(`WS on-open: sending Debugger.enable`);
			ws.send(`{ "id":2, "method":"Debugger.enable", "params":{ "maxScriptsCacheSize":10000000 } }`)
			testLog.message(`WS on-open: sending Debugger.pause`);
			ws.send(`{ "id":11, "method":"Debugger.pause", "params":{}}`);
		});


		// 	{ "id":2, "method":"Debugger.enable", "params":{ "maxScriptsCacheSize":10000000 } }

		ws.on('close', () => { testLog.message(`WS on-close`);});

		ws.on('message', (data) => {
			testLog.message(`WS on-message, typeof ${typeof data}`);

			if(typeof data == 'string') {
				testLog.message(`WS message data: "${data}"`);
			}
		});

		await sleep(5000);

		ws.close();

	}
	finally {
		await settings.uninitialize();
	}
});

test("enable-disable", async () => {
	testLog.message(`test enable-disable`);

	const settings = new PlaygroundDebugSettings({webDebugger: false, directDebugging: true, jsEngine: "Hermes"});
	await settings.initialize();
	try {
		await selectPackage("Samples\\debugTest01");

		const debugTargets = await getDebugTargets();
		const dbg = new CDPDebugger(debugTargets[0].webSocketDebuggerUrl);

		await dbg.DebuggerEnable();
		await dbg.DebuggerDisable();
		await dbg.checkOutstandingResponses(3000);

		dbg.close();
	}
	finally {
		await settings.uninitialize();
	}
});

test("pause-resume", async () => {
	testLog.message(`test pause-resume`);

	const settings = new PlaygroundDebugSettings({webDebugger: false, directDebugging: true, jsEngine: "Hermes"});
	await settings.initialize();
	try {
		await selectPackage("Samples\\debugTest01");

		const debugTargets = await getDebugTargets();
		const dbg = new CDPDebugger(debugTargets[0].webSocketDebuggerUrl);

		dbg.DebuggerEnable();


		const pausedEvent = dbg.expectEvent('paused');
		dbg.DebuggerPause();
		const eventArgs = await pausedEvent;

		// Example:
		//	{
		//		"method":"Debugger.paused",
		//		"params":
		//		{
		//			"callFrames":
		//			[
		//				{
		//					"callFrameId":"0",
		//					"functionName":"flushedQueue",
		//					"location":{"scriptId":"3","lineNumber":23258,"columnNumber":20},
		//					"scopeChain":
		//					[
		//						{
		//							"type":"global",
		//							"object":{"type":"object","className":"Object","objectId":"1"}
		//						}
		//					],
		//					"url":"",
		//					"this":{"type":"object","objectId":"2"}
		//				}
		//			],
		//			"reason":"other"
		//		}
		//	}
		expect(eventArgs).toHaveProperty("callFrames");
		expect(eventArgs).toHaveProperty("reason");
		expect(eventArgs.callFrames.length).toBeGreaterThanOrEqual(1);
		expect(eventArgs.callFrames[0]).toHaveProperty("functionName");

		const resumedEvent = dbg.expectEvent('resumed');
		dbg.DebuggerResume();
		await resumedEvent;

		await dbg.checkOutstandingResponses(3000);

		dbg.close();

	}
	finally {
		await settings.uninitialize();
	}
});

skip_test("breakpoint", async () => {
	testLog.message(`test breakpoint`);

	const settings = new PlaygroundDebugSettings({webDebugger: false, directDebugging: true, jsEngine: "Hermes"});
	await settings.initialize();
	try {
		await selectPackage("Samples\\debugTest01");

		const debugTargets = await getDebugTargets();
		const dbg = new CDPDebugger(debugTargets[0].webSocketDebuggerUrl);

		dbg.DebuggerEnable();

		const pausedEvent = dbg.expectEvent('paused');
		dbg.DebuggerPause();
		await pausedEvent;

		// currently not working, submitting to indicate intended API usage
		const [breakpointId, locations] = await dbg.DebuggerSetBreakpointByUrl(
			/* url */ "http://localhost:8081/Samples/debugTest01.bundle?platform=windows&dev=true&hot=true&inlineSourceMap=true",
			/* urlRegex */ null, 
			/* lineNumber */1392,
			/* columnNumber */ 0,
			/* scriptHash */ null);

		expect(breakpointId).toBe(123);

		const resumedEvent = dbg.expectEvent('resumed');
		dbg.DebuggerResume();
		await resumedEvent;

		await dbg.checkOutstandingResponses(3000);

		dbg.close();

	}
	finally {
		await settings.uninitialize();
	}
});