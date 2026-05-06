#include <nel-tools/usd/mesh-converter/ConverterCMesh.h>

#include <fmt/color.h>
#include <pxr/usd/usd/modelAPI.h>
#include <pxr/usd/usdGeom/metrics.h>
#include <pxr/usd/usdGeom/mesh.h>
#include <pxr/usd/usdGeom/tokens.h>
#include <pxr/usd/usdGeom/xform.h>

using namespace NL3D;
using namespace NLMISC;
using namespace std;
using namespace pxr;

uint32 getIndexAt(const CIndexBufferRead& buffer, const int index)
{
    switch (buffer.getFormat())
    {
    case CIndexBuffer::Indices32:
        return *(static_cast<const uint32*>(buffer.getPtr()) + index);
    case CIndexBuffer::Indices16:
    default:
        return *(static_cast<const uint16*>(buffer.getPtr()) + index);
    }
}


void ConverterCMesh::convert(UsdStageRefPtr& output)
{
    UsdGeomSetStageUpAxis(output, UsdGeomTokens->z);
    auto modelRoot = UsdGeomXform::Define(output, SdfPath("/shape"));
    auto outMesh = UsdGeomMesh::Define(output, SdfPath("/shape/mesh"));

    outMesh.CreatePointsAttr().Set(convertVertices());

    auto indices = convertIndices();
    outMesh.CreateFaceVertexIndicesAttr().Set(indices);
    outMesh.CreateFaceVertexCountsAttr().Set(convertFaceCount(indices));
}

VtArray<GfVec3f> ConverterCMesh::convertVertices() const
{
    CVertexBuffer vertexBuffer = mesh->getVertexBuffer();
    CVertexBufferRead vertexBufferRead;
    vertexBuffer.lock(vertexBufferRead);
    VtArray<GfVec3f> value;
    for (auto i = 0; i < vertexBuffer.getNumVertices(); ++i)
    {
        auto vertex = *vertexBufferRead.getVertexCoordPointer(i);
        value.emplace_back(vertex.x, vertex.y, vertex.z);
    }
    return value;
}

VtArray<int> ConverterCMesh::convertIndices() const
{
    VtArray<int> value;
    for (auto renderPass = 0; renderPass < mesh->getNbRdrPass(lodId); ++renderPass)
    {
        auto indexBuffer = mesh->getRdrPassPrimitiveBlock(lodId, renderPass);
        auto materialIndex = mesh->getRdrPassMaterial(lodId, renderPass);
        auto material = mesh->getMaterial(materialIndex);
        nlinfo("RenderPasss %i Elements %i Material %i", renderPass, indexBuffer.getNumIndexes(), materialIndex);

        CIndexBufferRead indexBufferRead;
        indexBuffer.lock(indexBufferRead);

        for (auto i = 0; i < indexBuffer.getNumIndexes(); ++i)
        {
            if (uint32 idx = getIndexAt(indexBufferRead, i); idx != -1)
            {
                value.emplace_back(idx);
            }
        }
        nldebug("index min %i max %i", *min_element(value.begin(), value.end()),
                *max_element(value.begin(), value.end()));
    }

    return value;
}

VtArray<int> ConverterCMesh::convertFaceCount(VtArray<int> indices) const
{
    VtArray<int> value;
    for (auto i = 0; i < indices.size(); i += 3)
    {
        value.emplace_back(3);
    }
    return value;
}
