#include "GhostHunter/Formatting.h"

namespace GhostHunter {
    std::string ToString(QualityType quality) {
        switch(quality) {
            using enum QualityType;
            case Awful: return "Awful";
            case Bad: return "Bad";
            case Okay: return "Okay";
            case PrettyGood: return "Pretty Good";
            case Great: return "Great";
            case Excellent: return "Excellent";
            case Wonderful: return "Wonderful";
            case Legendary: return "Legendary";
            default: return "Unset";
        }
    }

	std::string ToString(ToolName tool) {
        switch(tool) {
            using enum ToolName;
            case Camera: return "Camera";
            case Flashlight: return "Flashlight";
            case EmfDetector: return "EmfDetector";
            case ThermalCamera: return "ThermalCamera";
            case EvpRecorder: return "EvpRecorder";
            case LaserGrid: return "LaserGrid";
            case SpiritBox: return "SpiritBox";
            default: return "Unset";
        };
	}

    std::string ToString(LocationName location) {
        switch(location) {
            using enum LocationName;
            case Shed: return "Shed";
            case School: return "School";
            case House: return "House";
            case Church: return "Church";
            case Hotel: return "Hotel";
            case Prison: return "Prison";
            case Hospital: return "Hospital";
            case Sanitorium: return "Sanitorium";
            default: case Unset: return "Unset";
        }
    }

    std::string ToString(MediaType type) {
        switch(type) {
            using enum MediaType;
            case Picture: return "Picture";
            case Interview: return "Interview";
            case Article: return "Article";
            case Book: return "Book";
            case Podcast: return "Podcast";
            case TvShow: return "TV Show";
            case Movie: return "Movie";
            default: return "Unset";
        }
    }

    std::string ToString(ResourceName name) {
        switch(name) {
            using enum ResourceName;
            case Cash: return "Cash";
            case Images: return "Images";
            case Audio: return "Audio";
            case Video: return "Video";
            case Physical: return "Physical";
            default: break;
        }
        DR_ASSERT_MSG(false, "Invalid resource name");
        return "Unknown";
    }
} // namespace GhostHunter