/*  LOOT

    A load order optimisation tool for Oblivion, Skyrim, Fallout 3 and
    Fallout: New Vegas.

    Copyright (C) 2014-2016    WrinklyNinja

    This file is part of LOOT.

    LOOT is free software: you can redistribute
    it and/or modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation, either version 3 of
    the License, or (at your option) any later version.

    LOOT is distributed in the hope that it will
    be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with LOOT.  If not, see
    <https://www.gnu.org/licenses/>.
    */

#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>

#include "loot/api.h"
#include "version.h"

namespace fs = std::filesystem;

fs::path mockGameInstall() {
  auto dataPath = fs::temp_directory_path() / "Oblivion" / "Data";
  fs::create_directories(dataPath);

  std::ofstream dummyMaster(dataPath / "Oblivion.esm");
  dummyMaster.close();

  return dataPath.parent_path();
}

int main(int argc, char** argv) {
  using loot::LootVersion;
  using loot::validator::Version;
  using std::cout;
  using std::endl;

  // Print help text if -h, --help or invalid args are given (including no
  // args).
  if (argc < 2 || argc > 3 || (strcmp(argv[1], "-h") == 0) ||
      (strcmp(argv[1], "--help") == 0)) {
    cout << endl
         << "Usage: metadata-validator <metadata file path> [<prelude file "
            "path>]"
         << endl
         << endl
         << "Arguments:" << endl
         << endl
         << "  "
         << "<metadata file path>"
         << " "
         << "The metadata file to validate." << endl
         << "  "
         << "<prelude file path>"
         << "  "
         << "The prelude metadata file to substitute in before validation "
            "(optional)."
         << endl
         << "  "
         << "-v, --version"
         << "        "
         << "Prints version information for this utility." << endl
         << "  "
         << "-h, --help"
         << "           "
         << "Prints this help text." << endl
         << endl;
    return 1;
  }

  // Print version info if -v or --version are given.
  if ((strcmp(argv[1], "-v") == 0) || (strcmp(argv[1], "--version") == 0)) {
    cout << endl
         << "LOOT Metadata Validator" << endl
         << "v" << Version::string() << ", build revision " << Version::revision
         << endl
         << "Using libloot v" << LootVersion::GetVersionString()
         << ", build revision " << LootVersion::revision << endl
         << endl;
    return 0;
  }

  // Create a dummy game install.
  auto gamePath = mockGameInstall();
  try {
    cout << endl << "Validating metadata file: " << argv[1] << endl;
    if (argc == 3) {
      cout << "Using prelude file: " << argv[2] << endl;
    }
    cout << endl;

    // Create a handle for any game at any path, it doesn't matter.
    auto handle = loot::CreateGameHandle(
        loot::GameType::tes4, gamePath.string(), gamePath.string());

    if (argc == 2) {
      handle->GetDatabase()->LoadLists(argv[1]);
    } else if (argc == 3) {
      handle->GetDatabase()->LoadLists(argv[1], "", argv[2]);
    }
  } catch (const std::exception& e) {
    cout << "ERROR: " << e.what() << endl << endl;

    fs::remove_all(gamePath);
    return 1;
  }

  fs::remove_all(gamePath);
  cout << "SUCCESS!" << endl << endl;

  return 0;
}
