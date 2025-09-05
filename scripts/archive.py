#!/usr/bin/env python
# -*- coding: utf-8 -*-

import argparse
import os
import re
import shutil
import subprocess

INVALID_FILENAME_CHARACTERS = re.compile('[/<>"|]')

def get_archive_name_suffix(triple):
    if os.name == 'nt':
        # Assume 64-bit Windows if no triple is given, it's not worth checking.
        return 'win32' if triple and 'x86_64' not in triple else 'win64'

    return 'Linux'

def replace_invalid_filename_characters(filename):
    return INVALID_FILENAME_CHARACTERS.sub('-', filename)

def compress(source_path, destination_path):
    # Ensure that the output directory is empty.
    if os.path.exists(destination_path):
        os.remove(destination_path)

    filename = os.path.basename(destination_path)
    root_folder = os.path.basename(source_path)

    if os.name == 'nt':
        seven_zip_path = 'C:\\Program Files\\7-Zip\\7z.exe'
        if not os.path.exists(seven_zip_path):
            seven_zip_path = '7z'

        args = [seven_zip_path, 'a', '-r', filename, root_folder]
    else:
        args = ['tar', '-cJf', filename, root_folder]

    subprocess.run(
        args,
        cwd=os.path.dirname(source_path),
        check=True
    )

def prepare_archive(root_path, triple, executable_name, temp_path):
    if triple:
        executable_path = os.path.join(root_path, 'target', triple, 'release', executable_name)
    else:
        executable_path = os.path.join(root_path, 'target', 'release', executable_name)

    shutil.copy2(executable_path, temp_path)
    shutil.copy2(os.path.join(root_path, 'README.md'), temp_path)

def create_archive(root_path, triple, temp_path, destination_path):
    # Ensure that the output directory is empty.
    if os.path.exists(temp_path):
        shutil.rmtree(temp_path)

    os.makedirs(temp_path)

    executable_name = 'metadata_validator.exe' if os.name == 'nt' else 'metadata_validator'

    prepare_archive(root_path, triple, executable_name, temp_path)

    compress(temp_path, destination_path)

    shutil.rmtree(temp_path)

    print(destination_path)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description = 'Create an archive artifact')
    parser.add_argument('-r', '--root-path', required=True)
    parser.add_argument('-v', '--version', required=True)
    parser.add_argument('-t', '--triple')

    arguments = vars(parser.parse_args())

    root_path = arguments['root_path']
    version = replace_invalid_filename_characters(arguments['version'])
    triple = arguments['triple']

    file_extension = '.7z' if os.name == 'nt' else '.tar.xz'
    filename = f'metadata-validator-{version}-{get_archive_name_suffix(triple)}'

    create_archive(
        root_path,
        triple,
        os.path.join(root_path, 'target', 'package', filename),
        os.path.join(root_path, 'target', 'package', filename + file_extension)
    )
