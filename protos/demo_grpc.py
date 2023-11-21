import robot_service_pb2_grpc
import robot_service_pb2
import grpc
import robot_info_pb2
import robot_command_pb2
import geometry_pb2
import robot_state_pb2
import PyKDL as kdl

# 全局变量


channel=grpc.insecure_channel('localhost:30001')

stub = robot_service_pb2_grpc.RobotServiceStub(channel)
response=stub.ReadRobotState(robot_info_pb2.RobotInfoRequest())
response1=stub.ReadRobotInfo(robot_info_pb2.RobotInfoRequest())

# response = stub.Connect(robot_info_pb2.ConnectRequest())
print("Greeter client received: " + str(response1.robot_info.joint_infos[0].cnt_per_unit))



   
def moveJ(q):
    # Assuming stub is your gRPC client stub
    request = robot_command_pb2.RobotCommandRequest()
    movej = request.command.motion_command.move_j
    for angle in q:
        print(f"q: {angle}")
        movej.q.data.append(angle)
    movej.speed = 0.5
    movej.acceleration = 0.5
    movej.time = 0
    movej.radius = 0
    movej.asynchronous = True
    status=stub.WriteRobotCommmand(request)
    if status:
        print("Send command Ok")
        pass
    else:
        print("Send command Error")

def getjointPosition(id):
    response=stub.ReadRobotState(robot_state_pb2.RobotStateRequest())
    if response:
        print("Get joint position Ok")
        return response.robot_state.joint_states[id].position
    else:
        print("Get joint position Error")
        return None
def movel(pose):
    # Assuming stub is your gRPC client stub
    request = robot_command_pb2.RobotCommandRequest()
    movel = request.command.motion_command.move_l
    movel.pose.CopyFrom(pose)
    movel.speed = 0.5
    movel.acceleration = 0.5
    movel.time = 0
    movel.radius = 0
    movel.asynchronous = True
    status=stub.WriteRobotCommmand(request)
    if status:
        print("Send command Ok")
        pass
    else:
        print("Send command Error")
def getFlangePose():
    response=stub.ReadRobotState(robot_state_pb2.RobotStateRequest())
    if response:
        print("Get joint position Ok")
        return response.robot_state.tool_states[0].pose
    else:
        print("Get joint position Error")
        return None



if __name__ == '__main__':


    q=[0,1.5707,0,0,0,0,0] 
    moveJ(q)
    print(getjointPosition(1))


   
