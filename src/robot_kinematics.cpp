//
// Created by yuta on 2023/09/02.
//
/*
     * The unit of length is [mm], and the unit of angle is [rad].
     * Field origin is set at the front-left corner on the top surface of the field
     * Robot origin is set at the point where the orientation axis of th0 and th1 cross(675, -130, 228)
     * The origin of hand coordinate is set at the point that hand rotation axis and the bottom surface of the endfactor_adapter cross.
     * Positive of field Y axle is set toward opponent.
     * Positive of field X axle is set toward right facing opponent.
     * Joints and links are numbered from the base (0,1,2)
     * Origin of Joint0 is set toward positive field Y axle
     * Origins of Joint1 and 2 are set toward positive field Z axle
     *
     *  o_____________y
     *  |
     *  |
     *  |
     * R|
     *  |
     *  |
     *  |
     *  |_____________
     *  x
     *
     *
     *       /\
     *      /  \
     *     /    \ l1
     * l0 /      \
     *   /        \  l2
     *  /          \_____
     *                |
     *                |l3
     *
     */

#include "robot_kinematics.h"

#include <cmath>

/*
 * void posrot_sum(float *posrot0, float *posrot1, float *posrot2){
    for(int i=0; i<6; i++){
        posrot0[i] = posrot1[i] + posrot[i]
    }

}
 */


robot_kinematics::robot_kinematics(){


    link_len[0] = 555;
    link_len[1] = 450;
    link_len[2] =  43;
    link_len[3] =  20;

/*
    joint_angle_now[0] = 0;
    joint_angle_now[1] = PI * 1/4;
    joint_angle_now[2] = PI * 3/4;


    joint_angle_trg[0] = joint_angle_now[0];
    joint_angle_trg[1] = joint_angle_now[1];
    joint_angle_trg[2] = joint_angle_now[2];

*/

    //lower limit                   upper limit
    joint_angle_lim[0][0]=0;    joint_angle_lim[0][1]=2*PI;
    joint_angle_lim[1][0]=0;    joint_angle_lim[1][1]=2*PI;
    joint_angle_lim[2][0]=0;    joint_angle_lim[2][1]=2*PI;
    joint_angle_lim[3][0]=0;    joint_angle_lim[3][1]=2*PI;


    robot_kinematics::forward_kinematics(&posrot_now, &joint_angle_now);
    robot_kinematics::forward_kinematics(&posrot_trg, &joint_angle_trg);


}


void robot_kinematics::convert_field2robot(float *f_posrot, float *r_posrot) {
    for(int i=0; i<6; i++){
        r_posrot[i] = f_posrot[i] - robot_pos[i];
    }
}

void robot_kinematics::forward_kinematics(float *posrot, float *joint_angle) {
    float lxy = link_len[0]*std::sin(joint_angle[1]) + link_len[1]*std::sin(joint_angle[2]) + link_len[2];

    posrot[X] = lxy*(-1)*std::sin(joint_angle[0]);
    posrot[Y] = lxy*std::cos(joint_angle[0]);
    posrot[Z] = link_len[0]*std::cos(joint_angle[1]) + link_len[1]*std::cos(joint_angle[2]) - link_len[3];
    posrot[PHI] = joint_angle[0] + joint_angle[1];
    posrot[THE] = 0;
    posrot[PSI] = 0;

}

void robot_kinematics::inverse_kinematics(float *f_posrot, float *joint_angle) {

    float _posrot[6];

    convert_field2robot(f_posrot, _posrot);

    //Recalculate the point where the tip of the l1 is reaching
    float lxy = std::sqrt(std::pow(_posrot[X],2) + std::pow(_posrot[Y],2)) - link_len[2];
    float _z  = _posrot[Z] + link_len[3];
    float r   = std::sqrt(std::pow(lxy,2) + std::pow(_z,2))



    joint_angle[0] = std::atan2(_posrot[X], _porot[Y]);

    joint_angle[1] = PI - std::atan2(lxy, _z)
            - std::acos(std::pow(link_len[0], 2) + std::pow(r, 2) - std::pow(link_len[1], 2) / (2 * link_len[0] * r) );

    joint_angle[2] = PI / 2
            + std::asin(link_len[0] / link_len[1] * std::sin( std::acos(std::pow(link_len[0], 2) + std::pow(r, 2) - std::pow(link_len[1], 2)) / (2 * link_len[0] * r) ) )
            - std::atan2(lxy, _z);

    joint_angle[3] = _posrot[PHI] - joint_angle[0];
}
