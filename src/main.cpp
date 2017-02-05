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
#include <fstream>
#include <iostream>
#include <experimental/filesystem>

#include "loot/api.h"

namespace fs = std::experimental::filesystem;

fs::path mockGameInstall() {
  auto dataPath = fs::temp_directory_path() / "Oblivion" / "Data";
  fs::create_directories(dataPath);

  std::ofstream dummyMaster(dataPath / "Oblivion.esm");
  dummyMaster.close();

  return dataPath.parent_path();
}

int main(int argc, char **argv) {
  using std::cout;
  using std::endl;

  // Print help text if -h, --help or invalid args are given (including no args).
  if (argc != 2 || (strcmp(argv[1], "-h") == 0) || (strcmp(argv[1], "--help") == 0)) {
    cout << endl
      << "Usage: metadata-validator <arg>" << endl << endl
      << "Arguments:" << endl << endl
      << "  " << "<file>" << "         " << "The metadata file to validate." << endl
      << "  " << "-v, --version" << "  " << "Prints version information for this utility." << endl
      << "  " << "-h, --help" << "     " << "Prints this help text." << endl << endl;
    return 1;
  }

  // Print version info if -v or --version are given.
  if ((strcmp(argv[1], "-v") == 0) || (strcmp(argv[1], "--version") == 0)) {
    cout << endl << "LOOT Metadata Validator" << endl
      << "Using LOOT API v" << loot::LootVersion::major << "." << loot::LootVersion::minor
      << "." << loot::LootVersion::patch << ", build revision " << loot::LootVersion::revision
      << endl << endl;
    return 0;
  }

  // Create a dummy game install.
  auto gamePath = mockGameInstall();
  try {
    cout << endl << "Validating metadata file: " << argv[1] << endl << endl;

    // Create a handle for any game at any path, it doesn't matter.
    auto handle = loot::CreateDatabase(loot::GameType::tes4, gamePath.string(), gamePath.string());
    handle->LoadLists(argv[1]);
  } catch (std::exception& e) {
    cout << "ERROR: " << e.what() << endl << endl;

    fs::remove_all(gamePath);
    return 1;
  }

  fs::remove_all(gamePath);
  cout << "SUCCESS!" << endl << endl;

  return 0;
}
