/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as ora from 'ora';

export type Logger = {
  newTask: (taskName: string) => void;
  info: (message: string) => void;
  success: (taskName: string, body?: string) => void;
  warn: (taskName: string, body?: string) => void;
  error: (taskName: string, body?: string) => void;
  close: () => void;
};

export class ConsoleLogger implements Logger {
  private readonly spinner: ora.Ora;

  constructor(private readonly stream: NodeJS.WritableStream) {
    this.spinner = ora({stream});
  }

  newTask(taskName: string) {
    if (this.spinner.isSpinning) {
      throw new Error('A task has already been started');
    }

    this.spinner.start(taskName);
  }

  info(message: string) {
    this.spinner.info(message);
  }

  success(taskName: string, body?: string) {
    this.spinner.succeed(this.combineBody(taskName, body));
  }

  warn(taskName: string, body?: string) {
    this.spinner.warn(this.combineBody(taskName, body));
  }

  error(taskName: string, body?: string) {
    this.spinner.fail(this.combineBody(taskName, body));
  }

  close() {
    this.stream.end();
  }

  private combineBody(taskName: string, body?: string) {
    if (body === undefined) {
      return taskName;
    } else {
      return `${taskName}\n${body}`;
    }
  }
}

export class MarkdownLogger implements Logger {
  constructor(private readonly stream: NodeJS.WritableStream) {}

  newTask(_taskName: string) {
    // Ignore until completion
  }

  info(message: string, body?: string) {
    this.stream.write(`### ℹ ${message}\n`);
    this.logBody(body);
  }

  success(taskName: string, body?: string) {
    this.stream.write(`### ✅ ${taskName}\n`);
    this.logBody(body);
  }

  warn(taskName: string, body?: string) {
    this.stream.write(`### ⚠ ${taskName}\n`);
    this.logBody(body);
  }

  error(taskName: string, body?: string) {
    this.stream.write(`### ❌ ${taskName}\n`);
    this.logBody(body);
  }

  close() {
    this.stream.end();
  }

  private logBody(body?: string) {
    if (body) {
      this.stream.write('```\n' + body + '\n```\n');
    }
  }
}

export class CompositeLogger implements Logger {
  constructor(private readonly loggers: Logger[]) {}

  newTask(taskName: string) {
    this.loggers.forEach(logger => logger.newTask(taskName));
  }

  info(message: string) {
    this.loggers.forEach(logger => logger.info(message));
  }

  success(taskName: string, body?: string) {
    this.loggers.forEach(logger => logger.success(taskName, body));
  }

  warn(taskName: string, body?: string) {
    this.loggers.forEach(logger => logger.warn(taskName, body));
  }

  error(taskName: string, body?: string) {
    this.loggers.forEach(logger => logger.error(taskName, body));
  }

  close() {
    this.loggers.forEach(logger => logger.close());
  }
}
