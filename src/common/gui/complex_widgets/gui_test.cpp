#include <SFML/System/Time.hpp>
#include "gui_test.h"
#include "../../engine.h"

void fi::GUI_Test::guiTest()
{
    sf::Time Start = fi::getClock().getElapsedTime();
    const int RowHeight = fi::getGUI().getStyle()->RowHeight;

    // ---- menubar
    {
        fi::getGUI().selectNode(0);
        int Menu = fi::guiPanel(new fi::GUI_Node(fi::ANCHOR_START_ABSOLUTE, fi::ANCHOR_START,
                                                 fi::Axis_Constraint(fi::RECT_SIZE, fi::AXIS_X, 1.0f),
                                                 fi::Axis_Constraint(fi::RECT_SIZE, fi::AXIS_Y, RowHeight, 0.0f)));

        fi::getGUI().selectNode(Menu);

        // ---- speed buttons 1
        int SelectedTickSpeed = 1;
        int ButtonSize = 45;
        std::vector<std::string> Labels { "-", ">", ">>", ">>>" };
        if (fi::guiButtonsHorizontal(new fi::GUI_Node(fi::ANCHOR_START, fi::ANCHOR_START_ABSOLUTE, sf::Vector2f(ButtonSize, RowHeight)), Labels, SelectedTickSpeed))
        {
            if (SelectedTickSpeed == 0)
            {
                getCoreTick()->pauseTimestep();
            }
            else
            {
                getCoreTick()->unpauseTimestep();
            }
        }

        // ----
        ButtonSize = 77;
        {
            fi::GUI_Node *Node = new fi::GUI_Node(fi::ANCHOR_START, fi::ANCHOR_START_ABSOLUTE, sf::Vector2f(ButtonSize, RowHeight));
            if (ShowingProgressBarForTickCounter)
            {
                if (fi::guiProgressBar(Node, (float)getCoreTick()->getInterpolation(), fi::to_s(getCoreTick()->getTickCount())))
                {
                    ShowingProgressBarForTickCounter = !ShowingProgressBarForTickCounter;
                }
            }
            else
            {
                if (fi::guiButton(Node, fi::to_s(getCoreTick()->getTickCount())))
                {
                    ShowingProgressBarForTickCounter = !ShowingProgressBarForTickCounter;
                }
            }
        }

        // ----
        Labels = { "Game", "View", "Cheats" };
        int HandledIndex = -1;
        if (fi::guiButtonsHorizontal(new fi::GUI_Node(fi::ANCHOR_START, fi::ANCHOR_START_ABSOLUTE, sf::Vector2f(ButtonSize, RowHeight)), Labels, HandledIndex))
        {

        }
    }

    // ---- notification area
    {
        fi::getGUI().selectNode(0);
        int NotificationNode = fi::guiPanel(new fi::GUI_Node(fi::ANCHOR_START_ABSOLUTE, fi::ANCHOR_START_ABSOLUTE, fi::Axis_Constraint(fi::RECT_SIZE, fi::AXIS_X, 0.5f), fi::Axis_Constraint(fi::RECT_SIZE, fi::AXIS_Y, RowHeight * 3, 0.0f, 0.0f)));
        fi::getGUI().selectNode(NotificationNode);

        //fi::guiColumn(0.3);
        int FirstColumn = fi::guiColumns(3);
        fi::getGUI().selectNode(FirstColumn);
        fi::guiSlider(new fi::GUI_Node(fi::ANCHOR_START, fi::ANCHOR_END, fi::xConstraintNormalized(1.0f),
                                       fi::yConstraintNormalized(1.0f)), SliderValue);

        // ---- tooltip
        {
            fi::GUI_Node *TooltipNode = new fi::GUI_Node(fi::GUI_Rect(fi::getMouseWindowPosition2f().x + RowHeight, fi::getMouseWindowPosition2f().y - RowHeight, 200, RowHeight));
            if (fi::guiTooltip(TooltipNode))
            {
                fi::getGUI().selectNode(TooltipNode);

                fi::guiText(TooltipNode, "Yo this is the text dawg");
            }
        }
    }

    // ---- modal
    {
        fi::getGUI().selectNode(0);

        int ModalNode = fi::guiPanel(new fi::GUI_Node(fi::ANCHOR_CENTER_ABSOLUTE, fi::ANCHOR_CENTER_ABSOLUTE, sf::Vector2f(600,500)));
        fi::getGUI().selectNode(ModalNode);
        fi::guiPadding(10);

        // ---- vertical button test
        std::vector<std::string> labels { "first", "second", "third", "fourth", "fifth"  };
        fi::guiButtonsVertical(new fi::GUI_Node(fi::ANCHOR_START_ABSOLUTE, fi::ANCHOR_START,
                                                fi::Axis_Constraint(fi::RECT_SIZE, fi::AXIS_X, 1.0f),
                                                fi::Axis_Constraint(fi::RECT_SIZE, fi::AXIS_Y, 27.0f, 0.0f)),
                               labels, SelectedVerticalTest);


        fi::guiPanel(new fi::GUI_Node(fi::ANCHOR_START, fi::ANCHOR_END_ABSOLUTE, sf::Vector2f(100, 200)));
        fi::guiPanel(new fi::GUI_Node(fi::ANCHOR_START, fi::ANCHOR_END_ABSOLUTE, sf::Vector2f(100, 200)));
        fi::guiPanel(new fi::GUI_Node(fi::ANCHOR_START, fi::ANCHOR_END_ABSOLUTE, sf::Vector2f(100, 200)));
    }

    // ---- left modal
    {
        fi::getGUI().selectNode(0);

        int ModalNode = fi::guiPanel(new fi::GUI_Node(fi::ANCHOR_START_ABSOLUTE, fi::ANCHOR_CENTER_ABSOLUTE, sf::Vector2f(600,500)));
        fi::getGUI().selectNode(ModalNode);
        fi::guiPadding(10);

        fi::guiTextWrap(new fi::GUI_Node(fi::ANCHOR_START_ABSOLUTE, fi::ANCHOR_END_ABSOLUTE,
                                         fi::xConstraintNormalized(1.0f), fi::yConstraintNormalized(0.2f)), "okokokokokokokadfadfa ads fasdfa dfsaf asdfasfdokokokokokokokadfadfa ads fasdfa dfsaf asdfasfdokokokokokokokadfadfa ads fasdfa dfsaf asdfasfdokokokokokokokadfadfa ads fasdfa dfsaf asdfasfdokokokokokokokadfadfa ads fasdfa dfsaf asdfasfdokokokokokokokadfadfa ads fasdfa dfsaf asdfasfdokokokokokokokadfadfa ads fasdfa dfsaf asdfasfdokokokokokokokadfadfa ads fasdfa dfsaf asdfasfdokokokokokokokadfadfa ads fasdfa dfsaf asdfasfdokokokokokokokadfadfa ads fasdfa dfsaf asdfasfdokokokokokokokadfadfa ads fasdfa dfsaf asdfasfdokokokokokokokadfadfa ads fasdfa dfsaf asdfasfdokokokokokokokadfadfa ads fasdfa dfsaf asdfasfdokokokokokokokadfadfa ads fasdfa dfsaf asdfasfdokokokokokokokadfadfa ads fasdfa dfsaf asdfasfd",
                        12, sf::Color::Red, sf::Color::Black, 12, true);

        fi::guiColorPicker_Terrible(new fi::GUI_Node(fi::ANCHOR_START, fi::ANCHOR_CENTER_ABSOLUTE, 0.3f, 0.3f),
                                    ColorPickerTest);
    }

    // ---- right modal
    {
        fi::getGUI().selectNode(0);

        int ModalNode = fi::guiPanel(new fi::GUI_Node(fi::ANCHOR_END, fi::ANCHOR_CENTER_ABSOLUTE, sf::Vector2f(300,500)));
        fi::getGUI().selectNode(ModalNode);
        fi::guiPaddingTop(10);


        int FirstRow = fi::guiRows(4, 40);

        fi::getGUI().selectNode(FirstRow);

        fi::guiInput(new fi::GUI_Node(fi::ANCHOR_START_ABSOLUTE, fi::ANCHOR_START_ABSOLUTE,
                                      fi::xConstraintNormalized(1.0f), fi::yConstraintNormalized(1.0f)), inputText, "Placeholder text");

        fi::getGUI().selectNode(ModalNode);


        fi::guiImage(new fi::GUI_Node(fi::ANCHOR_START_ABSOLUTE, fi::ANCHOR_CENTER_ABSOLUTE), fi::getOS().getPathToExecutableDirectory() + "images/output2.png");
        fi::guiImage(300, 300, fi::getOS().getPathToExecutableDirectory() + "images/output2.png");

        fi::guiSeparatorHorizontal(new fi::GUI_Node(fi::ANCHOR_START_ABSOLUTE, fi::ANCHOR_START, fi::xConstraintNormalized(1.0f), fi::yConstraintMinHeightAsRowHeight()), sf::Color::White, 10);
        fi::guiSeparatorVertical(new fi::GUI_Node(fi::ANCHOR_START_ABSOLUTE, fi::ANCHOR_START, fi::xConstraintNormalized(1.0f), fi::yConstraintMinHeightAsRowHeight()), sf::Color::White, 0);
        fi::guiPointBar(new fi::GUI_Node(fi::ANCHOR_START, fi::ANCHOR_END_ABSOLUTE, sf::Vector2f(100,20)), 10, 6, 2, PointBarChangeCount);
    }

    // ---- footer
    {
        fi::getGUI().selectNode(0);
        int Footer = fi::getGUI().addChild(new fi::GUI_Node(fi::ANCHOR_START_ABSOLUTE, fi::ANCHOR_END,
                                                            fi::Axis_Constraint(fi::RECT_SIZE, fi::AXIS_X, 0.5f),
                                                            fi::Axis_Constraint(fi::RECT_SIZE, fi::AXIS_Y, 0.1f)));
    }

    // ---- benchmark
    {
        sf::Time End = fi::getClock().getElapsedTime();
        //fi::log((int)(End - Start).asMicroseconds());
    }

    //fi::getGUI().debugDrawNodes();
}

////////////////////////////////////////////////////////////
