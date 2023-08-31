import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import (
    CONF_SENSOR,
    CONF_EQUATION,
    CONF_TYPE,
    STATE_CLASS_MEASUREMENT,
    ICON_CHEMICAL_WEAPON,
)

air_quality_index_ns = cg.esphome_ns.namespace("air_quality_index")
AirQualityIndexComponent = air_quality_index_ns.class_(
    "AirQualityIndexComponent", sensor.Sensor, cg.Component
)

AirQualityIndexType = air_quality_index_ns.enum(
    "AirQualityIndexType"
)

PolutantType = air_quality_index_ns.enum(
    "PolutantType"
)

AQI_TYPE = {
    "AQI": AirQualityIndexType.AQI,
    "CAQI": AirQualityIndexType.CAQI,
}

POLUTANT_TUPE = {
    "PM_2_5": PolutantType.PM_2_5,
    "PM_10": PolutantType.PM_10,
}

CONFIG_SCHEMA = (
    sensor.sensor_schema(
        icon=ICON_CHEMICAL_WEAPON,
        accuracy_decimals=0,
        state_class=STATE_CLASS_MEASUREMENT,
    )
    .extend(
        {
            cv.GenerateID(): cv.declare_id(AirQualityIndexComponent),
            cv.Required(CONF_SENSOR): cv.use_id(sensor.Sensor),
            cv.Required(CONF_TYPE): cv.enum(POLUTANT_TUPE, upper=True),
            cv.Optional(CONF_EQUATION, default="AQI"): cv.enum(AQI_TYPE, upper=True),
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
)


async def to_code(config):
    var = await sensor.new_sensor(config)
    await cg.register_component(var, config)

    pm_sensor = await cg.get_variable(config[CONF_SENSOR])
    cg.add(var.set_pm_sensor(pm_sensor))
    cg.add(var.set_polutant_type(config[CONF_TYPE]))
    cg.add(var.set_aqi_type(config[CONF_EQUATION]))