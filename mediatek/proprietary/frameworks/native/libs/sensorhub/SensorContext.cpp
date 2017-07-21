#include <SensorContext.h>
#include <SensorData.h>

#include <shf_define.h>

namespace android {
// ----------------------------------------------------------------------------
#define CONTEXT_TYPE_CLOCK                   1000
#define CONTEXT_TYPE_INPOCKET                1001
#define CONTEXT_TYPE_FACEDOWN                1002
#define CONTEXT_TYPE_PEDOMETER               1003
#define CONTEXT_TYPE_PICKUP                  1004
#define CONTEXT_TYPE_SHAKE                   1005
#define CONTEXT_TYPE_ACTIVITY                1006
#define CONTEXT_TYPE_SIGNIFICANT_MOTION      CONTEXT_TYPE_ACTIVITY
#define CONTEXT_TYPE_GESTURE                 1007
#define CONTEXT_TYPE_NOISE_LEVEL             1008
#define CONTEXT_TYPE_FREE_FALL               1009
#define CONTEXT_TYPE_TAP                     1010
#define CONTEXT_TYPE_TWIST                   1011
#define CONTEXT_TYPE_SNAPSHOT                1012
#define CONTEXT_TYPE_PDR                     1013

// ----------------------------------------------------------------------------
SensorContext::SensorContext()
{
}

SensorContext::~SensorContext()
{
}

const int SensorContext::Clock::CONTEXT_TYPE = CONTEXT_TYPE_CLOCK;
const int SensorContext::Clock::DATA_INDEX_TIME = SHF_DATA_INDEX_CLOCK_TIME;

const int SensorContext::Pedometer::CONTEXT_TYPE = CONTEXT_TYPE_PEDOMETER;
const int SensorContext::Pedometer::DATA_INDEX_LENGTH = SHF_DATA_INDEX_PEDOMETER_LENGTH;
const int SensorContext::Pedometer::DATA_INDEX_FREQUENCY = SHF_DATA_INDEX_PEDOMETER_FREQUENCY;
const int SensorContext::Pedometer::DATA_INDEX_COUNT = SHF_DATA_INDEX_PEDOMETER_COUNT;
const int SensorContext::Pedometer::DATA_INDEX_DISTANCE = SHF_DATA_INDEX_PEDOMETER_DISTANCE;
const int SensorContext::Pedometer::DATA_INDEX_TIMESTAMP = SHF_DATA_INDEX_PEDOMETER_TIME;

const int SensorContext::Activity::CONTEXT_TYPE = CONTEXT_TYPE_ACTIVITY;
const int SensorContext::Activity::DATA_INDEX_IN_VEHICLE = SHF_DATA_INDEX_ACTIVITY_VEHICLE;
const int SensorContext::Activity::DATA_INDEX_ON_BICYCLE = SHF_DATA_INDEX_ACTIVITY_BIKE;
const int SensorContext::Activity::DATA_INDEX_ON_FOOT = SHF_DATA_INDEX_ACTIVITY_FOOT;
const int SensorContext::Activity::DATA_INDEX_STILL = SHF_DATA_INDEX_ACTIVITY_STILL;
const int SensorContext::Activity::DATA_INDEX_UNKNOWN = SHF_DATA_INDEX_ACTIVITY_UNKNOWN;
const int SensorContext::Activity::DATA_INDEX_TILTING = SHF_DATA_INDEX_ACTIVITY_TILT;
const int SensorContext::Activity::DATA_INDEX_TIMESTAMP = SHF_DATA_INDEX_ACTIVITY_TIME;
const int SensorContext::Activity::DATA_INDEX_WALKING = SHF_DATA_INDEX_ACTIVITY_WALKING;
const int SensorContext::Activity::DATA_INDEX_RUNNING   = SHF_DATA_INDEX_ACTIVITY_RUNNING;
const int SensorContext::Activity::DATA_INDEX_CLIMBING = SHF_DATA_INDEX_ACTIVITY_CLIMBING;

const int SensorContext::InPocket::CONTEXT_TYPE = CONTEXT_TYPE_INPOCKET;
const int SensorContext::InPocket::DATA_INDEX_INPOCKE = SHF_DATA_INDEX_INPOCKET_VALUE;
const int SensorContext::InPocket::DATA_INDEX_TIMESTAMP = SHF_DATA_INDEX_INPOCKET_TIME;

const int SensorContext::MostProbableActivity::CONTEXT_TYPE = CONTEXT_TYPE_ACTIVITY;
const int SensorContext::MostProbableActivity::DATA_INDEX_ACTIVITY = SHF_DATA_INDEX_MPACTIVITY_ACTIVITY;
const int SensorContext::MostProbableActivity::DATA_INDEX_CONFIDENCE = SHF_DATA_INDEX_MPACTIVITY_CONFIDENCE;
const int SensorContext::MostProbableActivity::DATA_INDEX_TIMESTAMP = SHF_DATA_INDEX_MPACTIVITY_TIME;

const int SensorContext::SignificantMotion::CONTEXT_TYPE = CONTEXT_TYPE_SIGNIFICANT_MOTION;
const int SensorContext::SignificantMotion::DATA_INDEX_MOTION_VALUE = SHF_DATA_INDEX_SIGNIFICANT_VALUE;
const int SensorContext::SignificantMotion::DATA_INDEX_TIMESTAMP = SHF_DATA_INDEX_SIGNIFICANT_TIME;

const int SensorContext::Pickup::CONTEXT_TYPE = CONTEXT_TYPE_PICKUP;
const int SensorContext::Pickup::DATA_INDEX_PICKUP_VALUE = SHF_DATA_INDEX_PICKUP_VALUE;
const int SensorContext::Pickup::DATA_INDEX_TIMESTAMP = SHF_DATA_INDEX_PICKUP_TIME;

const int SensorContext::FaceDown::CONTEXT_TYPE = CONTEXT_TYPE_FACEDOWN;
const int SensorContext::FaceDown::DATA_INDEX_FACEDOWN_VALUE = SHF_DATA_INDEX_FACEDOWN_VALUE;
const int SensorContext::FaceDown::DATA_INDEX_TIMESTAMP = SHF_DATA_INDEX_FACEDOWN_TIME;

const int SensorContext::Shake::CONTEXT_TYPE = CONTEXT_TYPE_SHAKE;
const int SensorContext::Shake::DATA_INDEX_SHAKE_VALUE = SHF_DATA_INDEX_SHAKE_VALUE;
const int SensorContext::Shake::DATA_INDEX_TIMESTAMP = SHF_DATA_INDEX_SHAKE_TIME;

const int SensorContext::Gesture::CONTEXT_TYPE = CONTEXT_TYPE_GESTURE;
const int SensorContext::Gesture::DATA_INDEX_GESTURE_VALUE = SHF_DATA_INDEX_GESTURE_VALUE;
const int SensorContext::Gesture::DATA_INDEX_TIMESTAMP = SHF_DATA_INDEX_GESTURE_TIME;

const int SensorContext::NoiseLevel::CONTEXT_TYPE = CONTEXT_TYPE_NOISE_LEVEL;
const int SensorContext::NoiseLevel::DATA_INDEX_NOISELEVEL_VALUE = SHF_DATA_INDEX_NOISE_LEVEL_VALUE;
const int SensorContext::NoiseLevel::DATA_INDEX_TIMESTAMP = SHF_DATA_INDEX_NOISE_LEVEL_TIME;

const int SensorContext::FreeFall::CONTEXT_TYPE = CONTEXT_TYPE_FREE_FALL;
const int SensorContext::FreeFall::DATA_INDEX_FREEFALL_VALUE = SHF_DATA_INDEX_FREE_FALL_VALUE;
const int SensorContext::FreeFall::DATA_INDEX_TIMESTAMP = SHF_DATA_INDEX_FREE_FALL_TIME;
//new add
const int SensorContext::Tap::CONTEXT_TYPE = CONTEXT_TYPE_TAP;
const int SensorContext::Tap::DATA_INDEX_TAP_VALUE = SHF_DATA_INDEX_TAP_VALUE;
const int SensorContext::Tap::DATA_INDEX_TIMESTAMP = SHF_DATA_INDEX_TAP_TIME;

const int SensorContext::Twist::CONTEXT_TYPE = CONTEXT_TYPE_TWIST;
const int SensorContext::Twist::DATA_INDEX_TWIST_VALUE = SHF_DATA_INDEX_TWIST_VALUE;
const int SensorContext::Twist::DATA_INDEX_TIMESTAMP = SHF_DATA_INDEX_TWIST_TIME;

const int SensorContext::Snapshot::CONTEXT_TYPE = CONTEXT_TYPE_SNAPSHOT;
const int SensorContext::Snapshot::DATA_INDEX_SNAPSHOT_VALUE = SHF_DATA_INDEX_SNAPSHOT_VALUE;
const int SensorContext::Snapshot::DATA_INDEX_TIMESTAMP = SHF_DATA_INDEX_SNAPSHOT_TIME;

const int SensorContext::PDR::CONTEXT_TYPE = CONTEXT_TYPE_PDR;
const int SensorContext::PDR::DATA_INDEX_PDR_X = SHF_DATA_INDEX_PDR_X;
const int SensorContext::PDR::DATA_INDEX_PDR_Y = SHF_DATA_INDEX_PDR_Y;
const int SensorContext::PDR::DATA_INDEX_PDR_Z = SHF_DATA_INDEX_PDR_Z;
const int SensorContext::PDR::DATA_INDEX_TIMESTAMP = SHF_DATA_INDEX_PDR_TIME;

int SensorContext::getDataType(int dataIndex)
{
    int dataType = DATA_TYPE_UINT32;
    switch(dataIndex) {
        case Clock::DATA_INDEX_TIME:
        case Pedometer::DATA_INDEX_TIMESTAMP:
        case Activity::DATA_INDEX_TIMESTAMP:
        case InPocket::DATA_INDEX_TIMESTAMP:
        case MostProbableActivity::DATA_INDEX_TIMESTAMP:
        case SignificantMotion::DATA_INDEX_TIMESTAMP:
        case Pickup::DATA_INDEX_TIMESTAMP:
        case FaceDown::DATA_INDEX_TIMESTAMP:
        case Shake::DATA_INDEX_TIMESTAMP:
        case Gesture::DATA_INDEX_TIMESTAMP:
        case NoiseLevel::DATA_INDEX_TIMESTAMP:
        case FreeFall::DATA_INDEX_TIMESTAMP:
        case Tap::DATA_INDEX_TIMESTAMP:
        case Twist::DATA_INDEX_TIMESTAMP:
        case Snapshot::DATA_INDEX_TIMESTAMP:
        case PDR::DATA_INDEX_TIMESTAMP:
            dataType = DATA_TYPE_UINT64;
            break;
        case Pedometer::DATA_INDEX_FREQUENCY:
            dataType = DATA_TYPE_FLOAT;
            break;
        default:
            break;
    }

    return dataType;
}

};
