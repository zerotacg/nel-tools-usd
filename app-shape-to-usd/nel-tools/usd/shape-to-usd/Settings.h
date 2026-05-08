#ifndef NEL_TOOLS_USD_SETTINGS_H
#define NEL_TOOLS_USD_SETTINGS_H

#include <string>
#include <vector>

#include <nel/misc/cmd_args.h>

#include <nel-tools/usd/mesh-converter/Converter.h>

namespace nel_tools::usd::shape_to_usd
{
    struct Settings
    {
        const std::string input;
        const std::string output;

        const struct
        {
            const std::vector<std::string> searchPaths;
        } assets;

        const Converter::Settings texture;

        static Settings from(const NLMISC::CCmdArgs& args)
        {
            return {
                .input = args.getAdditionalArg("input").front(),
                .output = args.getAdditionalArg("output").front(),
                .assets = {.searchPaths = args.getLongArg("add-search-path")},
                .texture = {
                    .convertToLowerCase = args.haveLongArg("texture-file-to-lower-case"),
                    .replaceExtension = args.haveLongArg("texture-file-replace-extension"),
                    .extension = firstOrEmpty(args.getLongArg("texture-file-replace-extension"))
                }
            };
        }

        static std::string firstOrEmpty(const std::vector<std::string>& values)
        {
            if (values.empty())
            {
                return {};
            }

            return values.front();
        }
    };
}

#endif //NEL_TOOLS_USD_SETTINGS_H
