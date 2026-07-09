# qtd — GTD Workflow Manager

## Project

C++20 + Qt 6.10+ (Core, Gui, Quick, QuickControls2, Sql, Test, QuickTest, Qml, Concurrent, Widgets).
QML frontend, C++ backend. CMake build system. SQLite via Qt Sql. GPLv3 licensed.

**Current state**: Early development — backend features functional, frontend is a proof of concept.

## Build & Run

```sh
cmake -B build/opencode -S .
cmake --build build/opencode            # build all
cmake --build build/opencode --target qtd  # main app only
```

Qt is globally installed — no `-DCMAKE_PREFIX_PATH` needed.

## Test

All tests:
```sh
ctest --test-dir build/opencode --output-on-failure -j8
```

Backend-only (exclude QML/frontend tests):
```sh
ctest --test-dir build/opencode -LE frontend-test -j8 --output-on-failure
```

Frontend-only:
```sh
ctest --test-dir build/opencode -L frontend-test -j8 --output-on-failure
```

## Code Quality

### Verification scripts

```sh
bash scripts/verify_license_headers.sh      # all .cpp/.h/.qml/.js need GPL header
bash scripts/verify_no_quuid_usage.sh       # QUuid only in qtdid.h/cpp
bash scripts/verify_no_include_guards.sh    # #pragma once required, no #ifndef guards
```

### clang-tidy

Run on individual files after editing (do NOT run the `run_linter_on_changed_files.sh` script):
```sh
clang-tidy -p build/opencode --config-file .clang-tidy <file>
```

Configuration in `.clang-tidy`: all checks enabled (minus non-applicable ones), `WarningsAsErrors: '*'`.

### Coverage (requires gcovr, GCC build)

```sh
cmake --build build/opencode --target coverage              # HTML report
cmake --build build/opencode --target verify-test-coverage    # ≥90% line & function
```

## Coding Conventions

| Aspect | Rule |
|---|---|
| **Include guards** | `#pragma once` only (enforced by CI) |
| **Class names** | PascalCase |
| **Functions/methods** | snake_case |
| **Member variables** | snake_case (`this->` always used) |
| **Getters** | `get_<name>()` with `[[nodiscard]]` |
| **Setters** | `set_<name>()` |
| **Constructors** | `explicit` on single-arg; `noexcept` on move; copy deleted |
| **Braces (functions)** | Allman (opening brace on new line) |
| **Braces (control flow)** | K&R (opening brace on same line) |
| **Pointer/reference** | Attached to type (`const QString&`, `QObject*`) |
| **Indentation** | 4 spaces |
| **License header** | Block `/** */` comment at top of every `.cpp`, `.h`, `.qml`, `.js` |
| **NOLINT** | Use with descriptive reason, e.g. `// NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)` |
| **Comments** | Use sparingly — Qt docs are the primary reference |
| **Git history** | Clean before pushing; PRs are not squashed on merge |


### Data flow

```
QML UI  ←→  QmlInterface (singleton)  ←→  *ItemModel  ←→  *Repository  ←→  SQLite
```

`QmlInterface` exposes models as `Q_PROPERTY` to QML. Filtered models are proxy models wrapping the source `TaskItemModel`/`TagItemModel`.

### Key patterns

- **QtdId** — UUID wrapper enforcing type safety (use `QtdId`/`TaskId`/`TagId`, never raw `QUuid`)
- **TreeItemModel** — `QAbstractItemModel` subclass using `TreeNode` for tree structure; supports `QModelIndex` hierarchy
- **Test pattern** — Models tested with `QAbstractItemModelTester` for validity, plus custom assertions via `TestHelpers`
- **QML interop** — `Q_PROPERTY` for data exposure, `Q_INVOKABLE` for actions, `Q_ENUM` for enums

## Temporary files
Put all files in a custom subdirectory of ./build or the global /tmp folder to avoid polluting the working directory.
