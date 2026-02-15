# SPDX-FileCopyrightText: 2024-present American Society Of Cinematographers
# SPDX-License-Identifier: Apache-2.0
# AUTO-GENERATED from fdl_api.yaml — DO NOT EDIT
"""FDL Core ctypes function signatures."""

from __future__ import annotations

import ctypes

from ._structs import (
    fdl_abi_version_t,
    fdl_clip_id_t,
    fdl_dimensions_f64_t,
    fdl_dimensions_i64_t,
    fdl_from_intent_result_t,
    fdl_geometry_t,
    fdl_parse_result_t,
    fdl_point_f64_t,
    fdl_rect_t,
    fdl_resolve_canvas_result_t,
    fdl_round_strategy_t,
    fdl_template_result_t,
)


def bind_functions(lib: ctypes.CDLL) -> None:
    """Set argtypes and restype for all fdl_core functions."""

    # Return the ABI version of the loaded library.
    lib.fdl_abi_version.argtypes = []
    lib.fdl_abi_version.restype = fdl_abi_version_t

    # Round a single float value according to FDL rounding rules.
    lib.fdl_round.argtypes = [ctypes.c_double, ctypes.c_uint32, ctypes.c_uint32]
    lib.fdl_round.restype = ctypes.c_int64

    # Round both width and height of dimensions.
    lib.fdl_round_dimensions.argtypes = [fdl_dimensions_f64_t, ctypes.c_uint32, ctypes.c_uint32]
    lib.fdl_round_dimensions.restype = fdl_dimensions_f64_t

    # Round both x and y of a point.
    lib.fdl_round_point.argtypes = [fdl_point_f64_t, ctypes.c_uint32, ctypes.c_uint32]
    lib.fdl_round_point.restype = fdl_point_f64_t

    # Normalize dimensions by applying anamorphic squeeze to width.
    lib.fdl_dimensions_normalize.argtypes = [fdl_dimensions_f64_t, ctypes.c_double]
    lib.fdl_dimensions_normalize.restype = fdl_dimensions_f64_t

    # Scale normalized dimensions and apply target squeeze.
    lib.fdl_dimensions_scale.argtypes = [fdl_dimensions_f64_t, ctypes.c_double, ctypes.c_double]
    lib.fdl_dimensions_scale.restype = fdl_dimensions_f64_t

    # Normalize and scale dimensions in one step.
    lib.fdl_dimensions_normalize_and_scale.argtypes = [fdl_dimensions_f64_t, ctypes.c_double, ctypes.c_double, ctypes.c_double]
    lib.fdl_dimensions_normalize_and_scale.restype = fdl_dimensions_f64_t

    # Subtract two dimensions.
    lib.fdl_dimensions_sub.argtypes = [fdl_dimensions_f64_t, fdl_dimensions_f64_t]
    lib.fdl_dimensions_sub.restype = fdl_dimensions_f64_t

    # Check approximate equality with FDL tolerances.
    lib.fdl_dimensions_equal.argtypes = [fdl_dimensions_f64_t, fdl_dimensions_f64_t]
    lib.fdl_dimensions_equal.restype = ctypes.c_int

    # Check if both width and height are zero.
    lib.fdl_dimensions_is_zero.argtypes = [fdl_dimensions_f64_t]
    lib.fdl_dimensions_is_zero.restype = ctypes.c_int

    # Check if both width and height are zero (int64 variant).
    lib.fdl_dimensions_i64_is_zero.argtypes = [fdl_dimensions_i64_t]
    lib.fdl_dimensions_i64_is_zero.restype = ctypes.c_int

    # Normalize int64 dimensions by applying anamorphic squeeze to width. Returns float dimensions.
    lib.fdl_dimensions_i64_normalize.argtypes = [fdl_dimensions_i64_t, ctypes.c_double]
    lib.fdl_dimensions_i64_normalize.restype = fdl_dimensions_f64_t

    # Convert float dimensions to int64 by truncation.
    lib.fdl_dimensions_f64_to_i64.argtypes = [fdl_dimensions_f64_t]
    lib.fdl_dimensions_f64_to_i64.restype = fdl_dimensions_i64_t

    # Greater-than (OR logic) for int64 dimensions.
    lib.fdl_dimensions_i64_gt.argtypes = [fdl_dimensions_i64_t, fdl_dimensions_i64_t]
    lib.fdl_dimensions_i64_gt.restype = ctypes.c_int

    # Less-than (OR logic) for int64 dimensions.
    lib.fdl_dimensions_i64_lt.argtypes = [fdl_dimensions_i64_t, fdl_dimensions_i64_t]
    lib.fdl_dimensions_i64_lt.restype = ctypes.c_int

    # Greater-than (OR logic) for float64 dimensions.
    lib.fdl_dimensions_f64_gt.argtypes = [fdl_dimensions_f64_t, fdl_dimensions_f64_t]
    lib.fdl_dimensions_f64_gt.restype = ctypes.c_int

    # Less-than (OR logic) for float64 dimensions.
    lib.fdl_dimensions_f64_lt.argtypes = [fdl_dimensions_f64_t, fdl_dimensions_f64_t]
    lib.fdl_dimensions_f64_lt.restype = ctypes.c_int

    # Normalize point by applying anamorphic squeeze to x.
    lib.fdl_point_normalize.argtypes = [fdl_point_f64_t, ctypes.c_double]
    lib.fdl_point_normalize.restype = fdl_point_f64_t

    # Scale a normalized point and apply target squeeze.
    lib.fdl_point_scale.argtypes = [fdl_point_f64_t, ctypes.c_double, ctypes.c_double]
    lib.fdl_point_scale.restype = fdl_point_f64_t

    # Add two points.
    lib.fdl_point_add.argtypes = [fdl_point_f64_t, fdl_point_f64_t]
    lib.fdl_point_add.restype = fdl_point_f64_t

    # Subtract two points.
    lib.fdl_point_sub.argtypes = [fdl_point_f64_t, fdl_point_f64_t]
    lib.fdl_point_sub.restype = fdl_point_f64_t

    # Multiply point by scalar.
    lib.fdl_point_mul_scalar.argtypes = [fdl_point_f64_t, ctypes.c_double]
    lib.fdl_point_mul_scalar.restype = fdl_point_f64_t

    # Clamp point values to specified range.
    lib.fdl_point_clamp.argtypes = [fdl_point_f64_t, ctypes.c_double, ctypes.c_double, ctypes.c_int, ctypes.c_int]
    lib.fdl_point_clamp.restype = fdl_point_f64_t

    # Check if both x and y are zero.
    lib.fdl_point_is_zero.argtypes = [fdl_point_f64_t]
    lib.fdl_point_is_zero.restype = ctypes.c_int

    # Normalize and scale a point in one step.
    lib.fdl_point_normalize_and_scale.argtypes = [fdl_point_f64_t, ctypes.c_double, ctypes.c_double, ctypes.c_double]
    lib.fdl_point_normalize_and_scale.restype = fdl_point_f64_t

    # Check approximate equality with FDL tolerances.
    lib.fdl_point_equal.argtypes = [fdl_point_f64_t, fdl_point_f64_t]
    lib.fdl_point_equal.restype = ctypes.c_int

    # Less-than (OR logic) for float64 points.
    lib.fdl_point_f64_lt.argtypes = [fdl_point_f64_t, fdl_point_f64_t]
    lib.fdl_point_f64_lt.restype = ctypes.c_int

    # Relative tolerance for floating-point comparison (1e-9).
    lib.fdl_fp_rel_tol.argtypes = []
    lib.fdl_fp_rel_tol.restype = ctypes.c_double

    # Absolute tolerance for floating-point comparison (1e-6).
    lib.fdl_fp_abs_tol.argtypes = []
    lib.fdl_fp_abs_tol.restype = ctypes.c_double

    # Fill gaps in the geometry hierarchy by propagating populated dimensions upward.
    lib.fdl_geometry_fill_hierarchy_gaps.argtypes = [fdl_geometry_t, fdl_point_f64_t]
    lib.fdl_geometry_fill_hierarchy_gaps.restype = fdl_geometry_t

    # Normalize and scale all 7 fields of the geometry.
    lib.fdl_geometry_normalize_and_scale.argtypes = [fdl_geometry_t, ctypes.c_double, ctypes.c_double, ctypes.c_double]
    lib.fdl_geometry_normalize_and_scale.restype = fdl_geometry_t

    # Round all 7 fields of the geometry.
    lib.fdl_geometry_round.argtypes = [fdl_geometry_t, fdl_round_strategy_t]
    lib.fdl_geometry_round.restype = fdl_geometry_t

    # Apply offset to all anchors. Returns clamped geometry. Theoretical anchors written to output pointers.
    lib.fdl_geometry_apply_offset.argtypes = [
        fdl_geometry_t,
        fdl_point_f64_t,
        ctypes.POINTER(fdl_point_f64_t),
        ctypes.POINTER(fdl_point_f64_t),
        ctypes.POINTER(fdl_point_f64_t),
    ]
    lib.fdl_geometry_apply_offset.restype = fdl_geometry_t

    # Crop all dimensions to visible portion within canvas.
    lib.fdl_geometry_crop.argtypes = [fdl_geometry_t, fdl_point_f64_t, fdl_point_f64_t, fdl_point_f64_t]
    lib.fdl_geometry_crop.restype = fdl_geometry_t

    # Extract dimensions and anchor from geometry by path. Returns 0 on success, -1 on invalid path.
    lib.fdl_geometry_get_dims_anchor_from_path.argtypes = [
        ctypes.POINTER(fdl_geometry_t),
        ctypes.c_uint32,
        ctypes.POINTER(fdl_dimensions_f64_t),
        ctypes.POINTER(fdl_point_f64_t),
    ]
    lib.fdl_geometry_get_dims_anchor_from_path.restype = ctypes.c_int

    # Resolve dims/anchor from canvas/framing handles by path. Returns 0=success, 1=absent, -1=invalid.
    lib.fdl_resolve_geometry_layer.argtypes = [
        ctypes.c_void_p,
        ctypes.c_void_p,
        ctypes.c_uint32,
        ctypes.POINTER(fdl_dimensions_f64_t),
        ctypes.POINTER(fdl_point_f64_t),
    ]
    lib.fdl_resolve_geometry_layer.restype = ctypes.c_int

    # Create a rect from raw coordinates.
    lib.fdl_make_rect.argtypes = [ctypes.c_double, ctypes.c_double, ctypes.c_double, ctypes.c_double]
    lib.fdl_make_rect.restype = fdl_rect_t

    # Get canvas rect as (0, 0, width, height).
    lib.fdl_canvas_get_rect.argtypes = [ctypes.c_void_p]
    lib.fdl_canvas_get_rect.restype = fdl_rect_t

    # Get effective rect. Returns 0 if absent, 1 if written to out_rect.
    lib.fdl_canvas_get_effective_rect.argtypes = [ctypes.c_void_p, ctypes.POINTER(fdl_rect_t)]
    lib.fdl_canvas_get_effective_rect.restype = ctypes.c_int

    # Get framing decision rect as (anchor_x, anchor_y, width, height).
    lib.fdl_framing_decision_get_rect.argtypes = [ctypes.c_void_p]
    lib.fdl_framing_decision_get_rect.restype = fdl_rect_t

    # Get protection rect. Returns 0 if absent, 1 if written to out_rect.
    lib.fdl_framing_decision_get_protection_rect.argtypes = [ctypes.c_void_p, ctypes.POINTER(fdl_rect_t)]
    lib.fdl_framing_decision_get_protection_rect.restype = ctypes.c_int

    # Calculate scale factor based on fit method.
    lib.fdl_calculate_scale_factor.argtypes = [fdl_dimensions_f64_t, fdl_dimensions_f64_t, ctypes.c_uint32]
    lib.fdl_calculate_scale_factor.restype = ctypes.c_double

    # Determine output canvas size for a single axis.
    lib.fdl_output_size_for_axis.argtypes = [ctypes.c_double, ctypes.c_double, ctypes.c_int, ctypes.c_int]
    lib.fdl_output_size_for_axis.restype = ctypes.c_double

    # Calculate content translation shift for a single axis.
    lib.fdl_alignment_shift.argtypes = [
        ctypes.c_double,
        ctypes.c_double,
        ctypes.c_double,
        ctypes.c_double,
        ctypes.c_double,
        ctypes.c_int,
        ctypes.c_double,
        ctypes.c_int,
    ]
    lib.fdl_alignment_shift.restype = ctypes.c_double

    # Clamp dimensions to maximum bounds. Delta offset written to out_delta.
    lib.fdl_dimensions_clamp_to_dims.argtypes = [fdl_dimensions_f64_t, fdl_dimensions_f64_t, ctypes.POINTER(fdl_point_f64_t)]
    lib.fdl_dimensions_clamp_to_dims.restype = fdl_dimensions_f64_t

    # Compute a framing decision from a framing intent.
    lib.fdl_compute_framing_from_intent.argtypes = [
        fdl_dimensions_f64_t,
        fdl_dimensions_f64_t,
        ctypes.c_double,
        fdl_dimensions_i64_t,
        ctypes.c_double,
        fdl_round_strategy_t,
    ]
    lib.fdl_compute_framing_from_intent.restype = fdl_from_intent_result_t

    # Create an empty FDL document. Returns NULL on allocation failure.
    lib.fdl_doc_create.argtypes = []
    lib.fdl_doc_create.restype = ctypes.c_void_p

    # Free an FDL document. Safe to call with NULL.
    lib.fdl_doc_free.argtypes = [ctypes.c_void_p]
    lib.fdl_doc_free.restype = None

    # Parse a JSON string into an FDL document. On success result.doc is non-NULL. On failure result.error is non-NULL (free with fdl_free).
    lib.fdl_doc_parse_json.argtypes = [ctypes.c_char_p, ctypes.c_size_t]
    lib.fdl_doc_parse_json.restype = fdl_parse_result_t

    # Serialize document to canonical JSON string. Caller owns the returned string (free with fdl_free).
    lib.fdl_doc_to_json.argtypes = [ctypes.c_void_p, ctypes.c_int]
    lib.fdl_doc_to_json.restype = ctypes.c_void_p

    # Serialize context sub-object to canonical JSON string. Caller owns (free with fdl_free).
    lib.fdl_context_to_json.argtypes = [ctypes.c_void_p, ctypes.c_int]
    lib.fdl_context_to_json.restype = ctypes.c_void_p

    # Serialize canvas sub-object to canonical JSON string. Caller owns (free with fdl_free).
    lib.fdl_canvas_to_json.argtypes = [ctypes.c_void_p, ctypes.c_int]
    lib.fdl_canvas_to_json.restype = ctypes.c_void_p

    # Serialize framing decision to canonical JSON string. Caller owns (free with fdl_free).
    lib.fdl_framing_decision_to_json.argtypes = [ctypes.c_void_p, ctypes.c_int]
    lib.fdl_framing_decision_to_json.restype = ctypes.c_void_p

    # Serialize framing intent to canonical JSON string. Caller owns (free with fdl_free).
    lib.fdl_framing_intent_to_json.argtypes = [ctypes.c_void_p, ctypes.c_int]
    lib.fdl_framing_intent_to_json.restype = ctypes.c_void_p

    # Serialize canvas template to canonical JSON string. Caller owns (free with fdl_free).
    lib.fdl_canvas_template_to_json.argtypes = [ctypes.c_void_p, ctypes.c_int]
    lib.fdl_canvas_template_to_json.restype = ctypes.c_void_p

    # Get the UUID. Returns NULL if not present.
    lib.fdl_doc_get_uuid.argtypes = [ctypes.c_void_p]
    lib.fdl_doc_get_uuid.restype = ctypes.c_char_p

    # Get the fdl_creator. Returns NULL if not present.
    lib.fdl_doc_get_fdl_creator.argtypes = [ctypes.c_void_p]
    lib.fdl_doc_get_fdl_creator.restype = ctypes.c_char_p

    # Get the default_framing_intent. Returns NULL if not present.
    lib.fdl_doc_get_default_framing_intent.argtypes = [ctypes.c_void_p]
    lib.fdl_doc_get_default_framing_intent.restype = ctypes.c_char_p

    # Get the FDL version major number. Returns 0 if not present.
    lib.fdl_doc_get_version_major.argtypes = [ctypes.c_void_p]
    lib.fdl_doc_get_version_major.restype = ctypes.c_int

    # Get the FDL version minor number. Returns 0 if not present.
    lib.fdl_doc_get_version_minor.argtypes = [ctypes.c_void_p]
    lib.fdl_doc_get_version_minor.restype = ctypes.c_int

    # Set the document UUID.
    lib.fdl_doc_set_uuid.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
    lib.fdl_doc_set_uuid.restype = None

    # Set the fdl_creator.
    lib.fdl_doc_set_fdl_creator.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
    lib.fdl_doc_set_fdl_creator.restype = None

    # Set the default_framing_intent.
    lib.fdl_doc_set_default_framing_intent.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
    lib.fdl_doc_set_default_framing_intent.restype = None

    # Set the FDL version.
    lib.fdl_doc_set_version.argtypes = [ctypes.c_void_p, ctypes.c_int, ctypes.c_int]
    lib.fdl_doc_set_version.restype = None

    # Create a new FDL document with header fields and empty collections.
    lib.fdl_doc_create_with_header.argtypes = [ctypes.c_char_p, ctypes.c_int, ctypes.c_int, ctypes.c_char_p, ctypes.c_char_p]
    lib.fdl_doc_create_with_header.restype = ctypes.c_void_p

    # Get number of framing intents.
    lib.fdl_doc_framing_intents_count.argtypes = [ctypes.c_void_p]
    lib.fdl_doc_framing_intents_count.restype = ctypes.c_uint32

    # Get framing intent by index.
    lib.fdl_doc_framing_intent_at.argtypes = [ctypes.c_void_p, ctypes.c_uint32]
    lib.fdl_doc_framing_intent_at.restype = ctypes.c_void_p

    # Find framing intent by ID. Returns NULL if not found.
    lib.fdl_doc_framing_intent_find_by_id.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
    lib.fdl_doc_framing_intent_find_by_id.restype = ctypes.c_void_p

    # Get number of contexts.
    lib.fdl_doc_contexts_count.argtypes = [ctypes.c_void_p]
    lib.fdl_doc_contexts_count.restype = ctypes.c_uint32

    # Get context by index.
    lib.fdl_doc_context_at.argtypes = [ctypes.c_void_p, ctypes.c_uint32]
    lib.fdl_doc_context_at.restype = ctypes.c_void_p

    # Find context by label. Returns NULL if not found.
    lib.fdl_doc_context_find_by_label.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
    lib.fdl_doc_context_find_by_label.restype = ctypes.c_void_p

    # Get number of canvas templates.
    lib.fdl_doc_canvas_templates_count.argtypes = [ctypes.c_void_p]
    lib.fdl_doc_canvas_templates_count.restype = ctypes.c_uint32

    # Get canvas template by index.
    lib.fdl_doc_canvas_template_at.argtypes = [ctypes.c_void_p, ctypes.c_uint32]
    lib.fdl_doc_canvas_template_at.restype = ctypes.c_void_p

    # Find canvas template by ID. Returns NULL if not found.
    lib.fdl_doc_canvas_template_find_by_id.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
    lib.fdl_doc_canvas_template_find_by_id.restype = ctypes.c_void_p

    # Get number of canvases in a context.
    lib.fdl_context_canvases_count.argtypes = [ctypes.c_void_p]
    lib.fdl_context_canvases_count.restype = ctypes.c_uint32

    # Get canvas by index.
    lib.fdl_context_canvas_at.argtypes = [ctypes.c_void_p, ctypes.c_uint32]
    lib.fdl_context_canvas_at.restype = ctypes.c_void_p

    # Find canvas by ID. Returns NULL if not found.
    lib.fdl_context_find_canvas_by_id.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
    lib.fdl_context_find_canvas_by_id.restype = ctypes.c_void_p

    # Get number of framing decisions in a canvas.
    lib.fdl_canvas_framing_decisions_count.argtypes = [ctypes.c_void_p]
    lib.fdl_canvas_framing_decisions_count.restype = ctypes.c_uint32

    # Get framing decision by index.
    lib.fdl_canvas_framing_decision_at.argtypes = [ctypes.c_void_p, ctypes.c_uint32]
    lib.fdl_canvas_framing_decision_at.restype = ctypes.c_void_p

    # Find framing decision by ID. Returns NULL if not found.
    lib.fdl_canvas_find_framing_decision_by_id.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
    lib.fdl_canvas_find_framing_decision_by_id.restype = ctypes.c_void_p

    # Get context label.
    lib.fdl_context_get_label.argtypes = [ctypes.c_void_p]
    lib.fdl_context_get_label.restype = ctypes.c_char_p

    # Get context_creator. Returns NULL if not present.
    lib.fdl_context_get_context_creator.argtypes = [ctypes.c_void_p]
    lib.fdl_context_get_context_creator.restype = ctypes.c_char_p

    # Get canvas label.
    lib.fdl_canvas_get_label.argtypes = [ctypes.c_void_p]
    lib.fdl_canvas_get_label.restype = ctypes.c_char_p

    # Get canvas ID.
    lib.fdl_canvas_get_id.argtypes = [ctypes.c_void_p]
    lib.fdl_canvas_get_id.restype = ctypes.c_char_p

    # Get source_canvas_id.
    lib.fdl_canvas_get_source_canvas_id.argtypes = [ctypes.c_void_p]
    lib.fdl_canvas_get_source_canvas_id.restype = ctypes.c_char_p

    # Get canvas dimensions.
    lib.fdl_canvas_get_dimensions.argtypes = [ctypes.c_void_p]
    lib.fdl_canvas_get_dimensions.restype = fdl_dimensions_i64_t

    # Check if canvas has effective dimensions. Returns 1 if present, 0 if not.
    lib.fdl_canvas_has_effective_dimensions.argtypes = [ctypes.c_void_p]
    lib.fdl_canvas_has_effective_dimensions.restype = ctypes.c_int

    # Get effective dimensions. Undefined if has_effective_dimensions returns 0.
    lib.fdl_canvas_get_effective_dimensions.argtypes = [ctypes.c_void_p]
    lib.fdl_canvas_get_effective_dimensions.restype = fdl_dimensions_i64_t

    # Get effective anchor point. Undefined if has_effective_dimensions returns 0.
    lib.fdl_canvas_get_effective_anchor_point.argtypes = [ctypes.c_void_p]
    lib.fdl_canvas_get_effective_anchor_point.restype = fdl_point_f64_t

    # Get anamorphic_squeeze.
    lib.fdl_canvas_get_anamorphic_squeeze.argtypes = [ctypes.c_void_p]
    lib.fdl_canvas_get_anamorphic_squeeze.restype = ctypes.c_double

    # Check if canvas has photosite dimensions. Returns 1 if present, 0 if not.
    lib.fdl_canvas_has_photosite_dimensions.argtypes = [ctypes.c_void_p]
    lib.fdl_canvas_has_photosite_dimensions.restype = ctypes.c_int

    # Get photosite dimensions. Undefined if has_photosite_dimensions returns 0.
    lib.fdl_canvas_get_photosite_dimensions.argtypes = [ctypes.c_void_p]
    lib.fdl_canvas_get_photosite_dimensions.restype = fdl_dimensions_i64_t

    # Check if canvas has physical dimensions. Returns 1 if present, 0 if not.
    lib.fdl_canvas_has_physical_dimensions.argtypes = [ctypes.c_void_p]
    lib.fdl_canvas_has_physical_dimensions.restype = ctypes.c_int

    # Get physical dimensions. Undefined if has_physical_dimensions returns 0.
    lib.fdl_canvas_get_physical_dimensions.argtypes = [ctypes.c_void_p]
    lib.fdl_canvas_get_physical_dimensions.restype = fdl_dimensions_f64_t

    # Check if context has a clip_id. Returns 1 if present, 0 if not.
    lib.fdl_context_has_clip_id.argtypes = [ctypes.c_void_p]
    lib.fdl_context_has_clip_id.restype = ctypes.c_int

    # Get clip_id as JSON string. Returns NULL if not present. Caller owns (free with fdl_free).
    lib.fdl_context_get_clip_id.argtypes = [ctypes.c_void_p]
    lib.fdl_context_get_clip_id.restype = ctypes.c_void_p

    # Get clip_id as typed struct. Strings are strdup'd; free with fdl_clip_id_free.
    lib.fdl_context_get_clip_id_struct.argtypes = [ctypes.c_void_p]
    lib.fdl_context_get_clip_id_struct.restype = fdl_clip_id_t

    # Free string fields in a clip_id struct.
    lib.fdl_clip_id_free.argtypes = [ctypes.POINTER(fdl_clip_id_t)]
    lib.fdl_clip_id_free.restype = None

    # Get framing decision label.
    lib.fdl_framing_decision_get_label.argtypes = [ctypes.c_void_p]
    lib.fdl_framing_decision_get_label.restype = ctypes.c_char_p

    # Get framing decision ID.
    lib.fdl_framing_decision_get_id.argtypes = [ctypes.c_void_p]
    lib.fdl_framing_decision_get_id.restype = ctypes.c_char_p

    # Get framing_intent_id.
    lib.fdl_framing_decision_get_framing_intent_id.argtypes = [ctypes.c_void_p]
    lib.fdl_framing_decision_get_framing_intent_id.restype = ctypes.c_char_p

    # Get framing decision dimensions.
    lib.fdl_framing_decision_get_dimensions.argtypes = [ctypes.c_void_p]
    lib.fdl_framing_decision_get_dimensions.restype = fdl_dimensions_f64_t

    # Get framing decision anchor point.
    lib.fdl_framing_decision_get_anchor_point.argtypes = [ctypes.c_void_p]
    lib.fdl_framing_decision_get_anchor_point.restype = fdl_point_f64_t

    # Check if framing decision has protection. Returns 1 if present, 0 if not.
    lib.fdl_framing_decision_has_protection.argtypes = [ctypes.c_void_p]
    lib.fdl_framing_decision_has_protection.restype = ctypes.c_int

    # Get protection dimensions. Undefined if has_protection returns 0.
    lib.fdl_framing_decision_get_protection_dimensions.argtypes = [ctypes.c_void_p]
    lib.fdl_framing_decision_get_protection_dimensions.restype = fdl_dimensions_f64_t

    # Get protection anchor point. Undefined if has_protection returns 0.
    lib.fdl_framing_decision_get_protection_anchor_point.argtypes = [ctypes.c_void_p]
    lib.fdl_framing_decision_get_protection_anchor_point.restype = fdl_point_f64_t

    # Get framing intent label.
    lib.fdl_framing_intent_get_label.argtypes = [ctypes.c_void_p]
    lib.fdl_framing_intent_get_label.restype = ctypes.c_char_p

    # Get framing intent ID.
    lib.fdl_framing_intent_get_id.argtypes = [ctypes.c_void_p]
    lib.fdl_framing_intent_get_id.restype = ctypes.c_char_p

    # Get framing intent aspect ratio.
    lib.fdl_framing_intent_get_aspect_ratio.argtypes = [ctypes.c_void_p]
    lib.fdl_framing_intent_get_aspect_ratio.restype = fdl_dimensions_i64_t

    # Get framing intent protection percentage.
    lib.fdl_framing_intent_get_protection.argtypes = [ctypes.c_void_p]
    lib.fdl_framing_intent_get_protection.restype = ctypes.c_double

    # Get canvas template label.
    lib.fdl_canvas_template_get_label.argtypes = [ctypes.c_void_p]
    lib.fdl_canvas_template_get_label.restype = ctypes.c_char_p

    # Get canvas template ID.
    lib.fdl_canvas_template_get_id.argtypes = [ctypes.c_void_p]
    lib.fdl_canvas_template_get_id.restype = ctypes.c_char_p

    # Get target dimensions.
    lib.fdl_canvas_template_get_target_dimensions.argtypes = [ctypes.c_void_p]
    lib.fdl_canvas_template_get_target_dimensions.restype = fdl_dimensions_i64_t

    # Get target anamorphic squeeze.
    lib.fdl_canvas_template_get_target_anamorphic_squeeze.argtypes = [ctypes.c_void_p]
    lib.fdl_canvas_template_get_target_anamorphic_squeeze.restype = ctypes.c_double

    # Get fit_source geometry path.
    lib.fdl_canvas_template_get_fit_source.argtypes = [ctypes.c_void_p]
    lib.fdl_canvas_template_get_fit_source.restype = ctypes.c_uint32

    # Get fit_method.
    lib.fdl_canvas_template_get_fit_method.argtypes = [ctypes.c_void_p]
    lib.fdl_canvas_template_get_fit_method.restype = ctypes.c_uint32

    # Get horizontal alignment method.
    lib.fdl_canvas_template_get_alignment_method_horizontal.argtypes = [ctypes.c_void_p]
    lib.fdl_canvas_template_get_alignment_method_horizontal.restype = ctypes.c_uint32

    # Get vertical alignment method.
    lib.fdl_canvas_template_get_alignment_method_vertical.argtypes = [ctypes.c_void_p]
    lib.fdl_canvas_template_get_alignment_method_vertical.restype = ctypes.c_uint32

    # Check if preserve_from_source_canvas is set. Returns 1 if present, 0 if not.
    lib.fdl_canvas_template_has_preserve_from_source_canvas.argtypes = [ctypes.c_void_p]
    lib.fdl_canvas_template_has_preserve_from_source_canvas.restype = ctypes.c_int

    # Get preserve_from_source_canvas path. Undefined if has_ returns 0.
    lib.fdl_canvas_template_get_preserve_from_source_canvas.argtypes = [ctypes.c_void_p]
    lib.fdl_canvas_template_get_preserve_from_source_canvas.restype = ctypes.c_uint32

    # Check if maximum_dimensions is set. Returns 1 if present, 0 if not.
    lib.fdl_canvas_template_has_maximum_dimensions.argtypes = [ctypes.c_void_p]
    lib.fdl_canvas_template_has_maximum_dimensions.restype = ctypes.c_int

    # Get maximum dimensions. Undefined if has_ returns 0.
    lib.fdl_canvas_template_get_maximum_dimensions.argtypes = [ctypes.c_void_p]
    lib.fdl_canvas_template_get_maximum_dimensions.restype = fdl_dimensions_i64_t

    # Get pad_to_maximum flag. Returns 1 if true, 0 if false.
    lib.fdl_canvas_template_get_pad_to_maximum.argtypes = [ctypes.c_void_p]
    lib.fdl_canvas_template_get_pad_to_maximum.restype = ctypes.c_int

    # Get rounding strategy.
    lib.fdl_canvas_template_get_round.argtypes = [ctypes.c_void_p]
    lib.fdl_canvas_template_get_round.restype = fdl_round_strategy_t

    # Apply a canvas template to a source canvas/framing. Returns template_result_t. Caller must free with fdl_template_result_free.
    lib.fdl_apply_canvas_template.argtypes = [
        ctypes.c_void_p,
        ctypes.c_void_p,
        ctypes.c_void_p,
        ctypes.c_char_p,
        ctypes.c_char_p,
        ctypes.c_char_p,
        ctypes.c_char_p,
    ]
    lib.fdl_apply_canvas_template.restype = fdl_template_result_t

    # Free a template result (doc + error string). Safe to call with NULL.
    lib.fdl_template_result_free.argtypes = [ctypes.POINTER(fdl_template_result_t)]
    lib.fdl_template_result_free.restype = None

    # Resolve canvas for given input dimensions. Returns non-owning handles. Caller must free error with fdl_free.
    lib.fdl_context_resolve_canvas_for_dimensions.argtypes = [ctypes.c_void_p, fdl_dimensions_f64_t, ctypes.c_void_p, ctypes.c_void_p]
    lib.fdl_context_resolve_canvas_for_dimensions.restype = fdl_resolve_canvas_result_t

    # Add a framing intent to the document. Returns handle (owned by doc).
    lib.fdl_doc_add_framing_intent.argtypes = [
        ctypes.c_void_p,
        ctypes.c_char_p,
        ctypes.c_char_p,
        ctypes.c_int64,
        ctypes.c_int64,
        ctypes.c_double,
    ]
    lib.fdl_doc_add_framing_intent.restype = ctypes.c_void_p

    # Add a context to the document. Returns handle (owned by doc).
    lib.fdl_doc_add_context.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_char_p]
    lib.fdl_doc_add_context.restype = ctypes.c_void_p

    # Add a canvas template to the document. Returns handle (owned by doc).
    lib.fdl_doc_add_canvas_template.argtypes = [
        ctypes.c_void_p,
        ctypes.c_char_p,
        ctypes.c_char_p,
        ctypes.c_int64,
        ctypes.c_int64,
        ctypes.c_double,
        ctypes.c_uint32,
        ctypes.c_uint32,
        ctypes.c_uint32,
        ctypes.c_uint32,
        fdl_round_strategy_t,
    ]
    lib.fdl_doc_add_canvas_template.restype = ctypes.c_void_p

    # Set preserve_from_source_canvas on a canvas template.
    lib.fdl_canvas_template_set_preserve_from_source_canvas.argtypes = [ctypes.c_void_p, ctypes.c_uint32]
    lib.fdl_canvas_template_set_preserve_from_source_canvas.restype = None

    # Set maximum_dimensions on a canvas template.
    lib.fdl_canvas_template_set_maximum_dimensions.argtypes = [ctypes.c_void_p, fdl_dimensions_i64_t]
    lib.fdl_canvas_template_set_maximum_dimensions.restype = None

    # Set pad_to_maximum on a canvas template.
    lib.fdl_canvas_template_set_pad_to_maximum.argtypes = [ctypes.c_void_p, ctypes.c_int]
    lib.fdl_canvas_template_set_pad_to_maximum.restype = None

    # Add a canvas to a context. Returns handle (owned by doc).
    lib.fdl_context_add_canvas.argtypes = [
        ctypes.c_void_p,
        ctypes.c_char_p,
        ctypes.c_char_p,
        ctypes.c_char_p,
        ctypes.c_int64,
        ctypes.c_int64,
        ctypes.c_double,
    ]
    lib.fdl_context_add_canvas.restype = ctypes.c_void_p

    # Set effective dimensions and anchor on a canvas.
    lib.fdl_canvas_set_effective_dimensions.argtypes = [ctypes.c_void_p, fdl_dimensions_i64_t, fdl_point_f64_t]
    lib.fdl_canvas_set_effective_dimensions.restype = None

    # Set photosite dimensions on a canvas.
    lib.fdl_canvas_set_photosite_dimensions.argtypes = [ctypes.c_void_p, fdl_dimensions_i64_t]
    lib.fdl_canvas_set_photosite_dimensions.restype = None

    # Set physical dimensions on a canvas.
    lib.fdl_canvas_set_physical_dimensions.argtypes = [ctypes.c_void_p, fdl_dimensions_f64_t]
    lib.fdl_canvas_set_physical_dimensions.restype = None

    # Add a framing decision to a canvas. Returns handle (owned by doc).
    lib.fdl_canvas_add_framing_decision.argtypes = [
        ctypes.c_void_p,
        ctypes.c_char_p,
        ctypes.c_char_p,
        ctypes.c_char_p,
        ctypes.c_double,
        ctypes.c_double,
        ctypes.c_double,
        ctypes.c_double,
    ]
    lib.fdl_canvas_add_framing_decision.restype = ctypes.c_void_p

    # Set protection dimensions and anchor on a framing decision.
    lib.fdl_framing_decision_set_protection.argtypes = [ctypes.c_void_p, fdl_dimensions_f64_t, fdl_point_f64_t]
    lib.fdl_framing_decision_set_protection.restype = None

    # Set aspect ratio on a framing intent.
    lib.fdl_framing_intent_set_aspect_ratio.argtypes = [ctypes.c_void_p, fdl_dimensions_i64_t]
    lib.fdl_framing_intent_set_aspect_ratio.restype = None

    # Set protection factor on a framing intent.
    lib.fdl_framing_intent_set_protection.argtypes = [ctypes.c_void_p, ctypes.c_double]
    lib.fdl_framing_intent_set_protection.restype = None

    # Set dimensions on a canvas.
    lib.fdl_canvas_set_dimensions.argtypes = [ctypes.c_void_p, fdl_dimensions_i64_t]
    lib.fdl_canvas_set_dimensions.restype = None

    # Set anamorphic squeeze on a canvas.
    lib.fdl_canvas_set_anamorphic_squeeze.argtypes = [ctypes.c_void_p, ctypes.c_double]
    lib.fdl_canvas_set_anamorphic_squeeze.restype = None

    # Set effective dimensions on a canvas (creates anchor at {0,0} if missing).
    lib.fdl_canvas_set_effective_dims_only.argtypes = [ctypes.c_void_p, fdl_dimensions_i64_t]
    lib.fdl_canvas_set_effective_dims_only.restype = None

    # Remove effective dimensions and anchor from a canvas.
    lib.fdl_canvas_remove_effective.argtypes = [ctypes.c_void_p]
    lib.fdl_canvas_remove_effective.restype = None

    # Set dimensions on a framing decision.
    lib.fdl_framing_decision_set_dimensions.argtypes = [ctypes.c_void_p, fdl_dimensions_f64_t]
    lib.fdl_framing_decision_set_dimensions.restype = None

    # Set anchor point on a framing decision.
    lib.fdl_framing_decision_set_anchor_point.argtypes = [ctypes.c_void_p, fdl_point_f64_t]
    lib.fdl_framing_decision_set_anchor_point.restype = None

    # Set protection dimensions on a framing decision.
    lib.fdl_framing_decision_set_protection_dimensions.argtypes = [ctypes.c_void_p, fdl_dimensions_f64_t]
    lib.fdl_framing_decision_set_protection_dimensions.restype = None

    # Set protection anchor point on a framing decision.
    lib.fdl_framing_decision_set_protection_anchor_point.argtypes = [ctypes.c_void_p, fdl_point_f64_t]
    lib.fdl_framing_decision_set_protection_anchor_point.restype = None

    # Remove protection dimensions and anchor from a framing decision.
    lib.fdl_framing_decision_remove_protection.argtypes = [ctypes.c_void_p]
    lib.fdl_framing_decision_remove_protection.restype = None

    # Adjust anchor_point on a framing decision based on alignment within canvas.
    lib.fdl_framing_decision_adjust_anchor.argtypes = [ctypes.c_void_p, ctypes.c_void_p, ctypes.c_uint32, ctypes.c_uint32]
    lib.fdl_framing_decision_adjust_anchor.restype = None

    # Adjust protection_anchor_point on a framing decision based on alignment within canvas.
    lib.fdl_framing_decision_adjust_protection_anchor.argtypes = [ctypes.c_void_p, ctypes.c_void_p, ctypes.c_uint32, ctypes.c_uint32]
    lib.fdl_framing_decision_adjust_protection_anchor.restype = None

    # Populate a framing decision from a canvas and framing intent.
    lib.fdl_framing_decision_populate_from_intent.argtypes = [ctypes.c_void_p, ctypes.c_void_p, ctypes.c_void_p, fdl_round_strategy_t]
    lib.fdl_framing_decision_populate_from_intent.restype = None

    # Set clip_id on context from JSON. Returns NULL on success, error on failure.
    lib.fdl_context_set_clip_id_json.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_size_t]
    lib.fdl_context_set_clip_id_json.restype = ctypes.c_void_p

    # Remove clip_id from context.
    lib.fdl_context_remove_clip_id.argtypes = [ctypes.c_void_p]
    lib.fdl_context_remove_clip_id.restype = None

    # Validate clip_id JSON for mutual exclusion. Returns NULL if valid.
    lib.fdl_clip_id_validate_json.argtypes = [ctypes.c_char_p, ctypes.c_size_t]
    lib.fdl_clip_id_validate_json.restype = ctypes.c_void_p

    # Run schema (Draft 2020-12) and semantic validators. Returns a result handle.
    lib.fdl_doc_validate.argtypes = [ctypes.c_void_p]
    lib.fdl_doc_validate.restype = ctypes.c_void_p

    # Get the number of validation errors.
    lib.fdl_validation_result_error_count.argtypes = [ctypes.c_void_p]
    lib.fdl_validation_result_error_count.restype = ctypes.c_uint32

    # Get a specific error message by index. Returns NULL if index is out of range.
    lib.fdl_validation_result_error_at.argtypes = [ctypes.c_void_p, ctypes.c_uint32]
    lib.fdl_validation_result_error_at.restype = ctypes.c_char_p

    # Free a validation result. Safe to call with NULL.
    lib.fdl_validation_result_free.argtypes = [ctypes.c_void_p]
    lib.fdl_validation_result_free.restype = None

    # Free memory allocated by fdl_core functions.
    lib.fdl_free.argtypes = [ctypes.c_void_p]
    lib.fdl_free.restype = None
