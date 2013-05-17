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


#include "oc3_menu.hpp"
#include "oc3_pushbutton.hpp"
#include "oc3_picture.hpp"
#include "oc3_sdl_facade.hpp"
#include "oc3_pic_loader.hpp"
#include "oc3_resourcegroup.hpp"
#include "oc3_event.hpp"
#include "oc3_buildmenu.hpp"
#include "oc3_guienv.hpp"
#include "oc3_gui_paneling.hpp"
#include "oc3_widgetpositionanimator.hpp"
#include "oc3_label.hpp"
#include "oc3_gettext.hpp"
#include "oc3_scenario.hpp"
#include "oc3_minimap_colours.hpp"
#include "oc3_gui_tilemap.hpp"
#include "oc3_tile.hpp"

static const int REMOVE_TOOL_ID = B_MAX + 1; 
static const int MAXIMIZE_ID = REMOVE_TOOL_ID + 1;

class Menu::Impl
{
public:
  Picture* bgPicture;

  Widget* lastPressed;
  PushButton* menuButton;
  PushButton* minimizeButton;
  PushButton* senateButton;
  PushButton* empireButton;
  PushButton* missionButton;
  PushButton* northButton;
  PushButton* rotateLeftButton;
  PushButton* rotateRightButton;
  PushButton* messageButton;
  PushButton* disasterButton;
  PushButton* houseButton;
  PushButton* waterButton;
  PushButton* clearButton;
  PushButton* roadButton;
  PushButton* administrationButton;
  PushButton* entertainmentButton;
  PushButton* educationButton;
  PushButton* templeButton;
  PushButton* commerceButton;
  PushButton* securityButton;
  PushButton* healthButton;
  PushButton* engineerButton;
  PushButton* cancelButton;
  PushButton* overlaysButton;
  Label* middleLabel;

oc3_signals public:
  Signal1< int > onCreateConstructionSignal;
  Signal0<> onRemoveToolSignal;
  Signal0<> onMaximizeSignal;
};

Signal1< int >& Menu::onCreateConstruction()
{
  return _d->onCreateConstructionSignal;
}

Signal0<>& Menu::onRemoveTool()
{
  return _d->onRemoveToolSignal;
}

class MenuButton : public PushButton
{
public:
  MenuButton( Widget* parent, const Rect& rectangle, const std::string& caption, int id, int midIconId )
    : PushButton( parent, rectangle, caption, id )
  {
    _midIconId = midIconId;
  }

  int getMidPicId() const { return _midIconId; }
  void setMidPicId( int id ) { _midIconId = id; }
private:
  int _midIconId;
};

Menu::Menu( Widget* parent, int id, const Rect& rectangle ) : Widget( parent, id, rectangle ), _d( new Impl )
{
    _d->lastPressed = 0;

    const bool haveSubMenu = true;
    _d->minimizeButton = _addButton( ResourceMenu::maximizeBtnPicId, false, 0, MAXIMIZE_ID, !haveSubMenu, ResourceMenu::emptyMidPicId );
    _d->minimizeButton->setGeometry( Rect( Point( 6, 4 ), Size( 31, 20 ) ) );

    _d->houseButton = _addButton( ResourceMenu::houseBtnPicId, true, 0, B_HOUSE, 
                                  !haveSubMenu, ResourceMenu::houseMidPicId, _("##houseBtnTooltip") );
    
    _d->clearButton = _addButton( 131, true, 1, REMOVE_TOOL_ID, 
                                  !haveSubMenu, ResourceMenu::clearMidPicId, _("##clearBtnTooltip") );
    
    _d->roadButton = _addButton( 135, true, 2, B_ROAD, !haveSubMenu, ResourceMenu::roadMidPicId, _("##roadBtnTooltip") );
    _d->waterButton = _addButton( 127, true, 3, BM_WATER, haveSubMenu, ResourceMenu::waterMidPicId, _("##waterBtnTooltip") );
    _d->healthButton = _addButton( 163, true, 4, BM_HEALTH, haveSubMenu, ResourceMenu::healthMidPicId, _("##healthBtnTooltip") );
    _d->templeButton = _addButton( 151, true, 5, BM_RELIGION, haveSubMenu, ResourceMenu::religionMidPicId, _("##templeBtnTooltip") );
    _d->educationButton = _addButton( 147, true, 6, BM_EDUCATION, haveSubMenu, ResourceMenu::educationMidPicId, _("##educationBtnTooltip") );
    
    _d->entertainmentButton = _addButton( 143, true, 7, BM_ENTERTAINMENT, haveSubMenu, 
                                          ResourceMenu::entertainmentMidPicId, _("##entertainmentBtnTooltip") );
    
    _d->administrationButton = _addButton( 139, true, 8, BM_ADMINISTRATION, haveSubMenu, 
                                           ResourceMenu::administrationMidPicId, _("##administractionBtnTooltip") );
    
    _d->engineerButton = _addButton( 167, true, 9, BM_ENGINEERING, haveSubMenu, 
                                     ResourceMenu::engineerMidPicId, _("##engineerBtnTooltip") );
    
    _d->securityButton = _addButton( 159, true, 10, BM_SECURITY, haveSubMenu, 
                                     ResourceMenu::securityMidPicId, _("##securityBtnTooltip") );
    
    _d->commerceButton = _addButton( 155, true, 11, BM_COMMERCE, haveSubMenu, 
                                     ResourceMenu::comerceMidPicId, _("##comerceBtnTooltip") );
}

PushButton* Menu::_addButton( int startPic, bool pushBtn, int yMul, 
                             int id, bool haveSubmenu, int midPic, const std::string& tooltip )
{
    Point offset( 1, 32 );
    int dy = 35;

    MenuButton* ret = new MenuButton( this, Rect( 0, 0, 39, 26), "", -1, -1 );
    ret->setID( id | ( haveSubmenu ? BuildMenu::subMenuCreateIdHigh : 0 ) );
    GuiPaneling::configureTexturedButton( ret, ResourceGroup::panelBackground, startPic, pushBtn );
    ret->setPosition( offset + Point( 0, dy * yMul ) );
    ret->setTooltipText( tooltip );

    if( MenuButton* btn = safety_cast< MenuButton* >( ret ) )
    {
        btn->setMidPicId( midPic );
    }

    return ret;
}

/* here will be helper functions for minimap generation */

Caesar3Colours *colours;

void getBitmapCoordinates(int x, int y, int mapsize, int& x_out, int& y_out)
{
  y_out = x + mapsize - y - 1;
  x_out = x + y;
}

void getTerrainColours(TerrainTile& tile, int &c1, int &c2)
{
  if (tile.isTree())
  {
    c1 = colours->colour(Caesar3Colours::MAP_TREE1, 0);
    c2 = colours->colour(Caesar3Colours::MAP_TREE2, 0);
  }
  else if (tile.isRock())
  {
    c1 = colours->colour(Caesar3Colours::MAP_ROCK1, 0);
    c2 = colours->colour(Caesar3Colours::MAP_ROCK2, 0);
  }
  else if (tile.isWater())
  {
    c1 = colours->colour(Caesar3Colours::MAP_WATER1, 0);
    c2 = colours->colour(Caesar3Colours::MAP_WATER2, 0);
  }
  else if (tile.isRoad())
  {
    c1 = colours->colour(Caesar3Colours::MAP_ROAD, 0);
    c2 = colours->colour(Caesar3Colours::MAP_ROAD, 1);
  }
  else if (tile.isMeadow())
  {
    c1 = colours->colour(Caesar3Colours::MAP_FERTILE1, 0);
    c2 = colours->colour(Caesar3Colours::MAP_FERTILE2, 0);    
  }
//  else if (tile.isWall())
//  {
//    c1 = colours->colour(Caesar3Colours::MAP_WALL, 0);
//    c2 = colours->colour(Caesar3Colours::MAP_WALL, 1);   
//  }
  else // plain terrain
  {
    c1 = colours->colour(Caesar3Colours::MAP_EMPTY1, 0);
    c2 = colours->colour(Caesar3Colours::MAP_EMPTY2, 0);
  }
}

/* end of helper functions */

namespace {
  static const int kWhite  = 0xFFFFFF;
  static const int kYellow = 0xFFFF00;
}

void Menu::draw( GfxEngine& painter )
{
  if( !isVisible() )
    return;

  painter.drawPicture( *_d->bgPicture, getScreenLeft(), getScreenTop() );
    
  Widget::draw( painter );
}

bool Menu::onEvent(const NEvent& event)
{
    if( event.EventType == OC3_GUI_EVENT && event.GuiEvent.EventType == OC3_BUTTON_CLICKED )
    {
        if( !event.GuiEvent.Caller )
            return false;

        int id = event.GuiEvent.Caller->getID();
        switch( id )
        {
        case MAXIMIZE_ID:
            _d->lastPressed = 0;
            _createBuildMenu( -1, this );
            _d->onMaximizeSignal.emit();
        break;

        case B_HOUSE:
        case B_ROAD:
            _d->lastPressed = event.GuiEvent.Caller;
            _d->onCreateConstructionSignal.emit( id );
            _createBuildMenu( -1, this );
        break;

        case REMOVE_TOOL_ID:
            _d->lastPressed = event.GuiEvent.Caller;
            _d->onRemoveToolSignal.emit();
            _createBuildMenu( -1, this );
        break;
        
        default:
            if( _d->lastPressed != event.GuiEvent.Caller )
            {
                if( event.GuiEvent.Caller->getParent() == this )
                    _d->lastPressed = event.GuiEvent.Caller;
                
                if( PushButton* btn = safety_cast< PushButton* >( event.GuiEvent.Caller ) )
                {
                    int id = btn->getID();
                    if( id & BuildMenu::subMenuCreateIdHigh )
                    {
                        _createBuildMenu( id & 0xff, event.GuiEvent.Caller );        
                    }
                    else
                    {
                        _d->onCreateConstructionSignal.emit( id );
                        _createBuildMenu( -1, this );
                        setFocus();
                        //_d->lastPressed = 0;
                    }
                }
            }

        break;
        }

        unselectAll();
        if( PushButton* btn = safety_cast< PushButton* >( _d->lastPressed ) )
        {
            btn->setPressed( true && btn->isPushButton() );
        }
        return true;
    }

    if( event.EventType == OC3_GUI_EVENT && event.GuiEvent.EventType == OC3_ELEMENT_FOCUS_LOST )
    {
        unselectAll();
        _d->lastPressed = 0;
    }

    if( event.EventType == OC3_MOUSE_EVENT )
    {
        switch( event.MouseEvent.Event )
        {
        case OC3_RMOUSE_LEFT_UP:
            _createBuildMenu( -1, this );
            unselectAll();
            _d->lastPressed = 0;
        return true;

        case OC3_LMOUSE_PRESSED_DOWN:
        case OC3_LMOUSE_LEFT_UP:
            {
                //lock movement for tilemap
                if( findChildren<BuildMenu*>().size() > 0 )
                    return true;
            }
        break;
        }
    }

    return Widget::onEvent( event );
}

Menu* Menu::create( Widget* parent, int id )
{
  Menu* ret = new Menu( parent, id, Rect( 0, 0, 1, 1 ) );

  SdlFacade &sdlFacade = SdlFacade::instance();

  const Picture& bground = PicLoader::instance().get_picture( ResourceGroup::panelBackground, 16 );
  const Picture& bottom  = PicLoader::instance().get_picture( ResourceGroup::panelBackground, 21 );

  ret->_d->bgPicture = &sdlFacade.createPicture( bground.get_width(), bground.get_height() + bottom.get_height() );
  sdlFacade.drawPicture( bground, *ret->_d->bgPicture , 0, 0);
  sdlFacade.drawPicture( bottom, *ret->_d->bgPicture , 0, bground.get_height() );

  ret->setGeometry( Rect( 0, 0, bground.get_width(), ret->_d->bgPicture->get_height() ) );

  return ret;
}

bool Menu::unselectAll()
{
    bool anyPressed = false;
    for( ConstChildIterator it=getChildren().begin(); it != getChildren().end(); it++ )
    {
        if( PushButton* btn = safety_cast< PushButton* >( *it ) )
        {
            anyPressed |= btn->isPressed();
            btn->setPressed( false );
        }
    }

    return anyPressed;
}

void Menu::_createBuildMenu( int type, Widget* parent )
{
    List< BuildMenu* > menus = findChildren<BuildMenu*>();
    for( List< BuildMenu* >::iterator it=menus.begin(); it != menus.end(); it++ )
        (*it)->deleteLater();

    BuildMenu* buildMenu = BuildMenu::getMenuInstance( (BuildMenuType)type, this );

    if( buildMenu != NULL )
    {
        buildMenu->setNotClipped( true );

        buildMenu->init();
       
        int y = math::clamp< int >( parent->getScreenTop() - getScreenTop(), 0, _environment->getRootWidget()->getHeight() - buildMenu->getHeight() );
        buildMenu->setPosition( Point( -(int)buildMenu->getWidth() - 5, y ) );
    }
}

Signal0<>& Menu::onMaximize()
{
    return _d->onMaximizeSignal;
}

ExtentMenu* ExtentMenu::create( Widget* parent, GuiTilemap& tmap, int id )
{
    ExtentMenu* ret = new ExtentMenu( parent, tmap, id, Rect( 0, 0, 1, 1 ) );

    SdlFacade &sdlFacade = SdlFacade::instance();

    const Picture& bground = PicLoader::instance().get_picture( ResourceGroup::panelBackground, 17 );
    const Picture& bottom = PicLoader::instance().get_picture( ResourceGroup::panelBackground, 20 );

    ret->_d->bgPicture = &sdlFacade.createPicture( bground.get_width(), bground.get_height() + bottom.get_height() );
    sdlFacade.drawPicture( bground, *ret->_d->bgPicture, 0, 0);
    sdlFacade.drawPicture( bottom, *ret->_d->bgPicture, 0, bground.get_height() );

    ret->setGeometry( Rect( 0, 0, bground.get_width(), ret->_d->bgPicture->get_height() ) );

    return ret;
}

void ExtentMenu::minimize()
{
    _d->lastPressed = 0;
    _createBuildMenu( -1, this );
    Point stopPos = getRelativeRect().UpperLeftCorner + Point( getWidth(), 0 );
    PositionAnimator* anim = new PositionAnimator( this, WidgetAnimator::removeSelf, 
                                                   stopPos, 300 );
}

void ExtentMenu::maximize()
{
    Point stopPos = getRelativeRect().UpperLeftCorner - Point( getWidth(), 0 );
    show();
    PositionAnimator* anim = new PositionAnimator( this, WidgetAnimator::showParent | WidgetAnimator::removeSelf, 
                                                   stopPos, 300 );
}

ExtentMenu::ExtentMenu( Widget* parent, GuiTilemap& tmap, int id, const Rect& rectangle )
    : Menu( parent, id, rectangle ), _tmap( tmap )
{
    GuiPaneling::configureTexturedButton( _d->minimizeButton, ResourceGroup::panelBackground, 97, false );
    _d->minimizeButton->setPosition( Point( 127, 5 ) );
    CONNECT( _d->minimizeButton, onClicked(), this, ExtentMenu::minimize );

    _d->houseButton->setPosition( Point( 13, 277 ) );
    _d->houseButton->setIsPushButton( false );
    _d->clearButton->setPosition( Point( 63, 277 ) );
    _d->clearButton->setIsPushButton( false );
    _d->roadButton->setPosition( Point( 113, 277 ) );
    _d->roadButton->setIsPushButton( false );

    _d->waterButton->setPosition( Point( 13, 313 ) );
    _d->healthButton->setPosition( Point( 63, 313 ) );
    _d->templeButton->setPosition( Point( 113, 313 ) );
    _d->educationButton->setPosition( Point(13, 349 ));
    _d->entertainmentButton->setPosition( Point(63, 349 ) );
    _d->administrationButton->setPosition( Point( 113, 349) );
    _d->engineerButton->setPosition( Point( 13, 385 ) );
    _d->securityButton->setPosition( Point( 63, 385 ) );
    _d->commerceButton->setPosition( Point( 113, 385) );

    // // header
    _d->senateButton = _addButton( 79, false, 0, -1, false, -1, _("##senateBtnTooltip") );
    _d->senateButton->setGeometry( Rect( Point( 7, 155 ), Size( 71, 23 ) ) );
    _d->empireButton = _addButton( 82, false, 0, -1, false, -1, _("##empireBtnTooltip") );
    _d->empireButton->setGeometry( Rect( Point( 84, 155 ), Size( 71, 23 ) ) );
   
    _d->missionButton = _addButton( 85, false, 0, -1, false, -1, _("##missionBtnTooltip") );
    _d->missionButton->setGeometry( Rect( Point( 7, 184 ), Size( 33, 22 ) ) );
    _d->northButton = _addButton( 88, false, 0, -1, false, -1, _("##northBtnTooltip") );
    _d->northButton->setGeometry( Rect( Point( 46, 184 ), Size( 33, 22 ) ) );
    _d->rotateLeftButton = _addButton( 91, false, 0, -1, false, -1, _("##rotateLeftBtnTooltip") );
    _d->rotateLeftButton->setGeometry( Rect( Point( 84, 184 ), Size( 33, 22 ) ) );
    _d->rotateRightButton = _addButton( 94, false, 0, -1, false, -1, _("##rotateRightBtnTooltip") );
    _d->rotateRightButton->setGeometry( Rect( Point( 123, 184 ), Size( 33, 22 ) ) );

    _d->cancelButton = _addButton( 171, false, 0, -1, false, -1, _("##cancelBtnTooltip") );
    _d->cancelButton->setGeometry( Rect( Point( 13, 421 ), Size( 39, 22 ) ) );
    _d->messageButton = _addButton( 115, false, 0, -1, false, -1, _("##messageBtnTooltip") );
    _d->messageButton->setGeometry( Rect( Point( 63, 421 ), Size( 39, 22 ) ) );
    _d->disasterButton = _addButton( 119, false, 0, -1, false, -1, _("##disasterBtnTooltip") );
    _d->disasterButton->setGeometry( Rect( Point( 113, 421 ), Size( 39, 22 ) ) );

    _d->middleLabel = new Label(this, Rect( Point( 7, 216 ), Size( 148, 52 )) );
    _d->middleLabel->setBackgroundPicture( PicLoader::instance().get_picture( ResourceGroup::menuMiddleIcons, ResourceMenu::emptyMidPicId ) );

    _d->overlaysButton = new PushButton( this, Rect( 6, 27, 127, 51), "Overlays" );

}

bool ExtentMenu::onEvent(const NEvent& event)
{
    if( event.EventType == OC3_GUI_EVENT && event.GuiEvent.EventType == OC3_BUTTON_CLICKED )
    {
        if( MenuButton* btn = safety_cast< MenuButton* >( event.GuiEvent.Caller ) )
        {
            int picId = btn->getMidPicId() > 0 ? btn->getMidPicId() : ResourceMenu::emptyMidPicId;
            _d->middleLabel->setBackgroundPicture( PicLoader::instance().get_picture( ResourceGroup::menuMiddleIcons, picId ) );
        }
    }

    return Menu::onEvent( event );
}

void ExtentMenu::draw( GfxEngine& painter )
{
  if( !isVisible() )
    return;

  Menu::draw( painter );
  // here we will draw minimap

  // TEMPORARY!!!

  // try to generate and show minimap
  // now we will show it at (0,0)
  // then we will show it in right place
  if (dynamic_cast<ExtentMenu*>(this) != NULL) // weird check if we have big menu
  {

    SdlFacade &sdlFacade = SdlFacade::instance();

    int mapsize = Scenario::instance().getCity().getTilemap().getSize();

    Picture& minimap = sdlFacade.createPicture(mapsize * 2 , mapsize * 2);
    SDL_Surface* surface = minimap.get_surface();

    sdlFacade.lockSurface(surface);
    // here we can draw anything

    // std::cout << "center is (" << _mapArea->getCenterX() << "," << _mapArea->getCenterZ() << ")" << std::endl;

    int border = (162 - mapsize) / 2;
    int max = border + mapsize;

    colours = new Caesar3Colours(Scenario::instance().getCity().getClimate());

    for (int y = border; y < max; y++)
    {
      for (int x = border; x < max; x++)
      {  
        TerrainTile& tile = Scenario::instance().getCity().getTilemap().at(x - border, y - border).get_terrain();

        int coords[2];
        int c1, c2;
        getBitmapCoordinates(x - border, y - border, mapsize, coords[0], coords[1]);
        getTerrainColours(tile, c1, c2);

        if( coords[0] >= surface->w-1 || coords[1] >= surface->h )
          continue;

        sdlFacade.set_pixel(surface, coords[0],     coords[1], c1);
        sdlFacade.set_pixel(surface, coords[0] + 1, coords[1], c2);
      }
    }

    delete colours;

    // show center of screen on minimap
    //Exit out of image size on small carts... please fix it
    /*
    sdlFacade.set_pixel(surface, GuiTilemap::instance().getMapArea().getCenterX(),     mapsize * 2 - GuiTilemap::instance().getMapArea().getCenterZ(), kWhite);
    sdlFacade.set_pixel(surface, GuiTilemap::instance().getMapArea().getCenterX() + 1, mapsize * 2 - GuiTilemap::instance().getMapArea().getCenterZ(), kWhite);
    sdlFacade.set_pixel(surface, GuiTilemap::instance().getMapArea().getCenterX(),     mapsize * 2 - GuiTilemap::instance().getMapArea().getCenterZ() + 1, kWhite);
    sdlFacade.set_pixel(surface, GuiTilemap::instance().getMapArea().getCenterX() + 1, mapsize * 2 - GuiTilemap::instance().getMapArea().getCenterZ() + 1, kWhite);

    for ( int i = GuiTilemap::instance().getMapArea().getCenterX() - 18; i <= GuiTilemap::instance().getMapArea().getCenterX() + 18; i++ )
    {
    sdlFacade.set_pixel(surface, i, mapsize * 2 - GuiTilemap::instance().getMapArea().getCenterZ() + 34, kYellow);
    sdlFacade.set_pixel(surface, i, mapsize * 2 - GuiTilemap::instance().getMapArea().getCenterZ() - 34, kYellow);
    }

    for ( int j = mapsize * 2 - GuiTilemap::instance().getMapArea().getCenterZ() - 34; j <= mapsize * 2 - GuiTilemap::instance().getMapArea().getCenterZ() + 34; j++ )
    {
    sdlFacade.set_pixel(surface, GuiTilemap::instance().getMapArea().getCenterX() - 18, j, kYellow);
    sdlFacade.set_pixel(surface, GuiTilemap::instance().getMapArea().getCenterX() + 18, j, kYellow);
    }*/


    // 159, 318 -> 0,159
    // 318, 159 -> 159,159
    // 0, 159   -> 0, 0
    // 159, 0   -> 159,0 


    // 42,35 -> 66, 11 -> 83, 27    ~57, 51

    sdlFacade.unlockSurface(surface);

    // this is window where minimap is displayed
    Picture& minimap_windows = sdlFacade.createPicture(144, 110);

    int i = _tmap.getMapArea().getCenterX();
    int j = _tmap.getMapArea().getCenterZ();

    // 146/2 112/2 = 

    sdlFacade.drawPicture( minimap, minimap_windows , 146/2 - i, 112/2 + j - mapsize*2 );

    painter.drawPicture(minimap_windows, getScreenLeft() + 8, getScreenTop() + 35); // 152, 145
    sdlFacade.deletePicture(minimap);
    sdlFacade.deletePicture(minimap_windows);

    // END OF TEMPORARY    
  }  
}