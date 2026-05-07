#ifndef MESH_MRM_PROCESSOR_H
#define MESH_MRM_PROCESSOR_H

#include <string>

#include <nel/3d/material.h>
#include <nel/3d/mesh.h>
#include <nel/3d/texture.h>
#include <nel/3d/texture_cube.h>
#include <nel/3d/texture_file.h>
#include <nel/3d/texture_multi_file.h>
#include <pxr/base/gf/vec3f.h>
#include <pxr/base/gf/vec2f.h>
#include <pxr/usd/usdGeom/mesh.h>
#include <pxr/usd/usdShade/material.h>

#include <nel-tools/usd/mesh-converter/Converter.h>

class ConverterCMesh : public Converter
{
public:
    explicit ConverterCMesh(pxr::UsdStageRefPtr& target, NL3D::CMesh* source)
        : Converter(target), mesh(source)
    {
    }

    void convert() override;

protected:
    pxr::VtArray<pxr::GfVec3f> convertVertices(const NL3D::CVertexBuffer& source) const;
    pxr::VtArray<pxr::GfVec3f> convertNormals(const NL3D::CVertexBuffer& source) const;
    pxr::VtArray<pxr::GfVec2f> convertUVs(const NL3D::CVertexBuffer& source) const;
    pxr::VtArray<int> convertIndices() const;
    pxr::VtArray<int> convertFaceCount(pxr::VtArray<int> indices) const;
    void convertSubsets(const pxr::SdfPath& root);
    void convertSubset(const pxr::SdfPath& root, uint renderPass);
    void convertMaterials();
    pxr::VtArray<int> convert(NL3D::CIndexBuffer& source) const;
    pxr::UsdShadeMaterial convert(NL3D::CMaterial& source, uint32 index);
    pxr::UsdShadeShader convert(pxr::SdfPath& root, NL3D::ITexture* source, uint32 index);
    pxr::UsdShadeShader convert(pxr::SdfPath& root, NL3D::CTextureCube& source, uint32 index);
    pxr::UsdShadeShader convert(pxr::SdfPath& root, NL3D::CTextureFile& source, uint32 index);
    pxr::UsdShadeShader convert(pxr::SdfPath& root, NL3D::CTextureMultiFile& source, uint32 index);

    pxr::UsdShadeMaterial defineMaterial(uint32 index);

private:
    NL3D::CMesh* mesh;


    const uint lodId = 0;
    const pxr::SdfPath meshPath = pxr::SdfPath("/root/model");
    const std::string stPrimvarName = "st";


    struct
    {
        const pxr::TfToken st = pxr::TfToken("st");
        const pxr::TfToken file = pxr::TfToken("file");
        const pxr::TfToken rgb = pxr::TfToken("rgb");
        const pxr::TfToken result = pxr::TfToken("result");
        const pxr::TfToken diffuseColor = pxr::TfToken("diffuseColor");
        const pxr::TfToken varname = pxr::TfToken("varname");
        const pxr::TfToken frame_stPrimvarName = pxr::TfToken("frame:stPrimvarName");
    } Tokens;
};

#endif // MESH_MRM_PROCESSOR_H
