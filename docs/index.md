# Welcome to ASC FDL

ASC FDL is a toolkit to parse, validate, and produce [Framing Decision List (FDL)](https://theasc.com/society/ascmitc/asc-framing-decision-list) files in Python.
In addition to parsing FDL files, ASC FDL provides a command line tool and extensible plugin system.

ASC FDL is modeled around the official FDL [spec](https://github.com/ascmitc/fdl/tree/main/Specification).

# Install

It's recommended to install packages like this in a virtual environment.
```shell
pip install fdl
```

# Features

| Feature                         | Read | Write | Notes                                                                    |
|:--------------------------------|:----:|:-----:|--------------------------------------------------------------------------|
| FDL files                       |  ✔   |   ✔   |                                                                          |
| Validate ID's and relationships |  ✔   |   ✔   | Enforces unique ID's and makes sure relationship between items are valid |
| Expandable through plugins      |  ✔   |   ✔   |                                                                          |
| Verify FDL with JSON Schema     |  ✔   |   ✔   |                                                                          |
| Commandline tool                |  ✔   |   ✔   | `fdl-validate` CLI for FDL validation                                    |

✔ Implemented
✖ Not implemented
N/A Not applicable
