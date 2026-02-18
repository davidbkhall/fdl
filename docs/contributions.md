# Contributions

Contributions come in many forms — reporting bugs, giving feedback, or submitting code.
Our goal is to provide a useful toolkit that suits the needs of its users.

## Architecture

ASC FDL is built around a shared C core library (`libfdl_core`) with auto-generated
bindings for Python and C++. The code generation pipeline uses Jinja2 templates
driven by `fdl_api.yaml`.

- **C/C++ API Reference**: [Doxygen docs on GitHub Pages](https://ascmitc.github.io/fdl/api/)
- **Python API Reference**: see the [FDL Classes](FDL Classes/fdl.md) section

## Package Management

ASC FDL uses [uv](https://docs.astral.sh/uv/) for Python package management.

## Running Tests

### Python

```commandline
uv run pytest
```

### C++

```commandline
cmake --build native/core/build
ctest --test-dir native/core/build
```

## Building Documentation

```commandline
# Install docs dependencies
uv pip install -e ".[docs]"

# Build the docs (output in "site" folder)
mkdocs build

# Serve locally on localhost:8000
mkdocs serve
```

The C/C++ Doxygen documentation is built automatically by CI on push to the `dev` branch
and deployed to [GitHub Pages](https://ascmitc.github.io/fdl/api/).

## Regenerating Bindings

After modifying `fdl_api.yaml` or Jinja2 templates, regenerate the bindings:

```commandline
cd native/tools && python -m codegen.generate --target python-ffi
cd native/tools && python -m codegen.generate --target python-facade
cd native/tools && python -m codegen.generate --target cpp-raii
```

## Checklist For Contributions

### Fork the repo

Please fork the repo on GitHub and clone your fork locally.

`git clone git@github.com:<USERNAME>/fdl.git`

### Create a feature branch

Always work in a feature branch. ***Do not submit Pull Requests directly from "main".***

`git checkout -b my_feature_branch`

### Write code

Please try to follow the style of the project when writing code.
Use type hints and provide docstrings in your code.

### Write tests

All contributions should provide tests for new/updated behavior.
We use [pytest](https://docs.pytest.org/en/stable/) for Python and
[Catch2](https://github.com/catchorg/Catch2) for C++.

### Documentation

Please add/update relevant documentation. We use [mkdocs](https://www.mkdocs.org/) and
[mkdocstrings](https://mkdocstrings.github.io/).

!!! note
    All Python code blocks in the documentation are tested automatically,
    so make sure to write valid, self-contained examples.

### Submitting a Pull Request

Push your feature branch to your repo and open a Pull Request on GitHub.

`git push origin my_feature_branch`
