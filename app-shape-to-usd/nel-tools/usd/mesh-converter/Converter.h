#ifndef MESH_CONVERTER_H
#define MESH_CONVERTER_H

#include <memory>

#include <pxr/usd/usd/stage.h>
#include <nel/3d/mesh.h>

class Converter
{
public:
	virtual ~Converter() = default;

	virtual void convert(pxr::UsdStageRefPtr& output) = 0;

	static std::unique_ptr<Converter> from(NL3D::IShape *shape, NL3D::IShape *skeleton);
};

#endif // MESH_CONVERTER_H
