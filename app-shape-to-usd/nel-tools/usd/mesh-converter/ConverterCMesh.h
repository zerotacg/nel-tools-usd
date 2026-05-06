#ifndef MESH_MRM_PROCESSOR_H
#define MESH_MRM_PROCESSOR_H

#include <nel/3d/mesh.h>
#include <pxr/base/gf/vec3f.h>
#include <pxr/base/gf/vec2f.h>
#include <pxr/usd/usdGeom/mesh.h>

#include <nel-tools/usd/mesh-converter/Converter.h>

class ConverterCMesh : public Converter
{
public:
    explicit ConverterCMesh(NL3D::CMesh* source)
        : mesh(source)
    {
    }

    void convert(pxr::UsdStageRefPtr& output) override;

protected:
    pxr::VtArray<pxr::GfVec3f> convertVertices() const;
    pxr::VtArray<pxr::GfVec3f> convertNormals() const;
    pxr::VtArray<pxr::GfVec2f> convertUVs() const;
    pxr::VtArray<int> convertIndices() const;
    pxr::VtArray<int> convertFaceCount(pxr::VtArray<int> indices) const;

private:
    NL3D::CMesh* mesh;

    const uint lodId = 0;
};

#endif // MESH_MRM_PROCESSOR_H
