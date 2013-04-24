#ifndef __OPENCAESAR3_ASTARPOINT_H_INCLUDED__
#define __OPENCAESAR3_ASTARPOINT_H_INCLUDED__

#include "oc3_positioni.hpp"

class AStarPoint
{
public:
  AStarPoint* parent;
  bool closed;
  bool opened;
  bool walkable;
  bool isRoad;
  TilePos pos;
  int f, g, h;

  AStarPoint()
  {
    parent = NULL;
    closed = false;
    opened = false;
    walkable = false;
    pos = TilePos( 0, 0 );
    isRoad = false;

    f = g = h = 0;
  }

  AStarPoint( const TilePos& p, bool w, bool r)
  {    
    parent = NULL;
    closed = false;
    opened = false;
    isRoad = r;

    f = g = h = 0;
    walkable = w;
    pos = p;
  }

  TilePos getPosition()
  {
    return pos;
  }

  AStarPoint* getParent()
  {
    return parent;
  }

  void setParent(AStarPoint* p)
  {
    parent = p;
  }

  int getGScore(AStarPoint* p)
  { 
    int offset = p->isRoad ? -5 : +10;
    return p->g + ((pos.getI() == p->pos.getI() || pos.getJ() == p->pos.getJ()) ? 10 : 14) + offset;
  }

  int getHScore(AStarPoint* p)
  {
    return (abs(p->pos.getI() - pos.getI()) + abs(p->pos.getJ() - pos.getJ())) * 10;
  }

  int getGScore()
  {
    return g;
  }

  int getHScore()
  {
    return h;
  }

  int getFScore()
  {
    return f;
  }

  void computeScores(AStarPoint* end)
  {
    g = getGScore(parent);
    h = getHScore(end);
    f = g + h;
  }

  bool hasParent()
  {
    return parent != NULL;
  }
};

#endif //__OPENCAESAR3_ASTARPOINT_H_INCLUDED__