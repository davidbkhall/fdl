# Native FDL Core — Conventions

## Naming: FDL, not Document

In all client-facing code (C++ RAII header, Python bindings, future language targets),
the top-level document class MUST be named `FDL`, never `Document`.

- **C ABI**: `fdl_doc_t`, `fdl_doc_*` functions — this is fine, `doc` is the internal handle name.
- **C++ RAII** (`fdl.hpp`): `fdl::FDL` — the owning wrapper class.
- **Python** (`fdl`): `FDL` — the facade class.

The rationale: an FDL *is* a Framing Decision List. Client code should read naturally
as `fdl::FDL::parse(...)` or `FDL.parse(...)`, not `Document`.
