#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION

#include "Commands.h"

#include <CliParser/CommandRegistry.h>
#include <stb_image.h>
#include <stb_image_write.h>
#include <stb_image_resize2.h>

#include <algorithm>
#include <cmath>
#include <cstring>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <iterator>
#include <optional>
#include <regex>
#include <string>
#include <sstream>
#include <vector>

namespace {
    struct Pos {
        size_t X{0};
        size_t Y{0};
    };

    struct Dims {
        size_t W{0};
        size_t H{0};

        Dims() = default;
        Dims(int w, int h) : W(static_cast<size_t>(w)), H(static_cast<size_t>(h)) {}
        Dims(size_t w, size_t h) : W(w), H(h) {}

        constexpr bool operator==(const Dims& other) const { return W == other.W && H == other.H; }
        constexpr size_t Area() const { return W * H; }
    };

    struct LoadedSprite {
        std::string Name{};
        Pos Pos{};
        Dims Dims{};
        std::vector<std::byte> Pixels{};
    };

    std::string GlobToRegex(std::string_view glob) {
        std::stringstream re;
        re << '^';
        for(char c : glob) {
            switch(c) { 
                case '*': re << ".*"; break;
            case '?': re << '.'; break;
                case '.':
                case '\\':
                case '+':
                case '^':
                case '$':
                case '(':
                case ')':
                case '|':
                case '{':
                case '}':
                    re << "\\";
                    re << c;
                    break;
                default: re << c; break;
            }
        }
        re << '$';
        return re.str();
    }

    bool GetImagePaths(const std::optional<std::string>& inputDir, const std::optional<std::string>& filter, std::vector<std::filesystem::path>& outPaths, std::ostream& err) {
        std::error_code ec;

        if(inputDir.has_value()) {
            auto imgDir = std::filesystem::path(inputDir.value());
            auto it = std::filesystem::directory_iterator(imgDir, ec);
            if(ec) {
                err << "Failed to read directory: " << ec.message() << "\n";
                return false;
            }

            if(filter.has_value()) {
                auto re = std::regex(GlobToRegex(filter.value()));
                for(const auto& entry: it) {
                    if(!entry.is_regular_file(ec) || ec) continue;
                    if(!std::regex_match(entry.path().filename().string(), re)) continue;
                    outPaths.push_back(entry.path());
                }
            } else {
                for(const auto& entry: it) {
                    if(!entry.is_regular_file(ec) || ec) continue;
                    outPaths.push_back(entry.path());
                }
            }
        }
        if(outPaths.empty()) {
            err << "No input images provided.\n";
            return false;
        }

        std::sort(outPaths.begin(), outPaths.end());
        outPaths.erase(std::unique(outPaths.begin(), outPaths.end()), outPaths.end());
        return true;
    }

    Dims GetSpriteDims(Dims dims, const std::optional<size_t>& resize) {
        if(!resize.has_value() || (dims.W <= *resize && dims.H <= *resize)) return dims;
        auto scale = std::min(static_cast<double>(*resize) / dims.W, static_cast<double>(*resize) / dims.H);
        return {
            std::max<size_t>(1, std::lround(dims.W * scale)),
            std::max<size_t>(1, std::lround(dims.H * scale))
        };
    }

    bool TryLoadSprite(const std::filesystem::path& path, const std::optional<size_t>& resize, LoadedSprite& outSprite, std::ostream& err) {
        int w = 0, h = 0, channels = 0;
        auto* data = stbi_load(path.string().c_str(), &w, &h, &channels, 4);
        if(!data) {
            err << "Failed to load image: " << path << "\n";
            return false;
        }
        auto originalDims = Dims(w, h);
        auto dims = GetSpriteDims(originalDims, resize);

        auto size = dims.Area() * 4;
        outSprite.Name = path.stem().string();
        outSprite.Dims = dims;
        outSprite.Pixels.resize(size);

        bool success = true;
        if(dims == originalDims) {
            std::memcpy(outSprite.Pixels.data(), data, size);
        } else {
            success = stbir_resize_uint8_srgb(
                data, w, h, 0, 
                reinterpret_cast<unsigned char*>(outSprite.Pixels.data()), static_cast<int>(dims.W), static_cast<int>(dims.H), 0, 
                STBIR_RGBA) != nullptr;
            if(!success) {
                err << "Failed to resize image: " << path << "\n";
            }
        }
        stbi_image_free(data);
        return success;
    }

    bool LoadSprites(const std::vector<std::filesystem::path>& paths, size_t padding, size_t maxWidth, const std::optional<size_t>& resize, std::vector<LoadedSprite>& outSprites, std::ostream& err) {
        outSprites.reserve(paths.size());
        for(const auto& iput: paths) {
            LoadedSprite sprite;
            if(!TryLoadSprite(iput, resize, sprite, err)) {
                return false;
            }
            if(sprite.Name.find_first_of(" \t\n\r") != std::string::npos) {
                err << "Sprite name '" << sprite.Name << "' contains whitespace.\n";
                return false;
            }
            if(sprite.Dims.W + (2 * padding) > maxWidth) {
                err << "Sprite '" << sprite.Name << "' is too wide for the atlas\n";
                return false;
            }
            outSprites.push_back(std::move(sprite));
        }

        return true;
    }

    void OrientSprites(
        std::vector<LoadedSprite>& sprites, size_t padding, size_t maxWidth, Dims& outDims) {
        // sort by height
        std::sort(sprites.begin(), sprites.end(), [](const auto& a, const auto& b) {
            return a.Dims.H != b.Dims.H ? a.Dims.H > b.Dims.H : a.Name < b.Name;
        });

        Pos pen{padding, padding};
        size_t shelfHeight = 0;
        outDims = {0, 0};
        for(auto& s: sprites) {
            if(pen.X + s.Dims.W + padding > maxWidth) {
                pen.X = padding;
                pen.Y += shelfHeight + padding;
                shelfHeight = 0;
            }
            s.Pos.X = pen.X;
            s.Pos.Y = pen.Y;
            pen.X += s.Dims.W + padding;
            shelfHeight = std::max(shelfHeight, s.Dims.H);
            outDims.W = std::max(outDims.W, pen.X);
            outDims.H = std::max(outDims.H, pen.Y + shelfHeight + padding);
        }
    }

    std::vector<std::byte> CombineSprites(const std::vector<LoadedSprite>& sprites, Dims dims) {
        std::vector<std::byte> atlas(dims.Area() * 4, std::byte(0));
        for(const auto& s : sprites) {
            for(size_t row = 0; row < s.Dims.H; ++row) {
                auto* src = &s.Pixels[row * s.Dims.W * 4];
                auto* dst = &atlas[(((s.Pos.Y + row) * dims.W) + s.Pos.X) * 4];
                std::memcpy(dst, src, s.Dims.W * 4);
            }
        }

        return atlas;
    }

    /////////
    // Split
    /////////
    struct AtlasRegion {
        std::string Name{};
        Pos Pos{};
        Dims Dims{};
    };

    bool ParseMetadata(const std::filesystem::path& txtPath, std::vector<AtlasRegion>& outRegions, std::ostream& err) {
        auto file = std::ifstream(txtPath, std::ios::binary);
        if(!file) {
            err << "Failed to open file: " << txtPath << "\n";
            return false;
        }
        std::string line;
        size_t lineNum = 0;
        while(std::getline(file, line)) {
            ++lineNum;
            if(!line.empty() && line.back() == '\r') line.pop_back();
            if(line.empty()) continue;

            AtlasRegion region;
            std::istringstream stream(line);
            if(!(stream >> region.Name >> region.Pos.X >> region.Pos.Y >> region.Dims.W >> region.Dims.H)) {
                err << "Malformed line: " << lineNum << "\n";
                return false;
            }
            outRegions.push_back(std::move(region));
        }

        if(outRegions.empty()) {
            err << "No regions found\n";
            return false;
        }

        return true;
    }

    bool ExtractRegion(const std::vector<std::byte>& atlas, Dims atlasDims, const AtlasRegion& region, const std::filesystem::path& dest, std::ostream& err) {
        if(region.Dims.W > atlasDims.W || region.Pos.X > atlasDims.W - region.Dims.W ||
            region.Dims.H > atlasDims.H || region.Pos.Y > atlasDims.H - region.Dims.H) {
            err << "Region '" << region.Name << "' is out of atlas bounds\n";
            return false;
        }
        if(region.Dims.Area() == 0) {
            err << "Region '" << region.Name << "' has zero area\n";
            return false;
        }

        auto pixels = std::vector<std::byte>(region.Dims.Area() * 4);
        for(size_t row = 0; row < region.Dims.H; ++row) {
            const auto* src = &atlas[(((region.Pos.Y + row) * atlasDims.W) + region.Pos.X) * 4];
            auto* dst = &pixels[row * region.Dims.W * 4];
            std::memcpy(dst, src, region.Dims.W * 4);
        }

        if(!stbi_write_png(
               dest.string().c_str(), // filename
               static_cast<int>(region.Dims.W), // width
               static_cast<int>(region.Dims.H), // height
               4, // channels
               pixels.data(), // data
               static_cast<int>(region.Dims.W * 4) // stride
           )) {
            err << "Failed to write image: " << dest << "\n";
            return false;
        }

        return true;
    }
}

struct BuildAtlasArgs : CliParser::IArgs {
    OPTION(std::optional<std::string>, 'd', ImgDirectory, "Directory to read all images from");
    OPTION(std::optional<std::string>, 'f', Filter, "Filter to apply to input images");
    OPTION(std::string, 'n', Name, "Name of the output atlas (without extension)");
    OPTION(std::optional<std::string>, 'o', OutDir, "Output directory for the atlas and metadata");
    OPTION(std::optional<size_t>, 'p', Padding, "Padding between images in the atlas", 2);
    OPTION(std::optional<size_t>, 'w', MaxWidth, "Maximum width of the atlas", 2'048);
    OPTION(std::optional<size_t>, 'r', Resize, "Resize sprites to fit within this size");
};

COMMAND(BuildAtlas, BuildAtlasArgs, args) { 
    std::vector<std::filesystem::path> imagePaths;
    if(!GetImagePaths(args.ImgDirectory, args.Filter, imagePaths, std::cerr)) {
        outResult = 1;
        return;
    }

    auto outDir = std::filesystem::path(args.OutDir.value_or(imagePaths[0].parent_path().string()));

    std::error_code ec;
    if(std::filesystem::create_directories(outDir, ec); ec) {
        std::cerr << "Failed to create output directory: " << ec.message() << "\n";
        outResult = 1;
        return;
    }

    size_t padding = args.Padding.value_or(2);
    size_t maxWidth = std::max(size_t(1), args.MaxWidth.value_or(2'048));

    std::vector<LoadedSprite> sprites;
    if(!LoadSprites(imagePaths, padding, maxWidth, args.Resize, sprites, std::cerr)) {
        outResult = 2;
        return;
    }

    Dims sheetDims;
    OrientSprites(sprites, padding, maxWidth, sheetDims);
    auto atlas = CombineSprites(sprites, sheetDims);

    auto pngPath = outDir / (args.Name + ".png");
    auto txtPath = outDir / (args.Name + ".txt");

    if(!stbi_write_png(pngPath.string().c_str(), static_cast<int>(sheetDims.W), static_cast<int>(sheetDims.H), 4, atlas.data(), static_cast<int>(sheetDims.W * 4))) {
        std::cerr << "Failed to write atlas image.\n";
        outResult = 3;
        return;
    }

    std::ofstream meta(txtPath, std::ios::binary);
    if(!meta) {
        std::cerr << "Failed to write metadata file.\n";
        outResult = 4;
        return;
    }

    for(const auto& s : sprites) {
        meta << std::format("{} {} {} {} {}\n", s.Name, s.Pos.X, s.Pos.Y, s.Dims.W, s.Dims.H);
    }

    std::cout << "\nAtlas created successfully: \n" << pngPath << "\n" << txtPath << "\n";
}

struct SplitAtlasArgs : CliParser::IArgs {
    OPTION(std::string, 'a', Atlas, "Path to the atlas");
    OPTION(std::optional<std::string>, 'o', OutDir, "Output directory for extracted images");
};

COMMAND(SplitAtlas, SplitAtlasArgs, args) {
    auto stem = args.Atlas;
    if(stem.ends_with(".png") || stem.ends_with(".txt")) {
        stem = stem.substr(0, stem.find_last_of('.'));
    }
    auto pngPath = std::filesystem::path(stem + ".png");
    auto txtPath = std::filesystem::path(stem + ".txt");
    if(!std::filesystem::exists(pngPath)) {
        std::cerr << "Atlas image not found: " << pngPath << "\n";
        outResult = 1;
        return;
    }
    if(!std::filesystem::exists(txtPath)) {
        std::cerr << "Metadata file not found: " << txtPath << "\n";
        outResult = 1;
        return;
    }

    std::vector<AtlasRegion> regions;
    if(!ParseMetadata(txtPath, regions, std::cerr)) {
        outResult = 2;
        return;
    }

    // load atlas
    int w = 0, h = 0, channels = 0;
    auto* data = stbi_load(pngPath.string().c_str(), &w, &h, &channels, 4);
    if(!data) {
        std::cerr << "Failed to load atlas image: " << pngPath << "\n";
        outResult = 3;
        return;
    }

    auto atlasDims = Dims{w, h};
    auto atlas = std::vector<std::byte>(atlasDims.Area() * 4);
    std::memcpy(atlas.data(), data, atlas.size());
    stbi_image_free(data);

    auto outDir = std::filesystem::path(args.OutDir.value_or(pngPath.parent_path().string()));
    std::error_code ec;
    if(std::filesystem::create_directories(outDir, ec); ec) {
        std::cerr << "Failed to create output directory: " << ec.message() << "\n";
        outResult = 4;
        return;
    }

    for(const auto& region : regions) {
        if(!ExtractRegion(atlas, atlasDims, region, outDir / (region.Name + ".png"), std::cerr)) {
            outResult = 5;
            return;
        }
    }

    std::cout << "\nAtlas split into " << regions.size() << " regions\n" << outDir << "\n";
}


// prevent the linker from stripping out the command functions
void InitializeCommands(){}