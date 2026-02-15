// SPDX-FileCopyrightText: 2024-present American Society Of Cinematographers
// SPDX-License-Identifier: Apache-2.0
#ifndef FDL_CORE_H
#define FDL_CORE_H

#include "fdl_export.h"
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* -----------------------------------------------------------------------
 * ABI version
 * ----------------------------------------------------------------------- */

typedef struct fdl_abi_version_t {
    uint32_t major;
    uint32_t minor;
    uint32_t patch;
} fdl_abi_version_t;

/** Return the ABI version of the loaded library. */
FDL_API fdl_abi_version_t fdl_abi_version(void);

/* -----------------------------------------------------------------------
 * Value types (pass by value, no heap allocation)
 * ----------------------------------------------------------------------- */

typedef struct fdl_dimensions_i64_t {
    int64_t width;
    int64_t height;
} fdl_dimensions_i64_t;

typedef struct fdl_dimensions_f64_t {
    double width;
    double height;
} fdl_dimensions_f64_t;

typedef struct fdl_point_f64_t {
    double x;
    double y;
} fdl_point_f64_t;

typedef struct fdl_rect_t {
    double x;
    double y;
    double width;
    double height;
} fdl_rect_t;

typedef struct fdl_file_sequence_t {
    const char* value;    /* strdup'd, caller-frees */
    const char* idx;      /* strdup'd, caller-frees (single char) */
    int64_t min;
    int64_t max;
} fdl_file_sequence_t;

typedef struct fdl_clip_id_t {
    const char* clip_name;             /* strdup'd, caller-frees */
    int has_file;
    const char* file;                  /* strdup'd, caller-frees, NULL if !has_file */
    int has_sequence;
    fdl_file_sequence_t sequence;      /* valid only if has_sequence */
} fdl_clip_id_t;

/* -----------------------------------------------------------------------
 * Enums (as uint32_t constants)
 * ----------------------------------------------------------------------- */

/* Rounding mode */
typedef uint32_t fdl_rounding_mode_t;
#define FDL_ROUNDING_MODE_UP    0
#define FDL_ROUNDING_MODE_DOWN  1
#define FDL_ROUNDING_MODE_ROUND 2

/* Rounding even */
typedef uint32_t fdl_rounding_even_t;
#define FDL_ROUNDING_EVEN_WHOLE 0
#define FDL_ROUNDING_EVEN_EVEN  1

/* Geometry path */
typedef uint32_t fdl_geometry_path_t;
#define FDL_GEOMETRY_PATH_CANVAS_DIMENSIONS            0
#define FDL_GEOMETRY_PATH_CANVAS_EFFECTIVE_DIMENSIONS   1
#define FDL_GEOMETRY_PATH_FRAMING_PROTECTION_DIMENSIONS 2
#define FDL_GEOMETRY_PATH_FRAMING_DIMENSIONS            3

/* Fit method */
typedef uint32_t fdl_fit_method_t;
#define FDL_FIT_METHOD_WIDTH   0
#define FDL_FIT_METHOD_HEIGHT  1
#define FDL_FIT_METHOD_FIT_ALL 2
#define FDL_FIT_METHOD_FILL    3

/* Horizontal alignment */
typedef uint32_t fdl_halign_t;
#define FDL_HALIGN_LEFT   0
#define FDL_HALIGN_CENTER 1
#define FDL_HALIGN_RIGHT  2

/* Vertical alignment */
typedef uint32_t fdl_valign_t;
#define FDL_VALIGN_TOP    0
#define FDL_VALIGN_CENTER 1
#define FDL_VALIGN_BOTTOM 2

/* -----------------------------------------------------------------------
 * Composite types
 * ----------------------------------------------------------------------- */

/** Geometry container for FDL template transformation processing.
 *  Holds 4 dimension layers + 3 anchor points through the pipeline. */
typedef struct fdl_geometry_t {
    fdl_dimensions_f64_t canvas_dims;
    fdl_dimensions_f64_t effective_dims;
    fdl_dimensions_f64_t protection_dims;
    fdl_dimensions_f64_t framing_dims;
    fdl_point_f64_t      effective_anchor;
    fdl_point_f64_t      protection_anchor;
    fdl_point_f64_t      framing_anchor;
} fdl_geometry_t;

/** Rounding strategy (even + mode). */
typedef struct fdl_round_strategy_t {
    fdl_rounding_even_t even;
    fdl_rounding_mode_t mode;
} fdl_round_strategy_t;

/** Result of computing a framing decision from a framing intent. */
typedef struct fdl_from_intent_result_t {
    fdl_dimensions_f64_t dimensions;
    fdl_point_f64_t      anchor_point;
    int                  has_protection;
    fdl_dimensions_f64_t protection_dimensions;
    fdl_point_f64_t      protection_anchor_point;
} fdl_from_intent_result_t;

/* -----------------------------------------------------------------------
 * Rounding functions
 * ----------------------------------------------------------------------- */

/** Round a single value according to FDL rounding rules.
 *  Returns the rounded integer value. */
FDL_API int64_t fdl_round(double value, fdl_rounding_even_t even, fdl_rounding_mode_t mode);

/** Round dimensions according to FDL rounding rules. */
FDL_API fdl_dimensions_f64_t fdl_round_dimensions(
    fdl_dimensions_f64_t dims,
    fdl_rounding_even_t even,
    fdl_rounding_mode_t mode
);

/** Round a point according to FDL rounding rules. */
FDL_API fdl_point_f64_t fdl_round_point(
    fdl_point_f64_t point,
    fdl_rounding_even_t even,
    fdl_rounding_mode_t mode
);

/* -----------------------------------------------------------------------
 * Dimensions operations
 * ----------------------------------------------------------------------- */

/** Normalize dimensions by applying anamorphic squeeze to width.
 *  width *= squeeze; height unchanged. */
FDL_API fdl_dimensions_f64_t fdl_dimensions_normalize(
    fdl_dimensions_f64_t dims,
    double squeeze
);

/** Scale normalized dimensions and apply target squeeze.
 *  width = (width * scale_factor) / target_squeeze;
 *  height = height * scale_factor. */
FDL_API fdl_dimensions_f64_t fdl_dimensions_scale(
    fdl_dimensions_f64_t dims,
    double scale_factor,
    double target_squeeze
);

/** Normalize and scale in one step. */
FDL_API fdl_dimensions_f64_t fdl_dimensions_normalize_and_scale(
    fdl_dimensions_f64_t dims,
    double input_squeeze,
    double scale_factor,
    double target_squeeze
);

/** Subtract two dimensions: result = a - b. */
FDL_API fdl_dimensions_f64_t fdl_dimensions_sub(
    fdl_dimensions_f64_t a,
    fdl_dimensions_f64_t b
);

/** Check if dimensions are approximately equal (FDL floating point tolerance).
 *  Returns 1 if equal, 0 otherwise. */
FDL_API int fdl_dimensions_equal(
    fdl_dimensions_f64_t a,
    fdl_dimensions_f64_t b
);

/** Check if a > b using OR logic (either width or height is greater).
 *  Returns 1 if true, 0 otherwise. */
FDL_API int fdl_dimensions_f64_gt(fdl_dimensions_f64_t a, fdl_dimensions_f64_t b);

/** Check if a < b using OR logic (either width or height is less).
 *  Returns 1 if true, 0 otherwise. */
FDL_API int fdl_dimensions_f64_lt(fdl_dimensions_f64_t a, fdl_dimensions_f64_t b);

/** Check if both width and height are zero.
 *  Returns 1 if zero, 0 otherwise. */
FDL_API int fdl_dimensions_is_zero(fdl_dimensions_f64_t dims);

/* -----------------------------------------------------------------------
 * Dimensions (integer) operations
 * ----------------------------------------------------------------------- */

/** Check if both width and height are zero (int64 variant).
 *  Returns 1 if zero, 0 otherwise. */
FDL_API int fdl_dimensions_i64_is_zero(fdl_dimensions_i64_t dims);

/** Normalize int64 dimensions by applying anamorphic squeeze to width.
 *  Returns float dimensions: width = width * squeeze, height unchanged. */
FDL_API fdl_dimensions_f64_t fdl_dimensions_i64_normalize(
    fdl_dimensions_i64_t dims,
    double squeeze
);

/** Convert float dimensions to int64 by truncation (int() cast). */
FDL_API fdl_dimensions_i64_t fdl_dimensions_f64_to_i64(fdl_dimensions_f64_t dims);

/** Check if a > b using OR logic (either width or height is greater).
 *  Returns 1 if true, 0 otherwise. */
FDL_API int fdl_dimensions_i64_gt(fdl_dimensions_i64_t a, fdl_dimensions_i64_t b);

/** Check if a < b using OR logic (either width or height is less).
 *  Returns 1 if true, 0 otherwise. */
FDL_API int fdl_dimensions_i64_lt(fdl_dimensions_i64_t a, fdl_dimensions_i64_t b);

/* -----------------------------------------------------------------------
 * Point operations
 * ----------------------------------------------------------------------- */

/** Normalize a point by applying anamorphic squeeze to x.
 *  x *= squeeze; y unchanged. */
FDL_API fdl_point_f64_t fdl_point_normalize(fdl_point_f64_t point, double squeeze);

/** Scale a normalized point and apply target squeeze.
 *  x = (x * scale_factor) / target_squeeze;
 *  y = y * scale_factor. */
FDL_API fdl_point_f64_t fdl_point_scale(
    fdl_point_f64_t point,
    double scale_factor,
    double target_squeeze
);

/** Add two points: result = a + b. */
FDL_API fdl_point_f64_t fdl_point_add(fdl_point_f64_t a, fdl_point_f64_t b);

/** Subtract two points: result = a - b. */
FDL_API fdl_point_f64_t fdl_point_sub(fdl_point_f64_t a, fdl_point_f64_t b);

/** Multiply point by scalar. */
FDL_API fdl_point_f64_t fdl_point_mul_scalar(fdl_point_f64_t a, double scalar);

/** Clamp point values to [min_val, max_val].
 *  has_min/has_max: set to 1 to apply the bound, 0 to skip. */
FDL_API fdl_point_f64_t fdl_point_clamp(
    fdl_point_f64_t point,
    double min_val,
    double max_val,
    int has_min,
    int has_max
);

/** Check if both x and y are zero.
 *  Returns 1 if zero, 0 otherwise. */
FDL_API int fdl_point_is_zero(fdl_point_f64_t point);

/** Normalize and scale a point in one step. */
FDL_API fdl_point_f64_t fdl_point_normalize_and_scale(
    fdl_point_f64_t point,
    double input_squeeze,
    double scale_factor,
    double target_squeeze
);

/** Check approximate equality with FDL tolerances.
 *  Returns 1 if equal, 0 otherwise. */
FDL_API int fdl_point_equal(fdl_point_f64_t a, fdl_point_f64_t b);

/** Check if a < b using OR logic (either x or y is less).
 *  Returns 1 if true, 0 otherwise. */
FDL_API int fdl_point_f64_lt(fdl_point_f64_t a, fdl_point_f64_t b);

/* -----------------------------------------------------------------------
 * Floating-point comparison constants
 * ----------------------------------------------------------------------- */

/** Relative tolerance for floating-point comparison (1e-9). */
FDL_API double fdl_fp_rel_tol(void);

/** Absolute tolerance for floating-point comparison (1e-6). */
FDL_API double fdl_fp_abs_tol(void);

/* -----------------------------------------------------------------------
 * Geometry operations
 * ----------------------------------------------------------------------- */

/** Fill gaps in the geometry hierarchy by propagating populated dimensions upward.
 *  Protection is NEVER filled from framing. Anchors offset by anchor_offset, clamped >= 0. */
FDL_API fdl_geometry_t fdl_geometry_fill_hierarchy_gaps(
    fdl_geometry_t geo,
    fdl_point_f64_t anchor_offset);

/** Normalize and scale all 7 fields of the geometry. */
FDL_API fdl_geometry_t fdl_geometry_normalize_and_scale(
    fdl_geometry_t geo,
    double source_squeeze,
    double scale_factor,
    double target_squeeze);

/** Round all 7 fields of the geometry. */
FDL_API fdl_geometry_t fdl_geometry_round(
    fdl_geometry_t geo,
    fdl_round_strategy_t strategy);

/** Apply offset to all anchors. Returns clamped geometry.
 *  Theoretical (unclamped) anchors written to theo_eff/prot/fram. */
FDL_API fdl_geometry_t fdl_geometry_apply_offset(
    fdl_geometry_t geo,
    fdl_point_f64_t offset,
    fdl_point_f64_t* theo_eff,
    fdl_point_f64_t* theo_prot,
    fdl_point_f64_t* theo_fram);

/** Crop all dimensions to visible portion within canvas.
 *  Enforces hierarchy: canvas >= effective >= protection >= framing. */
FDL_API fdl_geometry_t fdl_geometry_crop(
    fdl_geometry_t geo,
    fdl_point_f64_t theo_eff,
    fdl_point_f64_t theo_prot,
    fdl_point_f64_t theo_fram);

/** Extract dimensions and anchor from geometry by path.
 *  Returns 0 on success, -1 on invalid path. */
FDL_API int fdl_geometry_get_dims_anchor_from_path(
    const fdl_geometry_t* geo,
    fdl_geometry_path_t path,
    fdl_dimensions_f64_t* out_dims,
    fdl_point_f64_t* out_anchor);

/* -----------------------------------------------------------------------
 * Pipeline helper functions
 * ----------------------------------------------------------------------- */

/** Calculate scale factor based on fit method.
 *  fit_all=min ratios, fill=max, width/height=single axis. */
FDL_API double fdl_calculate_scale_factor(
    fdl_dimensions_f64_t fit_norm,
    fdl_dimensions_f64_t target_norm,
    fdl_fit_method_t fit_method);

/** Determine output canvas size for a single axis.
 *  PAD: pad_to_max -> max_size; CROP: canvas > max -> max_size; FIT: canvas_size. */
FDL_API double fdl_output_size_for_axis(
    double canvas_size,
    double max_size,
    int has_max,
    int pad_to_max);

/** Calculate content translation shift for a single axis.
 *  Three regimes: FIT (no shift), PAD (center/align), CROP (clip/align). */
FDL_API double fdl_alignment_shift(
    double fit_size,
    double fit_anchor,
    double output_size,
    double canvas_size,
    double target_size,
    int is_center,
    double align_factor,
    int pad_to_max);

/** Clamp dimensions to maximum bounds.
 *  Returns clamped dimensions; delta offset written to out_delta. */
FDL_API fdl_dimensions_f64_t fdl_dimensions_clamp_to_dims(
    fdl_dimensions_f64_t dims,
    fdl_dimensions_f64_t clamp_dims,
    fdl_point_f64_t* out_delta);

/* -----------------------------------------------------------------------
 * Framing from intent
 * ----------------------------------------------------------------------- */

/** Compute a framing decision from a framing intent.
 *  canvas_dims: full canvas dimensions (for anchor centering).
 *  working_dims: effective dimensions if available, else canvas dims (for aspect ratio fitting).
 *  Returns computed dimensions, anchors, and optional protection. */
FDL_API fdl_from_intent_result_t fdl_compute_framing_from_intent(
    fdl_dimensions_f64_t canvas_dims,
    fdl_dimensions_f64_t working_dims,
    double squeeze,
    fdl_dimensions_i64_t aspect_ratio,
    double protection,
    fdl_round_strategy_t rounding);

/* -----------------------------------------------------------------------
 * Document model (opaque handle)
 * ----------------------------------------------------------------------- */

/**
 * THREAD SAFETY
 *
 * libfdl_core provides per-document mutex locking:
 * - Different documents on different threads: SAFE
 * - Same document, concurrent reads: SAFE (serialized)
 * - Same document, concurrent read + write: SAFE (serialized)
 * - fdl_doc_free() during operations: NOT SAFE (caller must synchronize)
 *
 * String accessors return thread-local pointers keyed by field name.
 * Valid until the next call for the SAME field on the SAME thread.
 *
 * HANDLE VALIDITY
 *
 * Handles use index-based resolution and remain valid after collection
 * mutations (push_back). Repeated access for the same index returns the
 * same handle (deduplication). A handle is valid until:
 * - The owning document is freed (fdl_doc_free)
 * - The element at the handle's index is removed (not currently supported)
 */

/** Opaque handle to an FDL document. */
typedef struct fdl_doc fdl_doc_t;

/** Opaque handles to FDL sub-objects (index-based, stable across mutations). */
typedef struct fdl_context fdl_context_t;
typedef struct fdl_canvas fdl_canvas_t;
typedef struct fdl_framing_decision fdl_framing_decision_t;
typedef struct fdl_framing_intent fdl_framing_intent_t;
typedef struct fdl_canvas_template fdl_canvas_template_t;

/** Result of parsing JSON into an FDL document. */
typedef struct fdl_parse_result_t {
    fdl_doc_t* doc;      /**< non-NULL on success */
    const char* error;   /**< non-NULL on failure (free with fdl_free) */
} fdl_parse_result_t;

/** Create an empty FDL document. Returns NULL on allocation failure. */
FDL_API fdl_doc_t* fdl_doc_create(void);

/** Free an FDL document (and all handles). Safe to call with NULL. */
FDL_API void fdl_doc_free(fdl_doc_t* doc);

/** Parse a JSON string into an FDL document.
 *  On success, result.doc is non-NULL (caller owns, free with fdl_doc_free).
 *  On failure, result.error is non-NULL (caller owns, free with fdl_free). */
FDL_API fdl_parse_result_t fdl_doc_parse_json(const char* json_str, size_t json_len);

/** Get the UUID from a parsed FDL document.
 *  Returns NULL if not present. Pointer valid until doc is freed. */
FDL_API const char* fdl_doc_get_uuid(const fdl_doc_t* doc);

/** Get the fdl_creator from a parsed FDL document.
 *  Returns NULL if not present. Pointer valid until doc is freed. */
FDL_API const char* fdl_doc_get_fdl_creator(const fdl_doc_t* doc);

/** Get the default_framing_intent from a parsed FDL document.
 *  Returns NULL if not present. Pointer valid until doc is freed. */
FDL_API const char* fdl_doc_get_default_framing_intent(const fdl_doc_t* doc);

/** Get the FDL version major number. Returns 0 if not present. */
FDL_API int fdl_doc_get_version_major(const fdl_doc_t* doc);

/** Get the FDL version minor number. Returns 0 if not present. */
FDL_API int fdl_doc_get_version_minor(const fdl_doc_t* doc);

/** Serialize document to canonical JSON string.
 *  Keys ordered per FDL spec, null values excluded.
 *  Caller owns the returned string — free with fdl_free.
 *  Returns NULL if doc is NULL. */
FDL_API char* fdl_doc_to_json(const fdl_doc_t* doc, int indent);

/** Serialize a context sub-object to canonical JSON.
 *  Caller owns the returned string — free with fdl_free.
 *  Returns NULL if ctx is NULL. */
FDL_API char* fdl_context_to_json(const fdl_context_t* ctx, int indent);

/** Serialize a canvas sub-object to canonical JSON. Caller frees with fdl_free. */
FDL_API char* fdl_canvas_to_json(const fdl_canvas_t* canvas, int indent);

/** Serialize a framing decision to canonical JSON. Caller frees with fdl_free. */
FDL_API char* fdl_framing_decision_to_json(const fdl_framing_decision_t* fd, int indent);

/** Serialize a framing intent to canonical JSON. Caller frees with fdl_free. */
FDL_API char* fdl_framing_intent_to_json(const fdl_framing_intent_t* fi, int indent);

/** Serialize a canvas template to canonical JSON. Caller frees with fdl_free. */
FDL_API char* fdl_canvas_template_to_json(const fdl_canvas_template_t* ct, int indent);

/* -----------------------------------------------------------------------
 * Collection traversal
 * ----------------------------------------------------------------------- */

/** Framing intents (root-level collection). */
FDL_API uint32_t fdl_doc_framing_intents_count(fdl_doc_t* doc);
FDL_API fdl_framing_intent_t* fdl_doc_framing_intent_at(fdl_doc_t* doc, uint32_t index);
FDL_API fdl_framing_intent_t* fdl_doc_framing_intent_find_by_id(fdl_doc_t* doc, const char* id);

/** Contexts (root-level collection). */
FDL_API uint32_t fdl_doc_contexts_count(fdl_doc_t* doc);
FDL_API fdl_context_t* fdl_doc_context_at(fdl_doc_t* doc, uint32_t index);
FDL_API fdl_context_t* fdl_doc_context_find_by_label(fdl_doc_t* doc, const char* label);

/** Canvas templates (root-level collection). */
FDL_API uint32_t fdl_doc_canvas_templates_count(fdl_doc_t* doc);
FDL_API fdl_canvas_template_t* fdl_doc_canvas_template_at(fdl_doc_t* doc, uint32_t index);
FDL_API fdl_canvas_template_t* fdl_doc_canvas_template_find_by_id(fdl_doc_t* doc, const char* id);

/** Canvases (child of context). */
FDL_API uint32_t fdl_context_canvases_count(const fdl_context_t* ctx);
FDL_API fdl_canvas_t* fdl_context_canvas_at(fdl_context_t* ctx, uint32_t index);
FDL_API fdl_canvas_t* fdl_context_find_canvas_by_id(fdl_context_t* ctx, const char* id);

/** Framing decisions (child of canvas). */
FDL_API uint32_t fdl_canvas_framing_decisions_count(const fdl_canvas_t* canvas);
FDL_API fdl_framing_decision_t* fdl_canvas_framing_decision_at(fdl_canvas_t* canvas, uint32_t index);
FDL_API fdl_framing_decision_t* fdl_canvas_find_framing_decision_by_id(fdl_canvas_t* canvas, const char* id);

/* -----------------------------------------------------------------------
 * Field accessors — Context
 * ----------------------------------------------------------------------- */

FDL_API const char* fdl_context_get_label(const fdl_context_t* ctx);
FDL_API const char* fdl_context_get_context_creator(const fdl_context_t* ctx);
FDL_API int fdl_context_has_clip_id(const fdl_context_t* ctx);

/** Get clip_id as JSON string. Returns NULL if not present.
 *  Caller owns the returned string — free with fdl_free. */
FDL_API const char* fdl_context_get_clip_id(const fdl_context_t* ctx);

/** Get clip_id as typed struct. Only call after fdl_context_has_clip_id() returns 1.
 *  String fields are strdup'd copies — free with fdl_clip_id_free. */
FDL_API fdl_clip_id_t fdl_context_get_clip_id_struct(const fdl_context_t* ctx);

/** Free string fields in a clip_id struct populated by fdl_context_get_clip_id_struct. */
FDL_API void fdl_clip_id_free(fdl_clip_id_t* clip_id);

/* -----------------------------------------------------------------------
 * Field accessors — Canvas
 * ----------------------------------------------------------------------- */

FDL_API const char* fdl_canvas_get_label(const fdl_canvas_t* canvas);
FDL_API const char* fdl_canvas_get_id(const fdl_canvas_t* canvas);
FDL_API const char* fdl_canvas_get_source_canvas_id(const fdl_canvas_t* canvas);
FDL_API fdl_dimensions_i64_t fdl_canvas_get_dimensions(const fdl_canvas_t* canvas);
FDL_API int fdl_canvas_has_effective_dimensions(const fdl_canvas_t* canvas);
FDL_API fdl_dimensions_i64_t fdl_canvas_get_effective_dimensions(const fdl_canvas_t* canvas);
FDL_API fdl_point_f64_t fdl_canvas_get_effective_anchor_point(const fdl_canvas_t* canvas);
FDL_API double fdl_canvas_get_anamorphic_squeeze(const fdl_canvas_t* canvas);

FDL_API int fdl_canvas_has_photosite_dimensions(const fdl_canvas_t* canvas);
FDL_API fdl_dimensions_i64_t fdl_canvas_get_photosite_dimensions(const fdl_canvas_t* canvas);
FDL_API int fdl_canvas_has_physical_dimensions(const fdl_canvas_t* canvas);
FDL_API fdl_dimensions_f64_t fdl_canvas_get_physical_dimensions(const fdl_canvas_t* canvas);
/* -----------------------------------------------------------------------
 * Field accessors — Framing Decision
 * ----------------------------------------------------------------------- */

FDL_API const char* fdl_framing_decision_get_label(const fdl_framing_decision_t* fd);
FDL_API const char* fdl_framing_decision_get_id(const fdl_framing_decision_t* fd);
FDL_API const char* fdl_framing_decision_get_framing_intent_id(const fdl_framing_decision_t* fd);
FDL_API fdl_dimensions_f64_t fdl_framing_decision_get_dimensions(const fdl_framing_decision_t* fd);
FDL_API fdl_point_f64_t fdl_framing_decision_get_anchor_point(const fdl_framing_decision_t* fd);
FDL_API int fdl_framing_decision_has_protection(const fdl_framing_decision_t* fd);
FDL_API fdl_dimensions_f64_t fdl_framing_decision_get_protection_dimensions(const fdl_framing_decision_t* fd);
FDL_API fdl_point_f64_t fdl_framing_decision_get_protection_anchor_point(const fdl_framing_decision_t* fd);

/* -----------------------------------------------------------------------
 * Field accessors — Framing Intent
 * ----------------------------------------------------------------------- */

FDL_API const char* fdl_framing_intent_get_label(const fdl_framing_intent_t* fi);
FDL_API const char* fdl_framing_intent_get_id(const fdl_framing_intent_t* fi);
FDL_API fdl_dimensions_i64_t fdl_framing_intent_get_aspect_ratio(const fdl_framing_intent_t* fi);
FDL_API double fdl_framing_intent_get_protection(const fdl_framing_intent_t* fi);

/* -----------------------------------------------------------------------
 * Field accessors — Canvas Template
 * ----------------------------------------------------------------------- */

FDL_API const char* fdl_canvas_template_get_label(const fdl_canvas_template_t* ct);
FDL_API const char* fdl_canvas_template_get_id(const fdl_canvas_template_t* ct);
FDL_API fdl_dimensions_i64_t fdl_canvas_template_get_target_dimensions(const fdl_canvas_template_t* ct);
FDL_API double fdl_canvas_template_get_target_anamorphic_squeeze(const fdl_canvas_template_t* ct);
FDL_API fdl_geometry_path_t fdl_canvas_template_get_fit_source(const fdl_canvas_template_t* ct);
FDL_API fdl_fit_method_t fdl_canvas_template_get_fit_method(const fdl_canvas_template_t* ct);
FDL_API fdl_halign_t fdl_canvas_template_get_alignment_method_horizontal(const fdl_canvas_template_t* ct);
FDL_API fdl_valign_t fdl_canvas_template_get_alignment_method_vertical(const fdl_canvas_template_t* ct);
FDL_API int fdl_canvas_template_has_preserve_from_source_canvas(const fdl_canvas_template_t* ct);
FDL_API fdl_geometry_path_t fdl_canvas_template_get_preserve_from_source_canvas(const fdl_canvas_template_t* ct);
FDL_API int fdl_canvas_template_has_maximum_dimensions(const fdl_canvas_template_t* ct);
FDL_API fdl_dimensions_i64_t fdl_canvas_template_get_maximum_dimensions(const fdl_canvas_template_t* ct);
FDL_API int fdl_canvas_template_get_pad_to_maximum(const fdl_canvas_template_t* ct);
FDL_API fdl_round_strategy_t fdl_canvas_template_get_round(const fdl_canvas_template_t* ct);

/* -----------------------------------------------------------------------
 * Geometry layer resolution (from handle types)
 * ----------------------------------------------------------------------- */

/** Resolve dimensions and anchor directly from canvas/framing handles for a path.
 *  Returns: 0=success, 1=path valid but data absent (optional field not set),
 *  -1=invalid path. On absent (rc=1), out_dims/out_anchor are zero-initialized. */
FDL_API int fdl_resolve_geometry_layer(
    const fdl_canvas_t* canvas,
    const fdl_framing_decision_t* framing,
    fdl_geometry_path_t path,
    fdl_dimensions_f64_t* out_dims,
    fdl_point_f64_t* out_anchor);

/* -----------------------------------------------------------------------
 * Rect convenience — combine dims + anchor into (x, y, w, h)
 * ----------------------------------------------------------------------- */

/** Common rect constructor from raw coordinates. */
FDL_API fdl_rect_t fdl_make_rect(double x, double y, double width, double height);

/** Canvas rect: (0, 0, dims.width, dims.height). */
FDL_API fdl_rect_t fdl_canvas_get_rect(const fdl_canvas_t* canvas);

/** Canvas effective rect. Returns 0 if no effective dims, 1 if written to out_rect. */
FDL_API int fdl_canvas_get_effective_rect(const fdl_canvas_t* canvas, fdl_rect_t* out_rect);

/** Framing decision rect: (anchor.x, anchor.y, dims.width, dims.height). */
FDL_API fdl_rect_t fdl_framing_decision_get_rect(const fdl_framing_decision_t* fd);

/** Framing decision protection rect. Returns 0 if no protection, 1 if written to out_rect. */
FDL_API int fdl_framing_decision_get_protection_rect(const fdl_framing_decision_t* fd, fdl_rect_t* out_rect);

/* -----------------------------------------------------------------------
 * Template pipeline — full CanvasTemplate.apply()
 * ----------------------------------------------------------------------- */

/** Result of applying a canvas template. */
typedef struct fdl_template_result_t {
    fdl_doc_t*           output_fdl;          /**< Output FDL (caller owns, free with fdl_doc_free or fdl_template_result_free) */
    double               scale_factor;        /**< Computed scale factor */
    fdl_dimensions_f64_t scaled_bounding_box;  /**< Scaled bounding box before crop */
    fdl_point_f64_t      content_translation; /**< Content translation (shift) */
    const char*          context_label;       /**< Label of the new context (caller frees with fdl_free) */
    const char*          canvas_id;           /**< ID of the new canvas (caller frees with fdl_free) */
    const char*          framing_decision_id; /**< ID of the new framing decision (caller frees with fdl_free) */
    const char*          error;               /**< Error message on failure (free with fdl_free) */
} fdl_template_result_t;

/** Apply a canvas template to a source canvas/framing.
 *  new_canvas_id: ID for the output canvas (caller provides).
 *  source_context_label: label from source context (for label generation, may be NULL).
 *  Returns template_result_t with output_fdl on success, error on failure.
 *  Caller must free with fdl_template_result_free. */
FDL_API fdl_template_result_t fdl_apply_canvas_template(
    const fdl_canvas_template_t* tmpl,
    const fdl_canvas_t* source_canvas,
    const fdl_framing_decision_t* source_framing,
    const char* new_canvas_id,
    const char* new_fd_name,
    const char* source_context_label,
    const char* context_creator);

/** Free a template result (doc + error string). Safe to call with NULL. */
FDL_API void fdl_template_result_free(fdl_template_result_t* result);

/* -----------------------------------------------------------------------
 * Canvas resolution — find matching canvas for input dimensions
 * ----------------------------------------------------------------------- */

typedef struct fdl_resolve_canvas_result_t {
    fdl_canvas_t*            canvas;            /**< Resolved canvas (non-owning, do NOT free) */
    fdl_framing_decision_t*  framing_decision;  /**< Resolved framing decision (non-owning, do NOT free) */
    int                      was_resolved;      /**< 1 if a different canvas was found, 0 if original matched */
    const char*              error;             /**< Error message on failure (caller frees with fdl_free) */
} fdl_resolve_canvas_result_t;

/** Resolve canvas for given input dimensions.
 *  If input_dims match the canvas dimensions, returns the original canvas/framing with was_resolved=0.
 *  If they don't match and the canvas is derived (id != source_canvas_id), searches sibling
 *  canvases for one with matching dimensions and a framing decision with the same label.
 *  Returns error if no match is found. */
FDL_API fdl_resolve_canvas_result_t fdl_context_resolve_canvas_for_dimensions(
    fdl_context_t* ctx,
    fdl_dimensions_f64_t input_dims,
    fdl_canvas_t* canvas,
    fdl_framing_decision_t* framing);

/* -----------------------------------------------------------------------
 * Document builder — create and mutate FDL documents
 * ----------------------------------------------------------------------- */

/** Create a new FDL document with header fields and empty collections.
 *  default_framing_intent may be NULL. Caller owns, free with fdl_doc_free. */
FDL_API fdl_doc_t* fdl_doc_create_with_header(
    const char* uuid, int version_major, int version_minor,
    const char* fdl_creator, const char* default_framing_intent);

/** Set document-level fields. */
FDL_API void fdl_doc_set_uuid(fdl_doc_t* doc, const char* uuid);
FDL_API void fdl_doc_set_fdl_creator(fdl_doc_t* doc, const char* creator);
FDL_API void fdl_doc_set_default_framing_intent(fdl_doc_t* doc, const char* fi_id);
FDL_API void fdl_doc_set_version(fdl_doc_t* doc, int major, int minor);

/** Add a framing intent to the document. Returns handle (owned by doc). */
FDL_API fdl_framing_intent_t* fdl_doc_add_framing_intent(
    fdl_doc_t* doc, const char* id, const char* label,
    int64_t aspect_w, int64_t aspect_h, double protection);

/** Add a context to the document. Returns handle (owned by doc). */
FDL_API fdl_context_t* fdl_doc_add_context(
    fdl_doc_t* doc, const char* label, const char* context_creator);

/** Add a canvas to a context. Returns handle (owned by doc). */
FDL_API fdl_canvas_t* fdl_context_add_canvas(
    fdl_context_t* ctx, const char* id, const char* label,
    const char* source_canvas_id,
    int64_t dim_w, int64_t dim_h, double squeeze);

/** Set effective dimensions and anchor on a canvas. */
FDL_API void fdl_canvas_set_effective_dimensions(
    fdl_canvas_t* canvas, fdl_dimensions_i64_t dims, fdl_point_f64_t anchor);

/** Set photosite dimensions on a canvas. */
FDL_API void fdl_canvas_set_photosite_dimensions(
    fdl_canvas_t* canvas, fdl_dimensions_i64_t dims);

/** Set physical dimensions on a canvas. */
FDL_API void fdl_canvas_set_physical_dimensions(
    fdl_canvas_t* canvas, fdl_dimensions_f64_t dims);

/** Add a framing decision to a canvas. Returns handle (owned by doc). */
FDL_API fdl_framing_decision_t* fdl_canvas_add_framing_decision(
    fdl_canvas_t* canvas, const char* id, const char* label,
    const char* framing_intent_id,
    double dim_w, double dim_h,
    double anchor_x, double anchor_y);

/** Add a canvas template to the document. Returns handle (owned by doc). */
FDL_API fdl_canvas_template_t* fdl_doc_add_canvas_template(
    fdl_doc_t* doc, const char* id, const char* label,
    int64_t target_w, int64_t target_h, double target_squeeze,
    fdl_geometry_path_t fit_source, fdl_fit_method_t fit_method,
    fdl_halign_t halign, fdl_valign_t valign,
    fdl_round_strategy_t rounding);

/** Set preserve_from_source_canvas on a canvas template. */
FDL_API void fdl_canvas_template_set_preserve_from_source_canvas(
    fdl_canvas_template_t* ct, fdl_geometry_path_t path);

/** Set maximum_dimensions on a canvas template. */
FDL_API void fdl_canvas_template_set_maximum_dimensions(
    fdl_canvas_template_t* ct, fdl_dimensions_i64_t dims);

/** Set pad_to_maximum on a canvas template. */
FDL_API void fdl_canvas_template_set_pad_to_maximum(
    fdl_canvas_template_t* ct, int pad);

/** Set protection dimensions and anchor on a framing decision. */
FDL_API void fdl_framing_decision_set_protection(
    fdl_framing_decision_t* fd, fdl_dimensions_f64_t dims, fdl_point_f64_t anchor);

/** Set aspect ratio on a framing intent. */
FDL_API void fdl_framing_intent_set_aspect_ratio(
    fdl_framing_intent_t* fi, fdl_dimensions_i64_t dims);

/** Set protection factor on a framing intent. */
FDL_API void fdl_framing_intent_set_protection(
    fdl_framing_intent_t* fi, double protection);

/** Set dimensions on a canvas. */
FDL_API void fdl_canvas_set_dimensions(
    fdl_canvas_t* canvas, fdl_dimensions_i64_t dims);

/** Set anamorphic squeeze on a canvas. */
FDL_API void fdl_canvas_set_anamorphic_squeeze(
    fdl_canvas_t* canvas, double squeeze);

/** Set effective dimensions on a canvas (creates anchor at {0,0} if missing). */
FDL_API void fdl_canvas_set_effective_dims_only(
    fdl_canvas_t* canvas, fdl_dimensions_i64_t dims);

/** Remove effective dimensions and anchor from a canvas. */
FDL_API void fdl_canvas_remove_effective(fdl_canvas_t* canvas);

/** Set dimensions on a framing decision. */
FDL_API void fdl_framing_decision_set_dimensions(
    fdl_framing_decision_t* fd, fdl_dimensions_f64_t dims);

/** Set anchor point on a framing decision. */
FDL_API void fdl_framing_decision_set_anchor_point(
    fdl_framing_decision_t* fd, fdl_point_f64_t point);

/** Set protection dimensions on a framing decision (individual). */
FDL_API void fdl_framing_decision_set_protection_dimensions(
    fdl_framing_decision_t* fd, fdl_dimensions_f64_t dims);

/** Set protection anchor point on a framing decision (individual). */
FDL_API void fdl_framing_decision_set_protection_anchor_point(
    fdl_framing_decision_t* fd, fdl_point_f64_t point);

/** Remove protection dimensions and anchor from a framing decision. */
FDL_API void fdl_framing_decision_remove_protection(fdl_framing_decision_t* fd);

/* -----------------------------------------------------------------------
 * Framing decision business logic (handle-based)
 * ----------------------------------------------------------------------- */

/** Adjust anchor_point on a framing decision based on alignment within canvas.
 *  Reads canvas dimensions and FD dimensions, computes aligned anchor. */
FDL_API void fdl_framing_decision_adjust_anchor(
    fdl_framing_decision_t* fd,
    const fdl_canvas_t* canvas,
    fdl_halign_t h_align,
    fdl_valign_t v_align);

/** Adjust protection_anchor_point on a framing decision based on alignment within canvas.
 *  Reads canvas dimensions and FD protection_dimensions, computes aligned anchor. */
FDL_API void fdl_framing_decision_adjust_protection_anchor(
    fdl_framing_decision_t* fd,
    const fdl_canvas_t* canvas,
    fdl_halign_t h_align,
    fdl_valign_t v_align);

/** Populate a framing decision from a canvas and framing intent.
 *  Extracts values from canvas/intent handles, calls compute_framing_from_intent,
 *  writes dimensions, anchor_point, and optionally protection to the FD. */
FDL_API void fdl_framing_decision_populate_from_intent(
    fdl_framing_decision_t* fd,
    const fdl_canvas_t* canvas,
    const fdl_framing_intent_t* intent,
    fdl_round_strategy_t rounding);

/** Set clip_id on a context from a JSON string.
 *  Validates mutual exclusion (file vs sequence).
 *  Returns NULL on success, error string on failure (caller frees with fdl_free). */
FDL_API const char* fdl_context_set_clip_id_json(
    fdl_context_t* ctx, const char* json_str, size_t json_len);

/** Remove clip_id from a context. Safe to call if not present. */
FDL_API void fdl_context_remove_clip_id(fdl_context_t* ctx);

/** Validate clip_id JSON for mutual exclusion (file vs sequence).
 *  Returns NULL if valid, error string on failure (caller frees with fdl_free). */
FDL_API const char* fdl_clip_id_validate_json(
    const char* json_str, size_t json_len);

/* -----------------------------------------------------------------------
 * Validation (schema + semantic)
 * ----------------------------------------------------------------------- */

/** Opaque handle to a validation result. */
typedef struct fdl_validation_result fdl_validation_result_t;

/** Run schema and semantic validators on the document.
 *  Schema validation (Draft 2020-12) runs first; semantic validators
 *  run only if the document is structurally valid.
 *  Returns a result handle (caller owns, free with fdl_validation_result_free). */
FDL_API fdl_validation_result_t* fdl_doc_validate(const fdl_doc_t* doc);

/** Get the number of validation errors. */
FDL_API uint32_t fdl_validation_result_error_count(const fdl_validation_result_t* result);

/** Get a specific error message by index.
 *  Returns NULL if index is out of range. Pointer valid until result is freed. */
FDL_API const char* fdl_validation_result_error_at(const fdl_validation_result_t* result, uint32_t index);

/** Free a validation result. Safe to call with NULL. */
FDL_API void fdl_validation_result_free(fdl_validation_result_t* result);

/* -----------------------------------------------------------------------
 * Memory management
 * ----------------------------------------------------------------------- */

/** Free memory allocated by fdl_core functions. */
FDL_API void fdl_free(void* ptr);

#ifdef __cplusplus
}
#endif

#endif /* FDL_CORE_H */
