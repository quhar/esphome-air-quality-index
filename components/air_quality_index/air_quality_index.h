#pragma once

#include "esphome/components/sensor/sensor.h"
#include "esphome/core/component.h"

namespace esphome {
namespace air_quality_index {

enum PolutantType {
  PM_2_5,
  PM_10,
};

/// Enum listing all types of Air Quality Index.
enum AirQualityIndexType {
  AQI,   // Used in United States, based on https://www.airnow.gov/sites/default/files/2020-05/aqi-technical-assistance-document-sept2018.pdf
};


static const int AQI_NUMBER_OF_LEVELS = 6;

const int aqi_index_grid_[AQI_NUMBER_OF_LEVELS][2] = {
    {0, 50}, {51, 100}, {101, 150}, {151, 200}, {201, 300}, {301, 500},
};

const float aqi_pm2_5_calculation_grid_[AQI_NUMBER_OF_LEVELS][2] = {
    {0, 12.0},     {12.0, 35.4},   {35.4, 55.4},
    {55.4, 150.4}, {150.4, 250.4}, {250.4, 500.4},
};

const float aqi_pm10_0_calculation_grid_[AQI_NUMBER_OF_LEVELS][2] = {
    {0, 54}, {55, 154}, {155, 254}, {255, 354}, {355, 424}, {425, 604},
};

/// This class implements calculation of air quality index from raw PM2.5 or
/// PM10 readings.
class AirQualityIndexComponent : public sensor::Sensor, public Component {
 public:
  AirQualityIndexComponent() = default;

  void set_pm_sensor(sensor::Sensor *pm_sensor) {
    this->pm_sensor_ = pm_sensor;
  }
  void set_polutant_type(PolutantType polutant_type) {
    this->polutant_type_ = polutant_type;
  }
  void set_aqi_type(AirQualityIndexType aqi_type) {
    this->aqi_type_ = aqi_type;
  }

  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override;
  void loop() override;

 protected:
  void pm_callback_(float state) {
    this->next_update_ = true;
    this->pm_ = state;
  }
  static int get_grid_index_(float value,
                             const float array[AQI_NUMBER_OF_LEVELS][2]);

  static float calculate_aqi_pm_2_5_(float pm);
  static float calculate_aqi_pm_10_(float pm);

  static float calculate_aqi_from_array_(
      float pm, const float array[AQI_NUMBER_OF_LEVELS][2]);
  sensor::Sensor *pm_sensor_{nullptr};

  bool next_update_{false};

  float pm_{NAN};
  PolutantType polutant_type_;
  AirQualityIndexType aqi_type_;
};

}  // namespace air_quality_index
}  // namespace esphome