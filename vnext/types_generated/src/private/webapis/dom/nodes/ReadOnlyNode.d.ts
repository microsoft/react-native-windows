/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<e8f9cecd0e37e960294c34be53e2ec4c>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\src\private\webapis\dom\nodes\ReadOnlyNode.js
 */

import type NodeList from "../oldstylecollections/NodeList";
import type { InstanceHandle } from "./internals/NodeInternals";
import type ReactNativeDocument from "./ReactNativeDocument";
import type ReadOnlyElement from "./ReadOnlyElement";
import EventTarget from "../events/EventTarget";
declare const ReadOnlyNodeBase: typeof Object;
declare class ReadOnlyNode extends ReadOnlyNodeBase {
  protected constructor();
  get childNodes(): NodeList<ReadOnlyNode>;
  get firstChild(): ReadOnlyNode | null;
  get isConnected(): boolean;
  get lastChild(): ReadOnlyNode | null;
  get nextSibling(): ReadOnlyNode | null;
  /**
   * @abstract
   */
  get nodeName(): string;
  /**
   * @abstract
   */
  get nodeType(): number;
  /**
   * @abstract
   */
  get nodeValue(): string | null;
  get ownerDocument(): ReactNativeDocument | null;
  get parentElement(): ReadOnlyElement | null;
  get parentNode(): ReadOnlyNode | null;
  get previousSibling(): ReadOnlyNode | null;
  /**
   * @abstract
   */
  get textContent(): string;
  compareDocumentPosition(otherNode: ReadOnlyNode): number;
  contains(otherNode: ReadOnlyNode): boolean;
  getRootNode(): ReadOnlyNode;
  hasChildNodes(): boolean;
  /**
   * Type of Element, HTMLElement and ReactNativeElement instances.
   */
  static ELEMENT_NODE: number;
  /**
   * Currently Unused in React Native.
   */
  static ATTRIBUTE_NODE: number;
  /**
   * Text nodes.
   */
  static TEXT_NODE: number;
  /**
   * @deprecated Unused in React Native.
   */
  static CDATA_SECTION_NODE: number;
  /**
   * @deprecated
   */
  static ENTITY_REFERENCE_NODE: number;
  /**
   * @deprecated
   */
  static ENTITY_NODE: number;
  /**
   * @deprecated Unused in React Native.
   */
  static PROCESSING_INSTRUCTION_NODE: number;
  /**
   * @deprecated Unused in React Native.
   */
  static COMMENT_NODE: number;
  /**
   * Document nodes.
   */
  static DOCUMENT_NODE: number;
  /**
   * @deprecated Unused in React Native.
   */
  static DOCUMENT_TYPE_NODE: number;
  /**
   * @deprecated Unused in React Native.
   */
  static DOCUMENT_FRAGMENT_NODE: number;
  /**
   * @deprecated
   */
  static NOTATION_NODE: number;
  /**
   * Both nodes are in different documents.
   */
  static DOCUMENT_POSITION_DISCONNECTED: number;
  /**
   * `otherNode` precedes the node in either a pre-order depth-first traversal of a tree containing both
   * (e.g., as an ancestor or previous sibling or a descendant of a previous sibling or previous sibling of an ancestor)
   * or (if they are disconnected) in an arbitrary but consistent ordering.
   */
  static DOCUMENT_POSITION_PRECEDING: number;
  /**
   * `otherNode` follows the node in either a pre-order depth-first traversal of a tree containing both
   * (e.g., as a descendant or following sibling or a descendant of a following sibling or following sibling of an ancestor)
   * or (if they are disconnected) in an arbitrary but consistent ordering.
   */
  static DOCUMENT_POSITION_FOLLOWING: number;
  /**
   * `otherNode` is an ancestor of the node.
   */
  static DOCUMENT_POSITION_CONTAINS: number;
  /**
   * `otherNode` is a descendant of the node.
   */
  static DOCUMENT_POSITION_CONTAINED_BY: number;
  /**
   * @deprecated Unused in React Native.
   */
  static DOCUMENT_POSITION_IMPLEMENTATION_SPECIFIC: number;
}
export default ReadOnlyNode;
export declare const ReadOnlyNode_public: typeof ReadOnlyNode;
export declare type ReadOnlyNode_public = typeof ReadOnlyNode_public;
export type ReadOnlyNodeWithEventTarget = ReadOnlyNode & EventTarget;
export declare function getChildNodes(node: ReadOnlyNode, filter?: (node: ReadOnlyNode) => boolean): ReadonlyArray<ReadOnlyNode>;
