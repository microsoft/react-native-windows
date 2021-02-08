/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

/**
 * The Doxysaurus uses Doxygen to generate documentation based on code,
 * and then generates markdown files to be used by the Docusaurus service.
 */

import fs from 'fs';
import path from 'path';
import yargs from 'yargs';
import {log} from './logger';
import {getProjectConfigs, Config} from './config';
import {generateDoxygenXml} from './doxygen';
import {DoxModel} from './doxygen-model';
import {transformToMarkdown} from './transformer';
import {renderDocFiles} from './renderer';

const argv = yargs
  .options({
    config: {
      alias: 'c',
      describe: 'config file',
      type: 'string',
      demandOption: true,
    },
    log: {
      alias: 'l',
      describe: 'log file',
      type: 'string',
    },
    output: {
      alias: 'o',
      describe: 'output directory',
      type: 'string',
    },
    quiet: {
      alias: 'q',
      describe: 'quiet mode',
      type: 'boolean',
    },
    watch: {
      alias: 'w',
      describe: 'watch mode',
      type: 'boolean',
    },
  })
  .version(false)
  .help(false).argv;

fireAndForget(async () => {
  log.init({quiet: argv.quiet, logFile: argv.log});

  const configList: Config[] = [];
  for await (const config of getProjectConfigs(
    argv.config,
    argv.output ? path.resolve(process.cwd(), argv.output) : undefined,
  )) {
    configList.push(config);
    await processProject(config);
  }

  if (argv.watch) {
    for (const config of configList) {
      watchProject(config);
    }
  }
});

async function processProject(config: Config): Promise<void> {
  log(`[Start] processing project {${config.input}}`);
  log('Project config: ', config);

  await generateDoxygenXml(config);
  const doxModel = await DoxModel.load(config);
  const docModel = transformToMarkdown(doxModel, config);
  await renderDocFiles(docModel, config);

  log(`[Finished] processing project {${config.input}}`);
}

function watchProject(config: Config): void {
  log(`[Watching] folder {${config.input}} ...`);

  // We use delayTimer to coalesce multiple events coming at the same time
  let delayTimer: number | undefined;
  fs.watch(config.input, () => {
    clearTimeout(delayTimer);
    delayTimer = setTimeout(() => fireAndForget(startProcessingProject), 100);
  });

  // Make sure that we do not process the same project multiple times concurrently.
  let isProcessing = false;
  let shouldProcess = false;
  async function startProcessingProject() {
    if (isProcessing) {
      // Schedule the processing after the current one is done.
      shouldProcess = true;
    } else {
      isProcessing = true;
      await processProject(config);
      isProcessing = false;

      if (shouldProcess) {
        // Handle file changes that came after we started processing.
        shouldProcess = false;
        await startProcessingProject();
      } else {
        // We are done with processing. Wait for the next change.
        log(`[Watching] folder {${config.input}} ...`);
      }
    }
  }
}

/** Handles promise rejection and returns void instead of Promise<void>. */
function fireAndForget(asyncFunc: () => Promise<void>): void {
  (async () => {
    try {
      await asyncFunc();
    } catch (err) {
      try {
        log.error(err);
      } catch (logError) {
        console.error(logError);
      }

      process.exit(1);
    }
  })();
}
