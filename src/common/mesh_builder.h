#ifndef FI_MESH_BUILDER_H
#define FI_MESH_BUILDER_H

#include <SFML/Graphics.hpp>

namespace fi
{
    const int BLEND_MODE_DISABLED = 0;
    const int BLEND_MODE_NONE = 1;
    const int BLEND_MODE_ADD = 2;
    const int BLEND_MODE_ALPHA = 3;
    const int BLEND_MODE_MULTIPLY = 4;
    const int BLEND_MODE_COUNT = 5;

    const int PRIMITIVE_TYPE_POINTS = 0;
    const int PRIMITIVE_TYPE_LINES = 1;
    const int PRIMITIVE_TYPE_LINES_STRIP = 2;
    const int PRIMITIVE_TYPE_TRIANGLES = 3;
    const int PRIMITIVE_TYPE_TRIANGLES_STRIP = 4;
    const int PRIMITIVE_TYPE_TRIANGLES_FAN = 5;
    const int PRIMITIVE_TYPE_QUADS = 6;
    const int PRIMITIVE_TYPE_COUNT = 7;

    // ----

    class Mesh_Builder
    {
    public:
        Mesh_Builder();

        Mesh_Builder *point();
        Mesh_Builder *point(int BlendType);
        Mesh_Builder *point(sf::Vector2f Position, sf::Color Color);
        Mesh_Builder *point(sf::Vector2f Position, sf::Color Color, int BlendType);

        Mesh_Builder *line();
        Mesh_Builder *line(int BlendType);
        Mesh_Builder *line(sf::Vector2f aPosition, sf::Color aColor, sf::Vector2f bPosition, sf::Color bColor);
        Mesh_Builder *line(sf::Vector2f aPosition, sf::Color aColor, sf::Vector2f bPosition, sf::Color bColor, int BlendType);

        Mesh_Builder *lineStrip();
        Mesh_Builder *lineStrip(int BlendType);
        Mesh_Builder *lineStrip(sf::Vector2f Position, sf::Color Color);
        Mesh_Builder *lineStrip(sf::Vector2f Position, sf::Color Color, int BlendType);
        Mesh_Builder *lineStrip(sf::Vector2f aPosition, sf::Color aColor, sf::Vector2f bPosition, sf::Color bColor);
        Mesh_Builder *lineStrip(sf::Vector2f aPosition, sf::Color aColor, sf::Vector2f bPosition, sf::Color bColor, int BlendType);

        Mesh_Builder *triangle();
        Mesh_Builder *triangle(int BlendType);
        Mesh_Builder *triangle(sf::Vector2f Position, sf::Color Color);
        Mesh_Builder *triangle(sf::Vector2f Position, sf::Color Color, int BlendType);
        Mesh_Builder *triangle(sf::Vector2f aPosition, sf::Color aColor, sf::Vector2f bPosition, sf::Color bColor, sf::Vector2f cPosition, sf::Color cColor);
        Mesh_Builder *triangle(sf::Vector2f aPosition, sf::Color aColor, sf::Vector2f bPosition, sf::Color bColor, sf::Vector2f cPosition, sf::Color cColor, int BlendType);

        Mesh_Builder *triangleStrip();
        Mesh_Builder *triangleStrip(int BlendType);
        Mesh_Builder *triangleStrip(sf::Vector2f Position, sf::Color Color);
        Mesh_Builder *triangleStrip(sf::Vector2f Position, sf::Color Color, int BlendType);
        Mesh_Builder *triangleStrip(sf::Vector2f aPosition, sf::Color aColor, sf::Vector2f bPosition, sf::Color bColor, sf::Vector2f cPosition, sf::Color cColor);
        Mesh_Builder *triangleStrip(sf::Vector2f aPosition, sf::Color aColor, sf::Vector2f bPosition, sf::Color bColor, sf::Vector2f cPosition, sf::Color cColor, int BlendType);

        Mesh_Builder *triangleFan();
        Mesh_Builder *triangleFan(int BlendType);
        Mesh_Builder *triangleFan(sf::Vector2f Position, sf::Color Color);
        Mesh_Builder *triangleFan(sf::Vector2f Position, sf::Color Color, int BlendType);
        Mesh_Builder *triangleFan(sf::Vector2f aPosition, sf::Color aColor, sf::Vector2f bPosition, sf::Color bColor, sf::Vector2f cPosition, sf::Color cColor);
        Mesh_Builder *triangleFan(sf::Vector2f aPosition, sf::Color aColor, sf::Vector2f bPosition, sf::Color bColor, sf::Vector2f cPosition, sf::Color cColor, int BlendType);

        Mesh_Builder *quad();
        Mesh_Builder *quad(int BlendType);
        Mesh_Builder *quad(sf::Vector2f Position, sf::Color Color);
        Mesh_Builder *quad(sf::Vector2f Position, sf::Color Color, int BlendType);
        Mesh_Builder *quad(sf::Vector2f aPosition, sf::Color aColor, sf::Vector2f bPosition, sf::Color bColor, sf::Vector2f cPosition, sf::Color cColor, sf::Vector2f dPosition, sf::Color dColor);
        Mesh_Builder *quad(sf::Vector2f aPosition, sf::Color aColor, sf::Vector2f bPosition, sf::Color bColor, sf::Vector2f cPosition, sf::Color cColor, sf::Vector2f dPosition, sf::Color dColor, int BlendType);

        // todo circle, circleOutline, text, rectOutline, rectAroundText

        Mesh_Builder *append(sf::Vector2f Position, sf::Color Color);
        void drawToRenderTarget(sf::RenderTarget *RenderTarget);
        void clear();

    private:
        int VertexArrayIndex = -1;
        int BlendTypeIndex = -1;

        std::vector<std::vector<sf::VertexArray>> VertexArrays;
    };
}

#endif