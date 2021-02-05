/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

// Implementation of a simple template mechanism that is used for code generation.
// Each new line starts with a pipe '|' character.
// Any space before the pipe is ignored.
// The space after the pipe '|' and before text is interpreted as indentation
// where one indentation is two whitespace characters.
// Each indentation is replaced with provided indentation string.
// All end-of-lines (EOL) and surrounding space and tab characters are removed unless
// there is a pipe character.

export interface TemplateRules {
  indent: string;
  EOL: string;
}

export function applyTemplateRules(text: string, rules: TemplateRules): string {
  // Remove any leading or trailing spaces.
  const trimmedText = text.trim();

  // The very first pipe must not become EOL
  const startsWithPipe = trimmedText.startsWith('|');

  // Find string start or EOL characters optionally followed by a pipe.
  // We may have one or more such sequences.
  // Each pipe is going to be replaced by EOL.
  // After the last pipe we expect zero or more trailing spaces.
  // Each two trailing spaces are going to be replaced with one indentation.
  const transformed = trimmedText.replace(
    /((^|[ \t]*(\r\n|\n|\r)[ \t]*)\|?)+[ ]*/gm,
    (match: string) => {
      // count pipes and find the last pipe position
      let pipeCount = 0;
      let lastPipePos = match.length;
      let pos = 0;
      for (;;) {
        pos = match.indexOf('|', pos);
        if (pos >= 0) {
          lastPipePos = pos;
          ++pipeCount;
          ++pos;
        } else {
          break;
        }
      }

      // replace pipes with end of lines, and set the indentation
      const spaceCount = match.length - lastPipePos - 1;
      const indentCount = spaceCount / 2; // template uses two spaces for indent
      const extraSpaces = spaceCount > 0 ? spaceCount % 2 : 0;
      return (
        rules.EOL.repeat(pipeCount) +
        rules.indent.repeat(indentCount) +
        ' '.repeat(extraSpaces)
      );
    },
  );

  // return transformed text excpet for the EOL added by leading pipe
  return startsWithPipe ? transformed.substr(rules.EOL.length) : transformed;
}
