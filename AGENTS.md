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
bash scripts/run_qmllint.sh                 # qmllint on changed .qml files (CI only)
```

### clang-tidy

Run on individual files after editing (do NOT run the `run_linter_on_changed_files.sh` script):
```sh
clang-tidy -p build/opencode --config-file .clang-tidy <file>
```

Configuration in `.clang-tidy`: all checks enabled (minus non-applicable ones), `WarningsAsErrors: '*'`.

When testing clang-tidy setup changes, only run on single files instead of all files which takes too much time.

### Coverage (requires gcovr, GCC build)

```sh
cmake --build build/opencode --target check-coverage    # HTML report + ≥90% line & function
```

## Coding Conventions

| Aspect | Rule |
|---|---|
| **Include guards** | `#pragma once` only (enforced by CI) |
| **Class names** | PascalCase |
| **Functions/methods** | snake_case; Qt-idiomatic methods (`toString()`, `qHash()`) and required Qt overrides are exceptions |
| **Member variables** | snake_case, constants SCREAMING_SNAKE_CASE (`this->` always used) |
| **Getters** | `get_<name>()` with `[[nodiscard]]`; `is_`/`has_` prefix allowed for boolean accessors |
| **`override`** | Required on all overriding virtual functions |
| **Setters** | `set_<name>()` |
| **Access specifiers** | Order: `private:`, `protected:`, `public:` (private before public). `Q_OBJECT`, `QML_ELEMENT`, `Q_PROPERTY`, `Q_CLASSINFO` go before the first access specifier. |
| **Namespaces** | PascalCase |
| **Constructors** | `explicit` on single-arg; `noexcept` on move; copy deleted. Initializer list: colon on same line, 4-space indent, one per line, trailing commas. Prefer brace init `{}` over `()`. |
| **Braces** | Allman everywhere (opening brace on new line for functions, classes, and control flow). Exception only when braces open+close on the same line (e.g., small lambdas). No blank line between signature and brace. |
| **Pointer/reference** | Attached to type (`const QString&`, `QObject*`) — `*` and `&` are part of the type, not a prefix of the variable name. When parameter names are commented out, place `&`/`*` before the comment: `const QString& /*name*/`. |
| **Indentation** | 4 spaces |
| **`static const`** | Always `static const`, never `const static` |
| **CMake commands** | All uppercase (`ADD_EXECUTABLE`, `TARGET_LINK_LIBRARIES`, …) |
| **License header** | Block `/** */` comment at top of every `.cpp`, `.h`, `.qml`, `.js`, `.cpp.in` |
| **Include order** | Grouped blocks sorted alphabetically: (1) corresponding header (cpp files only), (2) standard library, (3) Qt headers, (4) project headers |
| **NOLINT** | Use `// NOLINTNEXTLINE(rule1, rule2)` on line preceding violation. Space after `//`, no space between `NOLINT` and `(`. |
| **Comments** | Use sparingly — Qt docs are the primary reference |
| **QML/JS naming** | PascalCase for components, snake_case for functions. QML signals are exempt from snake_case (Qt signals use camelCase). Omit trailing `;` on `property` declarations. |
| **Qt interop** | Prefer `Q_PROPERTY` for data binding, `Q_INVOKABLE` for actions. Use macro-defined `enum` pattern (`Q_ENUM`) for QML-facing enumerations. |
| **const correctness** | All read-only member functions must be `const`. `const` methods returning pointers should return `const*`. |
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
