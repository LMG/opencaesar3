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

#include "oc3_farm.hpp"
#include "oc3_positioni.hpp"
#include "oc3_exception.hpp"
#include "oc3_resourcegroup.hpp"
#include "oc3_scenario.hpp"
#include "oc3_tile.hpp"

FarmTile::FarmTile(const GoodType outGood, const TilePos& pos )
{
  _i = pos.getI();
  _j = pos.getJ();

  int picIdx = 0;
  switch (outGood)
  {
  case G_WHEAT:
    picIdx = 13;
    break;
  case G_VEGETABLE:
    picIdx = 18;
    break;
  case G_FRUIT:
    picIdx = 23;
    break;
  case G_OLIVE:
    picIdx = 28;
    break;
  case G_GRAPE:
    picIdx = 33;
    break;
  case G_MEAT:
    picIdx = 38;
    break;
  default:
    THROW("Unexpected farmType in farm:" << outGood);
  }

  _animation.load( ResourceGroup::commerce, picIdx, 5);
  computePicture(0);
}

void FarmTile::computePicture(const int percent)
{
  Animation::Pictures& pictures = _animation.getPictures();

  int picIdx = (percent * (pictures.size()-1)) / 100;
  _picture = *pictures[picIdx];
  _picture.add_offset(30*(_i+_j), 15*(_j-_i));
}

Picture& FarmTile::getPicture()
{
  return _picture;
}


Farm::Farm(const GoodType outGood, const BuildingType type ) 
: Factory(G_NONE, outGood, type, Size(3) )
{
  _picture = &_pictureBuilding;

  _pictureBuilding = Picture::load( ResourceGroup::commerce, 12);  // farm building
  _pictureBuilding.add_offset(30, 15);

  init();
}

bool Farm::canBuild(const TilePos& pos ) const
{
  bool is_constructible = Construction::canBuild( pos );
  bool on_meadow = false;

  Tilemap& tilemap = Scenario::instance().getCity().getTilemap();
  std::list<Tile*> rect = tilemap.getFilledRectangle( pos, Size( _size ) );
  for (std::list<Tile*>::iterator itTiles = rect.begin(); itTiles != rect.end(); ++itTiles)
  {
    on_meadow |= (*itTiles)->get_terrain().isMeadow();
  }

  return (is_constructible && on_meadow);  
}


void Farm::init()
{
  GoodType farmType = _outGoodType;
  // add subTiles in draw order
  _subTiles.push_back(FarmTile(farmType, TilePos( 0, 0 ) ));
  _subTiles.push_back(FarmTile(farmType, TilePos( 2, 2 ) ));
  _subTiles.push_back(FarmTile(farmType, TilePos( 1, 0 ) ));
  _subTiles.push_back(FarmTile(farmType, TilePos( 2, 1 ) ));
  _subTiles.push_back(FarmTile(farmType, TilePos( 2, 0 ) ));

  _fgPictures.resize(5);
  for (int n = 0; n<5; ++n)
  {
    _fgPictures[n] = &_subTiles[n].getPicture();
  }
}

void Farm::computePictures()
{
  int amount = getProgress();
  int percentTile;

  for (int n = 0; n<5; ++n)
  {
    if (amount >= 20)   // 20 = 100 / nbSubTiles
    {
      // this subtile is at maximum
      percentTile = 100;  // 100%
      amount -= 20;  // for next subTiles
    }
    else
    {
      // this subtile is not at maximum
      percentTile = 5 * amount;
      amount = 0;  // for next subTiles
    }
    _subTiles[n].computePicture(percentTile);
  }
}


void Farm::timeStep(const unsigned long time)
{
  Factory::timeStep(time);

  computePictures();
}


FarmWheat::FarmWheat() : Farm(G_WHEAT, B_WHEAT)
{
}

FarmOlive::FarmOlive() : Farm(G_OLIVE, B_OLIVE)
{
}

FarmGrape::FarmGrape() : Farm(G_GRAPE, B_GRAPE)
{
}

FarmMeat::FarmMeat() : Farm(G_MEAT, B_MEAT)
{
}

FarmFruit::FarmFruit() : Farm(G_FRUIT, B_FRUIT)
{
}

FarmVegetable::FarmVegetable() : Farm(G_VEGETABLE, B_VEGETABLE)
{
}
