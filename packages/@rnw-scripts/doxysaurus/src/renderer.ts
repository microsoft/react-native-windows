/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 **/

//
// Renders Markdown based documentation model to markdown files by applying Mustache templates.
//

import mustache from 'mustache';
import path from 'path';
import {log} from './logger';
import {promises as fs} from 'fs';
import {Config} from './config';
import {DocModel} from './doc-model';

const templateCache: {[index: string]: string} = {};

export async function renderDocFiles(docModel: DocModel, config: Config) {
  const renderedFiles: string[] = [];
  const outputPath = path.join(config.output, 'out');
  await fs.mkdir(outputPath).catch(err => {
    if (err.code !== 'EEXIST') throw err;
  });

  const templatePath = path.normalize(
    path.join(__dirname, '..', 'templates', 'cpp', 'class.md'),
  );
  const template = await getCachedTemplate(templatePath);

  for (const compound of Object.values(docModel.compounds)) {
    const outputFileName = path.join(outputPath, `${compound.docId}.md`);
    log(`[Rendering] file {${outputFileName}}`);
    const outputText = mustache.render(template, compound);
    await fs.writeFile(outputFileName, outputText, 'utf-8');
    renderedFiles.push(outputFileName);
  }

  if (config.index && config.indexTemplate) {
    const indexTemplatePath = path.join(config.input, config.indexTemplate);
    const indexTemplate = await getCachedTemplate(indexTemplatePath);
    const indexOutput = path.join(outputPath, config.index);
    log(`[Rendering] file {${indexOutput}}`);
    const outputText = mustache.render(indexTemplate, docModel);
    await fs.writeFile(indexOutput, outputText, 'utf-8');
    renderedFiles.push(indexOutput);
  }

  return renderedFiles;
}

async function getCachedTemplate(templatePath: string) {
  let template = templateCache[templatePath];
  if (!template) {
    template = await fs.readFile(templatePath, 'utf-8');
    templateCache[templatePath] = template;
    mustache.parse(template);
  }
  return template;
}
