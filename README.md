# MyGit

it is a rudimentary Git client implemented in C++17 to understand the core
internals behind Git.

built as part of the Systems and Security SIG
Recruitment Task - Version Control Systems.

## Features
- init
- add <file>
- commit -m
- log
- status
- branch
- checkout

## Architecture
Working Directory
       ↓
     Index
       ↓
     Tree
       ↓
    Commit
       ↓
    Branch
       ↑
      HEAD

## Object Model

Blob  → file contents
Tree  → filename → blob hash
Commit → tree + parent + metadata

## Build

g++ -std=c++17 main.cpp -lssl -lcrypto -o mygit.exe

## Usage

mygit init
mygit add <file>
mygit commit -m "message"
mygit log
mygit status
mygit branch <name>
mygit checkout <name>

## Checkout Safety

Checkout refuses to overwrite modified tracked files.

## Limitations

This is a rudimentary educational implementation and does not
implement the complete Git object format, merge, remote operations,
conflict resolution, etc.
