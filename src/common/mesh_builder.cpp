#include "mesh_builder.h"

fi::Mesh_Builder::Mesh_Builder()
{
    VertexArrays.resize(BLEND_MODE_COUNT);
    for (int i = 0; i < BLEND_MODE_COUNT; i++)
    {
        VertexArrays[i].resize(PRIMITIVE_TYPE_COUNT);
        VertexArrays[i][PRIMITIVE_TYPE_POINTS].setPrimitiveType(sf::Points);
        VertexArrays[i][PRIMITIVE_TYPE_LINES].setPrimitiveType(sf::Lines);
        VertexArrays[i][PRIMITIVE_TYPE_LINES_STRIP].setPrimitiveType(sf::LinesStrip);
        VertexArrays[i][PRIMITIVE_TYPE_TRIANGLES].setPrimitiveType(sf::Triangles);
        VertexArrays[i][PRIMITIVE_TYPE_TRIANGLES_FAN].setPrimitiveType(sf::TriangleFan);
        VertexArrays[i][PRIMITIVE_TYPE_TRIANGLES_STRIP].setPrimitiveType(sf::TrianglesStrip);
        VertexArrays[i][PRIMITIVE_TYPE_QUADS].setPrimitiveType(sf::Quads);
    }
}

////////////////////////////////////////////////////////////

fi::Mesh_Builder *fi::Mesh_Builder::point()
{
    this->VertexArrayIndex = PRIMITIVE_TYPE_POINTS;
    this->BlendTypeIndex = BLEND_MODE_DISABLED;
    return this;
}

////////////////////////////////////////////////////////////

fi::Mesh_Builder *fi::Mesh_Builder::point(int BlendType)
{
    this->VertexArrayIndex = PRIMITIVE_TYPE_POINTS;
    this->BlendTypeIndex = BlendType;
    return this;
}

////////////////////////////////////////////////////////////

fi::Mesh_Builder *fi::Mesh_Builder::point(sf::Vector2f Position, sf::Color Color)
{
    this->VertexArrayIndex = PRIMITIVE_TYPE_POINTS;
    this->BlendTypeIndex = BLEND_MODE_DISABLED;
    VertexArrays[BlendTypeIndex][VertexArrayIndex].append(sf::Vertex(Position, Color));
    return this;
}

////////////////////////////////////////////////////////////

fi::Mesh_Builder *fi::Mesh_Builder::point(sf::Vector2f Position, sf::Color Color, int BlendType)
{
    this->VertexArrayIndex = PRIMITIVE_TYPE_POINTS;
    this->BlendTypeIndex = BlendType;
    VertexArrays[BlendTypeIndex][VertexArrayIndex].append(sf::Vertex(Position, Color));
    return this;
}

////////////////////////////////////////////////////////////

fi::Mesh_Builder *fi::Mesh_Builder::line()
{
    this->VertexArrayIndex = PRIMITIVE_TYPE_LINES;
    this->BlendTypeIndex = BLEND_MODE_DISABLED;
    return this;
}

////////////////////////////////////////////////////////////

fi::Mesh_Builder *fi::Mesh_Builder::line(int BlendType)
{
    this->VertexArrayIndex = PRIMITIVE_TYPE_LINES;
    this->BlendTypeIndex = BlendType;
    return this;
}

////////////////////////////////////////////////////////////

fi::Mesh_Builder *fi::Mesh_Builder::line(sf::Vector2f aPosition, sf::Color aColor, sf::Vector2f bPosition, sf::Color bColor)
{
    this->VertexArrayIndex = PRIMITIVE_TYPE_LINES;
    this->BlendTypeIndex = BLEND_MODE_DISABLED;
    VertexArrays[BlendTypeIndex][VertexArrayIndex].append(sf::Vertex(aPosition, aColor));
    VertexArrays[BlendTypeIndex][VertexArrayIndex].append(sf::Vertex(bPosition, bColor));
    return this;
}

////////////////////////////////////////////////////////////

fi::Mesh_Builder *fi::Mesh_Builder::line(sf::Vector2f aPosition, sf::Color aColor, sf::Vector2f bPosition, sf::Color bColor, int BlendType)
{
    this->VertexArrayIndex = PRIMITIVE_TYPE_LINES;
    this->BlendTypeIndex = BlendType;
    VertexArrays[BlendTypeIndex][VertexArrayIndex].append(sf::Vertex(aPosition, aColor));
    VertexArrays[BlendTypeIndex][VertexArrayIndex].append(sf::Vertex(bPosition, bColor));
    return this;
}

////////////////////////////////////////////////////////////

fi::Mesh_Builder *fi::Mesh_Builder::lineStrip()
{
    this->VertexArrayIndex = PRIMITIVE_TYPE_LINES_STRIP;
    this->BlendTypeIndex = BLEND_MODE_DISABLED;
    return this;
}

////////////////////////////////////////////////////////////

fi::Mesh_Builder *fi::Mesh_Builder::lineStrip(int BlendType)
{
    this->VertexArrayIndex = PRIMITIVE_TYPE_LINES_STRIP;
    this->BlendTypeIndex = BlendType;
    return this;
}

////////////////////////////////////////////////////////////

fi::Mesh_Builder *fi::Mesh_Builder::lineStrip(sf::Vector2f Position, sf::Color Color)
{
    this->VertexArrayIndex = PRIMITIVE_TYPE_LINES_STRIP;
    this->BlendTypeIndex = BLEND_MODE_DISABLED;
    VertexArrays[BlendTypeIndex][VertexArrayIndex].append(sf::Vertex(Position, Color));
    return this;
}

////////////////////////////////////////////////////////////

fi::Mesh_Builder *fi::Mesh_Builder::lineStrip(sf::Vector2f Position, sf::Color Color, int BlendType)
{
    this->VertexArrayIndex = PRIMITIVE_TYPE_LINES_STRIP;
    this->BlendTypeIndex = BlendType;
    VertexArrays[BlendTypeIndex][VertexArrayIndex].append(sf::Vertex(Position, Color));
    return this;
}

////////////////////////////////////////////////////////////

fi::Mesh_Builder *fi::Mesh_Builder::lineStrip(sf::Vector2f aPosition, sf::Color aColor, sf::Vector2f bPosition, sf::Color bColor)
{
    this->VertexArrayIndex = PRIMITIVE_TYPE_LINES_STRIP;
    this->BlendTypeIndex = BLEND_MODE_DISABLED;
    VertexArrays[BlendTypeIndex][VertexArrayIndex].append(sf::Vertex(aPosition, aColor));
    VertexArrays[BlendTypeIndex][VertexArrayIndex].append(sf::Vertex(bPosition, bColor));
    return this;
}

////////////////////////////////////////////////////////////

fi::Mesh_Builder *fi::Mesh_Builder::lineStrip(sf::Vector2f aPosition, sf::Color aColor, sf::Vector2f bPosition, sf::Color bColor, int BlendType)
{
    this->VertexArrayIndex = PRIMITIVE_TYPE_LINES_STRIP;
    this->BlendTypeIndex = BlendType;
    VertexArrays[BlendTypeIndex][VertexArrayIndex].append(sf::Vertex(aPosition, aColor));
    VertexArrays[BlendTypeIndex][VertexArrayIndex].append(sf::Vertex(bPosition, bColor));
    return this;
}

////////////////////////////////////////////////////////////

fi::Mesh_Builder *fi::Mesh_Builder::triangle()
{
    this->VertexArrayIndex = PRIMITIVE_TYPE_TRIANGLES;
    this->BlendTypeIndex = BLEND_MODE_DISABLED;
    return this;
}

////////////////////////////////////////////////////////////

fi::Mesh_Builder *fi::Mesh_Builder::triangle(int BlendType)
{
    this->VertexArrayIndex = PRIMITIVE_TYPE_TRIANGLES;
    this->BlendTypeIndex = BlendType;
    return this;
}

////////////////////////////////////////////////////////////

fi::Mesh_Builder *fi::Mesh_Builder::triangle(sf::Vector2f Position, sf::Color Color)
{
    this->VertexArrayIndex = PRIMITIVE_TYPE_TRIANGLES;
    this->BlendTypeIndex = BLEND_MODE_DISABLED;
    VertexArrays[BlendTypeIndex][VertexArrayIndex].append(sf::Vertex(Position, Color));
    return this;
}

////////////////////////////////////////////////////////////

fi::Mesh_Builder *fi::Mesh_Builder::triangle(sf::Vector2f Position, sf::Color Color, int BlendType)
{
    this->VertexArrayIndex = PRIMITIVE_TYPE_TRIANGLES;
    this->BlendTypeIndex = BlendType;
    VertexArrays[BlendTypeIndex][VertexArrayIndex].append(sf::Vertex(Position, Color));
    return this;
}

////////////////////////////////////////////////////////////

fi::Mesh_Builder *fi::Mesh_Builder::triangle(sf::Vector2f aPosition, sf::Color aColor, sf::Vector2f bPosition, sf::Color bColor, sf::Vector2f cPosition, sf::Color cColor)
{
    this->VertexArrayIndex = PRIMITIVE_TYPE_TRIANGLES;
    this->BlendTypeIndex = BLEND_MODE_DISABLED;
    VertexArrays[BlendTypeIndex][VertexArrayIndex].append(sf::Vertex(aPosition, aColor));
    VertexArrays[BlendTypeIndex][VertexArrayIndex].append(sf::Vertex(bPosition, bColor));
    VertexArrays[BlendTypeIndex][VertexArrayIndex].append(sf::Vertex(cPosition, cColor));
    return this;
}

////////////////////////////////////////////////////////////

fi::Mesh_Builder *fi::Mesh_Builder::triangle(sf::Vector2f aPosition, sf::Color aColor, sf::Vector2f bPosition, sf::Color bColor, sf::Vector2f cPosition, sf::Color cColor, int BlendType)
{
    this->VertexArrayIndex = PRIMITIVE_TYPE_TRIANGLES;
    this->BlendTypeIndex = BlendType;
    VertexArrays[BlendTypeIndex][VertexArrayIndex].append(sf::Vertex(aPosition, aColor));
    VertexArrays[BlendTypeIndex][VertexArrayIndex].append(sf::Vertex(bPosition, bColor));
    VertexArrays[BlendTypeIndex][VertexArrayIndex].append(sf::Vertex(cPosition, cColor));
    return this;
}

////////////////////////////////////////////////////////////

fi::Mesh_Builder *fi::Mesh_Builder::triangleStrip()
{
    this->VertexArrayIndex = PRIMITIVE_TYPE_TRIANGLES_STRIP;
    this->BlendTypeIndex = BLEND_MODE_DISABLED;
    return this;
}

////////////////////////////////////////////////////////////

fi::Mesh_Builder *fi::Mesh_Builder::triangleStrip(int BlendType)
{
    this->VertexArrayIndex = PRIMITIVE_TYPE_TRIANGLES_STRIP;
    this->BlendTypeIndex = BlendType;
    return this;
}

////////////////////////////////////////////////////////////

fi::Mesh_Builder *fi::Mesh_Builder::triangleStrip(sf::Vector2f Position, sf::Color Color)
{
    this->VertexArrayIndex = PRIMITIVE_TYPE_TRIANGLES_STRIP;
    this->BlendTypeIndex = BLEND_MODE_DISABLED;
    VertexArrays[BlendTypeIndex][VertexArrayIndex].append(sf::Vertex(Position, Color));
    return this;
}

////////////////////////////////////////////////////////////

fi::Mesh_Builder *fi::Mesh_Builder::triangleStrip(sf::Vector2f Position, sf::Color Color, int BlendType)
{
    this->VertexArrayIndex = PRIMITIVE_TYPE_TRIANGLES_STRIP;
    this->BlendTypeIndex = BlendType;
    VertexArrays[BlendTypeIndex][VertexArrayIndex].append(sf::Vertex(Position, Color));
    return this;
}

////////////////////////////////////////////////////////////

fi::Mesh_Builder *fi::Mesh_Builder::triangleStrip(sf::Vector2f aPosition, sf::Color aColor, sf::Vector2f bPosition, sf::Color bColor, sf::Vector2f cPosition, sf::Color cColor)
{
    this->VertexArrayIndex = PRIMITIVE_TYPE_TRIANGLES_STRIP;
    this->BlendTypeIndex = BLEND_MODE_DISABLED;
    VertexArrays[BlendTypeIndex][VertexArrayIndex].append(sf::Vertex(aPosition, aColor));
    VertexArrays[BlendTypeIndex][VertexArrayIndex].append(sf::Vertex(bPosition, bColor));
    VertexArrays[BlendTypeIndex][VertexArrayIndex].append(sf::Vertex(cPosition, cColor));
    return this;
}

////////////////////////////////////////////////////////////

fi::Mesh_Builder *fi::Mesh_Builder::triangleStrip(sf::Vector2f aPosition, sf::Color aColor, sf::Vector2f bPosition, sf::Color bColor, sf::Vector2f cPosition, sf::Color cColor, int BlendType)
{
    this->VertexArrayIndex = PRIMITIVE_TYPE_TRIANGLES_STRIP;
    this->BlendTypeIndex = BlendType;
    VertexArrays[BlendTypeIndex][VertexArrayIndex].append(sf::Vertex(aPosition, aColor));
    VertexArrays[BlendTypeIndex][VertexArrayIndex].append(sf::Vertex(bPosition, bColor));
    VertexArrays[BlendTypeIndex][VertexArrayIndex].append(sf::Vertex(cPosition, cColor));
    return this;
}

////////////////////////////////////////////////////////////

fi::Mesh_Builder *fi::Mesh_Builder::triangleFan()
{
    this->VertexArrayIndex = PRIMITIVE_TYPE_TRIANGLES_FAN;
    this->BlendTypeIndex = BLEND_MODE_DISABLED;
    return this;
}

////////////////////////////////////////////////////////////

fi::Mesh_Builder *fi::Mesh_Builder::triangleFan(int BlendType)
{
    this->VertexArrayIndex = PRIMITIVE_TYPE_TRIANGLES_FAN;
    this->BlendTypeIndex = BlendType;
    return this;
}

////////////////////////////////////////////////////////////

fi::Mesh_Builder *fi::Mesh_Builder::triangleFan(sf::Vector2f Position, sf::Color Color)
{
    this->VertexArrayIndex = PRIMITIVE_TYPE_TRIANGLES_FAN;
    this->BlendTypeIndex = BLEND_MODE_DISABLED;
    VertexArrays[BlendTypeIndex][VertexArrayIndex].append(sf::Vertex(Position, Color));
    return this;
}

////////////////////////////////////////////////////////////

fi::Mesh_Builder *fi::Mesh_Builder::triangleFan(sf::Vector2f Position, sf::Color Color, int BlendType)
{
    this->VertexArrayIndex = PRIMITIVE_TYPE_TRIANGLES_FAN;
    this->BlendTypeIndex = BlendType;
    VertexArrays[BlendTypeIndex][VertexArrayIndex].append(sf::Vertex(Position, Color));
    return this;
}

////////////////////////////////////////////////////////////

fi::Mesh_Builder *fi::Mesh_Builder::triangleFan(sf::Vector2f aPosition, sf::Color aColor, sf::Vector2f bPosition, sf::Color bColor, sf::Vector2f cPosition, sf::Color cColor)
{
    this->VertexArrayIndex = PRIMITIVE_TYPE_TRIANGLES_FAN;
    this->BlendTypeIndex = BLEND_MODE_DISABLED;
    VertexArrays[BlendTypeIndex][VertexArrayIndex].append(sf::Vertex(aPosition, aColor));
    VertexArrays[BlendTypeIndex][VertexArrayIndex].append(sf::Vertex(bPosition, bColor));
    VertexArrays[BlendTypeIndex][VertexArrayIndex].append(sf::Vertex(cPosition, cColor));
    return this;
}

////////////////////////////////////////////////////////////

fi::Mesh_Builder *fi::Mesh_Builder::triangleFan(sf::Vector2f aPosition, sf::Color aColor, sf::Vector2f bPosition, sf::Color bColor, sf::Vector2f cPosition, sf::Color cColor, int BlendType)
{
    this->VertexArrayIndex = PRIMITIVE_TYPE_TRIANGLES_FAN;
    this->BlendTypeIndex = BlendType;
    VertexArrays[BlendTypeIndex][VertexArrayIndex].append(sf::Vertex(aPosition, aColor));
    VertexArrays[BlendTypeIndex][VertexArrayIndex].append(sf::Vertex(bPosition, bColor));
    VertexArrays[BlendTypeIndex][VertexArrayIndex].append(sf::Vertex(cPosition, cColor));
    return this;
}

////////////////////////////////////////////////////////////

fi::Mesh_Builder *fi::Mesh_Builder::quad()
{
    this->VertexArrayIndex = PRIMITIVE_TYPE_QUADS;
    this->BlendTypeIndex = BLEND_MODE_DISABLED;
    return this;
}

////////////////////////////////////////////////////////////

fi::Mesh_Builder *fi::Mesh_Builder::quad(int BlendType)
{
    this->VertexArrayIndex = PRIMITIVE_TYPE_QUADS;
    this->BlendTypeIndex = BlendType;
    return this;
}

////////////////////////////////////////////////////////////

fi::Mesh_Builder *fi::Mesh_Builder::quad(sf::Vector2f Position, sf::Color Color)
{
    this->VertexArrayIndex = PRIMITIVE_TYPE_QUADS;
    this->BlendTypeIndex = BLEND_MODE_DISABLED;
    VertexArrays[BlendTypeIndex][VertexArrayIndex].append(sf::Vertex(Position, Color));
    return this;
}

////////////////////////////////////////////////////////////

fi::Mesh_Builder *fi::Mesh_Builder::quad(sf::Vector2f Position, sf::Color Color, int BlendType)
{
    this->VertexArrayIndex = PRIMITIVE_TYPE_QUADS;
    this->BlendTypeIndex = BlendType;
    VertexArrays[BlendTypeIndex][VertexArrayIndex].append(sf::Vertex(Position, Color));
    return this;
}

////////////////////////////////////////////////////////////

fi::Mesh_Builder *fi::Mesh_Builder::quad(sf::Vector2f aPosition, sf::Color aColor, sf::Vector2f bPosition, sf::Color bColor,
                                         sf::Vector2f cPosition, sf::Color cColor, sf::Vector2f dPosition, sf::Color dColor)
{
    this->VertexArrayIndex = PRIMITIVE_TYPE_QUADS;
    this->BlendTypeIndex = BLEND_MODE_DISABLED;
    VertexArrays[BlendTypeIndex][VertexArrayIndex].append(sf::Vertex(aPosition, aColor));
    VertexArrays[BlendTypeIndex][VertexArrayIndex].append(sf::Vertex(bPosition, bColor));
    VertexArrays[BlendTypeIndex][VertexArrayIndex].append(sf::Vertex(cPosition, cColor));
    VertexArrays[BlendTypeIndex][VertexArrayIndex].append(sf::Vertex(dPosition, dColor));
    return this;
}

////////////////////////////////////////////////////////////

fi::Mesh_Builder *fi::Mesh_Builder::quad(sf::Vector2f aPosition, sf::Color aColor, sf::Vector2f bPosition, sf::Color bColor,
                                         sf::Vector2f cPosition, sf::Color cColor, sf::Vector2f dPosition, sf::Color dColor,
                                         int BlendType)

{

    this->VertexArrayIndex = PRIMITIVE_TYPE_QUADS;
    this->BlendTypeIndex = BlendType;
    VertexArrays[BlendTypeIndex][VertexArrayIndex].append(sf::Vertex(aPosition, aColor));
    VertexArrays[BlendTypeIndex][VertexArrayIndex].append(sf::Vertex(bPosition, bColor));
    VertexArrays[BlendTypeIndex][VertexArrayIndex].append(sf::Vertex(cPosition, cColor));
    VertexArrays[BlendTypeIndex][VertexArrayIndex].append(sf::Vertex(dPosition, dColor));
    return this;
}

////////////////////////////////////////////////////////////

fi::Mesh_Builder *fi::Mesh_Builder::append(sf::Vector2f Position, sf::Color Color)
{
    VertexArrays[BlendTypeIndex][VertexArrayIndex].append(sf::Vertex(Position, Color));
    return this;
}

////////////////////////////////////////////////////////////

void fi::Mesh_Builder::drawToRenderTarget(sf::RenderTarget *RenderTarget)
{
    for (int i = 0; i < PRIMITIVE_TYPE_COUNT; i++)
    {
        if (VertexArrays[BLEND_MODE_DISABLED][i].getVertexCount())
        {
            RenderTarget->draw(VertexArrays[BLEND_MODE_DISABLED][i]);
        }
        if (VertexArrays[BLEND_MODE_NONE][i].getVertexCount())
        {
            RenderTarget->draw(VertexArrays[BLEND_MODE_NONE][i], sf::BlendNone);
        }
        if (VertexArrays[BLEND_MODE_MULTIPLY][i].getVertexCount())
        {
            RenderTarget->draw(VertexArrays[BLEND_MODE_MULTIPLY][i], sf::BlendMultiply);
        }
        if (VertexArrays[BLEND_MODE_ADD][i].getVertexCount())
        {
            RenderTarget->draw(VertexArrays[BLEND_MODE_ADD][i], sf::BlendAdd);
        }
        if (VertexArrays[BLEND_MODE_ALPHA][i].getVertexCount())
        {
            RenderTarget->draw(VertexArrays[BLEND_MODE_ALPHA][i], sf::BlendAlpha);
        }
    }
}

////////////////////////////////////////////////////////////

void fi::Mesh_Builder::clear()
{
    for (int i = 0; i < VertexArrays.size(); i++)
    {
        for (int j = 0; j < VertexArrays[i].size(); j++)
        {
            VertexArrays[i][j].clear();
        }
    }
}

////////////////////////////////////////////////////////////
