# MyGit — A Custom Version Control System

A rudimentary Git client implemented in **C++17** to understand the core internals behind Git, using **OpenSSL** for SHA-256 hashing.

Built as part of the **Systems and Security SIG Recruitment Task** — Version Control Systems.

## Features

- `init` — Initialize a new repository
- `add <file>` — Stage a file for commit
- `commit -m "message"` — Create a commit with a message
- `log` — View commit history
- `status` — Show modified, untracked, and deleted files
- `branch <name>` — Create a new branch
- `checkout <name>` — Switch to another branch

## Architecture

```
Working Directory
       |
     Index
       |
     Tree
       |
    Commit
       |
    Branch
       |
      HEAD
```

## Object Model

- **Blob** → file contents
- **Tree** → filename → blob hash
- **Commit** → tree + parent + metadata

---

## Prerequisites

This project depends on **OpenSSL** (specifically `libcrypto`) for SHA-256 hashing. You must install OpenSSL development libraries before building.

### Windows (using MSYS2 / MinGW)

1. Install [MSYS2](https://www.msys2.org/)
2. Open the **MSYS2 MinGW 64-bit** terminal and run:

```bash
pacman -S mingw-w64-x86_64-openssl
```

3. Make sure `g++` is available:

```bash
pacman -S mingw-w64-x86_64-gcc
```

### Windows (using vcpkg)

```powershell
vcpkg install openssl:x64-windows
```

### Linux (Debian/Ubuntu)

```bash
sudo apt update
sudo apt install libssl-dev g++ make
```

### Linux (Fedora/RHEL)

```bash
sudo dnf install openssl-devel gcc-c++ make
```

### Linux (Arch)

```bash
sudo pacman -S openssl gcc make
```

### macOS

```bash
brew install openssl
```

> **Note (macOS):** Homebrew installs OpenSSL in a non-default location. You may need to pass the include and library paths when compiling:
> ```bash
> export CPPFLAGS="-I$(brew --prefix openssl)/include"
> export LDFLAGS="-L$(brew --prefix openssl)/lib"
> ```

---

## Building

```bash
g++ -std=c++17 main.cpp -lssl -lcrypto -o mygit
```

If OpenSSL is installed in a non-standard location, specify the paths:

```bash
g++ -std=c++17 main.cpp -I/path/to/openssl/include -L/path/to/openssl/lib -lssl -lcrypto -o mygit
```

### Example (MSYS2 MinGW on Windows)

```bash
g++ -std=c++17 main.cpp -lssl -lcrypto -o mygit.exe
```

---

## Usage

```bash
mygit init                      # Initialize a new repository
mygit add <file>                # Stage a file
mygit commit -m "message"       # Commit staged files
mygit log                       # View commit history
mygit status                    # Show file statuses
mygit branch <name>             # Create a new branch
mygit checkout <name>           # Switch to a branch
```

## Checkout Safety

Checkout refuses to overwrite modified tracked files.

---

## Project Structure

```
mygit/
├── main.cpp          # All source code (single-file implementation)
├── .mygit/           # Internal VCS data (created after `mygit init`)
│   ├── HEAD          # Points to the current branch
│   ├── index         # Staging area
│   ├── objects/      # Blob, tree, and commit objects (SHA-256 hashed)
│   └── refs/
│       └── heads/    # Branch references
├── .gitignore        # Ignore build artifacts
└── README.md         # This file
```

---

## How It Works

1. **Hashing:** Every file's content is hashed using SHA-256 (via OpenSSL) to produce a unique identifier.
2. **Objects:** File contents (blobs), directory listings (trees), and commit metadata are stored as objects in `.mygit/objects/`.
3. **Staging:** `mygit add` computes the hash and records the file→hash mapping in `.mygit/index`.
4. **Commits:** `mygit commit` creates a tree object from the index and a commit object referencing that tree (plus parent commit, author, and message).
5. **Branches:** Branch refs in `.mygit/refs/heads/` store the hash of the latest commit on that branch.
6. **Checkout:** Restores the working directory to match the tree of the target branch's latest commit.

---

## Dependencies Summary

| Dependency | Purpose | Required |
|---|---|---|
| **OpenSSL** (`libcrypto`) | SHA-256 hashing | ✅ Yes |
| **C++17 compiler** (g++, clang++, MSVC) | Building the project | ✅ Yes |
| **C++ Standard Library** (`<filesystem>`, `<fstream>`, etc.) | File I/O, directory operations | ✅ Yes (bundled with compiler) |

---

## Limitations

This is a rudimentary educational implementation and does not implement the complete Git object format, merge, remote operations, conflict resolution, etc.

---

## Demo

Video link: https://drive.google.com/file/d/1S46GznnR2q53uZmco1wJ5AjcV2K-it1J/view?usp=drive_link

---

## Author

**Pavan** — [GitHub](https://github.com/pavankanna766-netizen)
