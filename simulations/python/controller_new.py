import numpy as np
import math

# torque parameters
voltage = 0.1
B = 0.00003
I = 0.3 # amps
D = 0.001
A = D * D
N = 200

# board inertia parameters
l = .035
m = 0.005

time = 5 # seconds
time = 0.001 # seconds

def cross(a, b):
  assert len(a) == len(b) == 3, 'Vectors a, b must be three-dimensional'
  return [a[1]*b[2] - a[2]*b[1],
          a[2]*b[0] - a[0]*b[2],
          a[0]*b[1] - a[1]*b[0]]

def dot(a, b):
  return sum([i * j for i, j in zip(a, b)])

def scale(a, vec):
  return [a*v for v in vec]


def magnetorquer_out_new(mag_field, ang_vel, gain):
  #  Torque = mu * i x B
  # i is a unit vector for each axis: x = [1,0,0] y = [0,1,0] z = [0,0,1]


  D = 0.001
  A = D * D
  Is = l**2 / 3 * m
  Iz = l**2 / 6 * m
  const_1 = 200*1/3*A/Is
  const_2 = 200*1/3*A/Is
  const_3 = 30*1/3*.01*.01/Iz
  print(const_1,const_2,const_3)




  #gives us direction of torque in each axis via the corss product of all three axis with the magnetic field divded by I.
  # I * a = mu * i x B
  # a = mu * i x B / I
  mu_1 = scale(const_1, [1,0,0])
  a_1 = cross(mu_1, mag_field)
  mu_2 = scale(const_1, [0,1,0])
  a_2 = cross(mu_2, mag_field)
  mu_3 = scale(const_1, [0,0,1])
  a_3 = cross(mu_3, mag_field)

  #angular velocities wrt to each axis - this is a 3 x 3 list containing the respective angular velocities for each axis.
  angular = [a_1, a_2, a_3]

  similarity = [np.abs(dot(ang_vel,x)) for x in angular]
  best_index = np.argmax(similarity)
  power0 = (100/(math.pi/2))*math.atan(similarity[0]/100)
  power1 = (100/(math.pi/2))*math.atan(similarity[1])
  power2 = (100/(math.pi/2))*math.atan(similarity[2])
  # control_filter = 1
  # scaler_coef = control_filter/np.linalg.norm(mag_field)
  vec = scale(gain, angular[best_index]) 


  return 

def magnetorquer_out(mag_field, ang_vel, gain):
  #  Torque = mu * i x B
  # i is a unit vector for each axis: x = [1,0,0] y = [0,1,0] z = [0,0,1]


  D = 0.001
  A = D * D
  Is = l**2 / 3 * m
  Iz = l**2 / 6 * m
  const_1 = 200*1/3*A/Is
  const_2 = 200*1/3*A/Is
  const_3 = 30*1/3*.01*.01/Iz
  print(const_1,const_2,const_3)




  #gives us direction of torque in each axis via the corss product of all three axis with the magnetic field divded by I.
  # I * a = mu * i x B
  # a = mu * i x B / I
  mu_1 = scale(1, [1,0,0])
  a_1 = cross(mu_1, mag_field)
  mu_2 = scale(1, [0,1,0])
  a_2 = cross(mu_2, mag_field)
  mu_3 = scale(2, [0,0,1])
  a_3 = cross(mu_3, mag_field)

  #angular velocities wrt to each axis - this is a 3 x 3 list containing the respective angular velocities for each axis.
  angular = [a_1, a_2, a_3]

  similarity = [np.abs(dot(ang_vel,x)) for x in angular]
  best_index = np.argmax(similarity)
  power0 = (100/(math.pi/2))*math.atan(similarity[0]/100)
  power1 = (100/(math.pi/2))*math.atan(similarity[1])
  power2 = (100/(math.pi/2))*math.atan(similarity[2])
  # control_filter = 1
  # scaler_coef = control_filter/np.linalg.norm(mag_field)
  vec = scale(gain, similarity) 


  return best_index, vec

def torque(N, I, A, B, theta):
    return N * I * A * B * np.sin(theta)
def angular(t, l, m):
    Is = l**4 / 3 * m
    alpha = t / Is
    return alpha


def choose_coil(ang_vel):
    """
    Return the index of the torque coil to used and sign of the voltage through
    each coil based on their effective torques around each body-centered
    cartesian axis and the angular velocities around each axis.
    """
    effectiveness = [abs(t * w) for t, w in zip(ang_vel, ang_vel)]
    return effectiveness.index(max(effectiveness))
    #return torques.index(max(torques))

def choose_direction(torques, coil):
    return  1 if torques[coil] > 0  else -1

#  magnetic field strength = vector u = <1.0, 2.0, 3.0>
#   angular velocity = vector v = <5.0, 2.0, 5.0>
#   gain = 1.0

if __name__ == '__main__':
  time = 0.1
  Is = l**4 / 3 * m

  u = [3, 2, 5]
  v = [6, 3, 6]
  # torque_coil = choose_coil(v)
  axis, power = magnetorquer_out(u, v, 1000.0)
  print("axis", axis, "power", power)
  #print(magnetorquer_out(u, v, 10000))
  # pwm_value = vec[1]
  # print(pwm_value)
  #angular_acc = torque(N, I, A, B, 1) * pwm_value / 100 / Is
  # print(angular_acc)
  # change_in_rot = angular_ac * time
  # v_new  = v.copy()
  #angular_acc = torque(N, I, A, B, 1) * vec/Is
  #a_c = angular_acc.copy()
  # for x in range(1000):
  #   v_new[torque_coil] = v_new[torque_coil] - change_in_rot
  #   #a_c[torque_coil] = a_c[torque_coil] - change_in_rot

  #   x = x +1


