#ifndef MESH_MRM_PROCESSOR_H
#define MESH_MRM_PROCESSOR_H

#include <nel/3d/mesh.h>

#include <nel-tools/usd/mesh-converter/Converter.h>
#include <pxr/usd/usdGeom/mesh.h>

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
    pxr::VtArray<int> convertIndices() const;
    pxr::VtArray<int> convertFaceCount(pxr::VtArray<int> indices) const;

private:
    NL3D::CMesh* mesh;

    const uint lodId = 0;
};

#endif // MESH_MRM_PROCESSOR_H
