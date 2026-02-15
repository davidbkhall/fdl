# Contributions
Contributions come in many forms, be it reporting bugs, giving feedback or submitting code. It's all welcome
as our goal is to provide a useful toolkit that suits the needs of its users.

## Package Management
ASC FDL uses [uv](https://docs.astral.sh/uv/) for package management.

There is an environment setup for unit testing.

```commandline
uv run pytest
```

To build and serve the documentation locally, you may use one of the following commands.
```commandline
# Only build the docs. You'll find them in the "site" folder
mkdocs build

# Serve the docs on localhost:8000
mkdocs serve
```

## Checklist For Contributions
### Fork the repo
Please fork the repo on GitHub and clone your fork locally.
`git clone git@github.com:<USERNAME>/fdl.git`

### Create a feature branch
Always work in a feature branch. ***Do not submit Pull Requests directly from "main"***
Name your branch something relatable to the feature you're adding or a bug you're fixing.
Only address one feature/bug in a branch to the best of your judgement.
`git checkout -b my_feature_branch`

### Write code
Please try to follow the style of the project when writing code.
Use type hints and provide docstrings in your code.

### Write unittests
All contributions should provide tests for new/updated behavior. We use [pytest](https://docs.pytest.org/en/stable/), please consult
their documentation for info on usage.
New sample files may be added to the `tests/sample_data` folder if needed

### Documentation
Please add/update relevant documentation. We use [mkdocs](https://www.mkdocs.org/) and
[mkdocstrings](https://mkdocstrings.github.io/)
>**NOTE!** All python code blocks will get tested, so make sure to write valid examples.

### Submitting a Pull Request
Push your feature branch to your repo and open a Pull Request on GitHub.
`git push origin my_feature_branch`

Continue to push to this branch until the Pull Request is merged
