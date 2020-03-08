syms theta alpha a d theta1 theta2 theta3 q1(t) q2(t) q3(t) x1 x2 x3 x4 x5 x6 tau

% transformation matrix for each row of DH parameters
A(alpha, a, d, theta) = [cos(theta) -sin(theta)*cos(alpha) sin(theta)*sin(alpha)  a*cos(theta);
                         sin(theta) cos(theta)*cos(alpha)  -cos(theta)*sin(alpha) a*sin(theta);
                               0          sin(alpha)              cos(alpha)              d;
                               0              0                       0                   1];

m = [0.2619 0.123 0.0962];

% Inertia matrix as obtained from SolidWorks
J1 = [35  0  0;
      0  2.3 5.2;
      0  5.2 32.8] .* 1e-4;
J2 = [17  0  0;
      0 16.8 0.4;
      0  0.4 0.2] .* 1e-4;
J3 = [21  0  0;
      0  20.8 0.6;
      0  0.6  0.1] .* 1e-4;

% Similarity transformation to tranform to the same reference frame
I1 = rotx(-90)' * J1 * rotx(-90);
I2 = rotx(-90)' * roty(-90)' * J2 * roty(-90) * rotx(-90);
I3 = rotx(-90)' * roty(-90)' * J3 * roty(-90) * rotx(-90);

% measurements from SolidWorks
dc(1) = 0; dc(2) = 0.148; dc(3) = 0.009;
ac(1) = 0; ac(2) = 0.185; ac(3) = 0.18;
d(1) = 0; d(2) = 0.3; d(3) = 0.015;
a(1) = 0; a(2) = 0.3; a(3) = 0.45;

% DH parameters for center of mass and end of links
dhcom = [-pi/2, 0,     0,       theta1; 
           0,   0,     dc(2),   0; 
           0,   ac(2), 0.0075,  theta2 - (pi/2);
           0,   ac(3), dc(3),   theta3];
dh = [-pi/2, 0,     0,       theta1; 
        0,   0,     d(2),    0; 
        0,   a(2),  0.0269,  theta2 - (pi/2);
        0,   a(3),  d(3),    theta3];

% transformation matrix for each link-end seperately
A1 = A(dh(1,1), dh(1,2), dh(1,3), dh(1,4)) * A(dh(2,1), dh(2,2), dh(2,3), dh(2,4));
A2 = A(dh(3,1), dh(3,2), dh(3,3), dh(3,4));
A3 = A(dh(4,1), dh(4,2), dh(4,3), dh(4,4));

% transformation chain
T1 = A1;
T2 = A1 * A2;
T3 = A1 * A2 * A3;

% transformation matrix for each link' COM seperately
Ac1 = A(dhcom(1,1), dhcom(1,2), dhcom(1,3), dhcom(1,4)) * A(dhcom(2,1), dhcom(2,2), dhcom(2,3), dhcom(2,4));
Ac2 = A(dhcom(3,1), dhcom(3,2), dhcom(3,3), dhcom(3,4));
Ac3 = A(dhcom(4,1), dhcom(4,2), dhcom(4,3), dhcom(4,4));

% transformation matrix chain for link COMs
Tc1 = Ac1;
Tc2 = A1 * Ac2;
Tc3 = A1 * A2 * Ac3;

% height of link COMs from reference plane
z0 = [0; 0; 1];
z1 = [Tc1(1,3); Tc1(2,3); Tc1(3,3)];
z2 = [Tc2(1,3); Tc2(2,3); Tc2(3,3)];

% coordinates of each link-end
o0 = [0; 0; 0];
o1 = [T1(1,4); T1(2,4); T1(3,4)];
o2 = [T2(1,4); T2(2,4); T2(3,4)];
o3 = [T3(1,4); T3(2,4); T3(3,4)];

% coordinates of each link-end
oc0 = [0; 0; 0];
oc1 = [Tc1(1,4); Tc1(2,4); Tc1(3,4)];
oc2 = [Tc2(1,4); Tc2(2,4); Tc2(3,4)];
oc3 = [Tc3(1,4); Tc3(2,4); Tc3(3,4)];

% jacobian matrix for linear velocity of each link
Jcv1 = zeros(3,3);
Jcv2 = [cross(z0, oc2 - o0), cross(z1, oc2 - o1), [0;0;0]];
Jcv3 = [cross(z0, oc3 - o0), cross(z1, oc3 - o1), cross(z2, oc3 - o2)];

% jacobian matrix for angular velocity of each link
Jcw1 = [z0, [0;0;0], [0;0;0]];
Jcw2 = [z0, z1, [0;0;0]];
Jcw3 = [z0, z1, z2];

% rotation matrices that transform link COMs to global coordinate frame
R1 = Tc1(1:3, 1:3);
R2 = Tc2(1:3, 1:3);
R3 = Tc3(1:3, 1:3);

% gravity vector
g = [0; 0; 9.81];

% generalized coordinate matrix
q = [q1(t); q2(t); q3(t)];
qdot = diff(q, t);

% potential energy
P_theta = (m(1) .* (g' * oc1)) + (m(2) .* (g' * oc2)) + (m(3) .* (g' * oc3));
P = subs(P_theta, [theta1, theta2, theta3], [q(1), q(2), q(3)]);

% gravity matrix
G = [diff(P_theta, theta1) ; diff(P_theta, theta2) ; diff(P_theta, theta3)];
G = subs(G, [theta1, theta2, theta3], [q(1), q(2), q(3)]);

% mass matrix
M_theta = (m(1) .* (Jcv1' * Jcv1)) + (m(2) .* (Jcv2' * Jcv2)) + (m(3) .* (Jcv3' * Jcv3)) + (Jcw1' * R1 * I1 * R1' * Jcw1) + (Jcw2' * R2 * I2 * R2' * Jcw2) + (Jcw3' * R3 * I3 * R3' * Jcw3);
M = subs(M_theta, [theta1, theta2, theta3], [q(1), q(2), q(3)]);

% total energy
TE = P + ((1/2) .* qdot' * M * qdot);
TE_ref = subs(TE, [q1(t), q2(t), q3(t)], [0, 0, 0]);

% centrepetal/coriolis matrix
C_theta = sym(zeros(3,3));
theta_vector = [theta1; theta2; theta3];
for i=1:3
    for j = 1:3
        for k = 1:3
            C_theta(i,j) = C_theta(i,j) + ((1/2) * (diff(M_theta(i,j), theta_vector(k)) + diff(M_theta(i,k), theta_vector(j)) - diff(M_theta(k,j), theta_vector(i))));
        end
    end
end
C = subs(C_theta, [theta1, theta2, theta3], [q(1), q(2), q(3)]);

% dynamics
X = [x1; x2; x3; x4; x5; x6];
u = [tau; 0; 0];
accel = inv(M) * (u - (C * qdot) - G);
accel = subs(accel, [q(1), q(2), q(3), qdot(1), qdot(2), qdot(3)], [X(1), X(2), X(3), X(4), X(5), X(6)]);

equil = sym(zeros(3,1));
for i = 1:3
    equil(i) = subs(accel(i), [X(1), X(2), X(3), X(4), X(5), X(6), tau], [0, 0, 0, 0, 0, 0, 0]);
end

op = [0;0;pi;0;0;0];
A_matrix = zeros(6,6);
for i = 1:3
    A_matrix(i, i+3) = 1;
    for j = 1:6
        A_matrix(i+3, j) = subs(diff(accel(i), X(j)), [X(1) X(2) X(3) X(4) X(5) X(6) tau], [op(1) op(2) op(3) op(4) op(5) op(6) 0]);
    end
end

B_matrix = zeros(6,1);
for i = 1:3
    B_matrix(i+3) = subs(diff(accel(i), tau), [X(1) X(2) X(3) X(4) X(5) X(6) tau], [op(1) op(2) op(3) op(4) op(5) op(6) 0]);
end

disp(A_matrix)
disp(B_matrix)