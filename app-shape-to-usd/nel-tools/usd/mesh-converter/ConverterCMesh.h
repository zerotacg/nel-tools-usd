#ifndef MESH_MRM_PROCESSOR_H
#define MESH_MRM_PROCESSOR_H

#include <nel/3d/mesh.h>

#include <nel-tools/usd/mesh-converter/Converter.h>

class ConverterCMesh : public Converter
{
public:
	explicit ConverterCMesh(NL3D::CMesh *source)
	    : mesh(source)
	{
	}

	void process(pxr::UsdStageRefPtr& output) override;

private:
	NL3D::CMesh *mesh;
};

#endif // MESH_MRM_PROCESSOR_H
