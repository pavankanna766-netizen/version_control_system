# MyGit

it is a rudimentary Git client implemented in C++17 to understand the core
internals behind Git.

built as part of the Systems and Security SIG
Recruitment Task - Version Control Systems.

The goal is not to recreate the complete Git implementation, but to
understand and implement the fundamental concepts behind a distributed
version control system such as:

- Content-addressed storage
- SHA-256 hashing
- Blob objects
- Tree objects
- Commits
- Staging area / index
- Branches
- HEAD
- Checkout
- Commit history

---

## Features

### Implemented

- `init`
- `add <file>`
- `commit -m "<message>"`
- `log`
- `status`
- `branch <branch-name>`
- `checkout <branch-name>`

### Additional safety

Checkout detects modified tracked files and refuses to overwrite
uncommitted changes.

For example:

```text
Your local changes would be overwritten by checkout.
