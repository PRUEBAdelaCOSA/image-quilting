#include <agz-utils/file.h>
#include <agz-utils/image.h>
#include <agz-utils/string.h>
#include <cxxopts.hpp>

#include "TextureQuilter.h"

struct ProgramArgs
{
    std::string inputFile;
    std::string outputFile;

    int outputWidth  = 0;
    int outputHeight = 0;

    int tileWidth  = 0;
    int tileHeight = 0;

    int seamWidth  = 0;
    int seamHeight = 0;

    bool enableMSESelection = false;
    bool enableMinCut        = false;

    float tolerance = 0;
};

std::optional<ProgramArgs> parseArguments(int argc, char *argv[])
{
    cxxopts::Options options("TextureQuilting");
    options.add_options()
        ("input",      "Input image file",      cxxopts::value<std::string>())
        ("output",     "Output image file",     cxxopts::value<std::string>())
        ("width",      "Output image width",    cxxopts::value<int>())
        ("height",     "Output image height",   cxxopts::value<int>())
        ("tileW",      "Tile width",            cxxopts::value<int>())
        ("tileH",      "Tile height",           cxxopts::value<int>())
        ("seamW",      "Seam width",            cxxopts::value<int>())
        ("seamH",      "Seam height",           cxxopts::value<int>())
        ("mseSelect",  "Enable MSE selection",  cxxopts::value<bool>())
        ("minCut",     "Enable min cost cut",   cxxopts::value<bool>())
        ("tolerance",  "Selection tolerance",   cxxopts::value<float>())
        ("help",       "Display help");

    const auto args = options.parse(argc, argv);

    if(args.count("help"))
    {
        std::cout << options.help({ "" }) << std::endl;
        return {};
    }

    ProgramArgs result;

    try
    {
        result.inputFile  = args["input"] .as<std::string>();
        result.outputFile = args["output"].as<std::string>();
        result.outputWidth    = args["width"] .as<int>();
        result.outputHeight   = args["height"].as<int>();
        result.tileWidth     = args["tileW"].as<int>();
        result.tileHeight    = args["tileH"].as<int>();

        if(args.count("seamW"))
            result.seamWidth = args["seamW"].as<int>();
        else
            result.seamWidth = std::max(1, result.tileWidth / 6);

        if(args.count("seamH"))
            result.seamHeight = args["seamH"].as<int>();
        else
            result.seamHeight = std::max(1, result.tileHeight / 6);

        if(args.count("mseSelect"))
            result.enableMSESelection = args["mseSelect"].as<bool>();
        else
            result.enableMSESelection = true;

        if(args.count("minCut"))
            result.enableMinCut = args["minCut"].as<bool>();
        else
            result.enableMinCut = true;

        if(args.count("tolerance"))
            result.tolerance = args["tolerance"].as<float>();
        else
            result.tolerance = 0.1f;
    }
    catch(...)
    {
        std::cout << options.help({ "" }) << std::endl;
        return {};
    }

    return result;
}

void execute(int argc, char *argv[])
{
    using namespace agz;
    using namespace img;
    using namespace stdstr;

    const auto args = parseArguments(argc, argv);
    if(!args)
        return;

    TextureQuilter quilter;
    quilter.setTileParams(
        args->tileWidth,   args->tileHeight,
        args->seamWidth, args->seamHeight,
        args->tolerance);
    quilter.enableMSESelection(args->enableMSESelection);
    quilter.enableMinCut(args->enableMinCut);

    auto sourceTexture = Texture<Vec3>(load_rgb_from_file(
        args->inputFile).map(
            [](const math::color3b &c)
    {
        return Vec3(math::from_color3b<float>(c));
    }));

    auto outputTexture = quilter.quiltTexture(
        sourceTexture, args->outputWidth, args->outputHeight);

    auto outputTextureU8 = outputTexture.map([](const Vec3 &c)
    {
        return math::to_color3b<float>(c);
    }).get_data();

    file::create_directory_for_file(args->outputFile);

    const std::string lowerOutput = to_lower(args->outputFile);
    if(ends_with(lowerOutput, ".png"))
        save_rgb_to_png_file(args->outputFile, outputTextureU8);
    else if(ends_with(lowerOutput, ".jpg"))
        save_rgb_to_jpg_file(args->outputFile, outputTextureU8);
    else if(ends_with(lowerOutput, ".bmp"))
        save_rgb_to_bmp_file(args->outputFile, outputTextureU8);
    else
        throw std::runtime_error(
            "unsupported output format: " + args->outputFile);

    std::cout << "Texture generation complete..." << std::endl;
}

int main(int argc, char *argv[])
{
    try
    {
        execute(argc, argv);
    }
    catch(const std::exception &err)
    {
        std::cerr << err.what() << std::endl;
        return -1;
    }
}
