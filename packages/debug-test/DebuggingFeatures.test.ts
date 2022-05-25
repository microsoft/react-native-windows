import * as http from 'http';
import { CDPDebugger } from './CDPDebugger';
import { Metro } from './MetroAutomation';
import { PlaygroundDebugSettings, selectPackage } from './PlaygroundAutomation'
import { testLog } from './TestLog';
import { getDebugTargets } from './TestUtilities';

const metro = new Metro();

beforeAll(async () => {
	await metro.start();
});

afterAll(() => {
	metro.stop();
});

// beforeEach(() => {});
// afterEach(() => {});

// helper to deactivate test methods
function skipTest(_msg: string, _fn: () => {}){}

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

		const request = http.request(options);
		const dataPromise = new Promise<Buffer>((resolve, _reject) => {
			request.on("response", response => response.on("data", data => resolve(data)))
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

test("enable-disable", async () => {
	testLog.message(`test enable-disable`);

	const settings = new PlaygroundDebugSettings({webDebugger: false, directDebugging: true, jsEngine: "Hermes"});
	await settings.initialize();
	try {
		await selectPackage("Samples\\debugTest01");

		const debugTargets = await getDebugTargets();
		const dbg = new CDPDebugger(debugTargets[0].webSocketDebuggerUrl);

		await dbg.debuggerEnable();
		await dbg.debuggerDisable();
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

		await dbg.debuggerEnable();

		const pausedEvent = dbg.expectEvent('paused');
		await dbg.debuggerPause();
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
		await dbg.debuggerResume();
		await resumedEvent;

		await dbg.checkOutstandingResponses(3000);

		dbg.close();

	}
	finally {
		await settings.uninitialize();
	}
});

skipTest("breakpoint", async () => {
	testLog.message(`test breakpoint`);

	const settings = new PlaygroundDebugSettings({webDebugger: false, directDebugging: true, jsEngine: "Hermes"});
	await settings.initialize();
	try {
		await selectPackage("Samples\\debugTest01");

		const debugTargets = await getDebugTargets();
		const dbg = new CDPDebugger(debugTargets[0].webSocketDebuggerUrl);

		await dbg.debuggerEnable();

		const pausedEvent = dbg.expectEvent('paused');
		await dbg.debuggerPause();
		await pausedEvent;

		// currently not working, submitting to indicate intended API usage
		const [breakpointId] = await dbg.debuggerSetBreakpointByUrl(
			/* url */ "http://localhost:8081/Samples/debugTest01.bundle?platform=windows&dev=true&hot=true&inlineSourceMap=true",
			/* urlRegex */ "", 
			/* lineNumber */1392,
			/* columnNumber */ 0,
			/* scriptHash */ "");

		expect(breakpointId).toBe(123);

		const resumedEvent = dbg.expectEvent('resumed');
		await dbg.debuggerResume();
		await resumedEvent;

		await dbg.checkOutstandingResponses(3000);

		dbg.close();

	}
	finally {
		await settings.uninitialize();
	}
});