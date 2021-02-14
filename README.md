LOOT Metadata Validator
=======================

![CI](https://github.com/loot/metadata-validator/workflows/CI/badge.svg?branch=master&event=push)

This is a very simple validator for LOOT metadata files that tries to load a
given metadata file using libloot, and prints the message for any exception
thrown.

## Downloads

Releases are hosted on [GitHub](https://github.com/loot/metadata-validator/releases), and snapshot builds are available on [Bintray](https://bintray.com/loot/snapshots/metadata-validator). The snapshot build archives are named like so:

```
metadata-validator-<last tag>-<revisions since tag>-g<short revision ID>_<branch>-<platform>.7z
```

## Usage

```
metadata-validator ./masterlist.yaml
```

## Building From Source

Refer to `.github/workflows/ci.yml` for the build process.
