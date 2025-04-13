# Developer Notes
## Current State
This piece of software is in an early development stage. While some essential backend features are already in place, the frontend is still mostly a proof of concept.
A high-level overview over the planned milestones is given below; if you want to know more details, please consult the issues of this project.

## Milestone overview

- **Pre-Release:** Mostly improvements of the codebase to reduce techincal debt introduced in the prototyping phase.
- **Version 1:** This milestone comprises the most basic features required to execute the "Getting Things Done"-workflow in a desktop environment.
- **Version 1.1:** Quality of life improvements like shortcuts, design improvements, etc. 
- **Version 2:** Synchronisation across multiple devices and support for mobile platforms.

## Design
qtd is based on the Qt framework, the backend is implemented in C++, the frontend utilizes Qt's QML stack.
The model classes are based upon Qt's QAbstractItemModel and follow its "Model-View" design pattern.
A high-level overview over present and some planned backend components is sketched in [architecture.pdf](architecture.pdf) (not yet implemented features/components in red).

## Developing process
If you plan to start working on qtd, feel free to reach out or just start working on one of the issues. Make sure to be somewhat used to Qt before doing so.
All pull requests at least need to pass the tests run in the CI. In addition, new features should always be accompanied with automatic tests.
You can also start discussing the [issues labelled with "question"](https://github.com/xwst/qtd/issues?q=is%3Aissue%20state%3Aopen%20label%3Aquestion) or look out for bugs by using the software.
If you would like to contribute source code, make sure to adhere to the coding standards (use clang-tidy!), in particular:

* The number of external dependencies is supposed to be kept as small as possible.
* Use code comments sparsely. Documentation of a trivial getter is pointless and Qt related functions are extensively described in the Qt documentation.
* Clean your git history before pushing. PRs will not be squashed on merge. 

qtd is released under the GPL 3, so your contributions should be published under the same license.

## Compilation
Once you have installed Qt libraries and headers, move to the projects root directory use `cmake` to configure and build the project:

    cmake -B ./build -S . -DCMAKE_PREFIX_PATH=/path/to/your/Qt/installation
    cmake --build ./build

Afterwards, you can use

    cmake --build ./build --target test

on Linux/Windows (gcc) or

    cmake --build ./build --target RUN_TESTS

on Windows (Visual Studio) to execute all tests. If you have `gcovr` installed (`pip install gcovr`) you can also create a code coverage report:

    cmake --build ./build --target coverage

`gcovr` needs to be installed before the configuration step; otherwise, the `coverage`-target will not be set up.
