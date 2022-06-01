# An Implementation of Priority Queues using Binary Heaps in C

## Disclaimer

Only supported in Linux. Untested on Darwin (MacOS). Use WSL with Windows.

## Dependencies

Dependencies are `CUnit` for unit tests and `valgrind` for memory leak checks. Compilation tested with `gcc`. Install with apt if not installed:

```bash
sudo apt install CUnit valgrind gcc
```

## Tests

Run tests with:

```bash
make build_test
```

Delete built binaries by the tester in case of error with:

```bash
make clean
```

Run tests for memory leaks:

```bash
make memcheck
```