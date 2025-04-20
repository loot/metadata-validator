#!/usr/bin/env python
# -*- coding: utf-8 -*-

import argparse
import os
import re
import shutil
import subprocess

INVALID_FILENAME_CHARACTERS = re.compile('[/<>"|]')

def get_git_description(given_branch):
    result = subprocess.run(
        ['git', 'describe', '--tags', '--long', '--abbrev=7'],
        capture_output=True,
        text=True,
        check=True
    )

    describe = result.stdout[:-1]

    return describe + '_' + given_branch

def get_archive_name_suffix():
    if os.name == 'nt':
        return 'win64'

    return 'Linux'

def get_archive_file_extension():
    if os.name == 'nt':
        return '.7z'

    return '.tar.xz'

def replace_invalid_filename_characters(filename):
    return INVALID_FILENAME_CHARACTERS.sub('-', filename)

def compress(source_path, destination_path):
    # Ensure that the output directory is empty.
    if os.path.exists(destination_path):
        os.remove(destination_path)

    filename = os.path.basename(destination_path)
    root_folder = os.path.basename(source_path)
    working_directory = os.path.dirname(source_path)

    if os.name == 'nt':
        seven_zip_path = 'C:\\Program Files\\7-Zip\\7z.exe'
        if not os.path.exists(seven_zip_path):
            seven_zip_path = '7z'

        subprocess.run(
            [seven_zip_path, 'a', '-r', filename, root_folder],
            cwd=working_directory,
            check=True
        )
    else:
        subprocess.run(
            ['tar', '-cJf', filename, root_folder],
            cwd=working_directory,
            check=True
        )

def prepare_archive(root_path, executable_name, temp_path):
    shutil.copy2(
        os.path.join(root_path, 'target', 'release', executable_name),
        temp_path
    )
    shutil.copy2(
        os.path.join(root_path, 'README.md'),
        temp_path
    )

def create_archive(root_path, temp_path, destination_path):
    # Ensure that the output directory is empty.
    if os.path.exists(temp_path):
        shutil.rmtree(temp_path)

    os.makedirs(temp_path)

    if os.name == 'nt':
        prepare_archive(root_path, 'metadata-validator.exe', temp_path)
    else:
        prepare_archive(root_path, 'metadata-validator', temp_path)

    compress(temp_path, destination_path)

    shutil.rmtree(temp_path)

    print(destination_path)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description = 'Create an archive artifact')
    parser.add_argument('root-path', nargs=1)
    parser.add_argument('given-branch', nargs=1)

    arguments = vars(parser.parse_args())

    given_branch = arguments['given-branch'][0]
    root_path = arguments['root-path'][0]

    git_description = get_git_description(given_branch)
    file_extension = get_archive_file_extension()
    filename = f'metadata-validator-{replace_invalid_filename_characters(git_description)}-{get_archive_name_suffix()}'

    create_archive(
        root_path,
        os.path.join(root_path, 'target', 'package', filename),
        os.path.join(root_path, 'target', 'package', filename + file_extension)
    )
