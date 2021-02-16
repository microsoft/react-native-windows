/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

/**
 * Loads Doxygen model using xml2js component.
 * Provides partial typings for loaded data structures used by Doxysaurus project.
 */

import path from 'path';
import xml2js from 'xml2js';
import {log} from './logger';
import {promises as fs} from 'fs';
import {Config} from './config';

export class DoxModel {
  compounds: {[index: string]: DoxCompound} = {};

  constructor(public config: Config) {}

  static async load(config: Config): Promise<DoxModel> {
    const model = new DoxModel(config);
    const indexPath = path.join(config.buildDir, 'xml', 'index.xml');
    log(`Loading index {${indexPath}}`);
    const indexText = await fs.readFile(indexPath, 'utf8');
    const indexXml = <IndexRootType>await xml2js.parseStringPromise(indexText);
    await model.loadCompounds(indexXml);
    return model;
  }

  private async loadCompounds(indexXml: IndexRootType) {
    for (const compound of indexXml.doxygenindex.compound) {
      switch (compound.$.kind) {
        case 'class':
        case 'struct':
          await this.loadCompound(compound);
          break;
        default:
          break;
      }
    }
  }

  private async loadCompound(compound: IndexCompoundType) {
    const compoundPath = path.join(
      this.config.buildDir,
      'xml',
      `${compound.$.refid}.xml`,
    );
    log(`Loading compound {${compoundPath}}`);
    const compoundText = await fs.readFile(compoundPath, 'utf8');
    const compoundXml = <CompoundRootType>await xml2js.parseStringPromise(
      compoundText,
      {
        explicitChildren: true,
        preserveChildrenOrder: true,
        charsAsChildren: true,
        includeWhiteChars: true,
      },
    );
    const compoundDef = compoundXml.doxygen.compounddef[0];
    this.compounds[compoundDef.$.id] = compoundDef;
  }
}

interface CompoundRootType {
  doxygen: {compounddef: DoxCompound[]};
}

export interface DoxCompound {
  $: {id: string; kind: DoxCompoundKind};
  compoundname: {_: string}[]; // there is only one entry
  basecompoundref?: {$: {prot: string}; _: string}[];
  sectiondef?: DoxSection[];
  briefdescription: DoxDescription;
  detaileddescription: DoxDescription;
  location: {$: {file: string}}[];
}

export interface DoxSection {
  $: {kind: DoxSectionKind};
  memberdef: DoxMember[];
  header: {_: string}[];
}

export interface DoxMember {
  $: {
    id: string;
    kind: DoxMemberKind;
    prot: string;
    static: DoxBool;
    virt: string;
    explicit: DoxBool;
  };
  name: {_: string}[]; // there is only one entry
  templateparamlist?: DoxTemplateParamList[];
  type: DoxDescription;
  param?: {type: DoxDescription; declname: DoxDescription}[];
  briefdescription: DoxDescription;
  detaileddescription: DoxDescription;
  argsstring: DoxDescription;
  location: {$: {line: string}}[];
}

export interface DoxTemplateParamList {
  param?: {type: DoxDescription; defval?: {_: string}[]}[];
}

export type DoxDescription =
  | undefined
  | string
  | DoxDescription[]
  | DoxDescriptionElement;

export interface DoxDescriptionElement {
  '#name'?: string;
  $: {
    refid: string;
    kindref: DoxRefKind;
    kind: DoxDescriptionKind;
    thead: DoxBool;
    url: string;
    level?: string;
  };
  $$: DoxDescription[];
  _?: string;
}

interface IndexRootType {
  doxygenindex: {compound: IndexCompoundType[]};
}

interface IndexCompoundType {
  $: {
    refid: string;
    kind: DoxCompoundKind;
  };
}

export type DoxCompoundKind =
  | 'class'
  | 'struct'
  | 'union'
  | 'interface'
  | 'protocol'
  | 'category'
  | 'exception'
  | 'service'
  | 'singleton'
  | 'module'
  | 'type'
  | 'file'
  | 'namespace'
  | 'group'
  | 'page'
  | 'example'
  | 'dir';

export type DoxSectionKind =
  | 'user-defined'
  | 'public-type'
  | 'public-func'
  | 'public-attrib'
  | 'public-slot'
  | 'signal'
  | 'dcop-func'
  | 'property'
  | 'event'
  | 'public-static-func'
  | 'public-static-attrib'
  | 'protected-type'
  | 'protected-func'
  | 'protected-attrib'
  | 'protected-slot'
  | 'protected-static-func'
  | 'protected-static-attrib'
  | 'package-type'
  | 'package-func'
  | 'package-attrib'
  | 'package-static-func'
  | 'package-static-attrib'
  | 'private-type'
  | 'private-func'
  | 'private-attrib'
  | 'private-slot'
  | 'private-static-func'
  | 'private-static-attrib'
  | 'friend'
  | 'related'
  | 'define'
  | 'prototype'
  | 'typedef'
  | 'enum'
  | 'func'
  | 'var';

export type DoxMemberKind =
  | 'define'
  | 'property'
  | 'event'
  | 'variable'
  | 'typedef'
  | 'enum'
  | 'function'
  | 'signal'
  | 'prototype'
  | 'friend'
  | 'dcop'
  | 'slot'
  | 'interface'
  | 'service';

export type DoxRefKind = 'compound' | 'member' | '<unknown>';

export type DoxDescriptionKind =
  | 'exception'
  | 'attention'
  | 'return'
  | 'see'
  | 'param'
  | 'retval'
  | 'other';

export type DoxBool = 'yes' | 'no';
