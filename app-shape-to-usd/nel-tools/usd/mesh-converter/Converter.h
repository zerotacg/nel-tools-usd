#ifndef MESH_CONVERTER_H
#define MESH_CONVERTER_H

#include <memory>

#include <pxr/usd/usd/stage.h>
#include <nel/3d/mesh.h>

class Converter
{
public:
	virtual ~Converter() = default;

	static void from(pxr::UsdStageRefPtr& target, NL3D::IShape *shape, NL3D::IShape *skeleton);
protected:
	explicit Converter(pxr::UsdStageRefPtr& target)
		: stage(target)
	{
	}

	virtual void convert() = 0;

	pxr::UsdStageRefPtr& stage;
};

#endif // MESH_CONVERTER_H
