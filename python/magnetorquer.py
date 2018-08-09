# import numpy as np
# import math

# gryo stuff

# gyroscope sensitivty +- .0074768 deg/sec
# magnetometer sensitivy +- 0.00012207 gauss


class Coil(object):
    """
    Hold information about a torque coil
    """
    def __init__(self, num_wraps, area, voltage, resistance):
        self.num_wraps = num_wraps
        self.area = area
        self.voltage = voltage
        self.resistance = resistance
        self.effectiveness = num_wraps * area * voltage / resistance


def calculate_torque_effectiveness(b_vec, coils):
    """
    Calculate the effectiveness of each torque coil based on its properties and
    the b-field in a body-standard x-y-z frame
    """
    torque_1 = b_vec[2] * coils[1].effectiveness + b_vec[1] * coils[2].effectiveness
    torque_2 = -b_vec[0] * coils[0].effectiveness + b_vec[2] * coils[2].effectiveness
    torque_3 = b_vec[1] * coils[0].effectiveness - b_vec[2] * coils[1].effectiveness

    return torque_1, torque_2, torque_3


def choose_coil(effective_torques, angular_velocities):
    """
    Return the index of the torque coil to used and sign of the voltage through
    each coil based on their effective torques around each body-centered
    cartesian axis and the angular velocities around each axis.
    """
    effectiveness = [t * w for t, w
                     in zip(effective_torques, angular_velocities)]
    best = max(effectiveness)

    for i, val in enumerate(effectiveness):
        if val == best:
            return i


# def pitch():
#     pass


# def azimuth(x, y, z):
#     """
#     Calculate the azimuth of the satellite
#     """
#     return (180/math.pi * math.atan2(y, x)) % 360


# def altitude(x, y, z):
#     """
#     Calculate the altitude of the satellite
#     """
#     h = math.hypot(y, x)
#     return 180/math.pi * math.atan2(z, h)


# # float angle = 0
# # float gyroRate = 0


# def ComplentaryFilter(av_x,av_y,av_z, a_x,a_y,a_z):
#     pass


# def b_field(b_x, b_y, b_z):
#     bf_magnitude = np.sqrt(b_x**2 + b_y**2 + b_z**2)
#     if b_y > 0:
#         direction = 90 - (math.atan2(b_x, b_y)* 180/math.pi)
#     elif b_y < 0:
#         direction = 270 - (math.atan2(b_x, b_y)* 180/math.pi)

#     if b_y < b_x:
#         pass
#     else:
#         #Direction (y>0) = 90 - [arcTAN(x/y)]*180/¹
#         #Direction (y<0) = 270 - [arcTAN(x/y)]*180/¹
#         #Direction (y=0, x<0) = 180.0
#         #Direction (y=0, x>0) = 0.0
#         pass


# def torque(av_x, av_y, av_z, b_x, b_y, b_z):
#     pass


# def calculate_effective_torque(Bx, By, Bz):
#     pass


# def cartesian_to_spherical(x, y, z):
#     """
#     Convert spherical magitudes to cartesian magnitudes (magnetorquer values).

#     :param float x: magnitude of x vector
#     :param float y: magnitude of y vector
#     :param float z: magnitude of z vector
#     """
#     cylindrical_mag = x**2 + y**2
#     r = math.sqrt(cylindrical_mag + z**2)
#     zenith = math.atan2(z, math.sqrt(cylindrical_mag))
#     az = math.atan2(y, x)

#     return r, zenith, az


# def convert_321_rotation(phi, theta, psi, x, y, z):
#     """
#     Convert cartesian x, y, z magnitudes to r, theta, phi spherical magnitudes
#     by rotating around a set of 3-2-1 Euler angles
#     """
#     e1 = ((math.cos(theta) * math.cos(psi)) * x +
#           (math.cos(theta) * math.sin(psi)) * y +
#           (-math.sin(theta) * z))
#     e2 = ((math.sin(phi) * math.sin(theta) * math.cos(psi) - math.cos(phi) * math.sin(psi)) * x +
#           (math.sin(phi) * math.sin(theta) * math.sin(psi) + math.cos(phi) * math.cos(psi)) * y +
#           (math.sin(phi) * math.cos(theta)) * z)
#     e3 = ((math.cos(phi) * math.sin(theta) * math.cos(psi) + math.sin(phi) * math.sin(psi)) * x +
#           (math.cos(phi) * math.sin(theta) * math.sin(psi) - math.sin(phi) * math.cos(psi)) * y +
#           (math.cos(phi) * math.cos(theta)) * z)

#     return e1, e2, e3

# def convert_3_rotation(phi, x, y, z):
#     """
#     Convert magnitudes to e1, e2, e3 by rotating around E1
#     """
#     e1 = math.cos(psi) * x + math.sin(psi) * y
#     e2 = -sin(psi) * x + cos(psi) 



# def determine_torque_coil(x, y, z, xdot, ydot, zdot):
#     """
#     Determine which toque coil (x, y, or z) must be pulsed such that it will
#     align the PCB most with the earth's magnetic field.
#     """
#     e1_max = 0
#     euler_angles = []
#     for phi in range(-math.pi, math.pi, 0.01):
#         for theta in range(-math.pi, math.pi, 0.01):
#             for psi in range(-math.pi, math.pi, 0.01):
#                 e1, e2, e3 = convert_321_rotation(phi, theta, psi, x, y, z):
#                 if e1 > e1_max:
