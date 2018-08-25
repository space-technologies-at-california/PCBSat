SENSOR_PERIOD = 5

def on_sensor_read(gyro_data, magnetometer_data):
    """
    This function is called by the mission planner approximately every
    SENSOR_PERIOD seconds after data is read from the LSM.

    Returns: None
    """
    pass

def on_actuate(prev_axis, prev_duration, prev_setting, prev_failed):
    """
    This function is called by the mission planner whenever it is ready to
    actuate the torque coils, which is usually when all faults are cleared
    and there is sufficient power to actuate.

    The mission planner uses this function to decide which torque coil to
    actuate and for how long and how hard.

    Information is available about the previous actuation attempt and whether
    the previous settings resulted in a power-loss event.

    Returns: (axis, duration, setting)
        axis - which torque coil to activate
        duration - period of time in miliseconds to actuate
        setting - number between 0 and 100
    """
    return 0, 0, 0
