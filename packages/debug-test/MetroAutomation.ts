
// spawn: run process asynchronously (CreateProc?)
// exec: spawns a shell and runs a command within that shell, passing the stdout and stderr to a callback function when complete.
// Each of the methods returns a ChildProcess instance. 
// fork
// The child_process.fork() method is a special case of child_process.spawn() used specifically to spawn new Node.js processes. Like child_process.spawn()
import { ChildProcess, exec, fork, spawn } from 'child_process';
import * as path from 'path';
import {testLog} from './TestLog';

export class Metro {

	async start() : Promise<void> {
		return new Promise((resolve, reject) => {
			// TODO: check if Metro is already running
			// 
			// If metro is already running, we get
			//     error listen EADDRINUSE: address already in use :::8081.
			//     Error: listen EADDRINUSE: address already in use :::8081
			//         at Server.setupListenHandle [as _listen2] (node:net:1330:16)
			//         at listenInCluster (node:net:1378:12)
			// Thus, can we check if port 8081 is already in use?

			const repoRootDir = path.normalize(`${__dirname}\\..\\..`)
			const metroScript = `${repoRootDir}\\node_modules\\react-native\\cli.js`;
			const metroCurrentDir = `${repoRootDir}\\packages\\playground`;

			testLog.message(`Metro script "${metroScript}"`);
			testLog.message(`Metro current directory "${metroCurrentDir}"`);

			this.metroProcess = fork(metroScript, ['start', '--no-interactive'], {
				cwd: metroCurrentDir,
				stdio: 'pipe'
			});

			testLog.message(`Metro process ID ${this.metroProcess.pid}`);

			// trying to sync with Metro being ready to serve packages
			this.metroProcess.stdout.on('data', data => {
				const s = data.toString();
				// debugger;
				testLog.message(`Metro stdout "${s}"`);
				if (s.includes('Welcome to Metro!')) {
					testLog.message(`Metro appears to be ready`);
					resolve();
				}
			});
		});
	}

	public stop() {
		// REVIEW: Better way of ending Metro?
		const killResult = this.metroProcess.kill('SIGINT');
		testLog.message(`stopped Metro (${killResult})`);
	}

	private metroProcess : ChildProcess;
}
