#ifdef WIN32
#include "Platform/Image.h"
ImTextureID Image::ToHandle() const { 
	return reinterpret_cast<ImTextureID>(static_cast<unsigned __int64>(TextureId));
}
#endif
