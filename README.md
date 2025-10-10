LOOT Metadata Validator
=======================

![CI](https://github.com/loot/metadata-validator/actions/workflows/ci.yml/badge.svg?branch=master&event=push)

This is a very simple validator for LOOT metadata files that tries to load a
given metadata file using libloot, and prints the message for any exception
thrown.

## Downloads

Releases are hosted on [GitHub](https://github.com/loot/metadata-validator/releases).

Snapshot builds are available as artifacts from [GitHub Actions runs](https://github.com/loot/metadata-validator/actions), though they are only kept for 90 days and can only be downloaded when logged into a GitHub account. The snapshot build archives are named like so:

```
metadata-validator-<last tag>-<revisions since tag>-g<short revision ID>_<branch>-<platform>.7z
```

## Usage

```
metadata_validator ./masterlist.yaml
```

## Building From Source

Refer to `.github/workflows/ci.yml` for the build process.
