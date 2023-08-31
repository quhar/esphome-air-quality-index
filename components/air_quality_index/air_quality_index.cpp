#include "air_quality_index.h"
#include "esphome/core/log.h"

namespace esphome {
namespace air_quality_index {

static const char *const TAG = "air_quality_index.sensor";

void AirQualityIndexComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up air quality index '%s' PM: %d AQI: %d...",
                this->get_name().c_str(), this->aqi_type_,
                this->polutant_type_);
  if (!this->pm_sensor_) {
    ESP_LOGD(TAG, "PM SENSOR IS EMPTY");
    return;
  }
  this->pm_sensor_->add_on_state_callback(
      [this](float state) { this->pm_callback_(state); });
  if (this->pm_sensor_->has_state()) {
    this->pm_callback_(this->pm_sensor_->get_state());
  }
}

void AirQualityIndexComponent::dump_config() {
  LOG_SENSOR("", "Air Quality Index", this);

  switch (this->aqi_type_) {
    case AQI:
      ESP_LOGCONFIG(TAG, "Air Quality Index Type: AQI");
      break;
    default:
      ESP_LOGE(TAG, "Invalid air quality index type!");
      break;
  };

  switch (this->polutant_type_) {
    case PM_2_5:
      ESP_LOGCONFIG(TAG, "Polutant type: PM2.5μg/m³");
      break;
    case PM_10:
      ESP_LOGCONFIG(TAG, "Polutant type: PM10μg/m³");
      break;
    default:
      ESP_LOGE(TAG, "Invalid polutant type!");
      break;
  };

  ESP_LOGCONFIG(TAG, "Source");
  ESP_LOGCONFIG(TAG, "  PM: '%s'", this->pm_sensor_->get_name().c_str());
}

float AirQualityIndexComponent::get_setup_priority() const {
  return setup_priority::DATA;
}

void AirQualityIndexComponent::loop() {
  if (!this->next_update_) {
    return;
  }
  this->next_update_ = false;

  // Ensure we have source data
  const bool no_data = std::isnan(this->pm_);
  if (no_data) {
    ESP_LOGW(TAG, "No valid state from PM sensor.");
    this->publish_state(NAN);
    this->status_set_warning();
    return;
  }

  float calculated_aqi = 0.0;
  switch (this->aqi_type_) {
    case AQI:
      switch (this->polutant_type_) {
        case PM_2_5:
          calculated_aqi =
              AirQualityIndexComponent::calculate_aqi_pm_2_5_(this->pm_);
          break;
        case PM_10:
          calculated_aqi =
              AirQualityIndexComponent::calculate_aqi_pm_10_(this->pm_);
          break;
        default:
          ESP_LOGE(TAG, "Invalid AQI type!");
          break;
      };
      break;
    default:
      ESP_LOGE(TAG, "Invalid air quality index type!");
  }
  if (calculated_aqi < 0) {
    ESP_LOGW(TAG, "Invalid AQI value. %d", (int)calculated_aqi);
    this->publish_state(NAN);
    this->status_set_warning();
    return;
  }

  ESP_LOGD(TAG, "Publishing air quality index %d", int(calculated_aqi));
  this->status_clear_warning();
  this->publish_state(calculated_aqi);
}

int AirQualityIndexComponent::get_grid_index_(
    float value, const float array[AQI_NUMBER_OF_LEVELS][2]) {
  if (value == 0) return 0;
  for (int i = 0; i < AQI_NUMBER_OF_LEVELS; i++) {
    if (value > array[i][0] && value <= array[i][1]) {
      return i;
    }
  }
  return -1;
}

float AirQualityIndexComponent::calculate_aqi_pm_2_5_(float value) {
  return AirQualityIndexComponent::calculate_aqi_from_array_(
      value, aqi_pm2_5_calculation_grid_);
}

float AirQualityIndexComponent::calculate_aqi_pm_10_(float value) {
  return AirQualityIndexComponent::calculate_aqi_from_array_(
      value, aqi_pm10_0_calculation_grid_);
}

float AirQualityIndexComponent::calculate_aqi_from_array_(
    float value, const float array[AQI_NUMBER_OF_LEVELS][2]) {
  int grid_index = AirQualityIndexComponent::get_grid_index_(value, array);
  if (grid_index < 0) {
    return -1;
  }
  float aqi_lo = float(aqi_index_grid_[grid_index][0]);
  float aqi_hi = float(aqi_index_grid_[grid_index][1]);
  float conc_lo = float(array[grid_index][0]);
  float conc_hi = float(array[grid_index][1]);

  float aqi =
      ((aqi_hi - aqi_lo) / (conc_hi - conc_lo)) * (float(value) - conc_lo) +
      aqi_lo;
  return float(aqi);
}

}  // namespace air_quality_index
}  // namespace esphome