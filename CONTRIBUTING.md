# Contributing to Torque2D

Thank you for your interest in contributing to Torque2D! This document provides guidelines and instructions for contributing to the project.

## Code of Conduct

Please read and follow our [Code of Conduct](CODE_OF_CONDUCT.md) to maintain a welcoming and inclusive environment for all contributors.

## Reporting Bugs

Before submitting a bug report, please check if the issue has already been reported by searching the [issue tracker](https://github.com/TorqueGameEngines/Torque2D/issues).

When filing a new bug report, please include:

- A clear and descriptive title
- Steps to reproduce the issue
- Expected behavior vs. actual behavior
- Your development environment (OS, compiler version, etc.)
- Any relevant logs, screenshots, or code snippets
- If possible, a minimal test case that demonstrates the issue

## Proposing Features

Feature proposals are welcome! Before submitting:

1. Search existing issues to ensure the feature hasn't been proposed or rejected
2. Open a new issue with the "feature request" label
3. Describe the feature in detail, including:
   - The problem it solves or the use case it addresses
   - Proposed implementation approach
   - Any potential impact on existing functionality
   - Examples of how the feature would be used

The maintainers will review your proposal and provide feedback. Once approved, you can begin implementation.

## Submitting Pull Requests

### Before You Start

1. Fork the repository and create a new branch from `main`
2. Name your branch descriptively (e.g., `fix/collision-bug`, `feature/new-renderer`)
3. Ensure your changes address a specific issue or feature

### C++ Coding Standards

Torque2D is written in C++. Please adhere to the following guidelines:

- **Follow existing formatting styles**: Match the indentation, naming conventions, and code structure used in the surrounding code
- **Use meaningful variable and function names**: Prefer clarity over brevity
- **Add comments where necessary**: Explain complex logic, but let the code speak for itself when possible
- **Keep functions focused**: Each function should do one thing well
- **Avoid unnecessary dependencies**: Minimize external library requirements

### Commit Guidelines

- **Write atomic commits**: Each commit should represent a single logical change
- **Use clear commit messages**: Follow the format:
  ```
  <type>: <short description>

  <optional detailed description>
  ```
  Where type is one of: `feat`, `fix`, `docs`, `style`, `refactor`, `test`, `chore`
- **Reference issues**: Include `Fixes #123` or `Closes #456` in commit messages when applicable
- **Squash appropriately**: If your branch has multiple small fixes, consider squashing them before submission

### Pull Request Process

1. Ensure your code compiles without warnings
2. Run existing tests and add new tests for your changes
3. Update documentation as needed
4. Submit your pull request with:
   - A clear title and description
   - Reference to any related issues
   - Description of testing performed
5. Respond to reviewer feedback promptly
6. Be patient—reviews may take time depending on maintainer availability

## Questions?

If you have questions about contributing, feel free to open an issue or join our community discussions. We're happy to help!

Thank you for contributing to Torque2D!
