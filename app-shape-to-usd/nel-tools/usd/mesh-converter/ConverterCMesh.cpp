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
#include <pxr/usd/usdHydra/tokens.h>
#include <pxr/usd/usdShade/material.h>
#include <pxr/usd/usdShade/materialBindingAPI.h>
#include <pxr/usd/usdShade/shader.h>
#include <pxr/usd/usdShade/tokens.h>

#include <nel-tools/usd/convert/convert.h>

using namespace NL3D;
using namespace NLMISC;
using namespace std;
using namespace pxr;
using namespace nel_tools::usd;

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
    UsdGeomSetStageMetersPerUnit(stage, 1.0);
    auto modelRoot = UsdGeomXform::Define(stage, Paths.root);
    UsdModelAPI(modelRoot).SetKind(KindTokens->component);
    auto outMesh = UsdGeomMesh::Define(stage, Paths.model);

    outMesh.CreatePointsAttr().Set(convert::vertices(mesh->getVertexBuffer()));
    outMesh.CreateNormalsAttr().Set(convert::normals(mesh->getVertexBuffer()));
    UsdGeomPrimvarsAPI(outMesh)
        .CreatePrimvar(Tokens.st, SdfValueTypeNames->TexCoord2fArray, UsdGeomTokens->varying)
        .Set(convert::uvs(mesh->getVertexBuffer()));


    outMesh.GetPrim().ApplyAPI<UsdShadeMaterialBindingAPI>();
    auto material = defineMaterial(0);
    auto materialBindingAPI = UsdShadeMaterialBindingAPI(outMesh);
    materialBindingAPI.Bind(material);
    materialBindingAPI.SetMaterialBindSubsetsFamilyType(UsdGeomTokens->nonOverlapping);


    convertMaterials();
    convertSubsets(Paths.model);

    auto indices = convertIndices();
    outMesh.CreateFaceVertexIndicesAttr().Set(indices);
    outMesh.CreateFaceVertexCountsAttr().Set(convertFaceCount(indices));
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
        // TODO handle -1 indices
        target.emplace_back(offset + i);
    }

    return target;
}

void ConverterCMesh::convertSubsets(const SdfPath& root)
{
    int faceCount = 0;
    for (auto renderPass = 0; renderPass < mesh->getNbRdrPass(lodId); ++renderPass)
    {
        faceCount += convertSubset(root, renderPass, faceCount);
    }
}

size_t ConverterCMesh::convertSubset(const SdfPath& root, uint renderPass, int faceOffset)
{
    auto materialIndex = mesh->getRdrPassMaterial(lodId, renderPass);
    auto indexBuffer = mesh->getRdrPassPrimitiveBlock(lodId, renderPass);
    auto material = defineMaterial(materialIndex);

    auto subset = UsdGeomSubset::Define(stage, root.AppendPath(SdfPath(fmt::format("subset_{}", renderPass))));

    subset.CreateElementTypeAttr().Set(UsdGeomTokens->face);
    subset.CreateFamilyNameAttr().Set(UsdShadeTokens->materialBind);
    auto faceIndices = convertFaceIndices(indexBuffer, faceOffset);
    subset.CreateIndicesAttr().Set(faceIndices);
    subset.GetPrim().ApplyAPI<UsdShadeMaterialBindingAPI>();
    UsdShadeMaterialBindingAPI(subset).Bind(material);

    return faceIndices.size();
}

void ConverterCMesh::convertMaterials()
{
    UsdGeomScope::Define(stage, Paths.materials);
    for (auto i = 0; i < mesh->getNbMaterial(); ++i)
    {
        auto target = defineMaterial(i);
        convert(target, mesh->getMaterial(i));
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

void ConverterCMesh::convert(UsdShadeMaterial& target, const CMaterial& source)
{
    auto root = target.GetPath();
    auto pbrShader = UsdShadeShader::Define(stage, root.AppendPath(SdfPath("PBRShader")));
    pbrShader.CreateIdAttr().Set(TfToken("UsdPreviewSurface"));
    target.CreateSurfaceOutput().ConnectToSource(pbrShader.ConnectableAPI(), UsdShadeTokens->surface);

    auto uvmap = UsdShadeShader::Define(stage, root.AppendPath(SdfPath("uvmap")));
    uvmap.CreateIdAttr().Set(TfToken("UsdPrimvarReader_float2"));
    uvmap.CreateInput(Tokens.varname, SdfValueTypeNames->String).Set(stPrimvarName);
    auto uvmapResult = uvmap.CreateOutput(Tokens.result, SdfValueTypeNames->Float2);

    if (source.getDoubleSided())
    {
        nldebug("Material DoubleSided");
    }
    if (source.getBlend())
    {
        nldebug("Material Blend");
    }
    for (auto textureIndex = 0; textureIndex < IDRV_MAT_MAXTEXTURES; ++textureIndex)
    {
        if (source.texturePresent(textureIndex))
        {
            auto sourceTexture = source.getTexture(textureIndex);
            auto sampler = convert(root, sourceTexture, textureIndex);
            sampler.GetInput(Tokens.st).ConnectToSource(uvmapResult);
            pbrShader.CreateInput(Tokens.diffuseColor, SdfValueTypeNames->Color3f).ConnectToSource(
                sampler.ConnectableAPI(), Tokens.rgb);
            pbrShader.CreateInput(Tokens.opacity, SdfValueTypeNames->Float).ConnectToSource(
                sampler.ConnectableAPI(), Tokens.a);
        }
    }
}

UsdShadeShader ConverterCMesh::convert(SdfPath& root, ITexture* source, uint32 index)
{
    nldebug("Texture at index %i is %s", index, source->getClassName().c_str());
    if (const auto specific = dynamic_cast<CTextureFile*>(source))
    {
        return convert(root, *specific, index);
    }
    else if (const auto specific = dynamic_cast<CTextureMultiFile*>(source))
    {
        nldebug("CTextureMultiFile count %i", specific->getNumFileName());
        for (auto i = 0; i < specific->getNumFileName(); ++i)
        {
            const auto& fileName = specific->getFileName(i);
            nldebug("CTextureMultiFile %i %s ", i, fileName.c_str());
        }
    }
    else if (const auto specific = dynamic_cast<CTextureCube*>(source))
    {
        nldebug("CTextureCube");
    }
    else
    {
        nlwarning("Texture type not supported", source->getClassName().c_str());
    }

    return UsdShadeShader::Define(stage, root.AppendPath(SdfPath(fmt::format("texture_{}", index))));
}

UsdShadeShader ConverterCMesh::convert(const SdfPath& root, const CTextureFile& source, uint32 index) const
{
    nldebug("CTextureFile %s", source.getFileName().c_str());
    auto fileName = transformFilename(source.getFileName());
    auto resolvedPath = ArGetResolver().Resolve(fileName);
    if (resolvedPath.IsEmpty())
    {
        fmt::print(fg(fmt::color::red), "Could not resolve asset {}\n", fileName);
    }
    else
    {
        fmt::print(fg(fmt::color::forest_green), "Could resolve asset {} to {}\n", fileName, resolvedPath.GetPathString());
    }
    auto sampler = UsdShadeShader::Define(stage, root.AppendPath(SdfPath(fmt::format("texture_{}", index))));
    sampler.CreateIdAttr().Set(TfToken("UsdUVTexture"));
    sampler.CreateInput(Tokens.file, SdfValueTypeNames->Asset).Set(SdfAssetPath( fileName, resolvedPath.GetPathString()));
    sampler.CreateInput(Tokens.st, SdfValueTypeNames->Float2);
    sampler.CreateInput(UsdHydraTokens->wrapS, SdfValueTypeNames->Token).Set(convert(source.getWrapS()));
    sampler.CreateInput(UsdHydraTokens->wrapT, SdfValueTypeNames->Token).Set(convert(source.getWrapT()));
    sampler.CreateInput(UsdHydraTokens->magFilter, SdfValueTypeNames->Token).Set(convert(source.getMagFilter()));
    sampler.CreateInput(UsdHydraTokens->minFilter, SdfValueTypeNames->Token).Set(convert(source.getMinFilter()));
    sampler.CreateOutput(Tokens.rgb, SdfValueTypeNames->Color3f);

    return sampler;
}

const TfToken& ConverterCMesh::convert(const ITexture::TWrapMode& source) const
{
    switch (source)
    {
    default:
    case ITexture::TWrapMode::Repeat:
        return UsdHydraTokens->repeat;
    case ITexture::TWrapMode::Clamp:
        return UsdHydraTokens->clamp;
    }
}

const TfToken& ConverterCMesh::convert(const ITexture::TMinFilter& source) const
{
    switch (source)
    {
    default:
    case ITexture::NearestMipMapOff:
        return UsdHydraTokens->nearest;
    case ITexture::NearestMipMapNearest:
        return UsdHydraTokens->nearestMipmapNearest;
    case ITexture::NearestMipMapLinear:
        return UsdHydraTokens->nearestMipmapLinear;
    case ITexture::LinearMipMapOff:
        return UsdHydraTokens->linear;
    case ITexture::LinearMipMapNearest:
        return UsdHydraTokens->linearMipmapNearest;
    case ITexture::LinearMipMapLinear:
        return UsdHydraTokens->linearMipmapLinear;
    }
}

const TfToken& ConverterCMesh::convert(const ITexture::TMagFilter& source) const
{
    switch (source)
    {
    default:
    case ITexture::Nearest:
        return UsdHydraTokens->nearest;
    case ITexture::Linear:
        return UsdHydraTokens->linear;
    }
}

UsdShadeMaterial ConverterCMesh::defineMaterial(uint materialIndex)
{
    return UsdShadeMaterial::Define(
        stage, Paths.materials.AppendPath(SdfPath(fmt::format("material_{}_MAT", materialIndex))));
}

std::string ConverterCMesh::transformFilename(const std::string& input) const
{
    auto transformed = input;
    if (settings.convertToLowerCase)
    {
        transformed = toLower(transformed);
    }
    if (settings.replaceExtension)
    {
        transformed = CFile::getFilenameWithoutExtension(transformed);
        transformed += ".";
        transformed += "png";
    }
    return transformed;
}
