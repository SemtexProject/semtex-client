#pragma once

#include <google/protobuf/timestamp.pb.h>
#include <chrono>

namespace util {

    google::protobuf::Timestamp toProtobufTimestamp(const std::chrono::system_clock::time_point &tp) {
        google::protobuf::Timestamp timestamp;
        auto seconds = std::chrono::duration_cast<std::chrono::seconds>(tp.time_since_epoch());
        timestamp.set_seconds(seconds.count());
        timestamp.set_nanos(std::chrono::duration_cast<std::chrono::nanoseconds>(tp.time_since_epoch() - seconds).count());
        return timestamp;
    }

    std::chrono::system_clock::time_point toSystemClock(const google::protobuf::Timestamp &timestamp) {
        auto durationInSeconds = std::chrono::seconds(timestamp.seconds());
        auto durationInNanoseconds = std::chrono::nanoseconds(timestamp.nanos());

        // Convert seconds to nanoseconds and add them to the nanoseconds part
        auto totalDuration = std::chrono::duration_cast<std::chrono::system_clock::duration>(
            durationInSeconds + durationInNanoseconds
        );

        return std::chrono::system_clock::time_point(totalDuration);
    }


}