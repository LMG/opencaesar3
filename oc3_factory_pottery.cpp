// This file is part of openCaesar3.
//
// openCaesar3 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// openCaesar3 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with openCaesar3.  If not, see <http://www.gnu.org/licenses/>.

#include "oc3_factory_pottery.hpp"
#include "oc3_picture.hpp"
#include "oc3_resourcegroup.hpp"

FactoryPottery::FactoryPottery() : Factory(G_CLAY, G_POTTERY, B_POTTERY, Size(2))
{
  _picture = &Picture::load(ResourceGroup::commerce, 132);

  _animation.load(ResourceGroup::commerce, 133, 7);
  _animation.setFrameDelay( 3 );
  _fgPictures.resize(2);

  setMaxWorkers( 10 );
  setWorkers( 0 );
}