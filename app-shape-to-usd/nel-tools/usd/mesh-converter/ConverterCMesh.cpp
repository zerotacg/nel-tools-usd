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
    auto outMesh = UsdGeomMesh::Define(stage, SdfPath("/root/model"));

    outMesh.CreatePointsAttr().Set(convertVertices());
    outMesh.CreateNormalsAttr().Set(convertNormals());
    UsdGeomPrimvarsAPI(outMesh)
        .CreatePrimvar(Tokens.st, SdfValueTypeNames->TexCoord2fArray, UsdGeomTokens->varying)
        .Set(convertUVs());


    auto material = UsdShadeMaterial::Define(stage, SdfPath("/root/model/materialMAT"));
    auto pbrShader = UsdShadeShader::Define(stage, SdfPath("/root/model/materialMAT/PBRShader"));
    pbrShader.CreateIdAttr().Set(TfToken("UsdPreviewSurface"));
    material.CreateSurfaceOutput().ConnectToSource(pbrShader.ConnectableAPI(), UsdShadeTokens->surface);

    auto stReader = UsdShadeShader::Define(stage, SdfPath("/root/model/materialMAT/stReader"));
    stReader.CreateIdAttr().Set(TfToken("UsdPrimvarReader_float2"));

    auto diffuseTextureSampler = UsdShadeShader::Define(stage, SdfPath("/root/model/materialMAT/diffuseTexture"));
    diffuseTextureSampler.CreateIdAttr().Set(TfToken("UsdUVTexture"));
    auto& asserResolver = ArGetResolver();
    auto fileAsset = asserResolver.ResolveForNewAsset("textures/ca_ship_front1.png");
    if (fileAsset.IsEmpty())
    {
        fmt::print(fg(fmt::color::red), "Could not resolve asset {}\n", fileAsset.GetPathString());
    }
    else
    {
        fmt::print(fg(fmt::color::forest_green), "Could resolve asset {}\n", fileAsset.GetPathString());
    }
    diffuseTextureSampler.CreateInput(Tokens.file, SdfValueTypeNames->Asset).Set(
        SdfAssetPath("textures/ca_ship_front1.png"));
    diffuseTextureSampler.CreateInput(Tokens.st, SdfValueTypeNames->Float2).ConnectToSource(
        stReader.ConnectableAPI(), Tokens.result);
    diffuseTextureSampler.CreateOutput(Tokens.rgb, SdfValueTypeNames->Float3);
    pbrShader.CreateInput(Tokens.diffuseColor, SdfValueTypeNames->Color3f).ConnectToSource(
        diffuseTextureSampler.ConnectableAPI(), Tokens.rgb);
    auto stInput = material.CreateInput(Tokens.frame_stPrimvarName, SdfValueTypeNames->Token);
    stInput.Set(Tokens.st);
    stReader.CreateInput(Tokens.varname, SdfValueTypeNames->Token).ConnectToSource(stInput);
    outMesh.GetPrim().ApplyAPI<UsdShadeMaterialBindingAPI>();
    UsdShadeMaterialBindingAPI(outMesh).Bind(material);

    convertMaterials();

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

VtArray<GfVec3f> ConverterCMesh::convertNormals() const
{
    CVertexBuffer vertexBuffer = mesh->getVertexBuffer();
    CVertexBufferRead vertexBufferRead;
    vertexBuffer.lock(vertexBufferRead);
    VtArray<GfVec3f> value;

    for (auto i = 0; i < vertexBuffer.getNumVertices(); ++i)
    {
        auto normal = *vertexBufferRead.getNormalCoordPointer(i);
        value.emplace_back(normal.x, normal.y, normal.z);
    }

    return value;
}

VtArray<GfVec2f> ConverterCMesh::convertUVs() const
{
    CVertexBuffer vertexBuffer = mesh->getVertexBuffer();
    CVertexBufferRead vertexBufferRead;
    vertexBuffer.lock(vertexBufferRead);
    VtArray<GfVec2f> value;

    for (auto i = 0; i < vertexBuffer.getNumVertices(); ++i)
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

void ConverterCMesh::convertMaterials()
{
    UsdGeomScope::Define(stage, SdfPath("/root/_materials"));
    for (auto renderPass = 0; renderPass < mesh->getNbRdrPass(lodId); ++renderPass)
    {
        auto materialIndex = mesh->getRdrPassMaterial(lodId, renderPass);
        auto material = convert( mesh->getMaterial(materialIndex), materialIndex);
    }
}

UsdShadeMaterial ConverterCMesh::convert(CMaterial& source, uint32 materialIndex)
{
    auto materialPath = SdfPath(fmt::format("/root/_materials/material_{}_MAT", materialIndex));
    auto material = UsdShadeMaterial::Define(stage, materialPath);
    auto pbrShader = UsdShadeShader::Define(stage, materialPath.AppendPath(SdfPath("PBRShader")));
    pbrShader.CreateIdAttr().Set(TfToken("UsdPreviewSurface"));
    material.CreateSurfaceOutput().ConnectToSource(pbrShader.ConnectableAPI(), UsdShadeTokens->surface);
    material.CreateInput(Tokens.frame_stPrimvarName, SdfValueTypeNames->Token).Set(Tokens.st);

    auto uvmap = UsdShadeShader::Define(stage, materialPath.AppendPath(SdfPath("uvmap")));
    uvmap.CreateIdAttr().Set(TfToken("UsdPrimvarReader_float2"));
    uvmap.CreateInput(Tokens.varname, SdfValueTypeNames->Token);
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
            auto sampler = convert( materialPath, sourceTexture, textureIndex);
        }
    }

    return material;
}

UsdShadeShader ConverterCMesh::convert(SdfPath& root, ITexture* source, uint32 index)
{
    if (const auto specific = dynamic_cast<CTextureFile *>(source))
    {
        return convert(root, *specific, index);
    }
    else if (const auto specific = dynamic_cast<CTextureMultiFile *>(source))
    {
        nlinfo("CTextureMultiFile count %i", specific->getNumFileName());
        for (auto i = 0; i < specific->getNumFileName(); ++i)
        {
            const auto &fileName = specific->getFileName(i);
            nlinfo("CTextureMultiFile %i %s ", i, fileName.c_str());
        }
    }
    else if (const auto specific = dynamic_cast<CTextureCube *>(source))
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
    sampler.CreateInput(Tokens.file, SdfValueTypeNames->Asset).Set( SdfAssetPath(fileName));
    sampler.CreateInput(Tokens.st, SdfValueTypeNames->Float2);
    sampler.CreateOutput(Tokens.rgb, SdfValueTypeNames->Float3);

    return sampler;
}