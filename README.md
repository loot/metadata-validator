LOOT Metadata Validator
=======================

[![AppVeyor Build Status](https://ci.appveyor.com/api/projects/status/github/loot/metadata-validator?branch=master&svg=true)](https://ci.appveyor.com/project/WrinklyNinja/metadata-validator)
[![Travis Build Status](https://travis-ci.org/loot/metadata-validator.svg?branch=master)](https://travis-ci.org/loot/metadata-validator)

This is a very simple validator for LOOT metadata files that tries to load a
given metadata file using the LOOT API, and prints the message for any exception
thrown.

## Downloads

Releases are hosted on [GitHub](https://github.com/loot/metadata-validator/releases), and snapshot builds are available on [Bintray](https://bintray.com/wrinklyninja/loot/metadata-validator). The snapshot build archives are named like so:

```
metadata-validator-<last tag>-<revisions since tag>-g<short revision ID>_<branch>-<platform>.7z
```

## Usage

```
metadata-validator ./masterlist.yaml
```

## Building From Source

Refer to `appveyor.yml` and `.travis.yml` for the build processes on Windows and
Linux respectively.

Note that there is a dependency on the experimental standard filesystem library,
which may complicate compiler support. This dependency will be replaced by the
non-experimental library after MSVC 2017 is released.
