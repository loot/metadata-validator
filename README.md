LOOT Metadata Validator
=======================

This is a very simple validator for LOOT metadata files that tries to load a
given metadata file using the LOOT API, and prints the message for any exception
thrown.

The validator is cross-platform, but uses the experimental filesystem library
to provide cross-platform filesystem operations without adding a dependency on
Boost.Filesystem, which may complicate compiler support. It has been tested on
MSVC 2015.

Usage:

```
metadata-validator ./masterlist.yaml
```
