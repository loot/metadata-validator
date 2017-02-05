/*  LOOT

    A load order optimisation tool for Oblivion, Skyrim, Fallout 3 and
    Fallout: New Vegas.

    Copyright (C) 2017    WrinklyNinja

    This file is part of the LOOT metadata validator.

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

#ifndef LOOT_METADATA_VALIDATOR_VERSION
#define LOOT_METADATA_VALIDATOR_VERSION

#include <string>

namespace loot {
namespace validator {
class Version {
public:
  static const unsigned int major;
  static const unsigned int minor;
  static const unsigned int patch;
  static const std::string revision;

  static std::string string();
};
}
}

#endif
