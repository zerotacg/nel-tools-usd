module;
#include <memory>

#include <nel/3d/shape.h>
#include <pxr/usd/usd/stage.h>

module nel_tools.usd.usd_to_mesh.convert;
import nel_tools.usd.usd_to_mesh.convert.Converter;
import nel_tools.usd.usd_to_mesh.Settings;

namespace nel_tools::usd::usd_to_mesh::convert
{
    auto convert(const Settings& settings, const pxr::UsdStageRefPtr& source) -> std::unique_ptr<NL3D::IShape>
    {
        Converter converter(settings.unused, source);
        return converter.convert();
    }
}
