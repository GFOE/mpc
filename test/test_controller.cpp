#include <gtest/gtest.h>
//#include "controller.h"
#include "../src/controller.h"
using std::vector;
using std::pair;
using std::cerr;
using std::endl;

TEST(ControllerUnitTests, goNowhere)
{
    Controller controller(nullptr);
    State start(0,0,0,0,6);
    vector<State> reference;
    reference.emplace_back(0,0,0,0,7);
    double r, t;
    controller.mpc(r, t, start, reference, Controller::getTime() + 0.25);
//    EXPECT_DOUBLE_EQ(r, 0.0);
    EXPECT_DOUBLE_EQ(t, 0.0);
}

TEST(ControllerUnitTests, goNorth)
{
    Controller controller(nullptr);
    State start(0,0,0,0,Controller::getTime());
    vector<State> reference;
    double r, t;
    reference.emplace_back(0,0.02,0,0.1,Controller::getTime() + 1);
    reference.emplace_back(0,3.5,0,2,Controller::getTime() + 2);
    controller.mpc(r, t, start, reference, Controller::getTime() + 0.05);
    EXPECT_DOUBLE_EQ(r, 0.0);
    EXPECT_DOUBLE_EQ(t, 1.0);
}

TEST(ControllerUnitTests, goEast)
{
    Controller controller(nullptr);
    State start(0,0,0,0,Controller::getTime());
    vector<State> reference;
    double r, t;
    reference.emplace_back(1,0,M_PI/2,2.5,Controller::getTime() + 1);
    reference.emplace_back(3.5,0,M_PI/2,2.5,Controller::getTime() + 2);
    controller.mpc(r, t, start, reference, Controller::getTime() + 0.25);
    EXPECT_DOUBLE_EQ(r, 1);
    EXPECT_DOUBLE_EQ(t, 1.0);
}

TEST(ControllerUnitTests, goWest)
{
    Controller controller(nullptr);
    State start(0,0,0,0,Controller::getTime());
    vector<State> reference;
    double r, t;
    reference.emplace_back(-1,0,-M_PI/2,2.5,Controller::getTime() + 1);
    reference.emplace_back(-3.5,0,-M_PI/2,2.5,Controller::getTime() + 2);
    controller.mpc(r, t, start, reference, Controller::getTime() + 0.25);
    EXPECT_DOUBLE_EQ(r, -1);
    EXPECT_DOUBLE_EQ(t, 1.0);
}

TEST(ControllerUnitTests, realStateTest1)
{
    Controller controller(nullptr);
    VehicleState start(State(0,0,2,2.3,7));
    vector<State> reference;
    double r, t;
    reference.push_back(start.estimate(0.8, 0.75, 5, pair<double,double>(0,0)));
    controller.mpc(r, t, start, reference, Controller::getTime() + 0.25);
    EXPECT_LT(fabs(r - 0.8), 0.001);
    EXPECT_LT(fabs(t - 0.75), 0.001);
}

TEST(ControllerUnitTests, trajectoryDepthTest1)
{
    Controller controller(nullptr);
    VehicleState start(State(0,0,2,2.3,7));
    vector<State> reference;
    double r, t;
    auto vs = start.estimate(0, 1, 0.499, pair<double,double>(0,0));
    vs.time -= 0.2;
    reference.push_back(vs);
    vs.time += 0.2;
    reference.push_back(vs);
    controller.mpc(r, t, start, reference, Controller::getTime() + 0.25);
    EXPECT_LT(fabs(r), 0.001);
    EXPECT_LT(fabs(t - 1), 0.001);
}

TEST(ControllerUnitTests, turnAroundTest1)
{
    Controller controller(nullptr);
    VehicleState start(State(0,0,0,1,7.5));
    vector<State> reference;
    double r, t;
    reference.emplace_back(0, -2, M_PI, 2, 8);
    reference.emplace_back(0, -4, M_PI, 2, 9);
    reference.emplace_back(0, -6, M_PI, 2, 10);
    reference.emplace_back(0, -8, M_PI, 2, 11);
    reference.emplace_back(0, -10, M_PI, 2, 12);
    controller.mpc(r, t, start, reference, Controller::getTime() + 0.25);
    EXPECT_DOUBLE_EQ(fabs(r), 1);
    EXPECT_LT(t, 0.3);
}

TEST(ControllerUnitTests, futureEstimateTest1)
{
    Controller controller(nullptr);
    VehicleState start(State(0,0,0,0,4));
    vector<State> reference;
    double r, t;
    auto s1 = start.estimate(0.8, 1, 0.75, pair<double,double>(0,0));
    auto s2 = s1.estimate(0.8, 1, 0.75, pair<double,double>(0,0));
    auto s3 = s2.estimate(0.8, 1, 0.75, pair<double,double>(0,0));
    reference.push_back(s1);
    reference.push_back(s2);
    reference.push_back(s3);
    controller.mpc(r, t, start, reference, Controller::getTime() + 0.25);
    EXPECT_DOUBLE_EQ(r, 0.8);
    EXPECT_DOUBLE_EQ(t, 1);
    auto r1 = controller.estimateStateInFuture(4.75);
    EXPECT_EQ(r1.toString(), s1.toString());
}

TEST(ControllerUnitTests, futureEstimateTest2)
{
    Controller controller(nullptr);
    VehicleState start(State(0,0,0,0,4));
    vector<State> reference;
    double r, t;
    auto s1 = start.estimate(0.8, 1, 0.75, pair<double,double>(0,0));
    auto s2 = s1.estimate(0.8, 1, 0.75, pair<double,double>(0,0));
    auto s3 = s2.estimate(0.8, 1, 0.75, pair<double,double>(0,0));
    reference.push_back(s1);
    reference.push_back(s2);
    reference.push_back(s3);
    controller.mpc(r, t, start, reference, Controller::getTime() + 0.25);
    EXPECT_DOUBLE_EQ(r, 0.8);
    EXPECT_DOUBLE_EQ(t, 1);
    auto r1 = controller.estimateStateInFuture(5);
    auto r2 = start.estimate(0.8, 1, 1, pair<double,double>(0,0));
    EXPECT_DOUBLE_EQ(r1.time, r2.time);
    EXPECT_DOUBLE_EQ(r1.x, r2.x);
    EXPECT_DOUBLE_EQ(r1.y, r2.y);
    EXPECT_DOUBLE_EQ(r1.heading, r2.heading);
    EXPECT_DOUBLE_EQ(r1.speed, r2.speed);
}

TEST(CurrentEstimatorTests, currentEstimatorTest1)
{
    CurrentEstimator currentEstimator;
    vector<State> reference;
    reference.emplace_back(0,0,0,0,1.05);
    currentEstimator.updateEstimate(State(0,0,0,0,1), reference);
    pair<double,double> p(0,0);
    EXPECT_LT(fabs(currentEstimator.getCurrent().second - p.second), 0.001);
    currentEstimator.updateEstimate(State(0,0.1,0,0,1.05), reference);
    p.second = 0.1 / 50 / 0.05;
    // floating point math makes me do this instead of equality comparison
    EXPECT_LT(fabs(currentEstimator.getCurrent().second - p.second), 0.001);
}

TEST(VehicleStateTests, estimateTest0)
{
    VehicleState s1(State(0,0,2,2.3,7));
    auto s2 = s1.estimate(0, 1, 3, pair<double,double>(0,0));
    auto s3 = s1.estimate(0, 1, 3, pair<double,double>(0,0));
    EXPECT_DOUBLE_EQ(s2.time, s3.time);
    EXPECT_DOUBLE_EQ(s2.x, s3.x);
    EXPECT_DOUBLE_EQ(s2.y, s3.y);
    EXPECT_DOUBLE_EQ(s2.heading, s3.heading);
    EXPECT_DOUBLE_EQ(s2.speed, s3.speed);
}

TEST(VehicleStateTests, estimateTest1)
{
    VehicleState s1(State(0,0,2,2.3,7));
    auto sFinal = s1.estimate(0, 1, 5, pair<double,double>(0,0));
    auto s2 = s1.estimate(0, 1, 2, pair<double,double>(0,0));
    auto s3 = s2.estimate(0, 1, 3, pair<double,double>(0,0));
    EXPECT_DOUBLE_EQ(sFinal.time, s3.time);
    EXPECT_DOUBLE_EQ(sFinal.x, s3.x);
    EXPECT_DOUBLE_EQ(sFinal.y, s3.y);
    EXPECT_DOUBLE_EQ(sFinal.heading, s3.heading);
    EXPECT_DOUBLE_EQ(sFinal.speed, s3.speed);
}

TEST(VehicleStateTests, estimateTest2)
{
    // This test fails. I guess the model is that bad
    VehicleState start(State(0,0,0,0,4));
    auto s1 = start.estimate(0.8, 1, 0.75, pair<double,double>(0,0));
    auto s2 = s1.estimate(0.8, 1, 0.25, pair<double,double>(0,0));
    auto s3 = start.estimate(0.8, 1, 1, pair<double,double>(0,0));
    EXPECT_DOUBLE_EQ(s2.time, s3.time);
    EXPECT_DOUBLE_EQ(s2.x, s3.x);
    EXPECT_DOUBLE_EQ(s2.y, s3.y);
    EXPECT_DOUBLE_EQ(s2.heading, s3.heading);
    EXPECT_DOUBLE_EQ(s2.speed, s3.speed);
}

int main(int argc, char **argv){
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}