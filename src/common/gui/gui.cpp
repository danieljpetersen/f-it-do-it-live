#include <cmath>
#include "gui.h"
#include "../engine.h"

void fi::GUI::init()
{
    auto FontPath = getOS().getPathToExecutableDirectory() + getConfig()["gui"]["font-path"].get<std::string>();

    if (!font.loadFromFile(FontPath))
    {
        getEngine().quitFromError("Failed to load font");
    }

    SPACING_FROM_WIDGET_EDGE = fi::getConfig()["gui"]["spacing-from-widget-edge"].get<int>();;
    SPACING_FROM_SCREEN_EDGE = fi::getConfig()["gui"]["spacing-from-screen-edge"].get<int>();;
    Scale = fi::getConfig()["gui"]["scale"].get<float>();
    TextCharacterSizeToPixelSize.clear(); // recalculate everything incase Scale changed

    int i = 0;
    Styles.clear();
    for (auto jsonStyle : getConfig()["gui"]["styles"])
    {
        GUI_Style Style;
        Style.BGRegular = fi::jsonGetColor(jsonStyle, "BGRegular");
        Style.BorderRegular = fi::jsonGetColor(jsonStyle, "BorderRegular");;
        Style.TextRegular = fi::jsonGetColor(jsonStyle, "TextRegular");;

        Style.BGHighlight = fi::jsonGetColor(jsonStyle, "BGHighlight");;
        Style.BorderHighlight = fi::jsonGetColor(jsonStyle, "BorderHighlight");;
        Style.TextHighlight = fi::jsonGetColor(jsonStyle, "TextHighlight");;

        Style.BGSelected = fi::jsonGetColor(jsonStyle, "BGSelected");;
        Style.BorderSelected = fi::jsonGetColor(jsonStyle, "BorderSelected");;
        Style.TextSelected = fi::jsonGetColor(jsonStyle, "TextSelected");;

        Style.TextOutlineThickness = jsonStyle["text-outline-thickness"].get<int>();
        Style.TextOutlineColor = fi::jsonGetColor(jsonStyle, "text-outline-color");
        Style.TextCharacterSize = jsonStyle["text-character-size"].get<float>();

        Style.RowHeight = jsonStyle["row-height"].get<int>();

        if (i == 0)
        {
            initImgui(FontPath, jsonStyle["text-character-size"].get<float>());
        }

        Styles.push_back(Style);

        i++;
    }

    if (i == 0)
    {
        initImgui(FontPath, 12);
    }
}

////////////////////////////////////////////////////////////

bool fi::GUI_Node::hasFocus()
{
    return fi::getGUI().FocusedNodeIndex == NodeIndex;
}

////////////////////////////////////////////////////////////

void fi::GUI::selectNode(int NodeIndex)
{
    SelectedNode = NodeIndex;
}

////////////////////////////////////////////////////////////

void fi::GUI::selectNode(GUI_Node *Node)
{
    selectNode(Node->NodeIndex);
}

////////////////////////////////////////////////////////////

void fi::GUI::clearNodes()
{
    for (int i = 0; i < Nodes.size(); i++)
    {
        delete Nodes[i];
        Nodes[i] = nullptr;
    }
    Nodes.clear();
}

////////////////////////////////////////////////////////////

void fi::GUI::beginFrame()
{
    if (FocusLock != true)
    {
        if (fi::getInput().check("gui-previous-item"))
        {
            fi::getGUI().setFocusPreviousNode(true);
        }
        if (fi::getInput().check("gui-next-item"))
        {
            fi::getGUI().setFocusNextNode(true);
        }
    }

    FocusLock = false;
    Handled = false;
	MouseWithinBounds = false;
    clearNodes();
    Nodes.push_back(new GUI_Node());
    Nodes.front()->Rect = GUI_Rect(0, 0, getWindow().getSize().x, getWindow().getSize().y);;
    selectNode(0);
}

////////////////////////////////////////////////////////////

fi::GUI_Node *fi::GUI::getSelectedNode()
{
    return Nodes[SelectedNode];
}

////////////////////////////////////////////////////////////

int fi::GUI::addChild(fi::GUI_Node *GUINode)
{
    // this enables user to pass in a newly malloc'd node or an already existing node which is helpful for reusing nodes for functions like guiText
    if (GUINode->NodeIndex == -1)
    {
        Nodes.push_back(GUINode);
        Nodes.back()->NodeIndex = (int)Nodes.size() - 1;
        Nodes.back()->ParentNodeIndex = SelectedNode;
        Nodes.back()->onResize(Nodes[SelectedNode]->Rect, Scale);

        if (GUINode->CanTakeFocus)
        {
            if (GUINode->isMouseInside())
            {
				fi::getGUI().MouseWithinBounds = true;

				if ((fi::getInput().check(fi::key::Mouse1)) || (fi::getInput().check("gui-select-item")))
                {
                    setFocus(GUINode);
                }
            }
        }
    }

    return (int)Nodes.size() - 1;
}

////////////////////////////////////////////////////////////

void fi::GUI::drawNode(int NodeIndex)
{
    drawNode(Nodes[NodeIndex]);
}

////////////////////////////////////////////////////////////

void fi::GUI::setFocus(GUI_Node *Node)
{
    FocusedNodeIndex = Node->NodeIndex;
    FocusLock = false;
}

////////////////////////////////////////////////////////////

void fi::GUI::setFocus(int NodeIndex)
{
    if (NodeIndex >= Nodes.size())
    {
        NodeIndex = 0;
    }
    else if (NodeIndex < 0)
    {
        NodeIndex = (int)Nodes.size() - 1;
    }
    setFocus(Nodes[NodeIndex]);
}

////////////////////////////////////////////////////////////

void fi::GUI::setFocusPreviousNode(bool AllowJumpToDifferentParents)
{
    int NodeIndex = FocusedNodeIndex - 1;
    for (int iterationCount = 0; iterationCount < Nodes.size(); iterationCount++)
    {
        if (NodeIndex < 0)
        {
            NodeIndex = (int)Nodes.size() - 1;
        }

        if (Nodes[NodeIndex]->CanTakeFocus)
        {
            if ((AllowJumpToDifferentParents) || (FocusedNodeIndex == -1) || (Nodes[FocusedNodeIndex]->ParentNodeIndex == Nodes[NodeIndex]->ParentNodeIndex))
            {
                setFocus(NodeIndex);
                break;
            }
        }

        NodeIndex--;
    }
}

////////////////////////////////////////////////////////////

void fi::GUI::setFocusNextNode(bool AllowJumpToDifferentParents)
{
    int NodeIndex = FocusedNodeIndex + 1;
    for (int iterationCount = 0; iterationCount < Nodes.size(); iterationCount++)
    {
        if (NodeIndex >= Nodes.size())
        {
            NodeIndex = 0;
        }

        if (Nodes[NodeIndex]->CanTakeFocus)
        {
            if ((AllowJumpToDifferentParents) || (FocusedNodeIndex == -1) || (Nodes[FocusedNodeIndex]->ParentNodeIndex == Nodes[NodeIndex]->ParentNodeIndex))
            {
                setFocus(NodeIndex);
                break;
            }
        }

        NodeIndex++;
    }
}

////////////////////////////////////////////////////////////

void fi::GUI::setFocusPreviousNode()
{
    setFocusPreviousNode(false);
}

////////////////////////////////////////////////////////////

void fi::GUI::setFocusNextNode()
{
    setFocusNextNode(false);
}

////////////////////////////////////////////////////////////

int fi::GUI::getTextWidth(int CharacterSize)
{
    addCharacterSizeToPixelSizeMapIfApplicable(CharacterSize);
    return TextCharacterSizeToPixelSize[CharacterSize].first;
}

////////////////////////////////////////////////////////////

void fi::GUI::addCharacterSizeToPixelSizeMapIfApplicable(int CharacterSize)
{
    if (TextCharacterSizeToPixelSize.find(CharacterSize) == TextCharacterSizeToPixelSize.end())
    {
        sf::Text t;
        t.setFont(fi::getGUI().font);
        t.setString("W"); // going to assume all fonts are mono
        t.setCharacterSize(CharacterSize);
        t.setScale(fi::getGUI().Scale, fi::getGUI().Scale);

        std::pair<int, int> Size(t.getGlobalBounds().width, t.getGlobalBounds().height);
        TextCharacterSizeToPixelSize[CharacterSize] = Size;
    }
}

////////////////////////////////////////////////////////////

int fi::GUI::getTextHeight(int CharacterSize)
{
    addCharacterSizeToPixelSizeMapIfApplicable(CharacterSize);
    return TextCharacterSizeToPixelSize[CharacterSize].second;
}

////////////////////////////////////////////////////////////

fi::GUI_Rect fi::GUI::mockNodeCalculateSize(GUI_Node *Node)
{
    return mockNodeCalculateSize(Node, fi::getGUI().getSelectedNode());
}

////////////////////////////////////////////////////////////

fi::GUI_Rect fi::GUI::mockNodeCalculateSize(GUI_Node *Node, GUI_Node *ParentNode)
{
    GUI_Node FakeChild = (*Node);
    GUI_Node FakeParent = (*fi::getGUI().getSelectedNode());
    FakeChild.onResize(FakeParent.Rect, fi::getGUI().Scale);

    return FakeChild.Rect;
}

////////////////////////////////////////////////////////////

void fi::GUI::drawNode(GUI_Node *Node)
{
    float x = Node->Rect.Bounds[RECT_POSITION][AXIS_X];
    float y = Node->Rect.Bounds[RECT_POSITION][AXIS_Y];
    float w = Node->Rect.Bounds[RECT_SIZE][AXIS_X];
    float h = Node->Rect.Bounds[RECT_SIZE][AXIS_Y];

    sf::Color aColor = getStyle()->BGRegular;
    sf::Color bColor = getStyle()->BorderRegular;

    fi::Draw::rect(fi::getCanvasGUI().getRenderTarget(), sf::Vector2f(x, y), w, h, aColor);
    fi::Draw::rectOutline(fi::getCanvasGUI().getRenderTarget(), sf::Vector2i(x, y), w, h, bColor);
}

////////////////////////////////////////////////////////////

void fi::GUI::debugDrawNodes()
{
    for (int i = 1; i < Nodes.size(); i++)
    {
        float x = Nodes[i]->Rect.Bounds[RECT_POSITION][AXIS_X];
        float y = Nodes[i]->Rect.Bounds[RECT_POSITION][AXIS_Y];
        float w = Nodes[i]->Rect.Bounds[RECT_SIZE][AXIS_X];
        float h = Nodes[i]->Rect.Bounds[RECT_SIZE][AXIS_Y];

        sf::Color aColor = sf::Color(11,11,11);
        aColor.r *= i;
        aColor.g *= i;
        aColor.b *= i;
        sf::Color bColor = getStyle()->BorderRegular;

        fi::Draw::rect(fi::getCanvasGUI().getRenderTarget(), sf::Vector2f(x, y), w, h, aColor);
        fi::Draw::rectOutline(fi::getCanvasGUI().getRenderTarget(), sf::Vector2i(x, y), w, h, bColor);
    }
}

////////////////////////////////////////////////////////////

void fi::GUI::initImgui(std::string FontPath, int CharacterSize)
{
    ImGui::GetIO().Fonts->Clear();
    auto ImGuiFontSize = CharacterSize;
    ImGui::GetIO().Fonts->AddFontFromFileTTF(FontPath.c_str(), ImGuiFontSize);
    ImGui::SFML::UpdateFontTexture(); // important call: updates font texture
}

////////////////////////////////////////////////////////////

fi::GUI_Style *fi::GUI::getStyle()
{
    return &Styles[SelectedStyle];
}


////////////////////////////////////////////////////////////

sf::RenderTarget *fi::GUI::getRenderTarget()
{
    return fi::getCanvasGUI().getRenderTarget(); // todo add ability to change render target to different canvas / textures
}

////////////////////////////////////////////////////////////

void fi::GUI::setStyle(int StyleIndex)
{
    SelectedStyle = StyleIndex;
}

////////////////////////////////////////////////////////////

fi::Axis_Constraint fi::xConstraintNormalized(float Normalized)
{
    return fi::Axis_Constraint(fi::RECT_SIZE, fi::AXIS_X, Normalized);
}

////////////////////////////////////////////////////////////

fi::Axis_Constraint fi::yConstraintNormalized(float Normalized)
{
    return fi::Axis_Constraint(fi::RECT_SIZE, fi::AXIS_Y, Normalized);
}

fi::Axis_Constraint fi::xConstraintMinWidth(float MinWidth)
{
    return fi::Axis_Constraint(fi::RECT_SIZE, fi::AXIS_X, MinWidth, 0.0f);
}

////////////////////////////////////////////////////////////

fi::Axis_Constraint fi::yConstraintMinHeight(float MinHeight)
{
    return fi::Axis_Constraint(fi::RECT_SIZE, fi::AXIS_Y, MinHeight, 0.0f);
}

////////////////////////////////////////////////////////////

fi::Axis_Constraint fi::xConstraintMaxWidth(float MaxWidth)
{
    return fi::Axis_Constraint(fi::RECT_SIZE, fi::AXIS_X, 0.0f, MaxWidth);
}

////////////////////////////////////////////////////////////

fi::Axis_Constraint yConstraintMaxHeight(float MaxHeight)
{
    return fi::Axis_Constraint(fi::RECT_SIZE, fi::AXIS_Y, 0.0f, MaxHeight);
}

////////////////////////////////////////////////////////////

fi::Axis_Constraint fi::yConstraintMinHeightAsRowHeight()
{
    return fi::Axis_Constraint(fi::RECT_SIZE, fi::AXIS_Y, fi::getGUI().getStyle()->RowHeight, 0.0f);
}


////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

int fi::rowHeight()
{
    return getGUI().getStyle()->RowHeight;
}

////////////////////////////////////////////////////////////

void fi::guiText(int x, int y, std::string str, int CharacterSize, sf::Color FillColor, sf::Color OutlineColor, int TextOutlineThickness)
{
    sf::Text t;
    t.setFont(fi::getGUI().font);
    t.setPosition(x, y);
    t.setString(str);
    t.setCharacterSize(CharacterSize*fi::getGUI().Scale);
    t.setScale(fi::getGUI().Scale, fi::getGUI().Scale);
    t.setFillColor(FillColor);
    t.setOutlineColor(OutlineColor);
    t.setOutlineThickness(TextOutlineThickness);
    fi::getGUI().getRenderTarget()->draw(t);
}

////////////////////////////////////////////////////////////

void fi::guiText(int x, int y, std::string str)
{
    guiText(x, y, str, fi::getGUI().getStyle()->TextCharacterSize, fi::getGUI().getStyle()->TextRegular, fi::getGUI().getStyle()->TextOutlineColor, fi::getGUI().getStyle()->TextOutlineThickness);
}

////////////////////////////////////////////////////////////

void fi::guiTextCenter(int x, int y, std::string str)
{
    guiTextCenter(x, y, str, fi::getGUI().getStyle()->TextCharacterSize, fi::getGUI().getStyle()->TextRegular, fi::getGUI().getStyle()->TextOutlineColor, fi::getGUI().getStyle()->TextOutlineThickness);
}

////////////////////////////////////////////////////////////

void fi::guiTextCenter(int x, int y, std::string str, int CharacterSize, sf::Color FillColor, sf::Color OutlineColor, int OutlineThickness)
{
    sf::Text t;
    t.setFont(fi::getGUI().font);
    t.setString(str);
    t.setCharacterSize(fi::getGUI().getStyle()->TextCharacterSize*fi::getGUI().Scale);
    t.setFillColor(FillColor);
    t.setOutlineColor(OutlineColor);
    t.setOutlineThickness(OutlineThickness);
    t.setScale(fi::getGUI().Scale, fi::getGUI().Scale);
    t.setPosition(x - t.getGlobalBounds().width/2, y - t.getGlobalBounds().height/2);

    fi::getGUI().getRenderTarget()->draw(t);
}

////////////////////////////////////////////////////////////

void fi::guiText(fi::GUI_Node *Node, std::string str)
{
    fi::getGUI().addChild(Node);

    float x = Node->Rect.Bounds[RECT_POSITION][AXIS_X];
    float y = Node->Rect.Bounds[RECT_POSITION][AXIS_Y];
    float w = Node->Rect.Bounds[RECT_SIZE][AXIS_X];
    float h = Node->Rect.Bounds[RECT_SIZE][AXIS_Y];

    sf::Text t;
    t.setFont(fi::getGUI().font);
    t.setString(str);
    t.setCharacterSize(fi::getGUI().getStyle()->TextCharacterSize*fi::getGUI().Scale);
    t.setFillColor(fi::getGUI().getStyle()->TextRegular);
    t.setOutlineColor(fi::getGUI().getStyle()->TextOutlineColor);
    t.setOutlineThickness(fi::getGUI().getStyle()->TextOutlineThickness);
    t.setScale(fi::getGUI().Scale, fi::getGUI().Scale);

    float TextWidth = t.getGlobalBounds().width;
    float TextHeight = t.getGlobalBounds().height;
    t.setPosition(int(x + w / 2 - TextWidth / 2), int(y + h / 2 - TextHeight / 2));
    fi::getGUI().getRenderTarget()->draw(t);
}

////////////////////////////////////////////////////////////

void fi::guiText(fi::GUI_Node *Node, std::string str, int CharacterSize, sf::Color FillColor, sf::Color OutlineColor, int OutlineThickness)
{
    fi::getGUI().addChild(Node);

    float x = Node->Rect.Bounds[RECT_POSITION][AXIS_X];
    float y = Node->Rect.Bounds[RECT_POSITION][AXIS_Y];
    float w = Node->Rect.Bounds[RECT_SIZE][AXIS_X];
    float h = Node->Rect.Bounds[RECT_SIZE][AXIS_Y];

    sf::Text t;
    t.setFont(fi::getGUI().font);
    t.setString(str);
    t.setCharacterSize(CharacterSize);
    t.setFillColor(FillColor);
    t.setOutlineColor(OutlineColor);
    t.setOutlineThickness(OutlineThickness);
    t.setScale(fi::getGUI().Scale, fi::getGUI().Scale);

    float TextWidth = t.getGlobalBounds().width;
    float TextHeight = t.getGlobalBounds().height;
    t.setPosition(int(x + w / 2 - TextWidth / 2), int(y + h / 2 - TextHeight / 2)*fi::getGUI().Scale);
    fi::getGUI().getRenderTarget()->draw(t);
}

////////////////////////////////////////////////////////////

void fi::guiTextWrap(fi::GUI_Node *Node, std::string str, int CharacterSize, sf::Color FillColor, sf::Color OutlineColor, int OutlineThickness)
{
    guiTextWrap(Node, str, CharacterSize, FillColor, OutlineColor, OutlineThickness, false);
}

////////////////////////////////////////////////////////////

void fi::guiTextWrap(fi::GUI_Node *Node, std::string str)
{
    guiTextWrap(Node, str, false);
}

////////////////////////////////////////////////////////////

void fi::guiTextWrap(fi::GUI_Node *Node, std::string str, int CharacterSize, sf::Color FillColor, sf::Color OutlineColor, int OutlineThickness, bool ResizeNodeHeightIfTooSmall)
{
    auto Rect = fi::getGUI().mockNodeCalculateSize(Node);

    int CharacterWidth = fi::getGUI().getTextWidth(CharacterSize);
    const int NumberOfCharactersPerRow = (int)std::floor(Rect.getWidth()) / CharacterWidth;
    const int NumberOfRows = (int)str.size() / NumberOfCharactersPerRow;
    const int RowHeight = fi::getGUI().getStyle()->RowHeight;

    if (ResizeNodeHeightIfTooSmall)
    {
        Node->Constraints.push_back(fi::yConstraintMinHeight(NumberOfRows * RowHeight));
    }

    for (int i = NumberOfRows; i > 0; i--)
    {
        str.insert(i * NumberOfCharactersPerRow, "\n");
    }

    // strip spacing at start of new lines; - 2 because i don't want to overflow when i look ahead in the loop
    for (int i = 0; i < str.size()-2; i++)
    {
        if (str.at(i) == '\n')
        {
            while (true)
            {
                if (i + 1 < str.size())
                {
                    if (str[i + 1] == ' ')
                    {
                        str.erase(i + 1, 1);
                    }
                    else
                    {
                        break;
                    }
                }
                else
                {
                    break;
                }
            }
        }
    }

    guiText(Node, str);
}

////////////////////////////////////////////////////////////

void fi::guiTextWrap(fi::GUI_Node *Node, std::string str, bool ResizeNodeHeightIfTooSmall)
{
    guiText(Node, str, fi::getGUI().getStyle()->TextCharacterSize, fi::getGUI().getStyle()->TextRegular, fi::getGUI().getStyle()->TextOutlineColor, fi::getGUI().getStyle()->TextOutlineThickness);
}

////////////////////////////////////////////////////////////

int fi::guiPanel(fi::GUI_Node *Node)
{
    fi::getGUI().addChild(Node);

    float x = Node->Rect.Bounds[RECT_POSITION][AXIS_X];
    float y = Node->Rect.Bounds[RECT_POSITION][AXIS_Y];
    float w = Node->Rect.Bounds[RECT_SIZE][AXIS_X];
    float h = Node->Rect.Bounds[RECT_SIZE][AXIS_Y];

    sf::Color aColor = fi::getGUI().getStyle()->BGRegular;
    sf::Color bColor = fi::getGUI().getStyle()->BorderRegular;

    if (Node->hasFocus())
    {
        aColor = fi::getGUI().getStyle()->BGHighlight;
        bColor = fi::getGUI().getStyle()->BorderHighlight;
    }

    fi::Draw::rect(fi::getGUI().getRenderTarget(), sf::Vector2f(x, y), w, h, aColor);
    fi::Draw::rectOutline(fi::getGUI().getRenderTarget(), sf::Vector2i(x, y), w, h, bColor);

    return Node->NodeIndex;
}

////////////////////////////////////////////////////////////

int fi::guiPanel(fi::GUI_Node *Node, sf::Color BGColor, sf::Color BorderColor)
{
    fi::getGUI().addChild(Node);

    float x = Node->Rect.Bounds[RECT_POSITION][AXIS_X];
    float y = Node->Rect.Bounds[RECT_POSITION][AXIS_Y];
    float w = Node->Rect.Bounds[RECT_SIZE][AXIS_X];
    float h = Node->Rect.Bounds[RECT_SIZE][AXIS_Y];

    fi::Draw::rect(fi::getGUI().getRenderTarget(), sf::Vector2f(x, y), w, h, BGColor);
    fi::Draw::rectOutline(fi::getGUI().getRenderTarget(), sf::Vector2i(x, y), w, h, BorderColor);

    return Node->NodeIndex;
}

////////////////////////////////////////////////////////////

void fi::guiPadding(int LeftPadding, int TopPadding, int RightPadding, int BottomPadding)
{
    if (LeftPadding)
    {
        fi::getGUI().addChild(new fi::GUI_Node(fi::ANCHOR_START, fi::ANCHOR_START_ABSOLUTE, fi::Axis_Constraint(fi::RECT_SIZE, fi::AXIS_X, LeftPadding, 0.0f), fi::Axis_Constraint(fi::RECT_SIZE, fi::AXIS_Y, 1.0f)));
    }

    if (RightPadding)
    {
        fi::getGUI().addChild(new fi::GUI_Node(fi::ANCHOR_END, fi::ANCHOR_START_ABSOLUTE, fi::Axis_Constraint(fi::RECT_SIZE, fi::AXIS_X, LeftPadding, 0.0f), fi::Axis_Constraint(fi::RECT_SIZE, fi::AXIS_Y, 1.0f)));
    }

    if (TopPadding)
    {
        fi::getGUI().addChild(new fi::GUI_Node(fi::ANCHOR_START_ABSOLUTE, fi::ANCHOR_START, fi::Axis_Constraint(fi::RECT_SIZE, fi::AXIS_X, 1.0f), fi::Axis_Constraint(fi::RECT_SIZE, fi::AXIS_Y, TopPadding, 0.0f)));
    }

    if (RightPadding)
    {
        fi::getGUI().addChild(new fi::GUI_Node(fi::ANCHOR_START_ABSOLUTE, fi::ANCHOR_END, fi::Axis_Constraint(fi::RECT_SIZE, fi::AXIS_X, 1.0f), fi::Axis_Constraint(fi::RECT_SIZE, fi::AXIS_Y, TopPadding, 0.0f)));
    }
}

////////////////////////////////////////////////////////////

void fi::guiPadding(int PaddingAllDirections)
{
    guiPadding(PaddingAllDirections, PaddingAllDirections, PaddingAllDirections, PaddingAllDirections);
}

////////////////////////////////////////////////////////////

void fi::guiPaddingLeft(int Padding)
{
    guiPadding(Padding, 0, 0, 0);
}

////////////////////////////////////////////////////////////

void fi::guiPaddingRight(int Padding)
{
    guiPadding(0, 0, Padding, 0);
}

////////////////////////////////////////////////////////////

void fi::guiPaddingTop(int Padding)
{
    guiPadding(0, Padding, 0, 0);
}

////////////////////////////////////////////////////////////

void fi::guiPaddingBottom(int Padding)
{
    guiPadding(0, 0, 0, Padding);
}

////////////////////////////////////////////////////////////

bool fi::guiButton(GUI_Node *Node, std::string str, bool Selected)
{
    Node->CanTakeFocus = true;
    fi::getGUI().addChild(Node);

    sf::Color aColor = fi::getGUI().getStyle()->BGRegular;
    sf::Color bColor = fi::getGUI().getStyle()->BorderRegular;
    sf::Color TextFillColor = fi::getGUI().getStyle()->TextRegular;
    sf::Color TextOutlineColor = fi::getGUI().getStyle()->TextOutlineColor;

    bool Handled = false;

    if (Node->hasFocus())
    {
        if (fi::getInput().check("gui-select-item"))
        {
            Handled = true;
            aColor = fi::getGUI().getStyle()->BGSelected;
            bColor = fi::getGUI().getStyle()->BGSelected;
            TextFillColor = fi::getGUI().getStyle()->TextSelected;
        }
    }
    if (Node->isMouseInside())
    {
		fi::getGUI().MouseWithinBounds = true;

        if (fi::getInput().check(fi::key::Mouse1))
        {
            fi::getGUI().setFocus(Node->NodeIndex);
            fi::getGUI().Handled = true;
            Handled = true;
            aColor = fi::getGUI().getStyle()->BGSelected;
            bColor = fi::getGUI().getStyle()->BGSelected;
            TextFillColor = fi::getGUI().getStyle()->TextSelected;
        }
        else
        {
            aColor = fi::getGUI().getStyle()->BGHighlight;
            bColor = fi::getGUI().getStyle()->BorderHighlight;
            TextFillColor = fi::getGUI().getStyle()->TextHighlight;
        }
    }

    if (Selected)
    {
        if (!Handled)
        {
            aColor = fi::getGUI().getStyle()->BGSelected;
            bColor = fi::getGUI().getStyle()->BorderSelected;
            TextFillColor = fi::getGUI().getStyle()->TextSelected;
        }
        else
        {
            Selected = !Selected;
        }
    }

    float x = Node->Rect.Bounds[RECT_POSITION][AXIS_X];
    float y = Node->Rect.Bounds[RECT_POSITION][AXIS_Y];
    float w = Node->Rect.Bounds[RECT_SIZE][AXIS_X];
    float h = Node->Rect.Bounds[RECT_SIZE][AXIS_Y];

    if (Node->hasFocus())
    {
        if (Selected != true)
        {
            aColor = fi::getGUI().getStyle()->BGHighlight;
            bColor = fi::getGUI().getStyle()->BorderHighlight;
        }
    }

    fi::Draw::rect(fi::getGUI().getRenderTarget(), sf::Vector2f(x, y), w, h, aColor);
    fi::Draw::rectOutline(fi::getGUI().getRenderTarget(), sf::Vector2i(x, y), w, h, bColor);

    sf::Text t;
    t.setFont(fi::getGUI().font);
    t.setString(str);
    t.setCharacterSize(fi::getGUI().getStyle()->TextCharacterSize*fi::getGUI().Scale);
    t.setFillColor(TextFillColor);
    t.setOutlineColor(TextOutlineColor);
    t.setOutlineThickness(fi::getGUI().getStyle()->TextOutlineThickness);

    float TextWidth = t.getGlobalBounds().width;
    float TextHeight = t.getGlobalBounds().height;
    t.setPosition(int(x + w / 2 - TextWidth / 2), int(y + h / 2 - TextHeight / 2));
    fi::getGUI().getRenderTarget()->draw(t);

    return Handled;
}

////////////////////////////////////////////////////////////

bool fi::guiButton(GUI_Node *Node, std::string str)
{
    bool Selected = false;
    return guiButton(Node, str, Selected);
}

////////////////////////////////////////////////////////////

bool fi::guiButtonDropdownToRight(GUI_Node *FirstButtonNode, std::vector<std::string> str, bool &DropdownEnabled, int &HandledIndex)
{
    // todo
    return false;
}

////////////////////////////////////////////////////////////

bool fi::guiButtonDropdownToLeft(GUI_Node *FirstButtonNode, std::vector<std::string> str, bool &DropdownEnabled, int &HandledIndex)
{
    // todo
    return false;
}

////////////////////////////////////////////////////////////

bool fi::guiButtonDropdownToTop(GUI_Node *FirstButtonNode, std::vector<std::string> str, bool &DropdownEnabled, int &HandledIndex)
{
    // todo
    return false;
}

////////////////////////////////////////////////////////////

bool fi::guiButtonDropdownToBottom(GUI_Node *FirstButtonNode, std::vector<std::string> str, bool &DropdownEnabled, int &HandledIndex)
{
    bool Handled = false;

    // todo

    if (DropdownEnabled)
    {
        //guiButtonsVertical()
    }

    return Handled;
}

////////////////////////////////////////////////////////////

bool fi::guiButtonsVertical(GUI_Node *FirstButtonNode, std::vector<std::string> Labels, int &HandledIndex)
{
    // todo doesn't really work with ANCHOR_CENTER_ABSOLUTE

    int Initial = HandledIndex;

    for (int i = 0; i < Labels.size(); i++)
    {
        if (fi::guiButton(new fi::GUI_Node(FirstButtonNode), Labels[i], HandledIndex == i))
        {
            HandledIndex = i;
        }
    }

    return (Initial != HandledIndex);
}

////////////////////////////////////////////////////////////

bool fi::guiButtonsHorizontal(GUI_Node *FirstButtonNode, std::vector<std::string> Strings, int &HandledIndex)
{
    int Initial = HandledIndex;

    for (int i = 0; i < Strings.size(); i++)
    {
        if (fi::guiButton(new fi::GUI_Node(FirstButtonNode), Strings[i], HandledIndex == i))
        {
            HandledIndex = i;
        }
    }

    return (Initial != HandledIndex);
}

////////////////////////////////////////////////////////////

int fi::guiColumn(float NormalizedSplit)
{
    int aColumn = fi::getGUI().addChild(new fi::GUI_Node(fi::ANCHOR_START, fi::ANCHOR_START_ABSOLUTE, fi::Axis_Constraint(fi::RECT_SIZE, fi::AXIS_X, NormalizedSplit), fi::Axis_Constraint(fi::RECT_SIZE, fi::AXIS_Y, 1.0f)));
    int bColumn = fi::getGUI().addChild(new fi::GUI_Node(fi::ANCHOR_START, fi::ANCHOR_START_ABSOLUTE, fi::Axis_Constraint(fi::RECT_SIZE, fi::AXIS_X, 1.0f), fi::Axis_Constraint(fi::RECT_SIZE, fi::AXIS_Y, 1.0f)));

    fi::getGUI().drawNode(aColumn);

    return aColumn;
}

////////////////////////////////////////////////////////////

int fi::guiColumns(int NumberOfColumns)
{
    GUI_Node *Parent = fi::getGUI().getSelectedNode();

    const float WidthPerColumn = Parent->Rect.InternalBounds[fi::RECT_SIZE][fi::AXIS_X] / NumberOfColumns;

    int FirstColumn = (int)fi::getGUI().Nodes.size();
    for (int i = 0; i < NumberOfColumns; i++)
    {
        fi::getGUI().addChild(new fi::GUI_Node(fi::ANCHOR_START, fi::ANCHOR_START_ABSOLUTE, fi::Axis_Constraint(fi::RECT_SIZE, fi::AXIS_X, WidthPerColumn, 0.0f), fi::Axis_Constraint(fi::RECT_SIZE, fi::AXIS_Y, 1.0f)));
        fi::getGUI().drawNode(FirstColumn + i);
    }

    return FirstColumn;
}

////////////////////////////////////////////////////////////

int fi::guiRow(float NormalizedSplit)
{
    // todo discrepency in behavior between this and column; column creates two children, this does not

    int Row = fi::getGUI().addChild(new fi::GUI_Node(fi::ANCHOR_START_ABSOLUTE, fi::ANCHOR_START, fi::xConstraintNormalized(1.0f), fi::yConstraintNormalized(NormalizedSplit)));

    return Row;
}

////////////////////////////////////////////////////////////

int fi::guiRow()
{
    int Row = fi::getGUI().addChild(new fi::GUI_Node(fi::ANCHOR_START_ABSOLUTE, fi::ANCHOR_START, fi::Axis_Constraint(fi::RECT_SIZE, fi::AXIS_X, 1.0f), fi::Axis_Constraint(fi::RECT_SIZE, fi::AXIS_Y, fi::getGUI().getStyle()->RowHeight, 0.0f)));

    fi::getGUI().drawNode(Row);

    return Row;
}

////////////////////////////////////////////////////////////

int fi::guiRows(int NumberOfRows)
{
    int FirstRow = (int)fi::getGUI().Nodes.size();
    for (int i = 0; i < NumberOfRows; i++)
    {
        int Row = fi::getGUI().addChild(new fi::GUI_Node(fi::ANCHOR_START_ABSOLUTE, fi::ANCHOR_START, fi::Axis_Constraint(fi::RECT_SIZE, fi::AXIS_X, 1.0f), fi::Axis_Constraint(fi::RECT_SIZE, fi::AXIS_Y, fi::getGUI().getStyle()->RowHeight, 0.0f)));
        fi::getGUI().drawNode(Row);
    }

    return FirstRow;
}

////////////////////////////////////////////////////////////

int fi::guiRow(int RowHeight)
{
    int Row = fi::getGUI().addChild(new fi::GUI_Node(fi::ANCHOR_START_ABSOLUTE, fi::ANCHOR_START, fi::Axis_Constraint(fi::RECT_SIZE, fi::AXIS_X, 1.0f), fi::Axis_Constraint(fi::RECT_SIZE, fi::AXIS_Y, RowHeight, 0.0f)));

    fi::getGUI().drawNode(Row);

    return Row;
}

////////////////////////////////////////////////////////////

int fi::guiRows(int NumberOfRows, int RowHeight)
{
    int FirstRow = (int)fi::getGUI().Nodes.size();
    for (int i = 0; i < NumberOfRows; i++)
    {
        int Row = fi::getGUI().addChild(new fi::GUI_Node(fi::ANCHOR_START_ABSOLUTE, fi::ANCHOR_START, fi::Axis_Constraint(fi::RECT_SIZE, fi::AXIS_X, 1.0f), fi::Axis_Constraint(fi::RECT_SIZE, fi::AXIS_Y, RowHeight, 0.0f)));
        fi::getGUI().drawNode(Row);
    }

    return FirstRow;
}

////////////////////////////////////////////////////////////

int fi::guiImage(GUI_Node *Node, std::string imgPath)
{
    auto Optional = fi::getTextureStore().getAddTexture(imgPath);

    if (Optional.has_value())
    {
        auto Texture = Optional.value();

        // not intuitive but if user leaves rect.width and rect.height == 0, just set them to the size of the image
        if (Node->Rect.Bounds[RECT_SIZE][AXIS_X] == 0)
        {
            Node->Rect.Bounds[RECT_SIZE][AXIS_X] = Texture->getSize().x;
        }
        if (Node->Rect.Bounds[RECT_SIZE][AXIS_Y] == 0)
        {
            Node->Rect.Bounds[RECT_SIZE][AXIS_Y] = Texture->getSize().y;
        }

        fi::getGUI().addChild(Node);

        sf::Sprite Sprite;
        float Scale = fi::getGUI().Scale;
        Sprite.setScale(Scale, Scale);
        Sprite.setTexture(*Texture);
        int x = int(Node->Rect.Bounds[RECT_POSITION][AXIS_X] + (Node->Rect.Bounds[RECT_SIZE][AXIS_X] / 2) - (Texture->getSize().x * Scale / 2));
        int y = int(Node->Rect.Bounds[RECT_POSITION][AXIS_Y] + (Node->Rect.Bounds[RECT_SIZE][AXIS_Y] / 2) - (Texture->getSize().y * Scale / 2));
        Sprite.setPosition(x, y);

        fi::getGUI().getRenderTarget()->draw(Sprite);
    }

    return Node->NodeIndex;
}

////////////////////////////////////////////////////////////

int fi::guiImage(int x, int y, std::string imgPath)
{
    return guiImage(new fi::GUI_Node(fi::GUI_Rect(x, y, 0, 0)), imgPath);
}

////////////////////////////////////////////////////////////

bool fi::guiInput(GUI_Node *Node, std::string &Text, std::string PlaceholderText)
{
    Node->CanTakeFocus = true;
    fi::getGUI().addChild(Node);

    if (Node->hasFocus())
    {
        fi::getGUI().Handled = true;
        fi::getGUI().FocusLock = true;

        if (fi::getInput().getRawInput().empty() != true)
        {
            Text += fi::getInput().getRawInput();

            if (fi::getInput().check(fi::key::Backspace))
            {
                Text.pop_back();
                Text.pop_back();
            }
        }
    }

    if (Text.empty())
    {
        fi::guiText(Node, PlaceholderText, fi::getGUI().getStyle()->TextCharacterSize, fi::colorModify(fi::getGUI().getStyle()->TextRegular, -44), fi::getGUI().getStyle()->TextOutlineColor, fi::getGUI().getStyle()->TextOutlineThickness);
    }
    else
    {
        fi::guiText(Node, Text);
    }

    // needs to go after for text positioning but doesnt defocus after
    if (Node->hasFocus())
    {
        if ((fi::getInput().check(fi::key::Enter)) || (fi::getInput().check("gui-select-item")))
        {
            fi::getGUI().setFocus(Node->NodeIndex + 2);
        }
    }

    // todo there is a problem here -- stops iteration of previous/next gui item

    return false;
}

////////////////////////////////////////////////////////////

bool fi::guiProgressBar(GUI_Node *Node, float NormalizedComplete, std::string str)
{
    fi::getGUI().addChild(Node);

    float x = Node->Rect.Bounds[RECT_POSITION][AXIS_X];
    float y = Node->Rect.Bounds[RECT_POSITION][AXIS_Y];
    float w = Node->Rect.Bounds[RECT_SIZE][AXIS_X];
    float h = Node->Rect.Bounds[RECT_SIZE][AXIS_Y];

    sf::Color aColor = fi::getGUI().getStyle()->BGRegular;
    sf::Color bColor = fi::getGUI().getStyle()->BorderRegular;

    if (Node->hasFocus())
    {
        aColor = fi::getGUI().getStyle()->BGHighlight;
        bColor = fi::getGUI().getStyle()->BorderHighlight;
    }

    bool Handled = false;
    if (Node->isMouseInside())
    {
        if (getInput().check(fi::key::Mouse1))
        {
			fi::getGUI().MouseWithinBounds = true;
			Handled = true;
        }
    }

    fi::Draw::rect(fi::getGUI().getRenderTarget(), sf::Vector2f(x, y), w, h, aColor);
    fi::Draw::rect(fi::getGUI().getRenderTarget(), sf::Vector2f(x, y), (int)(w*NormalizedComplete), (int)h, fi::getGUI().getStyle()->BGSelected); // completed rect
    fi::Draw::rectOutline(fi::getGUI().getRenderTarget(), sf::Vector2i(x, y), w, h, bColor);

    if (str.empty() != true)
    {
        fi::guiText(Node, str);
    }

    return Handled;
}

bool fi::guiProgressBar(GUI_Node *Node, float NormalizedComplete)
{
    return guiProgressBar(Node, NormalizedComplete, "");
}

////////////////////////////////////////////////////////////

bool fi::guiSlider(fi::GUI_Node *Node, float &Normalized, std::string str)
{
    fi::getGUI().addChild(Node);

    if (Node->isMouseInside())
    {
		fi::getGUI().MouseWithinBounds = true;

		if (fi::getInput().check(fi::key::Mouse1))
        {
            Normalized = (fi::getMouseWindowPosition2f().x - Node->Rect.getLeft()) / Node->Rect.getWidth();
        }
    }

    return guiProgressBar(Node, Normalized, str);
}

////////////////////////////////////////////////////////////

bool fi::guiSlider(fi::GUI_Node *Node, float &Normalized)
{
    return guiSlider(Node, Normalized, "");
}

////////////////////////////////////////////////////////////

bool fi::guiTooltip(GUI_Node *Node)
{
    fi::getGUI().addChild(Node);

    if (fi::getInput().check(fi::key::Mouse1))
    {
        fi::getInput().MouseIdleTime = sf::Time::Zero; // just feels better
    }

    const int TOOLTIP_IDLE_TIME_MS = fi::getConfig()["gui"]["tooltip-hover-ms"].get<int>();
    if (fi::getInput().getMouseIdleTime().asMilliseconds() > TOOLTIP_IDLE_TIME_MS)
    {
        if (fi::getGUI().Nodes[Node->NodeIndex - 1]->isMouseInside())
        {
			guiPanel(Node);

            return true;
        }
    }

    return false;
}

////////////////////////////////////////////////////////////

bool fi::guiColorPicker_Terrible(GUI_Node *Node, sf::Color &Color)
{
    int OriginalParent = fi::getGUI().getSelectedNode()->NodeIndex;
    int Area = fi::getGUI().addChild(Node);
    fi::getGUI().selectNode(Area);

    float rNormalized = (float)Color.r / 255.f;
    float gNormalized = (float)Color.g / 255.f;
    float bNormalized = (float)Color.b / 255.f;
    float aNormalized = (float)Color.a / 255.f;

    float SliderHeight = fi::getGUI().getSelectedNode()->Rect.getHeight() / 4;
    fi::guiPanel(new fi::GUI_Node(fi::ANCHOR_START, fi::ANCHOR_START_ABSOLUTE, fi::xConstraintNormalized(0.4f), fi::yConstraintNormalized(1.0f)), Color, sf::Color::Black);

    bool Handled = false;
    Handled = fi::guiSlider(new fi::GUI_Node(ANCHOR_START_ABSOLUTE, ANCHOR_START, xConstraintNormalized(1.0f), yConstraintMinHeight(SliderHeight)), rNormalized) || Handled;
    Handled = fi::guiSlider(new fi::GUI_Node(ANCHOR_START_ABSOLUTE, ANCHOR_START, xConstraintNormalized(1.0f), yConstraintMinHeight(SliderHeight)), gNormalized) || Handled;
    Handled = fi::guiSlider(new fi::GUI_Node(ANCHOR_START_ABSOLUTE, ANCHOR_START, xConstraintNormalized(1.0f), yConstraintMinHeight(SliderHeight)), bNormalized) || Handled;
    Handled = fi::guiSlider(new fi::GUI_Node(ANCHOR_START_ABSOLUTE, ANCHOR_START, xConstraintNormalized(1.0f), yConstraintMinHeight(SliderHeight)), aNormalized) || Handled;

    Color.r = sf::Uint8(rNormalized * 255);
    Color.g = sf::Uint8(gNormalized * 255);
    Color.b = sf::Uint8(bNormalized * 255);
    Color.a = sf::Uint8(aNormalized * 255);

    fi::getGUI().selectNode(OriginalParent);

    return Handled;
}

////////////////////////////////////////////////////////////

bool fi::guiPointBar(GUI_Node *Node, int TotalNumberOfPoints, int SecondaryPointLimit, int FillCount, int &PointChangeCount)
{
    int OriginalPanel = fi::getGUI().getSelectedNode()->NodeIndex;
    int PanelIndex = fi::guiPanel(Node);
    fi::getGUI().selectNode(PanelIndex);
    fi::guiPadding(fi::getGUI().SPACING_FROM_WIDGET_EDGE);

    bool Handled = false;

    // todo -- need to add an additional point, NegativeRemoval....  or maybe just split PointChangeCount into PositivePoinntChangeCount, NegativePointChangeCount, where positive will first eat into negative

    if (Node->isMouseInside())
    {
		fi::getGUI().MouseWithinBounds = true;

		// todo keyboard selection -- probably need a rethink how we're doing this to make mouse + keyboard automatic; maybe just flag Node->IsInteractable; replacement for IsFocusable
        bool m1 = fi::getInput().check(fi::key::Mouse1);
        bool m2 = fi::getInput().check(fi::key::Mouse2);
        if (m1 || m2)
        {
            Handled = true;

            fi::getInput().resetKey(fi::key::Mouse2); // becausee camera moves on mouse 2 up, which will trigger next frame when Handled != true

            if (m1)
            {
                PointChangeCount += 1;

                if (FillCount + PointChangeCount > SecondaryPointLimit)
                {
                    PointChangeCount -= 1;
                }
            }
            else
            {
                PointChangeCount -= 1;

                if (PointChangeCount < 0)
                {
                    if (abs(PointChangeCount) > FillCount)
                    {
                        PointChangeCount += 1;
                    }
                }
            }
        }
    }

    const int SpacingBetweenBars = 1;
    const int TotalSpacingBetweenBars = (TotalNumberOfPoints - 1) * SpacingBetweenBars;
    const int BarWidth = int((Node->Rect.getWidthInternalBounds() - (float)TotalSpacingBetweenBars) / (float)TotalNumberOfPoints);
    const int BarHeight = (int)Node->Rect.getHeightInternalBounds();

    sf::Vector2f Pos = sf::Vector2f(Node->Rect.getLeftInternalBounds(), Node->Rect.getTopInternalBounds());
    int xPositionIncrement = SpacingBetweenBars + BarWidth;

    sf::VertexArray QuadsVertexArray(sf::Quads);
    sf::VertexArray LinesVertexArray(sf::Lines);

    int PositivePointChange = 0;
    int NegativePointChange = 0;

    if (PointChangeCount < 0)
    {
        NegativePointChange = abs(PointChangeCount);
    }
    else
    {
        PositivePointChange = PointChangeCount;
    }

    sf::Color GreyedOutLighter = sf::Color(90, 90, 90);
    sf::Color GreyedOutDarker = sf::Color(40, 40, 40);

    for (int i = 0; i < TotalNumberOfPoints; i++)
    {
        sf::Vector2f TopLeft = Pos;
        sf::Vector2f TopRight = sf::Vector2f(Pos.x + BarWidth, Pos.y);
        sf::Vector2f BottomRight = sf::Vector2f(Pos.x + BarWidth, Pos.y + BarHeight);
        sf::Vector2f BottomLeft = sf::Vector2f(Pos.x, Pos.y + BarHeight);

        sf::Color Color = GreyedOutDarker;

        if (i < SecondaryPointLimit)
        {
            Color = GreyedOutLighter;
        }
        if (i < FillCount + PositivePointChange)
        {
            Color = sf::Color::Green;
        }
        if (i < FillCount)
        {
            Color = fi::getGUI().getStyle()->BGSelected;
        }
        if (i < NegativePointChange)
        {
            Color = sf::Color::Red;
        }

        sf::Vertex Vert[4] =
        {
            sf::Vertex(TopLeft, Color),
            sf::Vertex(TopRight, Color),
            sf::Vertex(BottomRight, Color),
            sf::Vertex(BottomLeft, Color)
        };

        // ---- quad
        {
            QuadsVertexArray.append(Vert[0]);
            QuadsVertexArray.append(Vert[1]);
            QuadsVertexArray.append(Vert[2]);
            QuadsVertexArray.append(Vert[3]);
        }

        // ---- lines
        {
            sf::Color OutlineColor = fi::getGUI().getStyle()->BorderRegular;
            Vert[0].color = OutlineColor;
            Vert[1].color = OutlineColor;
            Vert[2].color = OutlineColor;
            Vert[3].color = OutlineColor;

            // topleft to topright
            LinesVertexArray.append(Vert[0]);
            LinesVertexArray.append(Vert[1]);

            // topright to bottomright
            LinesVertexArray.append(Vert[1]);
            LinesVertexArray.append(Vert[2]);

            // bottomright to bottomleft
            LinesVertexArray.append(Vert[2]);
            LinesVertexArray.append(Vert[3]);

            // bottomleft to topleft
            LinesVertexArray.append(Vert[3]);
            LinesVertexArray.append(Vert[0]);
        }

        Pos.x += xPositionIncrement;
    }

    fi::getGUI().getRenderTarget()->draw(QuadsVertexArray);
    fi::getGUI().getRenderTarget()->draw(LinesVertexArray);

    fi::getGUI().selectNode(OriginalPanel);
    return Handled;
}

////////////////////////////////////////////////////////////

void fi::guiSeparatorHorizontal(GUI_Node *Node, sf::Color Color, int Padding)
{
    int OriginalNode = fi::getGUI().getSelectedNode()->NodeIndex;
    fi::getGUI().addChild(Node);
    fi::getGUI().selectNode(Node);

    if (Padding)
    {
        fi::guiPaddingTop(Padding);
    }

    int x1 = int(Node->Rect.getLeftInternalBounds());
    int y1 = int(Node->Rect.getTop() + Node->Rect.getHeightInternalBounds()/2);
    int x2 = int(Node->Rect.getRightInternalBounds());
    int y2 = int(Node->Rect.getTop() + Node->Rect.getHeightInternalBounds()/2);

    fi::Draw::line(fi::getGUI().getRenderTarget(), sf::Vector2i(x1, y1), sf::Vector2i(x2, y2), Color);
    fi::getGUI().selectNode(OriginalNode);
}

////////////////////////////////////////////////////////////

void fi::guiSeparatorVertical(GUI_Node *Node, sf::Color Color, int Padding)
{
    int OriginalNode = fi::getGUI().getSelectedNode()->NodeIndex;
    fi::getGUI().addChild(Node);
    fi::getGUI().selectNode(Node);

    if (Padding)
    {
        fi::guiPaddingTop(Padding);
    }

    int x1 = int(Node->Rect.getLeft() + Node->Rect.getWidth()/2);
    int y1 = int(Node->Rect.getTopInternalBounds());
    int x2 = int(Node->Rect.getLeft() + Node->Rect.getWidth()/2);
    int y2 = int(Node->Rect.getBottomInternalBounds());

    fi::Draw::line(fi::getGUI().getRenderTarget(), sf::Vector2i(x1, y1), sf::Vector2i(x2, y2), Color);
    fi::getGUI().selectNode(OriginalNode);
}
