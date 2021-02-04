/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 **/

//
// Documentation model.
//

// Documentation model for a code project.
export class DocModel {
  // The code project for the model.
  codeProject = '';

  // List of all documented compounds indexed by their name.
  compounds: {[index: string]: DocCompound} = {};

  // List of all structs and classes sorted by name.
  classes: DocCompound[] = [];
}

// DocCompound represents a document for a type.
// We generate one file per compound.
export class DocCompound {
  // Documented type name.
  name: string = '';

  // Name of the code file where the type is defined.
  codeFileName = '';

  // Docusaurus Id for the generate Markdown documentation file.
  docId: string = '';

  // The type namespace.
  namespace: string = '';

  // All namespace aliases defined for the namespace in the project config.
  namespaceAliases: string[] = [];

  // Code type declaration.
  declaration: string = '';

  // Brief type description.
  brief: string = '';

  // Detailed type description.
  details: string = '';

  // Summary type description compiled from brief and details descriptions.
  summary: string = '';

  // Sections with type members and with related definitions.
  sections: DocSection[] = [];
}

// DocSection is a group of compound members such as 'Public members' or 'Related definitions'.
export class DocSection {
  // The section name.
  name = '';

  // List of member overloads in this section.
  memberOverloads: DocMemberOverload[] = [];

  // First member overload source code line number. It is used for sorting.
  line = 0;
}

// A group of members with the same name.
export class DocMemberOverload {
  // Name of the member group with the same name.
  // It can be different from the member names. E.g. '(constructor)' instead of class name.
  name = '';

  // List of members in the group.
  members: DocMember[] = [];

  // The anchor link to the member overload doc.
  anchor = '#';

  // The summary of the first member.
  summary = '';

  // First member source code line number. It is used for sorting.
  line = 0;
}

// Represents a compound member such as field or function.
export class DocMember {
  // Name of the member such as function or field name.
  name = '';

  // Member code declaration.
  declaration: string = '';

  // Brief member description.
  brief: string = '';

  // Detailed member description.
  details: string = '';

  // Summary member description compiled from brief and details descriptions.
  summary: string = '';

  // Source code line number. It is used for sorting.
  line = 0;
}
