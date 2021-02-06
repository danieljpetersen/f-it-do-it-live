#ifndef FI_GUI_H
#define FI_GUI_H

#include <SFML/Graphics/Font.hpp>
#include "../draw.h"
#include "../canvas.h"
#include "../input.h"

namespace fi
{
    // where absolute will not modify internal rect bounds;
    const int ANCHOR_START_ABSOLUTE = 0;
    const int ANCHOR_START = 1;
    const int ANCHOR_CENTER_ABSOLUTE = 2;
    const int ANCHOR_END_ABSOLUTE = 3;
    const int ANCHOR_END = 4;

    // ----

    const int RECT_POSITION = 0;
    const int RECT_SIZE = 1;
    const int AXIS_X = 0;
    const int AXIS_Y = 1;

    struct GUI_Rect
    {
        GUI_Rect() = default;

        GUI_Rect(float x, float y, float w, float h)
        {
            Bounds[RECT_POSITION][AXIS_X] = x;
            Bounds[RECT_POSITION][AXIS_Y] = y;
            Bounds[RECT_SIZE][AXIS_X] = w;
            Bounds[RECT_SIZE][AXIS_Y] = h;
            InternalBounds[RECT_POSITION][AXIS_X] = x;
            InternalBounds[RECT_POSITION][AXIS_Y] = y;
            InternalBounds[RECT_SIZE][AXIS_X] = w;
            InternalBounds[RECT_SIZE][AXIS_Y] = h;
        }

        float Bounds[2][2];
        float InternalBounds[2][2];

        float getLeft() { return Bounds[RECT_POSITION][AXIS_X]; }
        float getRight()  { return Bounds[RECT_POSITION][AXIS_X] + Bounds[RECT_SIZE][AXIS_X]; }
        float getTop()  { return Bounds[RECT_POSITION][AXIS_Y]; }
        float getBottom()  { return Bounds[RECT_POSITION][AXIS_Y] + Bounds[RECT_SIZE][AXIS_Y]; }
        float getWidth() { return Bounds[RECT_SIZE][AXIS_X]; }
        float getHeight() { return Bounds[RECT_SIZE][AXIS_Y]; }

        float getLeftInternalBounds() { return InternalBounds[RECT_POSITION][AXIS_X]; }
        float getRightInternalBounds()  { return InternalBounds[RECT_POSITION][AXIS_X] + InternalBounds[RECT_SIZE][AXIS_X]; }
        float getTopInternalBounds()  { return InternalBounds[RECT_POSITION][AXIS_Y]; }
        float getBottomInternalBounds()  { return InternalBounds[RECT_POSITION][AXIS_Y] + InternalBounds[RECT_SIZE][AXIS_Y]; }
        float getWidthInternalBounds() { return InternalBounds[RECT_SIZE][AXIS_X]; }
        float getHeightInternalBounds() { return InternalBounds[RECT_SIZE][AXIS_Y]; }
    };

    // ----

    class Axis_Constraint
    {
    public:
        Axis_Constraint()
        {
            void reset();
        }

        int PropertiesIndex, AxisIndex;
        float Min, Max; // if 0.0f, does nothing
        float Normalized; // if 0.0f, does nothing
        float xPositionOffset, yPositionOffset;

        void reset()
        {
            PropertiesIndex = -1;
            AxisIndex = -1;
            Min=0.0f;
            Max=0.0f;
            Normalized=0.0f;
            xPositionOffset = 0.0f;
            yPositionOffset = 0.0f;
        }

        Axis_Constraint(int PropertiesIndex, int AxisIndex, float Min, float Max, float Normalized, sf::Vector2f PositionOffset)
        {
            reset();
            this->PropertiesIndex = PropertiesIndex;
            this->AxisIndex = AxisIndex;
            this->Min = Min;
            this->Max = Max;
            this->Normalized = Normalized;
            xPositionOffset = PositionOffset.x;
            yPositionOffset = PositionOffset.y;
        }

        Axis_Constraint(int PropertiesIndex, int AxisIndex, float Min, float Max, float Normalized)
        {
            reset();
            this->PropertiesIndex = PropertiesIndex;
            this->AxisIndex = AxisIndex;
            this->Min = Min;
            this->Max = Max;
            this->Normalized = Normalized;
        }

        Axis_Constraint(int PropertiesIndex, int AxisIndex, float Min, float Max)
        {
            reset();
            this->PropertiesIndex = PropertiesIndex;
            this->AxisIndex = AxisIndex;
            this->Min = Min;
            this->Max = Max;
        }

        Axis_Constraint(int PropertiesIndex, int AxisIndex, float Normalized)
        {
            reset();
            this->PropertiesIndex = PropertiesIndex;
            this->AxisIndex = AxisIndex;
            this->Normalized = Normalized;
        }

        Axis_Constraint(int PropertiesIndex, int AxisIndex, sf::Vector2f PositionOffset)
        {
            reset();
            this->PropertiesIndex = PropertiesIndex;
            this->AxisIndex = AxisIndex;
            this->xPositionOffset = PositionOffset.x;
            this->xPositionOffset = PositionOffset.y;
        }
    };

    // shortcuts because the syntax sucks balls
    fi::Axis_Constraint xConstraintNormalized(float Normalized);
    fi::Axis_Constraint yConstraintNormalized(float Normalized);
    fi::Axis_Constraint xConstraintMinWidth(float MinWidth);
    fi::Axis_Constraint yConstraintMinHeight(float MinHeight);
    fi::Axis_Constraint xConstraintMaxWidth(float MaxWidth);
    fi::Axis_Constraint yConstraintMaxHeight(float MaxHeight);
    fi::Axis_Constraint yConstraintMinHeightAsRowHeight();

    // ----

    class GUI_Node
    {
    public:
        GUI_Rect Rect;
        bool CanTakeFocus;
        int NodeIndex, ParentNodeIndex;
        int xAnchor, yAnchor;
        std::vector<Axis_Constraint> Constraints;

        // ----

        GUI_Node()
        {
            reset();
        }

        GUI_Node(GUI_Node *Node)
        {
            reset();
            this->xAnchor = Node->xAnchor;
            this->yAnchor = Node->yAnchor;
            this->Constraints = Node->Constraints;
            this->Rect = Node->Rect;
        }

        void reset()
        {
            CanTakeFocus = false;
            xAnchor = -1;
            yAnchor = -1;
            ParentNodeIndex = -1;
            NodeIndex = -1;
            Rect.Bounds[0][0] = 0;
            Rect.Bounds[0][1] = 0;
            Rect.Bounds[1][0] = 0;
            Rect.Bounds[1][1] = 0;
            Constraints.clear();
        }

        // todo -- i think this should be refactored to be:
        //      ANCHOR_START/ANCHOR_END == next to internal bounds
        //      ANCHOR_START_ABSOLUTE/ANCHOR_END_ABSOLUTE == next to outter bounds
        //      bool xMoveInternalBounds
        //      bool yMoveInternalBounds
        // alternatively, can create an entirely new set of anchors which represent that position
        //      ANCHOR_START_IMPACT (impact layout; need better name)
        //      ANCHOR_START_ABSOLUTE_IMPACT
        //      ANCHOR_END_IMPACT
        //      ANCHOR_END_ABSOLUTE_IMPACT


        GUI_Node(GUI_Rect Rect)
        {
            reset();
            this->Rect = Rect;
        }

        GUI_Node(int xAnchor, int yAnchor) // only usable if widget function sets bounds...  used for images
        {
            reset();
            this->xAnchor = xAnchor;
            this->yAnchor = yAnchor;
        }

        GUI_Node(int xAnchor, int yAnchor, sf::Vector2f Size)
        {
            reset();
            this->xAnchor = xAnchor;
            this->yAnchor = yAnchor;
            this->Rect.Bounds[RECT_SIZE][AXIS_X] = Size.x;
            this->Rect.Bounds[RECT_SIZE][AXIS_Y] = Size.y;
        }

        GUI_Node(int xAnchor, int yAnchor, float xNormalized, float yNormalized)
        {
            reset();
            this->xAnchor = xAnchor;
            this->yAnchor = yAnchor;
            Constraints.push_back(xConstraintNormalized(xNormalized));
            Constraints.push_back(yConstraintNormalized(yNormalized));
        }

        GUI_Node(int xAnchor, int yAnchor, std::vector<Axis_Constraint> Constraints)
        {
            reset();
            this->xAnchor = xAnchor;
            this->yAnchor = yAnchor;
            this->Constraints = Constraints;
        }

        GUI_Node(int xAnchor, int yAnchor, Axis_Constraint Constraint)
        {
            reset();
            this->xAnchor = xAnchor;
            this->yAnchor = yAnchor;
            this->Constraints = std::vector<fi::Axis_Constraint> { Constraint };
        }

        GUI_Node(int xAnchor, int yAnchor, Axis_Constraint Constraint1, Axis_Constraint Constraint2)
        {
            reset();
            this->xAnchor = xAnchor;
            this->yAnchor = yAnchor;
            this->Constraints = std::vector<fi::Axis_Constraint> { Constraint1, Constraint2 };
        }

        GUI_Node(int xAnchor, int yAnchor, Axis_Constraint Constraint1, Axis_Constraint Constraint2, Axis_Constraint Constraint3)
        {
            reset();
            this->xAnchor = xAnchor;
            this->yAnchor = yAnchor;
            this->Constraints = std::vector<fi::Axis_Constraint> { Constraint1, Constraint2, Constraint3 };
        }

        GUI_Node(int xAnchor, int yAnchor, Axis_Constraint Constraint1, Axis_Constraint Constraint2, Axis_Constraint Constraint3, Axis_Constraint Constraint4)
        {
            reset();
            this->xAnchor = xAnchor;
            this->yAnchor = yAnchor;
            this->Constraints = std::vector<fi::Axis_Constraint> { Constraint1, Constraint2, Constraint3, Constraint4 };
        }

        bool isMouseInside()
        {
            auto MousePosition = fi::getMouseWindowPosition2f();

            if (Rect.Bounds[RECT_POSITION][AXIS_X] < MousePosition.x)
            {
                if (Rect.Bounds[RECT_POSITION][AXIS_Y] < MousePosition.y)
                {
                    if (Rect.Bounds[RECT_POSITION][AXIS_X] + Rect.Bounds[RECT_SIZE][AXIS_X] > MousePosition.x)
                    {
                        if (Rect.Bounds[RECT_POSITION][AXIS_Y] + Rect.Bounds[RECT_SIZE][AXIS_Y] > MousePosition.y)
                        {
							return true;
                        }
                    }
                }
            }

            return false;
        }

        bool hasFocus();

        virtual void onResize(GUI_Rect &ParentBounds, float Scale)
        {
            Rect.Bounds[RECT_SIZE][AXIS_X] *= Scale;
            Rect.Bounds[RECT_SIZE][AXIS_Y] *= Scale;

            float xOffset = 0;
            float yOffset = 0;

            // collect to apply at end because we need constraint order to not matter
            float MinConstraints[2][2] = {{ 0.0f, 0.0f }, { 0.0f, 0.0f }};
            float MaxConstraints[2][2] = {{ 0.0f, 0.0f }, { 0.0f, 0.0f }};

            for (int i = 0; i < Constraints.size(); i++)
            {
                int PropertiesIndex = Constraints[i].PropertiesIndex;
                int AxisIndex = Constraints[i].AxisIndex;

                Constraints[i].Min *= Scale;
                Constraints[i].Max *= Scale;

                if (Constraints[i].Normalized)
                {
                    if (PropertiesIndex == RECT_POSITION)
                    {
                        Rect.Bounds[PropertiesIndex][AxisIndex] = ParentBounds.InternalBounds[RECT_POSITION][AxisIndex] + ParentBounds.InternalBounds[RECT_SIZE][AxisIndex] * Constraints[i].Normalized;
                    }
                    else if (PropertiesIndex == RECT_SIZE)
                    {
                        Rect.Bounds[PropertiesIndex][AxisIndex] = ParentBounds.InternalBounds[RECT_SIZE][AxisIndex] * Constraints[i].Normalized;
                    }
                }

                // probably only makes sense to apply min/max to w, h, but w/e
                if (Constraints[i].Min)
                {
                    if ((Constraints[i].Min < MinConstraints[Constraints[i].PropertiesIndex][Constraints[i].AxisIndex]) || (MinConstraints[Constraints[i].PropertiesIndex][Constraints[i].AxisIndex] == 0.0f))
                    {
                        MinConstraints[Constraints[i].PropertiesIndex][Constraints[i].AxisIndex] = Constraints[i].Min;
                    }
                }

                if (Constraints[i].Max)
                {
                    if ((Constraints[i].Max < MaxConstraints[Constraints[i].PropertiesIndex][Constraints[i].AxisIndex]) || (MaxConstraints[Constraints[i].PropertiesIndex][Constraints[i].AxisIndex] == 0.0f))
                    {
                        MaxConstraints[Constraints[i].PropertiesIndex][Constraints[i].AxisIndex] = Constraints[i].Max;
                    }
                }

                if (Constraints[i].xPositionOffset)
                {
                    xOffset += Constraints[i].xPositionOffset;
                }
                if (Constraints[i].yPositionOffset)
                {
                    yOffset += Constraints[i].yPositionOffset;
                }
            }

            // apply constraints
            {
                for (int i = 0; i < 2; i++)
                {
                    for (int j = 0; j < 2; j++)
                    {
                        if (MinConstraints[i][j])
                        {
                            if (Rect.Bounds[i][j] < MinConstraints[i][j])
                            {
                                Rect.Bounds[i][j] = MinConstraints[i][j];
                            }
                        }

                        if (MaxConstraints[i][j])
                        {
                            if (Rect.Bounds[i][j] > MaxConstraints[i][j])
                            {
                                Rect.Bounds[i][j] = MaxConstraints[i][j];
                            }
                        }
                    }
                }
            }

            if (xAnchor != -1)
            {
                handleAnchorPositioning(AXIS_X, xAnchor, ParentBounds);
            }

            if (yAnchor != -1)
            {
                handleAnchorPositioning(AXIS_Y, yAnchor, ParentBounds);
            }

            Rect.Bounds[RECT_POSITION][AXIS_X] += xOffset;
            Rect.Bounds[RECT_POSITION][AXIS_Y] += yOffset;

            Rect.InternalBounds[0][0] = Rect.Bounds[0][0];
            Rect.InternalBounds[0][1] = Rect.Bounds[0][1];
            Rect.InternalBounds[1][0] = Rect.Bounds[1][0];
            Rect.InternalBounds[1][1] = Rect.Bounds[1][1];
        }

    private:
        void handleAnchorPositioning(int AxisIndex, int Anchor, GUI_Rect &ParentBounds)
        {
            // todo -- i think i should expand this to allow for an item to be placed at start / end of bounds absolutely ignoring previously moved internal bounds?

            if ((Anchor == ANCHOR_START_ABSOLUTE) || (Anchor == ANCHOR_START))
            {
                Rect.Bounds[RECT_POSITION][AxisIndex] = ParentBounds.InternalBounds[RECT_POSITION][AxisIndex];

                if (Anchor == ANCHOR_START)
                {
                    ParentBounds.InternalBounds[RECT_POSITION][AxisIndex] += Rect.Bounds[RECT_SIZE][AxisIndex];
                    ParentBounds.InternalBounds[RECT_SIZE][AxisIndex] -= Rect.Bounds[RECT_SIZE][AxisIndex];
                }
            }
            else if (Anchor == ANCHOR_CENTER_ABSOLUTE)
            {
                Rect.Bounds[RECT_POSITION][AxisIndex] = ParentBounds.InternalBounds[RECT_POSITION][AxisIndex] + (ParentBounds.InternalBounds[RECT_SIZE][AxisIndex] / 2) - (Rect.Bounds[RECT_SIZE][AxisIndex] / 2);

                // todo absolute positioning + multiple centers together
                // i'm not sure how to implement multiple without delaying until all children are declared which would fuck things like drawing up
            }
            else if ((Anchor == ANCHOR_END_ABSOLUTE) || (Anchor == ANCHOR_END))
            {
                Rect.Bounds[RECT_POSITION][AxisIndex] = ParentBounds.InternalBounds[RECT_POSITION][AxisIndex] + ParentBounds.InternalBounds[RECT_SIZE][AxisIndex] - Rect.Bounds[RECT_SIZE][AxisIndex];

                if (Anchor == ANCHOR_END)
                {
                    ParentBounds.InternalBounds[RECT_SIZE][AxisIndex] -= Rect.Bounds[RECT_SIZE][AxisIndex];
                }
            }
        }
    };

    GUI_Node *copyNode(GUI_Node *Original);

    // ----

    class GUI_Style
    {
    public:
        sf::Color TextOutlineColor;
        sf::Color BGRegular, BorderRegular, TextRegular;
        sf::Color BGHighlight, BorderHighlight, TextHighlight;
        sf::Color BGSelected, BorderSelected, TextSelected;
        int TextOutlineThickness;
        int TextCharacterSize;
        int RowHeight;
    };

    // ----

    class GUI
    {
    public:
        bool Handled, MouseWithinBounds;
        int SelectedStyle = 0;
        int SelectedNode = 0; // current parent node
        int FocusedNodeIndex = -1; // for knowing when input has focus; for controller ui
        bool FocusLock = false; // so items like Input can ignore inputs like focus next gui item
        std::vector<GUI_Node*> Nodes; // [0] == dimensions of sf::window
        sf::Font font;
        std::vector<GUI_Style> Styles;
        float Scale = 1.0f;
        int SPACING_FROM_WIDGET_EDGE;
        int SPACING_FROM_SCREEN_EDGE;

        // ----

        void init();
        void clearNodes();
        void selectNode(int NodeIndex);
        void selectNode(GUI_Node *Node);
        void beginFrame();
        int addChild(GUI_Node *GUINode);
        GUI_Node *getSelectedNode();
        void debugDrawNodes();
        sf::RenderTarget *getRenderTarget();
        GUI_Style *getStyle();
        void setStyle(int StyleIndex);
        void drawNode(int NodeIndex);
        void drawNode(GUI_Node *Node);

        void setFocus(GUI_Node *Node);
        void setFocus(int NodeIndex);
        void setFocusPreviousNode(bool AllowJumpToDifferentParents);
        void setFocusNextNode(bool AllowJumpToDifferentParents);
        void setFocusPreviousNode(); // defaults to AllowJump == false
        void setFocusNextNode(); // defaults to AllowJump == false

        int getTextWidth(int CharacterSize);
        int getTextHeight(int CharacterSize);

        // for text wrap and probably others in future this allows me to add additional constraints based on what the nodes width / height would be if calculated
        fi::GUI_Rect mockNodeCalculateSize(GUI_Node *Node);
        fi::GUI_Rect mockNodeCalculateSize(GUI_Node *ChildNode, GUI_Node *ParentNode);

    private:
        void initImgui(std::string FontPath, int CharacterSize);

        std::unordered_map<int, std::pair<int, int>> TextCharacterSizeToPixelSize;
        void addCharacterSizeToPixelSizeMapIfApplicable(int CharacterSize);
    };


    // ----

    // todo -- currently relying on manual text sizing to fit inside rects. should be able to look at single character width/height to do the following
    // todo 1) allow rects to be resized to contain text bounds
    // todo 2) allow text to be resized to be contained within rect bounds

    int rowHeight();

    void guiText(int x, int y, std::string str);
    void guiText(int x, int y, std::string str, int CharacterSize, sf::Color FillColor, sf::Color OutlineColor, int OutlineThickness);
    void guiTextCenter(int x, int y, std::string str);
    void guiTextCenter(int x, int y, std::string str, int CharacterSize, sf::Color FillColor, sf::Color OutlineColor, int OutlineThickness);
    void guiText(fi::GUI_Node *Node, std::string str);
    void guiText(fi::GUI_Node *Node, std::string str, int CharacterSize, sf::Color FillColor, sf::Color OutlineColor, int OutlineThickness);

    void guiTextWrap(fi::GUI_Node *Node, std::string str, int CharacterSize, sf::Color FillColor, sf::Color OutlineColor, int OutlineThickness);
    void guiTextWrap(fi::GUI_Node *Node, std::string str);
    void guiTextWrap(fi::GUI_Node *Node, std::string str, int CharacterSize, sf::Color FillColor, sf::Color OutlineColor, int OutlineThickness, bool ResizeNodeHeightIfTooSmall);
    void guiTextWrap(fi::GUI_Node *Node, std::string str, bool ResizeNodeHeightIfTooSmall);

    int guiPanel(GUI_Node *Node);
    int guiPanel(GUI_Node *Node, sf::Color BGColor, sf::Color BorderColor);

    void guiPadding(int LeftPadding, int TopPadding, int RightPadding, int BottomPadding);
    void guiPadding(int PaddingAllDirections);
    void guiPaddingLeft(int Padding);
    void guiPaddingRight(int Padding);
    void guiPaddingTop(int Padding);
    void guiPaddingBottom(int Padding);

    bool guiButton(GUI_Node *Node, std::string str);
    bool guiButton(GUI_Node *Node, std::string str, bool Selected);
    bool guiButtonsVertical(GUI_Node *FirstButtonNode, std::vector<std::string> str, int &HandledIndex);
    bool guiButtonsHorizontal(GUI_Node *FirstButtonNode, std::vector<std::string> str, int &HandledIndex);
    bool guiButtonDropdownToRight(GUI_Node *FirstButtonNode, std::vector<std::string> str, bool &DropdownEnabled, int &HandledIndex); // todo
    bool guiButtonDropdownToLeft(GUI_Node *FirstButtonNode, std::vector<std::string> str, bool &DropdownEnabled, int &HandledIndex); // todo
    bool guiButtonDropdownToTop(GUI_Node *FirstButtonNode, std::vector<std::string> str, bool &DropdownEnabled, int &HandledIndex); // todo
    bool guiButtonDropdownToBottom(GUI_Node *FirstButtonNode, std::vector<std::string> str, bool &DropdownEnabled, int &HandledIndex); // todo

    int guiColumn(float NormalizedSplit); // returns first column node index; second column is returnValue + 1
    int guiColumns(int NumberOfColumns);  // returns first column node index; second column is returnValue + 1, etc

    int guiRow();
    int guiRow(int RowHeight);
    int guiRow(float NormalizedSplit);
    int guiRows(int NumberOfRows);
    int guiRows(int NumberOfRows, int RowHeight);

    int guiImage(int x, int y, std::string imgPath);
    int guiImage(GUI_Node *Node, std::string imgPath);

    bool guiInput(GUI_Node *Node, std::string &Text, std::string PlaceholderText);

    bool guiProgressBar(GUI_Node *Node, float NormalizedComplete);
    bool guiProgressBar(GUI_Node *Node, float NormalizedComplete, std::string str);

    bool guiSlider(GUI_Node *Node, float &Normalized, std::string str);
    bool guiSlider(GUI_Node *Node, float &Normalized);

    bool guiTooltip(GUI_Node *Node);

    bool guiColorPicker_Terrible(GUI_Node *Node, sf::Color &Color);

    bool guiPointBar(GUI_Node *Node, int TotalNumberOfPoints, int SecondaryPointLimit, int FillCount, int &PointChangeCount); // PointChange positive == green at back; negative == red at front

    void guiSeparatorHorizontal(GUI_Node *Node, sf::Color Color, int Padding);
    void guiSeparatorVertical(GUI_Node *Node, sf::Color Color, int Padding);

    /*
     * graph() // icebox
     */
}

#endif