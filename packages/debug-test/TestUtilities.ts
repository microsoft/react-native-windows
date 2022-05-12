import * as http from 'http';

export async function sleep(ms: number) : Promise<void> {
	return new Promise(resolve => setTimeout(resolve, ms));
}

export function shortenIfNeeded(s: string, maxLength: number): string {
	if (maxLength < 4) throw new Error("shortenIfNeeded requires minimum length of 4");
	if (s.length <= maxLength) {
		return s;
	}
	return s.slice(0, maxLength - /* length of "..." */ 3) + "...";
}

export class DebugTarget {
	id:string;
	title: string;
	faviconUrl: string;
	devtoolsFrontendUrl: string;
	type: string;
	webSocketDebuggerUrl: string;
	vm: string;
}

export async function getDebugTargets() : Promise<DebugTarget[]> {
	return new Promise((resolve, reject) => {
		const options : http.RequestOptions = {
			hostname: 'localhost',
			port: 8081,
			path: '/json',
			method: 'GET'
		};

		const request = http.request(options, response => {
			response.on('data', data => {
				const debugTargets = JSON.parse(data.toString());
				resolve(debugTargets);
			})
		});

		request.end();
	});
}
