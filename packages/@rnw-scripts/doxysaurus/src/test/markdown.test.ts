/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import xml2js from 'xml2js';
import {applyTemplateRules} from './string-template';
import {DocCompound, DocMemberOverload} from '../doc-model';
import {DoxMember} from '../doxygen-model';
import {LinkResolver, TypeLinks, toMarkdown} from '../markdown';

//
// Test conversion from Doxygen XML to Markdown.
//
// These tests are using the string-template to have more natural string look and feel.
// The leading pipeline '|' is used as a start of a new line.
// All other end-of-line characters and leading spaces are removed from the string.
//
// The XML spaces are significant in Doxygen XML docs.
// We keep them as we parse XML.
// Using the pipeline '|' character we indent the XML text for better
// readability while keeping the original Doxygen XML formatting.
//

test('Empty brief description', async () => {
  const memberDef = await parse(`
    |<memberdef>
    |  <briefdescription>
    |  </briefdescription>
    |</memberdef>`);

  const text = toMarkdown(memberDef.briefdescription);
  expect(text).toBe('');
});

test('One paragraph', async () => {
  const memberDef = await parse(`
    |<memberdef>
    |  <briefdescription>
        |<para>Test</para>
    |  </briefdescription>
    |</memberdef>`);

  const text = toMarkdown(memberDef.briefdescription);
  expect(text).toBe('Test');
});

test('Three paragraphs', async () => {
  const memberDef = await parse(`
    |<memberdef>
    |  <briefdescription>
        |<para>Paragraph1</para>
        |<para>Paragraph2</para>
        |<para>Paragraph3</para>
    |  </briefdescription>
    |</memberdef>`);

  const text = toMarkdown(memberDef.briefdescription);
  expect(text).toBe(
    t(`Paragraph1
      |
      |Paragraph2
      |
      |Paragraph3`),
  );
});

test('Itemized list', async () => {
  const memberDef = await parse(`
    |<memberdef>
    |  <briefdescription>
        |<para><itemizedlist>
        |<listitem><para>item1</para>
        |</listitem><listitem><para>item2</para>
        |</listitem><listitem><para>item3</para>
        |</listitem></itemizedlist>
        |</para>
    |  </briefdescription>
    |</memberdef>`);

  const text = toMarkdown(memberDef.briefdescription);
  expect(text).toBe(
    t(`* item1
      |* item2
      |* item3
      `),
  );
});

test('Itemized list with paragraphs', async () => {
  const memberDef = await parse(`
  |<memberdef>
  |  <briefdescription>
      |<para><itemizedlist>
      |<listitem><para>item1 para1</para>
      |<para>item1 para2</para>
      |<para>item1 para3</para>
      |</listitem><listitem><para>item2</para>
      |</listitem><listitem><para>item3</para>
      |</listitem></itemizedlist>
      |</para>
  |  </briefdescription>
  |</memberdef>`);

  const text = toMarkdown(memberDef.briefdescription);
  expect(text).toBe(
    t(`* item1 para1
      |
      |  item1 para2
      |
      |  item1 para3
      |* item2
      |* item3
      `),
  );
});

test('Ordered list', async () => {
  const memberDef = await parse(`
  |<memberdef>
  |  <briefdescription>
      |<para><orderedlist>
      |<listitem><para>item1</para>
      |</listitem><listitem><para>item2</para>
      |</listitem><listitem><para>item3</para>
      |</listitem></orderedlist>
      |</para>
  |  </briefdescription>
  |</memberdef>`);

  const text = toMarkdown(memberDef.briefdescription);
  expect(text).toBe(
    t(`1. item1
      |1. item2
      |1. item3
      `),
  );
});

test('Ordered list with paragraphs', async () => {
  const memberDef = await parse(`
    |<memberdef>
    |  <briefdescription>
        |<para><orderedlist>
        |<listitem><para>item1 para1</para>
        |<para>item1 para2</para>
        |<para>item1 para3</para>
        |</listitem><listitem><para>item2</para>
        |</listitem><listitem><para>item3</para>
        |</listitem></orderedlist>
        |</para>
    |  </briefdescription>
    |</memberdef>`);

  const text = toMarkdown(memberDef.briefdescription);
  expect(text).toBe(
    t(`1. item1 para1
      |
      |   item1 para2
      |
      |   item1 para3
      |1. item2
      |1. item3
      `),
  );
});

test('Nested ordered list', async () => {
  const memberDef = await parse(`
    |<memberdef>
    |  <briefdescription>
        |<para><orderedlist>
        |<listitem><para>item1<orderedlist>
        |<listitem><para>item1 item1</para>
        |</listitem><listitem><para>item1 item2</para>
        |</listitem></orderedlist>
        |</para>
        |</listitem><listitem><para>item2<itemizedlist>
        |<listitem><para>item2 item1</para>
        |</listitem><listitem><para>item2 item2</para>
        |</listitem></itemizedlist>
        |</para>
        |</listitem><listitem><para>item3</para>
        |</listitem></orderedlist>
        |</para>
    |  </briefdescription>
    |</memberdef>`);

  const text = toMarkdown(memberDef.briefdescription);
  expect(text).toBe(
    t(`1. item1
      |   1. item1 item1
      |   1. item1 item2
      |1. item2
      |   * item2 item1
      |   * item2 item2
      |1. item3
      `),
  );
});

test('Nested itemized list', async () => {
  const memberDef = await parse(`
    |<memberdef>
    |  <briefdescription>
        |<para><itemizedlist>
        |<listitem><para>item1<orderedlist>
        |<listitem><para>item1 item1</para>
        |</listitem><listitem><para>item1 item2</para>
        |</listitem></orderedlist>
        |</para>
        |</listitem><listitem><para>item2<itemizedlist>
        |<listitem><para>item2 item1</para>
        |</listitem><listitem><para>item2 item2</para>
        |</listitem></itemizedlist>
        |</para>
        |</listitem><listitem><para>item3 </para>
        |</listitem></itemizedlist>
        |</para>
    |  </briefdescription>
    |</memberdef>`);

  const text = toMarkdown(memberDef.briefdescription);
  expect(text).toBe(
    t(`* item1
      |  1. item1 item1
      |  1. item1 item2
      |* item2
      |  * item2 item1
      |  * item2 item2
      |* item3
      `),
  );
});

test('Itemized list between paragraphs', async () => {
  const memberDef = await parse(`
    |<memberdef>
    |  <briefdescription>
        |<para>text1</para>
        |<para><itemizedlist>
        |<listitem><para>item1</para>
        |</listitem><listitem><para>item2</para>
        |</listitem><listitem><para>item3</para>
        |</listitem></itemizedlist>
        |</para>
        |<para>text2 </para>
    |  </briefdescription>
    |</memberdef>`);

  const text = toMarkdown(memberDef.briefdescription);
  expect(text).toBe(
    t(`text1
      |
      |* item1
      |* item2
      |* item3
      |
      |text2
      `),
  );
});

test('Bold and emphasis', async () => {
  const memberDef = await parse(`
    |<memberdef>
    |  <briefdescription>
        |<para><bold>bold</bold> <bold>several words</bold></para>
        |<para><emphasis>emphasis</emphasis> <emphasis>several words</emphasis></para>
    |  </briefdescription>
    |</memberdef>`);

  const text = toMarkdown(memberDef.briefdescription);
  expect(text).toBe(
    t(`**bold** **several words**
      |
      |*emphasis* *several words*
      `),
  );
});

test('Line break', async () => {
  const memberDef = await parse(`
    |<memberdef>
    |  <briefdescription>
        |<para>line1<linebreak/>line2</para>
    |  </briefdescription>
    |</memberdef>`);

  const text = toMarkdown(memberDef.briefdescription);
  expect(text).toBe(
    t(`line1<br/>line2
      `),
  );
});

test('White spaces', async () => {
  const memberDef = await parse(`
    |<memberdef>
    |  <briefdescription>
        |<para>Spaces: text<sp/>text<nonbreakablespace/>text<ensp/>text<emsp/>text<thinsp/>text</para>
    |  </briefdescription>
    |</memberdef>`);

  const text = toMarkdown(memberDef.briefdescription);
  expect(text).toBe(
    t(`Spaces: text text&nbsp;text&ensp;text&emsp;text&thinsp;text`),
  );
});

test('Hyphen and dashes', async () => {
  const memberDef = await parse(`
    |<memberdef>
    |  <briefdescription>
        |<para>Dashes: hypen: - ndash: <ndash/> mdash: <mdash/></para>
    |  </briefdescription>
    |</memberdef>`);

  const text = toMarkdown(memberDef.briefdescription);
  expect(text).toBe(t(`Dashes: hypen: - ndash: &ndash; mdash: &mdash;`));
});

test('Horizontal ruler', async () => {
  const memberDef = await parse(`
    |<memberdef>
    |  <briefdescription>
        |<para><hruler/></para>
    |  </briefdescription>
    |</memberdef>`);

  const text = toMarkdown(memberDef.briefdescription);
  expect(text).toBe(t(`---`));
});

test('Headings', async () => {
  const memberDef = await parse(`
    |<memberdef>
    |  <briefdescription>
        |<sect1 id="type_autotoc_md2">
        |<title>heading 1</title>
        |<para>text 1</para>
        |<sect2 id="type_autotoc_md3">
        |<title>heading 2</title>
        |<para>text 2</para>
        |<sect3 id="type_autotoc_md4">
        |<title>heading 3</title>
        |<para>text 3</para>
        |<sect4 id="type_autotoc_md5">
        |<title>heading 4</title>
        |<para>text 4</para>
        |<para><anchor id="type_autotoc_md6"/> <heading level="5">heading 5</heading>
        |</para>
        |<para>text 5</para>
        |<para><heading level="6">heading 6</heading>
        |</para>
        |<para>text 6</para>
        |<para><heading level="6"># heading 7</heading>
        |</para>
        |<para>text 7</para>
        |</sect4>
        |</sect3>
        |</sect2>
        |</sect1>
    |  </briefdescription>
    |</memberdef>`);

  const text = toMarkdown(memberDef.briefdescription);
  expect(text).toBe(
    t(`# heading 1
      |
      |text 1
      |
      |## heading 2
      |
      |text 2
      |
      |### heading 3
      |
      |text 3
      |
      |#### heading 4
      |
      |text 4
      |
      |##### heading 5
      |
      |text 5
      |
      |###### heading 6
      |
      |text 6
      |
      |###### # heading 7
      |
      |text 7`),
  );
});

test('Parameters', async () => {
  const memberDef = await parse(`
    |<memberdef>
    |  <detaileddescription>
        |<para><parameterlist kind="param"><parameteritem>
        |<parameternamelist>
        |<parametername>param1</parametername>
        |</parameternamelist>
        |<parameterdescription>
        |<para>the first parameter</para>
        |</parameterdescription>
        |</parameteritem>
        |<parameteritem>
        |<parameternamelist>
        |<parametername>param2</parametername>
        |</parameternamelist>
        |<parameterdescription>
        |<para>the second parameter</para>
        |</parameterdescription>
        |</parameteritem>
        |</parameterlist>
        |<parameterlist kind="retval"><parameteritem>
        |<parameternamelist>
        |<parametername>result</parametername>
        |</parameternamelist>
        |<parameterdescription>
        |<para>the return parameter</para>
        |</parameterdescription>
        |</parameteritem>
        |</parameterlist>
        |<simplesect kind="return"><para>function return value</para>
        |</simplesect>
        |</para>
    |  </detaileddescription>
    |</memberdef>`);

  const text = toMarkdown(memberDef.detaileddescription);
  expect(text).toBe(
    t(`### Parameters
      |
      |* \`param1\` the first parameter
      |* \`param2\` the second parameter
      |
      |### Return values
      |
      |* \`result\` the return parameter
      |
      |### Returns
      |
      |function return value`),
  );
});

test('Compound ref found', async () => {
  const memberDef = await parse(`
    |<memberdef>
    |  <detaileddescription>
        |<para>Type: <ref refid="type1" kindref="compound">Type1</ref> found</para>
    |  </detaileddescription>
    |</memberdef>`);

  const linkResolver = getLinkResolver({
    resolveCompoundId: (doxCompoundId: string) => {
      if (doxCompoundId === 'type1') {
        return {docId: 'type1_ref'} as DocCompound;
      } else {
        return undefined;
      }
    },
  });

  const text = toMarkdown(memberDef.detaileddescription, linkResolver);
  expect(text).toBe(t(`Type: [\`Type1\`](type1_ref) found`));
});

test('Compound ref not found', async () => {
  const memberDef = await parse(`
    |<memberdef>
    |  <detaileddescription>
        |<para>Type: <ref refid="type1" kindref="compound">Type1</ref> not found</para>
    |  </detaileddescription>
    |</memberdef>`);

  const text = toMarkdown(memberDef.detaileddescription, getLinkResolver());
  expect(text).toBe(t(`Type: \`Type1\` not found`));
});

test('Member ref found', async () => {
  const memberDef = await parse(`
    |<memberdef>
    |  <detaileddescription>
        |<para>Member: <ref refid="member1" kindref="member">Member1</ref> found</para>
    |  </detaileddescription>
    |</memberdef>`);

  const linkResolver = getLinkResolver({
    resolveMemberId: (doxMemberId: string) =>
      doxMemberId === 'member1'
        ? [
            {docId: 'type1_ref'} as DocCompound,
            {anchor: '#member1'} as DocMemberOverload,
          ]
        : [undefined, undefined],
  });

  const text = toMarkdown(memberDef.detaileddescription, linkResolver);
  expect(text).toBe(t(`Member: [\`Member1\`](type1_ref#member1) found`));
});

test('Member ref not found', async () => {
  const memberDef = await parse(`
    |<memberdef>
    |  <detaileddescription>
        |<para>Member: <ref refid="member1" kindref="member">Member1</ref> not found</para>
    |  </detaileddescription>
    |</memberdef>`);

  const text = toMarkdown(memberDef.detaileddescription, getLinkResolver());
  expect(text).toBe(t(`Member: \`Member1\` not found`));
});

test('std::vector ref found', async () => {
  const memberDef = await parse(`
    |<memberdef>
    |  <detaileddescription>
        |<para>Vector: std::vector found</para>
    |  </detaileddescription>
    |</memberdef>`);

  const linkResolver = getLinkResolver({
    stdTypeLinks: {
      linkPrefix: 'ref_site/',
      linkMap: new Map<string, string>([['std::vector', 'vector']]),
    },
  });

  const text = toMarkdown(memberDef.detaileddescription, linkResolver);
  expect(text).toBe(t('Vector: [`std::vector`](ref_site/vector) found'));
});

test('std::vector ref not found', async () => {
  const memberDef = await parse(`
    |<memberdef>
    |  <detaileddescription>
        |<para>Vector: std::vector not found</para>
    |  </detaileddescription>
    |</memberdef>`);

  const linkResolver = getLinkResolver({
    stdTypeLinks: {
      linkPrefix: 'ref_site/',
      linkMap: new Map<string, string>(),
    },
  });

  const text = toMarkdown(memberDef.detaileddescription, linkResolver);
  expect(text).toBe(t('Vector: std::vector not found'));
});

test('std::vector ref found and std::string not found', async () => {
  const memberDef = await parse(`
    |<memberdef>
    |  <detaileddescription>
        |<para>std::vector found and std::string not found</para>
    |  </detaileddescription>
    |</memberdef>`);

  const linkResolver = getLinkResolver({
    stdTypeLinks: {
      linkPrefix: 'ref_site/',
      linkMap: new Map<string, string>([['std::vector', 'vector']]),
    },
  });

  const text = toMarkdown(memberDef.detaileddescription, linkResolver);
  expect(text).toBe(
    t('[`std::vector`](ref_site/vector) found and std::string not found'),
  );
});

test('std::vector ref not found and std::string found', async () => {
  const memberDef = await parse(`
    |<memberdef>
    |  <detaileddescription>
        |<para>std::vector found and std::string not found</para>
    |  </detaileddescription>
    |</memberdef>`);

  const linkResolver = getLinkResolver({
    stdTypeLinks: {
      linkPrefix: 'ref_site/',
      linkMap: new Map<string, string>([['std::string', 'basic_string']]),
    },
  });

  const text = toMarkdown(memberDef.detaileddescription, linkResolver);
  expect(text).toBe(
    t('std::vector found and [`std::string`](ref_site/basic_string) not found'),
  );
});

test('std::vector find end of string', async () => {
  const memberDef = await parse(`
    |<memberdef>
    |  <detaileddescription>
        |<para>Vector: std::vector</para>
    |  </detaileddescription>
    |</memberdef>`);

  const linkResolver = getLinkResolver({
    stdTypeLinks: {
      linkPrefix: 'ref_site/',
      linkMap: new Map<string, string>([['std::vector', 'vector']]),
    },
  });

  const text = toMarkdown(memberDef.detaileddescription, linkResolver);
  expect(text).toBe(t('Vector: [`std::vector`](ref_site/vector)'));
});

test('std::vector of int', async () => {
  const memberDef = await parse(`
    |<memberdef>
    |  <detaileddescription>
        |<para>Vector std::vector&lt;int&gt; template</para>
    |  </detaileddescription>
    |</memberdef>`);

  const linkResolver = getLinkResolver({
    stdTypeLinks: {
      linkPrefix: 'ref_site/',
      linkMap: new Map<string, string>([['std::vector', 'vector']]),
    },
  });

  const text = toMarkdown(memberDef.detaileddescription, linkResolver);
  expect(text).toBe(
    t('Vector [`std::vector`](ref_site/vector)`<int>` template'),
  );
});

test('Not matching std::vector of int', async () => {
  const memberDef = await parse(`
    |<memberdef>
    |  <detaileddescription>
        |<para>Vector std::vector&lt;int&gt; template</para>
    |  </detaileddescription>
    |</memberdef>`);

  const text = toMarkdown(memberDef.detaileddescription, getLinkResolver());
  expect(text).toBe('Vector std::vector<int> template');
});

test('Do not change stand alone angle brackets', async () => {
  const memberDef = await parse(`
    |<memberdef>
    |  <detaileddescription>
        |<para>Text &lt;int&gt; std::vector&lt;int&gt; std::basic_string&lt;char&gt; &lt;uint&gt; text</para>
    |  </detaileddescription>
    |</memberdef>`);

  const linkResolver = getLinkResolver({
    stdTypeLinks: {
      linkPrefix: 'ref_site/',
      linkMap: new Map<string, string>([['std::vector', 'vector']]),
    },
  });

  const text = toMarkdown(memberDef.detaileddescription, linkResolver);
  expect(text).toBe(
    'Text <int> [`std::vector`](ref_site/vector)`<int>` std::basic_string<char> <uint> text',
  );
});

test('Nested std::vector', async () => {
  const memberDef = await parse(`
    |<memberdef>
    |  <detaileddescription>
        |<para>Text std::vector&lt;std::vector&lt;int&gt;&gt; text</para>
    |  </detaileddescription>
    |</memberdef>`);

  const linkResolver = getLinkResolver({
    stdTypeLinks: {
      linkPrefix: 'ref_site/',
      linkMap: new Map<string, string>([['std::vector', 'vector']]),
    },
  });

  const text = toMarkdown(memberDef.detaileddescription, linkResolver);
  expect(text).toBe(
    'Text [`std::vector`](ref_site/vector)`<`[`std::vector`](ref_site/vector)`<int>>` text',
  );
});

test('std::vector method', async () => {
  const memberDef = await parse(`
    |<memberdef>
    |  <detaileddescription>
        |<para>Text std::vector&lt;int&gt;::emplace_back() text</para>
    |  </detaileddescription>
    |</memberdef>`);

  const linkResolver = getLinkResolver({
    stdTypeLinks: {
      linkPrefix: 'ref_site/',
      linkMap: new Map<string, string>([['std::vector', 'vector']]),
    },
  });

  const text = toMarkdown(memberDef.detaileddescription, linkResolver);
  expect(text).toBe(
    'Text [`std::vector`](ref_site/vector)`<int>::`[`emplace_back()`](ref_site/vector/emplace_back) text',
  );
});

test('std::vector method no template', async () => {
  const memberDef = await parse(`
    |<memberdef>
    |  <detaileddescription>
        |<para>Text std::vector::emplace_back() text</para>
    |  </detaileddescription>
    |</memberdef>`);

  const linkResolver = getLinkResolver({
    stdTypeLinks: {
      linkPrefix: 'ref_site/',
      linkMap: new Map<string, string>([['std::vector', 'vector']]),
    },
  });

  const text = toMarkdown(memberDef.detaileddescription, linkResolver);
  expect(text).toBe(
    'Text [`std::vector`](ref_site/vector)`::`[`emplace_back()`](ref_site/vector/emplace_back) text',
  );
});

test('std::vector operator[]', async () => {
  const memberDef = await parse(`
    |<memberdef>
    |  <detaileddescription>
        |<para>Text std::vector&lt;int&gt;::operator[] text</para>
    |  </detaileddescription>
    |</memberdef>`);

  const linkResolver = getLinkResolver({
    stdTypeLinks: {
      linkPrefix: 'ref_site/',
      linkMap: new Map<string, string>([['std::vector', 'vector']]),
      operatorMap: new Map<string, string>([['operator[]', 'operator_at']]),
    },
  });

  const text = toMarkdown(memberDef.detaileddescription, linkResolver);
  expect(text).toBe(
    'Text [`std::vector`](ref_site/vector)`<int>::`[`operator[]`](ref_site/vector/operator_at) text',
  );
});

test('std::vector operator[] no template', async () => {
  const memberDef = await parse(`
    |<memberdef>
    |  <detaileddescription>
        |<para>Text std::vector::operator[] text</para>
    |  </detaileddescription>
    |</memberdef>`);

  const linkResolver = getLinkResolver({
    stdTypeLinks: {
      linkPrefix: 'ref_site/',
      linkMap: new Map<string, string>([['std::vector', 'vector']]),
      operatorMap: new Map<string, string>([['operator[]', 'operator_at']]),
    },
  });

  const text = toMarkdown(memberDef.detaileddescription, linkResolver);
  expect(text).toBe(
    'Text [`std::vector`](ref_site/vector)`::`[`operator[]`](ref_site/vector/operator_at) text',
  );
});

test('std::vector unknown operator[]', async () => {
  const memberDef = await parse(`
    |<memberdef>
    |  <detaileddescription>
        |<para>Text std::vector&lt;int&gt;::operator[] text</para>
    |  </detaileddescription>
    |</memberdef>`);

  const linkResolver = getLinkResolver({
    stdTypeLinks: {
      linkPrefix: 'ref_site/',
      linkMap: new Map<string, string>([['std::vector', 'vector']]),
    },
  });

  const text = toMarkdown(memberDef.detaileddescription, linkResolver);
  expect(text).toBe(
    'Text [`std::vector`](ref_site/vector)`<int>::operator[]` text',
  );
});

test('std::vector unknown operator[] no template', async () => {
  const memberDef = await parse(`
    |<memberdef>
    |  <detaileddescription>
        |<para>Text std::vector::operator[] text</para>
    |  </detaileddescription>
    |</memberdef>`);

  const linkResolver = getLinkResolver({
    stdTypeLinks: {
      linkPrefix: 'ref_site/',
      linkMap: new Map<string, string>([['std::vector', 'vector']]),
    },
  });

  const text = toMarkdown(memberDef.detaileddescription, linkResolver);
  expect(text).toBe('Text [`std::vector`](ref_site/vector)`::operator[]` text');
});

test('std::vector operator()', async () => {
  const memberDef = await parse(`
    |<memberdef>
    |  <detaileddescription>
        |<para>Text std::vector&lt;int&gt;::operator() text</para>
    |  </detaileddescription>
    |</memberdef>`);

  const linkResolver = getLinkResolver({
    stdTypeLinks: {
      linkPrefix: 'ref_site/',
      linkMap: new Map<string, string>([['std::vector', 'vector']]),
      operatorMap: new Map<string, string>([['operator()', 'operator()']]),
    },
  });

  const text = toMarkdown(memberDef.detaileddescription, linkResolver);
  expect(text).toBe(
    'Text [`std::vector`](ref_site/vector)`<int>::`[`operator()`](ref_site/vector/operator()) text',
  );
});

test('std::vector operator() no template', async () => {
  const memberDef = await parse(`
    |<memberdef>
    |  <detaileddescription>
        |<para>Text std::vector::operator() text</para>
    |  </detaileddescription>
    |</memberdef>`);

  const linkResolver = getLinkResolver({
    stdTypeLinks: {
      linkPrefix: 'ref_site/',
      linkMap: new Map<string, string>([['std::vector', 'vector']]),
      operatorMap: new Map<string, string>([['operator()', 'operator()']]),
    },
  });

  const text = toMarkdown(memberDef.detaileddescription, linkResolver);
  expect(text).toBe(
    'Text [`std::vector`](ref_site/vector)`::`[`operator()`](ref_site/vector/operator()) text',
  );
});

test('std::vector of IJSValue', async () => {
  const memberDef = await parse(`
    |<memberdef>
    |  <detaileddescription>
        |<para>Text std::vector&lt;IJSValue&gt; text</para>
    |  </detaileddescription>
    |</memberdef>`);

  const linkResolver = getLinkResolver({
    stdTypeLinks: {
      linkPrefix: 'std/',
      linkMap: new Map<string, string>([['std::vector', 'vector']]),
      operatorMap: new Map<string, string>([['operator()', 'operator()']]),
    },
    idlTypeLinks: {
      linkMap: new Map<string, string>([['IJSValue', 'idl-IJSValue']]),
    },
  });

  const text = toMarkdown(memberDef.detaileddescription, linkResolver);
  expect(text).toBe(
    'Text [`std::vector`](std/vector)`<`[`IJSValue`](idl-IJSValue)`>` text',
  );
});

test('std::vector of JSValue', async () => {
  const memberDef = await parse(`
    |<memberdef>
    |  <detaileddescription>
        |<para>Text std::vector&lt;<ref refid="type1" kindref="compound">JSValue</ref>&gt; text</para>
    |  </detaileddescription>
    |</memberdef>`);

  const linkResolver = getLinkResolver({
    stdTypeLinks: {
      linkPrefix: 'std/',
      linkMap: new Map<string, string>([['std::vector', 'vector']]),
    },
    resolveCompoundId: (doxCompoundId: string) => {
      if (doxCompoundId === 'type1') {
        return {docId: 'type1_ref'} as DocCompound;
      } else {
        return undefined;
      }
    },
  });

  const text = toMarkdown(memberDef.detaileddescription, linkResolver);
  expect(text).toBe(
    'Text [`std::vector`](std/vector)`<`[`JSValue`](type1_ref)`>` text',
  );
});

async function parse(xmlText: string) {
  const xml = await xml2js.parseStringPromise(t(xmlText), {
    explicitChildren: true,
    preserveChildrenOrder: true,
    charsAsChildren: true,
    includeWhiteChars: true,
  });
  return xml.memberdef as DoxMember;
}

function t(text: string) {
  return applyTemplateRules(text, {indent: '  ', EOL: '\n'});
}

interface PartialLinkResolver {
  resolveCompoundId?: (doxCompoundId: string) => DocCompound | undefined;
  resolveMemberId?: (
    doxMemberId: string,
  ) => [DocCompound | undefined, DocMemberOverload | undefined];
  stdTypeLinks?: Partial<TypeLinks>;
  idlTypeLinks?: Partial<TypeLinks>;
}

function getLinkResolver(init: PartialLinkResolver = {}): LinkResolver {
  const emptyLinkResolver: LinkResolver = {
    resolveCompoundId: _ => undefined,
    resolveMemberId: _ => [undefined, undefined],
    stdTypeLinks: {
      linkPrefix: '',
      linkMap: new Map<string, string>(),
      operatorMap: new Map<string, string>(),
    },
    idlTypeLinks: {
      linkPrefix: '',
      linkMap: new Map<string, string>(),
      operatorMap: new Map<string, string>(),
    },
  };

  if (init.resolveCompoundId) {
    emptyLinkResolver.resolveCompoundId = init.resolveCompoundId;
  }
  if (init.resolveMemberId) {
    emptyLinkResolver.resolveMemberId = init.resolveMemberId;
  }
  if (init.stdTypeLinks) {
    emptyLinkResolver.stdTypeLinks = Object.assign(
      emptyLinkResolver.stdTypeLinks,
      init.stdTypeLinks,
    );
  }
  if (init.idlTypeLinks) {
    emptyLinkResolver.idlTypeLinks = Object.assign(
      emptyLinkResolver.idlTypeLinks,
      init.idlTypeLinks,
    );
  }
  return emptyLinkResolver;
}
