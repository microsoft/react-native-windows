/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * This code is partially based on moxygen and doxygen2md code.
 * The copyrights of moxygen and doxygen2md projects:
 * Copyright (c) 2016 Philippe FERDINAND
 * Copyright (c) 2016 Kam Low
 *
 * @format
 **/

//
// Transforms Doxygen documentation model to the Markdown based model to be used by Docusaurus service.
//

import {Config} from './config';
import {DoxModel, DoxCompound, DoxMember} from './doxygen-model';
import {
  DocModel,
  DocCompound,
  DocSection,
  DocMemberOverload,
  DocMember,
} from './doc-model';
import GithubSlugger from 'github-slugger';
import path from 'path';
import {log} from './logger';
import {toMarkdown, LinkResolver, StringBuilder} from './markdown';

export function transformToMarkdown(doxModel: DoxModel, config: Config) {
  const docModel = new DocModel();

  const docIdToDoxCompound = new Map<string, DoxCompound | undefined>();
  const doxIdToDocCompound = new Map<string, DocCompound | undefined>();

  const doxMemberIdToMemberOverload = new Map<
    string,
    DocMemberOverload | undefined
  >();
  const memberOverloadToCompound = new Map<DocMemberOverload, DocCompound>();
  const memberToDoxMember = new Map<DocMember, DoxMember>();

  const types = new Set<string>(config.types ?? []);
  const namespaceAliases = new Map<string, string[] | undefined>(
    config.namespaceAliases ?? [],
  );
  const knownSections = new Map<string, string>(config.sections ?? []);

  const linkResolver: LinkResolver = {
    stdTypeLinks: {
      linkPrefix: config.stdTypeLinks?.linkPrefix ?? '',
      linkMap: new Map<string, string>(config.stdTypeLinks?.linkMap ?? []),
      operatorMap: new Map<string, string>(
        config.stdTypeLinks?.operatorMap ?? [],
      ),
    },
    idlTypeLinks: {
      linkPrefix: config.idlTypeLinks?.linkPrefix ?? '',
      linkMap: new Map<string, string>(config.idlTypeLinks?.linkMap ?? []),
      operatorMap: new Map<string, string>(
        config.idlTypeLinks?.operatorMap ?? [],
      ),
    },
    resolveCompoundId: (doxCompoundId: string): DocCompound | undefined => {
      return doxIdToDocCompound.get(doxCompoundId);
    },
    resolveMemberId: (
      doxMemberId: string,
    ): [DocCompound | undefined, DocMemberOverload | undefined] => {
      const memberOverload = doxMemberIdToMemberOverload.get(doxMemberId);
      if (memberOverload) {
        return [memberOverloadToCompound.get(memberOverload), memberOverload];
      } else {
        return [undefined, undefined];
      }
    },
  };

  for (const doxCompoundId of Object.keys(doxModel.compounds)) {
    const doxCompound = doxModel.compounds[doxCompoundId];
    switch (doxCompound.$.kind) {
      case 'struct':
      case 'class':
        transformClass(doxCompound);
        break;
      default:
        break;
    }
  }

  for (const compound of Object.values(docModel.compounds)) {
    compoundToMarkdown(compound);
  }

  return docModel;

  // eslint-disable-next-line complexity
  function transformClass(doxCompound: DoxCompound) {
    const doxCompoundName = doxCompound.compoundname[0]._;
    log(`[Transforming] ${doxCompoundName}`);
    const noTemplateName = doxCompoundName.split('<')[0];
    const nsp = noTemplateName.split('::');
    const compound = new DocCompound();
    compound.namespace = nsp.splice(0, nsp.length - 1).join('::');
    compound.namespaceAliases = [];
    compound.namespaceAliases = namespaceAliases.get(compound.namespace) ?? [];
    compound.name = nsp[nsp.length - 1];
    if (!types.has(compound.name)) {
      log(`[Skipped] {${doxCompoundName}}: not in config.types`);
      return;
    }
    compound.docId = `${config.prefix}${compound.name.toLowerCase()}`;
    compound.codeFileName = path.basename(doxCompound.location[0].$.file);
    docModel.compounds[compound.docId] = compound;
    docModel.classes.push(compound);
    doxIdToDocCompound.set(doxCompound.$.id, compound);
    docIdToDoxCompound.set(compound.docId, doxCompound);

    const compoundMemberOverloads = new Map<string, DocMemberOverload>();

    const slugger = new GithubSlugger();

    if (Array.isArray(doxCompound.sectiondef)) {
      const visibleSections: {[index: string]: DocSection} = {};

      for (const sectionDef of doxCompound.sectiondef) {
        const sectionKind = sectionDef.$.kind;
        const sectionName = knownSections.get(sectionKind);

        if (typeof sectionName === 'undefined') {
          throw new Error(`Unknown section kind ${sectionKind}`);
        }

        if (sectionName === '<not visible>') {
          continue;
        }

        let section: DocSection;
        if (sectionName === '<user defined>') {
          section = new DocSection();
          section.name = sectionDef.header[0]._;
          visibleSections[section.name] = section;
        } else {
          section = visibleSections[sectionName];
          if (typeof section === 'undefined') {
            section = new DocSection();
            section.name = sectionName;
            visibleSections[sectionName] = section;
          }
        }

        const memberOverloads = new Map<string, DocMemberOverload>();

        for (const memberDef of sectionDef.memberdef) {
          const memberName = memberDef.name[0]._;
          const member = new DocMember();
          member.line = Number(memberDef.location[0].$.line);
          memberToDoxMember.set(member, memberDef);
          member.name = memberName;
          const overloadName =
            memberName === compound.name
              ? '(constructor)'
              : memberName === '~' + compound.name
              ? '(destructor)'
              : memberName === 'operator='
              ? 'assignment operator='
              : memberName === 'operator=='
              ? 'equal operator=='
              : memberName === 'operator!='
              ? 'not equal operator!='
              : memberName === 'operator[]'
              ? 'subscript operator[]'
              : memberName;
          let memberOverload = compoundMemberOverloads.get(overloadName);
          if (typeof memberOverload === 'undefined') {
            memberOverload = new DocMemberOverload();
            memberOverloadToCompound.set(memberOverload, compound);
            memberOverload.name = overloadName;
            memberOverload.line = member.line;
            compoundMemberOverloads.set(overloadName, memberOverload);
          }
          if (!memberOverloads.has(overloadName)) {
            memberOverloads.set(overloadName, memberOverload);
            section.memberOverloads.push(memberOverload);
          }
          memberOverload.members.push(member);
          memberOverload.line = Math.min(memberOverload.line, member.line);

          doxMemberIdToMemberOverload.set(memberDef.$.id, memberOverload);

          if (section.line === 0) {
            section.line = member.line;
          } else {
            section.line = Math.min(section.line, member.line);
          }
        }

        section.memberOverloads.sort((a, b) => a.line - b.line);
      }

      const sections: DocSection[] = Object.values(visibleSections);
      // Put deprecated section to the end
      sections.forEach(
        s =>
          (s.line = s.name.includes('Deprecated')
            ? Number.MAX_SAFE_INTEGER
            : s.line),
      );
      sections.sort((a, b) => a.line - b.line);
      compound.sections = sections;
    }

    for (const section of compound.sections) {
      for (const memberOverload of section.memberOverloads) {
        memberOverload.anchor = '#' + slugger.slug(memberOverload.name);
      }
    }

    log('[Compound] dump: ', compound);
  }

  function compoundToMarkdown(compound: DocCompound) {
    const doxCompound = docIdToDoxCompound.get(compound.docId);
    compound.brief = toMarkdown(doxCompound?.briefdescription, linkResolver);
    compound.details = toMarkdown(
      doxCompound?.detaileddescription,
      linkResolver,
    );
    compound.summary = createSummary(compound.brief, compound.details);

    compound.declaration = createCompoundDeclaration();
    for (const section of compound.sections) {
      for (const memberOverload of section.memberOverloads) {
        for (const member of memberOverload.members) {
          const memberDef = memberToDoxMember.get(member)!;
          member.brief = toMarkdown(memberDef.briefdescription, linkResolver);
          member.details = toMarkdown(
            memberDef.detaileddescription,
            linkResolver,
          );
          member.summary = createSummary(member.brief, member.details);

          if (!memberOverload.summary) {
            if (memberOverload.name === '(constructor)') {
              memberOverload.summary = `constructs the ${'[`' +
                compound.name +
                '`](' +
                compound.docId +
                ')'}`;
            } else if (memberOverload.name === '(destructor)') {
              memberOverload.summary = `destroys the ${'[`' +
                compound.name +
                '`](' +
                compound.docId +
                ')'}`;
            } else {
              memberOverload.summary = member.summary.replace(/^[A-Z]/, match =>
                match.toLowerCase(),
              );
            }
          }

          member.declaration = createMemberDeclaration(memberDef);
        }
      }
    }

    function createSummary(brief: string, details: string) {
      let summary = brief.trim();
      if (!summary) summary = details.trim().split('\n', 1)[0];
      if (!summary) summary = '&nbsp;';
      return summary;
    }

    function createCompoundDeclaration() {
      const sb = new StringBuilder();
      sb.write(doxCompound?.$.kind ?? '', ' ', compound.name);
      doxCompound?.basecompoundref?.forEach((base, index) => {
        sb.write('\n    ', index ? ', ' : ': ');
        sb.write(base.$.prot, ' ');
        sb.write(base._.replace('< ', '<').replace(' >', '>'));
      });
      return sb.toString();
    }

    function createMemberDeclaration(memberDef: DoxMember) {
      const sb = new StringBuilder();
      if (memberDef.templateparamlist) {
        sb.write('template<');
        memberDef.templateparamlist[0].param?.forEach((param, index) => {
          sb.writeIf(',', index !== 0);
          sb.write(toMarkdown(param.type));
          if (param.defval?.[0]._) {
            sb.write(' = ', param.defval[0]._);
          }
        });
        sb.write('>  \n');
      }
      sb.write(memberDef.$.prot, ': '); // public, private, ...
      sb.writeIf('static ', memberDef.$.static === 'yes');
      sb.writeIf('virtual ', memberDef.$.virt === 'virtual');
      writeType();
      sb.writeIf('explicit ', memberDef.$.explicit === 'yes');
      sb.write(memberDef.name[0]._);
      sb.write(
        toMarkdown(memberDef.argsstring)
          .trim()
          .replace('=', ' = '),
      );
      sb.write(';');

      return sb.toString();

      function writeType() {
        let memberType = toMarkdown(memberDef.type);
        if (memberType.trim() !== '') {
          memberType = memberType.replace(/\s+/g, ' ');
          sb.write(memberType);
          sb.write(
            memberType.endsWith('&') || memberType.endsWith('*') ? '' : ' ',
          );
        }
      }
    }
  }
}
