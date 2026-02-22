// SPDX-FileCopyrightText: 2024-present American Society Of Cinematographers
// SPDX-License-Identifier: Apache-2.0
// AUTO-GENERATED from ascfdl.schema.json — DO NOT EDIT

export type FdlId = string;
export type FdlIdFramingDecision = string;

export enum FitSource {
  framing_decision_dimensions = "framing_decision.dimensions",
  framing_decision_protection_dimensions = "framing_decision.protection_dimensions",
  canvas_dimensions = "canvas.dimensions",
  canvas_effective_dimensions = "canvas.effective_dimensions",
}

export enum FitMethod {
  width = "width",
  height = "height",
  fit_all = "fit_all",
  fill = "fill",
}

export enum AlignmentMethodVertical {
  top = "top",
  center = "center",
  bottom = "bottom",
}

export enum AlignmentMethodHorizontal {
  left = "left",
  center = "center",
  right = "right",
}

export enum PreserveFromSourceCanvas {
  framing_decision_dimensions = "framing_decision.dimensions",
  framing_decision_protection_dimensions = "framing_decision.protection_dimensions",
  canvas_dimensions = "canvas.dimensions",
  canvas_effective_dimensions = "canvas.effective_dimensions",
}

export enum Even {
  whole = "whole",
  even = "even",
}

export enum Mode {
  up = "up",
  down = "down",
  round = "round",
}

export interface DimensionsIntModel {
  width: number;
  height: number;
  [k: string]: unknown;
}

export interface DimensionsFloatModel {
  width: number;
  height: number;
  [k: string]: unknown;
}

export interface PointFloatModel {
  x: number;
  y: number;
  [k: string]: unknown;
}

export interface FramingDecisionList {
  uuid: string;
  version: VersionModel;
  fdl_creator?: string;
  default_framing_intent?: FdlId;
  framing_intents?: FramingIntentModel[];
  contexts?: ContextModel[];
  canvas_templates?: CanvasTemplateModel[];
  [k: string]: unknown;
}

export interface VersionModel {
  major: 2;
  minor: 0;
  [k: string]: unknown;
}

export interface FramingIntentModel {
  label?: string;
  id: FdlId;
  aspect_ratio: DimensionsIntModel;
  protection?: number;
  [k: string]: unknown;
}

export interface ContextModel {
  label?: string;
  context_creator?: string;
  clip_id?: ClipIDModel;
  canvases?: CanvasModel[];
  [k: string]: unknown;
}

export interface ClipIDModel {
  clip_name: string;
  file?: string;
  sequence?: FileSequenceModel;
  [k: string]: unknown;
}

export interface FileSequenceModel {
  value: string;
  idx: string;
  min: number;
  max: number;
  [k: string]: unknown;
}

export interface CanvasModel {
  label?: string;
  id: FdlId;
  source_canvas_id: FdlId;
  dimensions: DimensionsIntModel;
  effective_dimensions?: DimensionsIntModel;
  effective_anchor_point?: PointFloatModel;
  photosite_dimensions?: DimensionsIntModel;
  physical_dimensions?: DimensionsFloatModel;
  anamorphic_squeeze?: number;
  framing_decisions?: FramingDecisionModel[];
  [k: string]: unknown;
}

export interface FramingDecisionModel {
  label?: string;
  id: FdlIdFramingDecision;
  framing_intent_id: FdlId;
  dimensions: DimensionsFloatModel;
  anchor_point: PointFloatModel;
  protection_dimensions?: DimensionsFloatModel;
  protection_anchor_point?: PointFloatModel;
  [k: string]: unknown;
}

export interface CanvasTemplateModel {
  label?: string;
  id: FdlId;
  target_dimensions: DimensionsIntModel;
  target_anamorphic_squeeze: number;
  fit_source: FitSource;
  fit_method: FitMethod;
  alignment_method_vertical?: AlignmentMethodVertical;
  alignment_method_horizontal?: AlignmentMethodHorizontal;
  preserve_from_source_canvas?: PreserveFromSourceCanvas;
  maximum_dimensions?: DimensionsIntModel;
  pad_to_maximum?: boolean;
  round?: RoundModel;
  [k: string]: unknown;
}

export interface RoundModel {
  even: Even;
  mode: Mode;
  [k: string]: unknown;
}
