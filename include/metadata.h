//
// Created by ek on 29.07.2022.
//

#ifndef DETTRACKPL_METADATA_H
#define DETTRACKPL_METADATA_H

#include <glib.h>
#include <gst/gst.h>
#include <cstdint>
#include <string>
#include <sstream>
#include <map>
#include <array>
#include <unordered_map>
#include <memory>

#include "gstnvdsmeta.h"
#include "nvds_analytics_meta.h"
#include "nvdsmeta.h"




namespace metadata {


    constexpr auto MAX_DISPLAY_LEN = 70;
    constexpr auto PGIE_CLASS_ID_BUS = 0;
    constexpr auto PGIE_CLASS_ID_CAR = 1;
    constexpr auto FONT_SERIF = "Serif";

    constexpr auto CROSSING_N = "N";
    constexpr auto CROSSING_NE = "NE";
    constexpr auto CROSSING_SE = "SE";
    constexpr auto CROSSING_SV = "SV";
    constexpr auto CROSSING_NV = "NV";

    constexpr auto CROSSING_IDX_N = 0;
    constexpr auto CROSSING_IDX_NE = 1;
    constexpr auto CROSSING_IDX_SE = 2;
    constexpr auto CROSSING_IDX_SV = 3;
    constexpr auto CROSSING_IDX_NV = 4;

    constexpr int N = 5;
    using crossing_t = std::array<std::uint16_t, N>;
    using crossings_t = std::array<crossing_t, N>;


    static crossings_t crossings{{
                                            /*N-Entry*/  {0, 0, 0, 0, 0}, // N-Exit, NE-Exit, SE-Exit, SV-Exit, NV-Exit
                                            /*NE-Entry*/ {0, 0, 0, 0, 0},
                                            /*SE-Entry*/ {0, 0, 0, 0, 0},
                                            /*SV-Entry*/ {0, 0, 0, 0, 0},
                                            /*NV-Entry*/ {0, 0, 0, 0, 0}}
    };


    using object_entry_t = std::unordered_map<std::uint64_t, std::size_t>;
    static object_entry_t objEntries;


    //extern meta_producer_t producer;

    struct DisplayInfo {
        std::string fps{"FPS Info: "};
        std::string roi;
        std::map<std::string, std::uint32_t> crossings;
    };
    using display_info_t = struct DisplayInfo;

    using object_entry_t = std::unordered_map<std::uint64_t, std::size_t>;

    //using meta_producer_t = std::weak_ptr<::kafkaproducer::KafkaProducer>;

    /* functions */
    std::string getLCFromIdx(const std::size_t idx);
    std::size_t getLCIdxFromString(const std::string& crossing);
    void displayInfoToFrame(NvDsBatchMeta *batch_meta, NvDsFrameMeta * const frame_meta,
                            const display_info_t &displayInfo);

    GstPadProbeReturn nvdsanalyticsSrcPadBufferProbe_vis (GstPad *, GstPadProbeInfo *, gpointer);
    void printCrossingsMatrix();
    void setText(NvOSD_TextParams *txt_params, const int xOffset, const int yOffset,
                 const std::string &display_text);




} // namespace metadata







#endif //DETTRACKPL_METADATA_H
