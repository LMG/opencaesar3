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

#ifndef __OPENCAESAR3_SERVICEWALKER_H_INCLUDED__
#define __OPENCAESAR3_SERVICEWALKER_H_INCLUDED__

#include "oc3_walker.hpp"

/** This walker gives a service to buildings along the road */
class ServiceWalker : public Walker
{
public:
  typedef std::set<BuildingPtr> ReachedBuildings;

  static ServiceWalkerPtr create( BuildingPtr base, const ServiceType service );

  ServiceType getService();
  BuildingPtr getBase();

  void send2City();
  virtual float getServiceValue() const;
  virtual void onDestination();
  virtual void onNewTile();  // called when the walker is on a new tile

  // evaluates the service demand on the given pathWay
  float evaluatePath(PathWay &pathWay);
  void reservePath(PathWay &pathWay);
  ReachedBuildings getReachedBuildings(const TilePos& pos );

  virtual unsigned int getReachDistance() const;

  void setMaxDistance( const int distance );

  void save( VariantMap& stream) const;
  void load( const VariantMap& stream);

protected:
  ServiceWalker( BuildingPtr base, const ServiceType service );
  void init(const ServiceType service);
  void computeWalkerPath();

private:
  ServiceType _service;
  BuildingPtr _base;
  int _maxDistance;
};

#endif