/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

/**
 * Documentation model.
 */

/** Documentation model for a code project. */
export interface DocModel {
  /** List of all documentation compounds. */
  compounds: DocCompound[];

  /** List of all structs and classes sorted by name. */
  classes: DocCompound[];
}

/**
 * DocCompound represents a documentation for a type.
 */
export interface DocCompound {
  /** Documented type name. */
  name: string;

  /** Name of the code file where the type is defined. */
  codeFileName: string;

  /** Docusaurus Id for the generate Markdown documentation file. */
  docId: string;

  /** The type namespace. */
  namespace: string;

  /** All namespace aliases defined for the namespace in the project config. */
  namespaceAliases: string[];

  /** Code type declaration. */
  declaration: string;

  /** Brief type description. */
  brief: string;

  /** Detailed type description. */
  details: string;

  /** Summary type description compiled from brief and details descriptions. */
  summary: string;

  /** Sections with type members and with related definitions. */
  sections: DocSection[];
}

/** DocSection is a group of compound members such as 'Public members' or 'Related definitions'. */
export interface DocSection {
  /** The section name. */
  name: string;

  /** List of member overloads in this section. */
  memberOverloads: DocMemberOverload[];

  /** First member overload source code line number. It is used for sorting. */
  line: number;
}

/** A group of members with the same name. */
export interface DocMemberOverload {
  /**
   * Name of the member group with the same name.
   * It can be different from the member names.
   * E.g. '(constructor)' instead of class name.
   */
  name: string;

  /** List of members in the group. */
  members: DocMember[];

  /** The anchor link to the member overload doc. */
  anchor: string;

  /** The summary of the first member. */
  summary: string;

  /** First member source code line number. It is used for sorting. */
  line: number;
}

/** Represents a compound member such as field or function. */
export interface DocMember {
  /** Name of the member such as function or field name. */
  name: string;

  /** Member code declaration. */
  declaration: string;

  /** Brief member description. */
  brief: string;

  /** Detailed member description. */
  details: string;

  /** Summary member description compiled from brief and details descriptions. */
  summary: string;

  /** Source code line number. It is used for sorting. */
  line: number;
}
