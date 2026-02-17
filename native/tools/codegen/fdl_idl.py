# SPDX-FileCopyrightText: 2024-present American Society Of Cinematographers
# SPDX-License-Identifier: Apache-2.0
"""
Parse fdl_api.yaml into typed dataclass models for code generation.

Supports both the flat function list (for low-level FFI generation) and
the ``object_model`` section (for facade generation).
"""

from __future__ import annotations

from dataclasses import dataclass, field
from pathlib import Path

import yaml

from .ir import (
    IR,
    IRClass,
    IRCollection,
    IRErrorHandling,
    IRInit,
    IRInitParam,
    IRInitPostSetter,
    IRMethod,
    IRMethodParam,
    IRProperty,
    IRResultField,
)

# -----------------------------------------------------------------------
# Low-level IDL dataclasses (unchanged from Phase 3)
# -----------------------------------------------------------------------


@dataclass
class ABIVersion:
    major: int
    minor: int
    patch: int


@dataclass
class StructField:
    name: str
    c_type: str
    ownership: str | None = None
    direction: str | None = None
    nullable: bool = False


@dataclass
class VTMethodParam:
    name: str
    param_type: str
    nullable: bool = False


@dataclass
class VTOutParam:
    name: str
    param_type: str


@dataclass
class VTMethod:
    name: str
    c_function: str | None = None
    pure: bool = False
    kind: str = "instance"  # "instance" or "classmethod"
    params: list[VTMethodParam] = field(default_factory=list)
    returns: str | None = None
    out_params: list[VTOutParam] = field(default_factory=list)


@dataclass
class VTOperator:
    op: str
    c_function: str | None = None
    pure: bool = False
    param_type: str | None = None
    returns: str = "bool"


@dataclass
class ValueType:
    name: str
    fields: list[StructField]
    facade_class: str | None = None
    facade_converter: str | None = None
    facade_defaults: dict[str, str] | None = None
    cross_eq: str | None = None
    methods: list[VTMethod] = field(default_factory=list)
    operators: list[VTOperator] = field(default_factory=list)


@dataclass
class EnumValue:
    name: str
    value: int


@dataclass
class EnumType:
    name: str
    underlying: str
    values: list[EnumValue]
    facade_class: str | None = None
    facade_prefix: str | None = None
    string_values: dict[str, str] | None = None


@dataclass
class FunctionParam:
    name: str
    c_type: str
    direction: str | None = None
    nullable: bool = False


@dataclass
class Function:
    name: str
    returns: str
    params: list[FunctionParam]
    doc: str = ""
    ownership: str | None = None
    nullable: bool = False
    category: str | None = None


# -----------------------------------------------------------------------
# Object model dataclasses (new for Phase 4 groundwork)
# -----------------------------------------------------------------------


@dataclass
class PropertyMapping:
    name: str
    getter: str
    setter: str | None = None
    remover: str | None = None
    has_fn: str | None = None
    value_type: str = "string"
    nullable: bool = False


@dataclass
class CollectionPattern:
    name: str
    item_class: str
    count_fn: str
    at_fn: str
    find_by_id_fn: str | None = None
    find_by_label_fn: str | None = None
    add_fn: str | None = None


@dataclass
class MethodParamMapping:
    name: str
    param_type: str
    nullable: bool = False
    default: str | None = None
    expand: bool = False
    source_class: str | None = None
    global_fallback: str | None = None


@dataclass
class ResultFieldMapping:
    name: str
    source: str
    extract: str  # "handle", "scalar", "value_type", "string"
    wrap_class: str | None = None
    converter: str | None = None
    scalar_type: str | None = None
    private: bool = False


@dataclass
class ErrorHandlingMapping:
    pattern: str  # "null_check", "result_struct", "result_struct_multi", "validation"
    error_field: str | None = None
    success_field: str | None = None
    error_class: str | None = None
    free_fn: str | None = None
    count_fn: str | None = None
    at_fn: str | None = None
    result_fields: list[ResultFieldMapping] | None = None


@dataclass
class MethodMapping:
    name: str
    function: str
    kind: str = "instance"  # "static_factory", "instance", "builder", "compound_setter", "alias", "class_factory"
    params: list[MethodParamMapping] | None = None
    returns: str | None = None
    error_handling: ErrorHandlingMapping | None = None
    doc: str = ""
    alias_of: str | None = None
    compose_from: dict[str, str] | None = None
    factory_kwargs: list[dict] | None = None


@dataclass
class InitParam:
    name: str
    param_type: str
    nullable: bool = False
    default: str | None = None


@dataclass
class InitPostSetter:
    kind: str  # "property" or "compound"
    property: str | None = None
    method: str | None = None
    condition: str | None = None
    args: dict[str, str] | None = None
    always: bool = False


@dataclass
class InitDef:
    depth: int
    builder_method: str
    params: list[InitParam]
    post_setters: list[InitPostSetter] = field(default_factory=list)


@dataclass
class ObjectClass:
    name: str
    c_handle: str
    owns_handle: bool = False
    parent: str | None = None
    factory: str | None = None
    destructor: str | None = None
    properties: list[PropertyMapping] = field(default_factory=list)
    collections: list[CollectionPattern] = field(default_factory=list)
    methods: list[MethodMapping] = field(default_factory=list)
    init: InitDef | None = None


@dataclass
class ObjectModel:
    classes: list[ObjectClass] = field(default_factory=list)


# -----------------------------------------------------------------------
# Auxiliary types (errors, version, etc.)
# -----------------------------------------------------------------------


@dataclass
class ErrorDef:
    name: str
    parent: str
    doc: str = ""


@dataclass
class VersionSlot:
    name: str
    slot_type: str
    default: str


@dataclass
class VersionDef:
    class_name: str
    doc: str
    slots: list[VersionSlot]


@dataclass
class DataclassField:
    name: str
    field_type: str
    private: bool = False


@dataclass
class DataclassAccessor:
    name: str
    returns: str
    doc: str
    lookup: str  # "by_label" or "by_id"
    collection: str  # e.g. "fdl.contexts", "context.canvases"
    key_field: str  # e.g. "context_label"


@dataclass
class DataclassDef:
    class_name: str
    doc: str
    fields: list[DataclassField]
    accessors: list[DataclassAccessor] | None = None


@dataclass
class JsonWrapperField:
    name: str
    field_type: str
    nullable: bool = False
    max_length: int | None = None
    min_value: int | None = None
    c_has_flag: str | None = None


@dataclass
class JsonWrapperDef:
    class_name: str
    doc: str
    fields: list[JsonWrapperField]
    c_struct: str | None = None
    free_fn: str | None = None
    mutual_exclusion: list[str] | None = None


@dataclass
class AuxiliaryTypes:
    errors: list[ErrorDef] = field(default_factory=list)
    version: VersionDef | None = None
    dataclasses: list[DataclassDef] = field(default_factory=list)
    json_wrappers: list[JsonWrapperDef] = field(default_factory=list)


# -----------------------------------------------------------------------
# Free functions (top-level Python functions wrapping C ABI)
# -----------------------------------------------------------------------


@dataclass
class FreeFunctionParam:
    name: str
    param_type: str


@dataclass
class FreeFunctionDef:
    name: str
    display_name: str
    c_function: str
    doc: str
    params: list[FreeFunctionParam]
    returns: str
    module: str = "rounding"  # target Python module: "rounding" or "utils"


# -----------------------------------------------------------------------
# Utility functions (generated per-language from template)
# -----------------------------------------------------------------------


@dataclass
class UtilityDef:
    name: str
    kind: str  # "c_abi", "collection_find", "io", "rounding_state", "constant"
    doc: str
    c_function: str | None = None
    extract: str | None = None  # "dims" or "anchor" (for c_abi kind)


# -----------------------------------------------------------------------
# Complete IDL
# -----------------------------------------------------------------------


@dataclass
class IDL:
    abi_version: ABIVersion
    value_types: list[ValueType]
    enums: list[EnumType]
    opaque_types: list[str]
    functions: list[Function]
    object_model: ObjectModel = field(default_factory=ObjectModel)
    auxiliary_types: AuxiliaryTypes = field(default_factory=AuxiliaryTypes)
    free_functions: list[FreeFunctionDef] = field(default_factory=list)
    utilities: list[UtilityDef] = field(default_factory=list)


# -----------------------------------------------------------------------
# Parsers
# -----------------------------------------------------------------------


def _parse_field(raw: dict) -> StructField:
    return StructField(
        name=raw["name"],
        c_type=raw["type"],
        ownership=raw.get("ownership"),
        direction=raw.get("direction"),
        nullable=raw.get("nullable", False),
    )


def _parse_value_type(name: str, raw: dict) -> ValueType:
    facade = raw.get("facade", {})

    methods = []
    for m in raw.get("methods", []):
        params = [VTMethodParam(name=p["name"], param_type=p["type"], nullable=p.get("nullable", False)) for p in m.get("params", [])]
        out_params = [VTOutParam(name=p["name"], param_type=p["type"]) for p in m.get("out_params", [])]
        methods.append(
            VTMethod(
                name=m["name"],
                c_function=m.get("c_function"),
                pure=m.get("pure", False),
                kind=m.get("kind", "instance"),
                params=params,
                returns=m.get("returns"),
                out_params=out_params,
            )
        )

    operators = [
        VTOperator(
            op=o["op"],
            c_function=o.get("c_function"),
            pure=o.get("pure", False),
            param_type=o.get("param_type"),
            returns=o.get("returns", "bool"),
        )
        for o in raw.get("operators", [])
    ]

    return ValueType(
        name=name,
        fields=[_parse_field(f) for f in raw["fields"]],
        facade_class=facade.get("class_name"),
        facade_converter=facade.get("converter"),
        facade_defaults=facade.get("defaults"),
        cross_eq=raw.get("cross_eq"),
        methods=methods,
        operators=operators,
    )


def _parse_enum(name: str, raw: dict) -> EnumType:
    values = [EnumValue(name=k, value=v) for k, v in raw["values"].items()]
    facade = raw.get("facade", {})
    return EnumType(
        name=name,
        underlying=raw["underlying"],
        values=values,
        facade_class=facade.get("class_name"),
        facade_prefix=facade.get("prefix"),
        string_values=raw.get("string_values"),
    )


def _parse_function(raw: dict) -> Function:
    params = []
    for p in raw.get("params", []):
        params.append(
            FunctionParam(
                name=p["name"],
                c_type=p["type"],
                direction=p.get("direction"),
                nullable=p.get("nullable", False),
            )
        )
    return Function(
        name=raw["name"],
        returns=raw["returns"],
        params=params,
        doc=raw.get("doc", ""),
        ownership=raw.get("ownership"),
        nullable=raw.get("nullable", False),
        category=raw.get("category"),
    )


def _parse_property(name: str, raw: dict) -> PropertyMapping:
    return PropertyMapping(
        name=name,
        getter=raw["getter"],
        setter=raw.get("setter"),
        remover=raw.get("remover"),
        has_fn=raw.get("has"),
        value_type=raw.get("type", "string"),
        nullable=raw.get("nullable", False),
    )


def _parse_collection(name: str, raw: dict) -> CollectionPattern:
    return CollectionPattern(
        name=name,
        item_class=raw["item_class"],
        count_fn=raw["count"],
        at_fn=raw["at"],
        find_by_id_fn=raw.get("find_by_id"),
        find_by_label_fn=raw.get("find_by_label"),
        add_fn=raw.get("add"),
    )


def _parse_method(name: str, raw: dict) -> MethodMapping:
    params = None
    if "params" in raw:
        params = [
            MethodParamMapping(
                name=p["name"],
                param_type=p["type"],
                nullable=p.get("nullable", False),
                default=p.get("default"),
                expand=p.get("expand", False),
                source_class=p.get("source_class"),
                global_fallback=p.get("global_fallback"),
            )
            for p in raw["params"]
        ]
    error_handling = None
    if "error_handling" in raw:
        eh = raw["error_handling"]
        result_fields = None
        if "fields" in eh:
            result_fields = [
                ResultFieldMapping(
                    name=f["name"],
                    source=f["source"],
                    extract=f["extract"],
                    wrap_class=f.get("wrap_class"),
                    converter=f.get("converter"),
                    scalar_type=f.get("scalar_type"),
                    private=f.get("private", False),
                )
                for f in eh["fields"]
            ]
        error_handling = ErrorHandlingMapping(
            pattern=eh["pattern"],
            error_field=eh.get("error_field"),
            success_field=eh.get("success_field"),
            error_class=eh.get("error_class"),
            free_fn=eh.get("free_fn"),
            count_fn=eh.get("count_fn"),
            at_fn=eh.get("at_fn"),
            result_fields=result_fields,
        )
    return MethodMapping(
        name=name,
        function=raw.get("function", ""),
        kind=raw.get("kind", "instance"),
        params=params,
        returns=raw.get("returns"),
        error_handling=error_handling,
        doc=raw.get("doc", ""),
        alias_of=raw.get("alias_of"),
        compose_from=raw.get("compose_from"),
        factory_kwargs=raw.get("factory_kwargs"),
    )


def _parse_init(raw: dict) -> InitDef:
    params = [
        InitParam(
            name=p["name"],
            param_type=p["type"],
            nullable=p.get("nullable", False),
            default=p.get("default"),
        )
        for p in raw.get("params", [])
    ]
    post_setters = [
        InitPostSetter(
            kind=ps["kind"],
            property=ps.get("property"),
            method=ps.get("method"),
            condition=ps.get("condition"),
            args=ps.get("args"),
            always=ps.get("always", False),
        )
        for ps in raw.get("post_setters", [])
    ]
    return InitDef(
        depth=raw["depth"],
        builder_method=raw["builder_method"],
        params=params,
        post_setters=post_setters,
    )


def _parse_object_class(name: str, raw: dict) -> ObjectClass:
    properties = [_parse_property(k, v) for k, v in raw.get("properties", {}).items()]
    collections = [_parse_collection(k, v) for k, v in raw.get("collections", {}).items()]
    methods = [_parse_method(k, v) for k, v in raw.get("methods", {}).items()]
    init = _parse_init(raw["init"]) if "init" in raw else None

    return ObjectClass(
        name=name,
        c_handle=raw["c_handle"],
        owns_handle=raw.get("owns_handle", False),
        parent=raw.get("parent"),
        factory=raw.get("factory"),
        destructor=raw.get("destructor"),
        properties=properties,
        collections=collections,
        methods=methods,
        init=init,
    )


def _parse_object_model(raw: dict | None) -> ObjectModel:
    if not raw:
        return ObjectModel()
    classes = [_parse_object_class(name, cls) for name, cls in raw.get("classes", {}).items()]
    return ObjectModel(classes=classes)


def _parse_auxiliary_types(raw: dict | None) -> AuxiliaryTypes:
    if not raw:
        return AuxiliaryTypes()
    errors = [ErrorDef(name=e["name"], parent=e["parent"], doc=e.get("doc", "")) for e in raw.get("errors", [])]
    version = None
    if "version" in raw:
        v = raw["version"]
        slots = [VersionSlot(name=s["name"], slot_type=s["type"], default=str(s["default"])) for s in v.get("slots", [])]
        version = VersionDef(class_name=v["class_name"], doc=v.get("doc", ""), slots=slots)
    dataclasses_list = []
    for dc_name, dc_raw in raw.get("dataclasses", {}).items():
        fields = [DataclassField(name=f["name"], field_type=f["type"], private=f.get("private", False)) for f in dc_raw.get("fields", [])]
        accessors = None
        if "accessors" in dc_raw:
            accessors = [
                DataclassAccessor(
                    name=a["name"],
                    returns=a["returns"],
                    doc=a.get("doc", ""),
                    lookup=a["lookup"],
                    collection=a["collection"],
                    key_field=a["key_field"],
                )
                for a in dc_raw["accessors"]
            ]
        dataclasses_list.append(
            DataclassDef(
                class_name=dc_name,
                doc=dc_raw.get("doc", ""),
                fields=fields,
                accessors=accessors,
            )
        )
    json_wrappers_list = []
    for jw_name, jw_raw in raw.get("json_wrappers", {}).items():
        jw_fields = [
            JsonWrapperField(
                name=f["name"],
                field_type=f["type"],
                nullable=f.get("nullable", False),
                max_length=f.get("max_length"),
                min_value=f.get("min_value"),
                c_has_flag=f.get("c_has_flag"),
            )
            for f in jw_raw.get("fields", [])
        ]
        json_wrappers_list.append(
            JsonWrapperDef(
                class_name=jw_name,
                doc=jw_raw.get("doc", ""),
                fields=jw_fields,
                c_struct=jw_raw.get("c_struct"),
                free_fn=jw_raw.get("free_fn"),
                mutual_exclusion=jw_raw.get("mutual_exclusion"),
            )
        )
    return AuxiliaryTypes(errors=errors, version=version, dataclasses=dataclasses_list, json_wrappers=json_wrappers_list)


def parse_idl(path: Path) -> IDL:
    with path.open(encoding="utf-8") as f:
        data = yaml.safe_load(f)

    abi = data["abi_version"]
    abi_version = ABIVersion(major=abi["major"], minor=abi["minor"], patch=abi["patch"])

    value_types = [_parse_value_type(name, vt) for name, vt in data.get("value_types", {}).items()]
    enums = [_parse_enum(name, e) for name, e in data.get("enums", {}).items()]
    opaque_types = data.get("opaque_types", [])
    functions = [_parse_function(f) for f in data.get("functions", [])]
    object_model = _parse_object_model(data.get("object_model"))
    auxiliary_types = _parse_auxiliary_types(data.get("auxiliary_types"))

    # Parse free functions
    free_functions = []
    for ff_raw in data.get("free_functions", []):
        params = [FreeFunctionParam(name=p["name"], param_type=p["type"]) for p in ff_raw.get("params", [])]
        free_functions.append(
            FreeFunctionDef(
                name=ff_raw["name"],
                display_name=ff_raw["display_name"],
                c_function=ff_raw["c_function"],
                doc=ff_raw.get("doc", ""),
                params=params,
                returns=ff_raw.get("returns", "None"),
                module=ff_raw.get("module", "rounding"),
            )
        )

    # Parse utilities
    utilities = []
    for u_raw in data.get("utilities", []):
        utilities.append(
            UtilityDef(
                name=u_raw["name"],
                kind=u_raw.get("kind", "c_abi"),
                doc=u_raw.get("doc", ""),
                c_function=u_raw.get("c_function"),
                extract=u_raw.get("extract"),
            )
        )

    return IDL(
        abi_version=abi_version,
        value_types=value_types,
        enums=enums,
        opaque_types=opaque_types,
        functions=functions,
        object_model=object_model,
        auxiliary_types=auxiliary_types,
        free_functions=free_functions,
        utilities=utilities,
    )


# -----------------------------------------------------------------------
# IDL → IR transformation
# -----------------------------------------------------------------------


_IDENTITY_ATTRS: dict[str, str] = {
    "Context": "label",
    "Canvas": "id",
    "FramingDecision": "id",
    "FramingIntent": "id",
    "CanvasTemplate": "id",
}


def build_ir(idl: IDL) -> IR:
    """Transform the IDL object model into the language-neutral IR."""
    ir = IR()
    for cls in idl.object_model.classes:
        ir_class = IRClass(
            name=cls.name,
            handle_type=cls.c_handle,
            owns_handle=cls.owns_handle,
            parent=cls.parent,
            factory=cls.factory,
            destructor=cls.destructor,
            identity_attr=_IDENTITY_ATTRS.get(cls.name),
        )

        for prop in cls.properties:
            ir_class.properties.append(
                IRProperty(
                    name=prop.name,
                    type_key=prop.value_type,
                    getter_fn=prop.getter,
                    setter_fn=prop.setter,
                    remover_fn=prop.remover,
                    has_fn=prop.has_fn,
                    nullable=prop.nullable,
                )
            )

        for coll in cls.collections:
            ir_class.collections.append(
                IRCollection(
                    name=coll.name,
                    item_class=coll.item_class,
                    count_fn=coll.count_fn,
                    at_fn=coll.at_fn,
                    find_by_id_fn=coll.find_by_id_fn,
                    find_by_label_fn=coll.find_by_label_fn,
                    add_fn=coll.add_fn,
                )
            )

        for method in cls.methods:
            ir_params = []
            if method.params:
                for p in method.params:
                    ir_params.append(
                        IRMethodParam(
                            name=p.name,
                            type_key=p.param_type,
                            nullable=p.nullable,
                            default=p.default,
                            expand=p.expand,
                            source_class=p.source_class,
                            global_fallback=p.global_fallback,
                        )
                    )
            ir_error = None
            if method.error_handling:
                eh = method.error_handling
                ir_result_fields = None
                if eh.result_fields:
                    ir_result_fields = [
                        IRResultField(
                            name=rf.name,
                            source=rf.source,
                            extract=rf.extract,
                            wrap_class=rf.wrap_class,
                            converter=rf.converter,
                            scalar_type=rf.scalar_type,
                            private=rf.private,
                        )
                        for rf in eh.result_fields
                    ]
                ir_error = IRErrorHandling(
                    pattern=eh.pattern,
                    error_field=eh.error_field,
                    success_field=eh.success_field,
                    error_class=eh.error_class,
                    free_fn=eh.free_fn,
                    count_fn=eh.count_fn,
                    at_fn=eh.at_fn,
                    result_fields=ir_result_fields,
                )
            ir_class.methods.append(
                IRMethod(
                    name=method.name,
                    function=method.function,
                    kind=method.kind,
                    params=ir_params,
                    returns=method.returns,
                    error_handling=ir_error,
                    doc=method.doc,
                    alias_of=method.alias_of,
                    compose_from=method.compose_from,
                    factory_kwargs=method.factory_kwargs,
                )
            )

        if cls.init:
            ir_class.init = IRInit(
                depth=cls.init.depth,
                builder_method=cls.init.builder_method,
                params=[
                    IRInitParam(
                        name=p.name,
                        type_key=p.param_type,
                        nullable=p.nullable,
                        default=p.default,
                    )
                    for p in cls.init.params
                ],
                post_setters=[
                    IRInitPostSetter(
                        kind=ps.kind,
                        property=ps.property,
                        method=ps.method,
                        condition=ps.condition,
                        args=ps.args,
                        always=ps.always,
                    )
                    for ps in cls.init.post_setters
                ],
            )

        ir.classes.append(ir_class)

    return ir
