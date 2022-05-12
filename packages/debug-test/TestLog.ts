
import * as fs from 'fs'

// REVIEW: Does jest have a more suitable logging facility?

export const testLog = new class {
	constructor() {
		this.fileName = `${__dirname}/test.${this.formatDateTime(new Date())}.log`;
	}

	public message(message: string) {
		this.write(message);
	}

	public warning(message: string) {
		this.write(`WARNING: ${message}`);
	}

	public error(message: string) {
		this.write(`ERROR: ${message}`);
	}

	private write(message: string) {
		// console.log(message);
		const time = new Date();
		fs.writeFileSync(this.fileName, `${this.formatTime(time)}: ${message}\n`, {flag: "a"});
	}

	private formatDateTime(dt: Date) {
		const YYYY = ("000" + dt.getFullYear()).slice(-4);
		const MM = ("0" + (dt.getMonth() + 1)).slice(-2);
		const DD = ("0" + (dt.getDate())).slice(-2);
		const hh = ("0" + (dt.getHours())).slice(-2);
		const mm = ("0" + (dt.getMinutes())).slice(-2);
		const ss = ("0" + (dt.getSeconds())).slice(-2);
		return `${YYYY}${MM}${DD}-${hh}${mm}${ss}`;
	}

	private formatTime(dt: Date) {
		const hh = ("0" + (dt.getHours())).slice(-2);
		const mm = ("0" + (dt.getMinutes())).slice(-2);
		const ss = ("0" + (dt.getSeconds())).slice(-2);
		const ms = ("000" + dt.getMilliseconds()).slice(-4)
		return `${hh}:${mm}:${ss}.${ms}`;
	}
	private fileName : string;
};
