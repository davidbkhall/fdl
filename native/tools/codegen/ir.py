# SPDX-FileCopyrightText: 2024-present American Society Of Cinematographers
# SPDX-License-Identifier: Apache-2.0
"""
Intermediate Representation (IR) for code generation.

Transforms parsed IDL object model into a language-neutral IR that
Jinja2 templates consume. Each IRClass represents an idiomatic class
in the target language, with properties backed by C ABI getters/setters
and collections backed by count/at/find patterns.
"""

from __future__ import annotations

from dataclasses import dataclass, field


@dataclass
class IRProperty:
    """A readable (and optionally writable) property on a class."""

    name: str
    type_key: str  # Language-neutral key resolved by type_maps
    getter_fn: str
    setter_fn: str | None = None
    remover_fn: str | None = None
    has_fn: str | None = None
    nullable: bool = False
    converter: str | None = None  # e.g. "dims_i64", "string", "enum_geometry_path"

    @property
    def read_only(self) -> bool:
        return self.setter_fn is None


@dataclass
class IRCollection:
    """A typed collection (list-like) exposed on a class."""

    name: str
    item_class: str  # Name of the IRClass for the item type
    count_fn: str
    at_fn: str
    find_by_id_fn: str | None = None
    find_by_label_fn: str | None = None
    add_fn: str | None = None


@dataclass
class IRMethodParam:
    """A parameter for a method."""

    name: str
    type_key: str  # Language-neutral key resolved by type_maps
    nullable: bool = False
    default: str | None = None  # Default value expression (e.g. "None")
    expand: bool = False  # Expand value type fields as separate C args
    source_class: str | None = None  # For handle params: which facade class
    global_fallback: str | None = None  # For nullable params: fallback function name


@dataclass
class IRResultField:
    """A field in a multi-field result struct extraction."""

    name: str  # Python field name (e.g. "doc")
    source: str  # C struct field name (e.g. "output_doc")
    extract: str  # "handle", "scalar", "value_type", "string"
    wrap_class: str | None = None  # For extract=handle
    converter: str | None = None  # For extract=value_type
    python_type: str | None = None  # For extract=scalar
    private: bool = False  # If True, prefix field name with _ in constructor call


@dataclass
class IRErrorHandling:
    """Error handling pattern for a method."""

    pattern: str  # "null_check", "result_struct", "result_struct_multi", "validation"
    error_field: str | None = None
    success_field: str | None = None
    error_class: str | None = None
    free_fn: str | None = None
    count_fn: str | None = None  # For validation: error count function
    at_fn: str | None = None  # For validation: error-at function
    result_fields: list[IRResultField] | None = None  # For result_struct_multi


@dataclass
class IRMethod:
    """A method on a class (instance, static_factory, builder, compound_setter, etc.)."""

    name: str
    function: str  # C ABI function name
    kind: str  # "instance", "static_factory", "builder", "compound_setter", "alias"
    params: list[IRMethodParam] = field(default_factory=list)
    returns: str | None = None  # Return class/type name
    error_handling: IRErrorHandling | None = None
    doc: str = ""
    alias_of: str | None = None  # For "alias" kind: target method name
    compose_from: dict[str, str] | None = None  # For "composite_property" kind: {field: source_prop}
    factory_kwargs: list[dict] | None = None  # For "class_factory" kind: [{name, value}]


@dataclass
class IRInitParam:
    """A parameter for a kwargs __init__ method."""

    name: str
    type_key: str  # Language-neutral key resolved by type_maps
    nullable: bool = False
    default: str | None = None


@dataclass
class IRInitPostSetter:
    """A post-construction setter call in kwargs __init__."""

    kind: str  # "property" or "compound"
    property: str | None = None
    method: str | None = None
    condition: str | None = None
    args: dict[str, str] | None = None
    always: bool = False


@dataclass
class IRInit:
    """Kwargs __init__ definition for a facade class."""

    depth: int  # scaffolding depth: 1=doc->obj, 2=doc->ctx->obj, 3=doc->ctx->canvas->obj
    builder_method: str  # name of builder method on parent class
    params: list[IRInitParam] = field(default_factory=list)
    post_setters: list[IRInitPostSetter] = field(default_factory=list)


@dataclass
class IRClass:
    """A class in the facade, backed by an opaque C handle."""

    name: str
    handle_type: str  # C opaque handle type, e.g. "fdl_doc_t"
    owns_handle: bool
    parent: str | None = None
    factory: str | None = None
    destructor: str | None = None
    identity_attr: str | None = None  # "id" for Canvas/FD/FI/CT, "label" for Context, None for FDL
    properties: list[IRProperty] = field(default_factory=list)
    collections: list[IRCollection] = field(default_factory=list)
    methods: list[IRMethod] = field(default_factory=list)
    init: IRInit | None = None


@dataclass
class IR:
    """Complete intermediate representation for facade generation."""

    classes: list[IRClass] = field(default_factory=list)
