/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

/**
 * Renders Markdown based documentation model to markdown files by applying Mustache templates.
 */

import mustache from 'mustache';
import path from 'path';
import {log} from './logger';
import {promises as fs} from 'fs';
import {Config} from './config';
import {DocModel} from './doc-model';

const templateCache: {[index: string]: string} = {};

export async function renderDocFiles(
  docModel: DocModel,
  config: Config,
): Promise<string[]> {
  const renderedFiles: string[] = [];
  await fs.mkdir(config.outputDir, {recursive: true});

  const templatePath = path.normalize(
    path.join(__dirname, '..', 'templates', 'cpp', 'class.md.mustache'),
  );
  const template = await getCachedTemplate(templatePath);

  for (const compound of Object.values(docModel.compounds)) {
    const outputFileName = path.join(config.outputDir, `${compound.docId}.md`);
    log(`[Rendering] file {${outputFileName}}`);
    const outputText = mustache.render(template, compound);
    await fs.writeFile(outputFileName, outputText, 'utf-8');
    renderedFiles.push(outputFileName);
  }

  if (config.indexFilename && config.indexTemplatePath) {
    const indexTemplatePath = path.join(config.input, config.indexTemplatePath);
    const indexTemplate = await getCachedTemplate(indexTemplatePath);
    const indexOutput = path.join(config.outputDir, config.indexFilename);
    log(`[Rendering] file {${indexOutput}}`);
    const outputText = mustache.render(indexTemplate, docModel);
    await fs.writeFile(indexOutput, outputText, 'utf-8');
    renderedFiles.push(indexOutput);
  }

  return renderedFiles;
}

async function getCachedTemplate(templatePath: string): Promise<string> {
  let template = templateCache[templatePath];
  if (!template) {
    template = await fs.readFile(templatePath, 'utf-8');
    templateCache[templatePath] = template;
    mustache.parse(template);
  }
  return template;
}
