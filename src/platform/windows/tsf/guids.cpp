#include "suborno/platform/windows/tsf/guids.hpp"

#include <initguid.h>

namespace suborno::platform::windows {

// {4F1A2B3C-5D6E-7F8A-9B0C-1D2E3F4A5B6C}
DEFINE_GUID(CLSID_SubornoTextService, 0x4f1a2b3c, 0x5d6e, 0x7f8a, 0x9b, 0x0c, 0x1d, 0x2e, 0x3f,
            0x4a, 0x5b, 0x6c);

// {7A1B2C3D-4E5F-6A7B-8C9D-0E1F2A3B4C5D}
DEFINE_GUID(GUID_LANGUAGE_PROFILE_SUBORNO_BENGALI, 0x7a1b2c3d, 0x4e5f, 0x6a7b, 0x8c, 0x9d, 0x0e,
            0x1f, 0x2a, 0x3b, 0x4c, 0x5d);

// {8F1A2B3C-5D6E-7F8A-9B0C-1D2E3F4A5B6C}
DEFINE_GUID(CATID_TEXT_INPUT_PROCESSOR_SUBORNO, 0x8f1a2b3c, 0x5d6e, 0x7f8a, 0x9b, 0x0c, 0x1d, 0x2e,
            0x3f, 0x4a, 0x5b, 0x6c);

}  // namespace suborno::platform::windows
