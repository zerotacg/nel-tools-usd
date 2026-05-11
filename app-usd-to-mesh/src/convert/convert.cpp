module;
#include <memory>
#include <vector>

#include <nel/3d/shape.h>
#include <nel/misc/vector.h>
#include <pxr/base/gf/vec3f.h>
#include <pxr/base/vt/array.h>
#include <pxr/usd/usd/attribute.h>
#include <pxr/usd/usd/stage.h>

module nel_tools.usd.usd_to_mesh.convert;
import nel_tools.usd.usd_to_mesh.convert.Converter;
import nel_tools.usd.usd_to_mesh.Settings;

namespace nel_tools::usd::usd_to_mesh::convert
{
    auto convert(const Settings& settings, const pxr::UsdStageRefPtr& source) -> std::unique_ptr<NL3D::IShape>
    {
        Converter converter(settings.unused, source);
        return converter.run();
    }

    auto convert(const pxr::VtArray<pxr::GfVec3f>& source) -> std::vector<NLMISC::CVector>
    {
        std::vector<NLMISC::CVector> target;
        target.reserve(source.size());
        for (auto element : source)
        {
            target.emplace_back(element[0], element[1], element[2]);
        }

        return target;
    }

    auto convertVector(const pxr::UsdAttribute& source) -> std::vector<NLMISC::CVector>
    {
        std::vector<NLMISC::CVector> target;
        pxr::VtArray<pxr::GfVec3f> temp;
        source.Get(&temp);

        return convert(temp);
    }
}
