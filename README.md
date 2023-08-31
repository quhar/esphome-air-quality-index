# esphome-air-quality-index
Simple Esphome component that calculates AQI based on the provided particle matter sensor.


Example config using SDS011 as a PM sensor, but any PM2.5/PM10 can be used.:

```yaml

external_components:
  - source: github://quhar/esphome-air-quality-index
    components:
      - air_quality_index

uart:
  rx_pin: 20
  tx_pin: 21
  baud_rate: 9600

sensor:
  - platform: sds011
    pm_2_5:
      name: "Particulate Matter <2.5µm Concentration"
      id: pm_2_5
    pm_10_0:
      name: "Particulate Matter <10.0µm Concentration"
      id: pm_10_0
    update_interval: 5min

  - platform: air_quality_index
    id: aqi_2_5
    sensor: pm_2_5
    name: AQI PM2.5
    icon: mdi:chemical-weapon
    type: PM_2_5
    equation: AQI

  - platform: air_quality_index
    id: aqi_10
    sensor: pm_10_0
    name: AQI PM10.0
    icon: mdi:chemical-weapon
    type: PM_10
    equation: AQI
```
