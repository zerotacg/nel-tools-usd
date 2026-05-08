#ifndef MESH_CONVERTER_H
#define MESH_CONVERTER_H

#include <pxr/usd/usd/stage.h>
#include <nel/3d/mesh.h>

class Converter
{
public:
    virtual ~Converter() = default;

    const struct Settings
    {
        const bool convertToLowerCase;
        const bool replaceExtension;
        const std::string extension;
        const std::string prefix;
    }& settings;

    static void from(const Settings& settings,pxr::UsdStageRefPtr& target, NL3D::IShape* shape, NL3D::IShape* skeleton);

protected:
    explicit Converter(const Settings& settings, pxr::UsdStageRefPtr& target)
        : settings(settings), stage(target)
    {
    }

    virtual void convert() = 0;

    pxr::UsdStageRefPtr& stage;
};

#endif // MESH_CONVERTER_H
