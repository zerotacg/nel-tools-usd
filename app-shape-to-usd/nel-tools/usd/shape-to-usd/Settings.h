#ifndef NEL_TOOLS_USD_SETTINGS_H
#define NEL_TOOLS_USD_SETTINGS_H

#include <nel/misc/cmd_args.h>

namespace nel_tools::usd::shape_to_usd
{
    struct Settings
    {
        std::string input;
        std::string output;

        static Settings from(const NLMISC::CCmdArgs& args)
        {
            return {
                .input = args.getAdditionalArg("input").front(),
                .output = args.getAdditionalArg("output").front(),
            };
        }
    };
}

#endif //NEL_TOOLS_USD_SETTINGS_H
