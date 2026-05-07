#include <nel-tools/usd/mesh-converter/ConverterCMesh.h>

#include <fmt/color.h>
#include <fmt/format.h>
#include <nel/misc/common.h>
#include <nel/misc/path.h>
#include <pxr/usd/ar/resolver.h>
#include <pxr/usd/kind/registry.h>
#include <pxr/usd/usd/modelAPI.h>
#include <pxr/usd/usdGeom/mesh.h>
#include <pxr/usd/usdGeom/metrics.h>
#include <pxr/usd/usdGeom/modelAPI.h>
#include <pxr/usd/usdGeom/primvarsAPI.h>
#include <pxr/usd/usdGeom/scope.h>
#include <pxr/usd/usdGeom/subset.h>
#include <pxr/usd/usdGeom/tokens.h>
#include <pxr/usd/usdGeom/xform.h>
#include <pxr/usd/usdShade/material.h>
#include <pxr/usd/usdShade/materialBindingAPI.h>
#include <pxr/usd/usdShade/shader.h>
#include <pxr/usd/usdShade/tokens.h>

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


void ConverterCMesh::convert()
{
    UsdGeomSetStageUpAxis(stage, UsdGeomTokens->z);
    auto modelRoot = UsdGeomXform::Define(stage, SdfPath("/root"));
    UsdModelAPI(modelRoot).SetKind(KindTokens->component);
    auto outMesh = UsdGeomMesh::Define(stage, meshPath);

    outMesh.CreatePointsAttr().Set(convertVertices(mesh->getVertexBuffer()));
    outMesh.CreateNormalsAttr().Set(convertNormals(mesh->getVertexBuffer()));
    UsdGeomPrimvarsAPI(outMesh)
        .CreatePrimvar(Tokens.st, SdfValueTypeNames->TexCoord2fArray, UsdGeomTokens->varying)
        .Set(convertUVs(mesh->getVertexBuffer()));


    outMesh.GetPrim().ApplyAPI<UsdShadeMaterialBindingAPI>();
    auto material = UsdShadeMaterial::Define(stage, SdfPath("/root/_materials/material_0_MAT"));
    auto materialBindingAPI = UsdShadeMaterialBindingAPI(outMesh);
    materialBindingAPI.Bind(material);
    materialBindingAPI.SetMaterialBindSubsetsFamilyType(UsdGeomTokens->nonOverlapping);


    convertMaterials();
    convertSubsets(meshPath);

    auto indices = convertIndices();
    outMesh.CreateFaceVertexIndicesAttr().Set(indices);
    outMesh.CreateFaceVertexCountsAttr().Set(convertFaceCount(indices));
}

VtArray<GfVec3f> ConverterCMesh::convertVertices(const CVertexBuffer& source) const
{
    VtArray<GfVec3f> value;
    CVertexBufferRead vertexBufferRead;
    source.lock(vertexBufferRead);

    for (auto i = 0; i < source.getNumVertices(); ++i)
    {
        auto vertex = *vertexBufferRead.getVertexCoordPointer(i);
        value.emplace_back(vertex.x, vertex.y, vertex.z);
    }

    return value;
}

VtArray<GfVec3f> ConverterCMesh::convertNormals(const CVertexBuffer& source) const
{
    VtArray<GfVec3f> value;
    CVertexBufferRead vertexBufferRead;
    source.lock(vertexBufferRead);

    for (auto i = 0; i < source.getNumVertices(); ++i)
    {
        auto normal = *vertexBufferRead.getNormalCoordPointer(i);
        value.emplace_back(normal.x, normal.y, normal.z);
    }

    return value;
}

VtArray<GfVec2f> ConverterCMesh::convertUVs(const CVertexBuffer& source) const
{
    VtArray<GfVec2f> value;
    CVertexBufferRead vertexBufferRead;
    source.lock(vertexBufferRead);

    for (auto i = 0; i < source.getNumVertices(); ++i)
    {
        auto uv = *vertexBufferRead.getTexCoordPointer(i);
        value.emplace_back(uv.U, uv.V);
    }

    return value;
}

VtArray<int> ConverterCMesh::convertIndices() const
{
    VtArray<int> value;
    for (auto renderPass = 0; renderPass < mesh->getNbRdrPass(lodId); ++renderPass)
    {
        auto indexBuffer = mesh->getRdrPassPrimitiveBlock(lodId, renderPass);

        CIndexBufferRead indexBufferRead;
        indexBuffer.lock(indexBufferRead);

        for (auto i = 0; i < indexBuffer.getNumIndexes(); ++i)
        {
            if (uint32 idx = getIndexAt(indexBufferRead, i); idx != -1)
            {
                value.emplace_back(idx);
            }
        }
    }

    return value;
}

VtArray<int> ConverterCMesh::convertFaceCount(VtArray<int>& source) const
{
    VtArray<int> target;

    for (auto i = 0; i < source.size(); i += 3)
    {
        target.emplace_back(3);
    }

    return target;
}

VtArray<int> ConverterCMesh::convertFaceIndices(const CIndexBuffer& source, int offset) const
{
    VtArray<int> target;
    CIndexBufferRead indexBufferRead;
    source.lock(indexBufferRead);

    for (auto i = 0; i < source.getNumIndexes() / 3; ++i)
    {
        target.emplace_back(offset + i);
    }

    return target;
}

void ConverterCMesh::convertSubsets(const SdfPath& root)
{
    int faceCount = 0;
    for (auto renderPass = 0; renderPass < mesh->getNbRdrPass(lodId); ++renderPass)
    {
        convertSubset(root, renderPass, faceCount);
    }
}

void ConverterCMesh::convertSubset(const SdfPath& root, uint renderPass, int faceOffset)
{
    auto materialIndex = mesh->getRdrPassMaterial(lodId, renderPass);
    auto indexBuffer = mesh->getRdrPassPrimitiveBlock(lodId, renderPass);
    auto material = defineMaterial(materialIndex);

    auto subset = UsdGeomSubset::Define(stage, root.AppendPath(SdfPath(fmt::format("subset_{}", renderPass))));

    subset.CreateElementTypeAttr().Set(UsdGeomTokens->point);
    subset.CreateFamilyNameAttr().Set(UsdShadeTokens->materialBind);
    subset.CreateIndicesAttr().Set(convertFaceIndices(indexBuffer, faceOffset));
    subset.GetPrim().ApplyAPI<UsdShadeMaterialBindingAPI>();
    UsdShadeMaterialBindingAPI(subset).Bind(material);
}

void ConverterCMesh::convertMaterials()
{
    UsdGeomScope::Define(stage, SdfPath("/root/_materials"));
    for (auto renderPass = 0; renderPass < mesh->getNbRdrPass(lodId); ++renderPass)
    {
        auto materialIndex = mesh->getRdrPassMaterial(lodId, renderPass);
        auto material = convert(mesh->getMaterial(materialIndex), materialIndex);
    }
}

VtArray<int> ConverterCMesh::convert(const CIndexBuffer& source) const
{
    VtArray<int> target;
    CIndexBufferRead indexBufferRead;
    source.lock(indexBufferRead);

    for (auto i = 0; i < source.getNumIndexes(); ++i)
    {
        if (uint32 idx = getIndexAt(indexBufferRead, i); idx != -1)
        {
            target.emplace_back(idx);
        }
    }

    return target;
}

UsdShadeMaterial ConverterCMesh::convert(CMaterial& source, uint32 materialIndex)
{
    auto material = defineMaterial(materialIndex);
    auto materialPath = material.GetPath();
    auto pbrShader = UsdShadeShader::Define(stage, materialPath.AppendPath(SdfPath("PBRShader")));
    pbrShader.CreateIdAttr().Set(TfToken("UsdPreviewSurface"));
    material.CreateSurfaceOutput().ConnectToSource(pbrShader.ConnectableAPI(), UsdShadeTokens->surface);
    auto stInput = material.CreateInput(Tokens.frame_stPrimvarName, SdfValueTypeNames->String);
    stInput.Set(stPrimvarName);

    auto uvmap = UsdShadeShader::Define(stage, materialPath.AppendPath(SdfPath("uvmap")));
    uvmap.CreateIdAttr().Set(TfToken("UsdPrimvarReader_float2"));
    uvmap.CreateInput(Tokens.varname, SdfValueTypeNames->String).ConnectToSource(stInput);
    uvmap.CreateOutput(Tokens.result, SdfValueTypeNames->Float2);

    if (source.getBlend())
    {
        nlinfo("Material Blend");
    }
    for (auto textureIndex = 0; textureIndex < IDRV_MAT_MAXTEXTURES; ++textureIndex)
    {
        if (source.texturePresent(textureIndex))
        {
            auto sourceTexture = source.getTexture(textureIndex);
            nlinfo("Texture at index %i is %s", textureIndex, sourceTexture->getClassName().c_str());
            auto sampler = convert(materialPath, sourceTexture, textureIndex);
        }
    }

    return material;
}

UsdShadeShader ConverterCMesh::convert(SdfPath& root, ITexture* source, uint32 index)
{
    if (const auto specific = dynamic_cast<CTextureFile*>(source))
    {
        return convert(root, *specific, index);
    }
    else if (const auto specific = dynamic_cast<CTextureMultiFile*>(source))
    {
        nlinfo("CTextureMultiFile count %i", specific->getNumFileName());
        for (auto i = 0; i < specific->getNumFileName(); ++i)
        {
            const auto& fileName = specific->getFileName(i);
            nlinfo("CTextureMultiFile %i %s ", i, fileName.c_str());
        }
    }
    else if (const auto specific = dynamic_cast<CTextureCube*>(source))
    {
        nlinfo("CTextureCube");
    }
    else
    {
        nlwarning("Texture type not supported", source->getClassName().c_str());
    }

    return UsdShadeShader::Define(stage, root.AppendPath(SdfPath(fmt::format("texture_{}", index))));
}

UsdShadeShader ConverterCMesh::convert(SdfPath& root, CTextureFile& source, uint32 index)
{
    auto fileName = source.getFileName();
    nlinfo("CTextureFile %s", fileName.c_str());
    fileName = toLower(fileName);
    fileName = CFile::getFilenameWithoutExtension(fileName);
    fileName += ".";
    fileName += "png";

    auto sampler = UsdShadeShader::Define(stage, root.AppendPath(SdfPath(fmt::format("texture_{}", index))));
    sampler.CreateIdAttr().Set(TfToken("UsdUVTexture"));
    sampler.CreateInput(Tokens.file, SdfValueTypeNames->Asset).Set(SdfAssetPath(fileName));
    sampler.CreateInput(Tokens.st, SdfValueTypeNames->Float2);
    sampler.CreateOutput(Tokens.rgb, SdfValueTypeNames->Float3);

    return sampler;
}

UsdShadeMaterial ConverterCMesh::defineMaterial(uint32 materialIndex)
{
    return UsdShadeMaterial::Define(stage, SdfPath(fmt::format("/root/_materials/material_{}_MAT", materialIndex)));
}
