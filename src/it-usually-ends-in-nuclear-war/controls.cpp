#include "controls.h"
#include "globals.h"
#include "cheats.h"
#include "iueinw.h"

void IUEINW::IUEINW_Plugin_Controls::work(const int Event)
{
    if (fi::getInput().check(fi::key::U))
    {
        getMapDrawables().ShowDebugProvinceTiles = !getMapDrawables().ShowDebugProvinceTiles;
        fi::getPlugins().delayedExecute(EVENT_BUILD_MAP_DRAWABLES);
    }


    //fi::getCanvasWorld().processStandardInput_PanZoom(true, true, true, true, true, true, true, true, true);


    // ---- load / save / map gen
    {
        if (fi::getInput().check("Generate New Map"))
        {
            fi::getPlugins().execute(EVENT_MAP_GENERATION);
        }
        //else if (fi::getInput().check(fi::key::code::L))
        //{
        //    fi::getEngine().triggerLoadData(getApp().ProgramState_Game);
        //}
        //else if (fi::getInput().check(fi::key::code::K))
        //{
        //    fi::Engine::instance().executeSaveData();
        //}
    }

    // ---- cheats
    {
        if (fi::getInput().check("Cheat Reveal Terrain"))
        {
            getCheats().toggleRevealTerrain();
        }

        if (fi::getInput().check("Cheat Reveal Fog of War"))
        {
            getCheats().toggleFow();
        }
    }

    const int KeyboardPanSpeed = 2;
    if (fi::getInput().check("Camera Pan Up"))
    {
        fi::getCanvasWorld().pan(TOP, KeyboardPanSpeed);
    }
    if (fi::getInput().check("Camera Pan Down"))
    {
        fi::getCanvasWorld().pan(BOTTOM, KeyboardPanSpeed);
    }
    if (fi::getInput().check("Camera Pan Left"))
    {
        fi::getCanvasWorld().pan(LEFT, KeyboardPanSpeed);
    }
    if (fi::getInput().check("Camera Pan Right"))
    {
        fi::getCanvasWorld().pan(RIGHT, KeyboardPanSpeed);
    }
    if (fi::getInput().check("Camera Zoom In"))
    {
        fi::getCanvasWorld().zoom(true);
    }
    if (fi::getInput().check("Camera Zoom Out"))
    {
        fi::getCanvasWorld().zoom(false);
    }

    if (fi::getInput().check("Camera Center On Mouse"))
    {
        bool PerformCentering = true;
        sf::Int32 TimeOfLastInitialDown = fi::getClock().getElapsedTime().asMilliseconds() - fi::getInput().timeOfLastInitialDown("Camera Center On Mouse").asMilliseconds();

        int xChange = fi::getMouseWindowPosition2i().x - fi::getInput().MouseWindowPositionOnLastMouseDown.x;
        int yChange = fi::getMouseWindowPosition2i().y - fi::getInput().MouseWindowPositionOnLastMouseDown.y;
        int TotalChange = abs(xChange) + abs(yChange);
        if (TotalChange > 10)
        {
            PerformCentering = false;
        }
        else if (TimeOfLastInitialDown > 200) // because if user holds down left click should cancel it out after n time
        {
            PerformCentering = false;
        }

        if (PerformCentering)
        {
           fi::getCanvasWorld().setCenter(fi::getInput().MouseWorldPosition);
        }

        //FollowingUnit = -1;
    }

    if (fi::getInput().check("Camera Follows Selected Unit"))
    {
        //	if (App.Units.SelectedUnitIndexes.size())
        //	{
        //		App.Units.CameraFollowingUnit = App.Units.SelectedUnitIndexes.front();
        //	}
    }

    if (fi::getInput().check("Camera Zoom Reset"))
    {
        fi::getCanvasWorld().resetZoom();
        //App.GameScreenGUI.CameraZoomGUI.notifyMeThatCameraZoomHasChanged();
        //App.NameTags.recreateNameTagVector();
    }

    if (fi::getInput().check("Camera Mouse Drag"))
    {
        sf::Vector2i LMD = fi::getInput().MouseWindowPositionOnLastLoop;
        sf::Vector2i NOW = fi::getMouseWindowPosition2i();
        sf::Vector2f LMD_World = fi::getCanvasWorld().mapPixelToCoords(LMD);
        sf::Vector2f NOW_World = fi::getCanvasWorld().mapPixelToCoords(NOW);
        float xDiff = float(LMD_World.x - NOW_World.x);
        float yDiff = float(LMD_World.y - NOW_World.y);
        sf::Vector2f NewCenter = fi::getCanvasWorld().getCenter();
        NewCenter.x += xDiff;
        NewCenter.y += yDiff;
        fi::getCanvasWorld().setCenter(NewCenter);
    }

    // ---- mouse window edge scrolling todo
    {
        // todo
        //if ((fi::getInput().check("Camera Mouse Drag")) || (App.Controls.selectionBoxActive()))
        //{
            //TimeOfLastWindowEdgeScrollingDelayAction = gr::Clock.getElapsedTime().asMilliseconds();
        //}
        //
        //bool AttemptMouseWindowEdgeScrolling = false;
        //if ((gr::Window.isFullscreen() != true) && (WindowedMouseOnWindowEdgeScrolling))
        //{
        //    AttemptMouseWindowEdgeScrolling = true;
        //}
        //else if ((gr::Window.isFullscreen()) && (FullscreenMouseOnWindowEdgeScrolling))
        //{
        //    AttemptMouseWindowEdgeScrolling = true;
        //}
        //
        //if (AttemptMouseWindowEdgeScrolling)
        //{
        //    if (gr::Clock.getElapsedTime().asMilliseconds() > TimeOfLastWindowEdgeScrollingDelayAction + 400)
        //    {
        //        const int EDGE_SCROLL_SIZE = 2;
        //
        //        // ---- x
        //        {
        //            if (fi::getMouseWindowPosition2f().x < EDGE_SCROLL_SIZE)
        //            {
        //                pan(LEFT, MouseWindowEdgePanSpeed);
        //            }
        //            else if (fi::getMouseWindowPosition2f().x > gr::Window.getSize().x - EDGE_SCROLL_SIZE)
        //            {
        //                pan(RIGHT, MouseWindowEdgePanSpeed);
        //            }
        //        }
        //
        //        // ---- y
        //        {
        //            if (fi::getMouseWindowPosition2f().y < EDGE_SCROLL_SIZE)
        //            {
        //                pan(TOP, MouseWindowEdgePanSpeed);
        //            }
        //            else if (fi::getMouseWindowPosition2f().y > gr::Window.getSize().y - EDGE_SCROLL_SIZE)
        //            {
        //                pan(BOTTOM, MouseWindowEdgePanSpeed);
        //            }
        //        }
        //    }
        //}
    }

}

////////////////////////////////////////////////////////////

