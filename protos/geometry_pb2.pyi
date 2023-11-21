from google.protobuf import wrappers_pb2 as _wrappers_pb2
from google.protobuf.internal import containers as _containers
from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from typing import ClassVar as _ClassVar, Iterable as _Iterable, Mapping as _Mapping, Optional as _Optional, Union as _Union

DESCRIPTOR: _descriptor.FileDescriptor

class Vec2(_message.Message):
    __slots__ = ["x", "y"]
    X_FIELD_NUMBER: _ClassVar[int]
    Y_FIELD_NUMBER: _ClassVar[int]
    x: float
    y: float
    def __init__(self, x: _Optional[float] = ..., y: _Optional[float] = ...) -> None: ...

class Vec3(_message.Message):
    __slots__ = ["x", "y", "z"]
    X_FIELD_NUMBER: _ClassVar[int]
    Y_FIELD_NUMBER: _ClassVar[int]
    Z_FIELD_NUMBER: _ClassVar[int]
    x: float
    y: float
    z: float
    def __init__(self, x: _Optional[float] = ..., y: _Optional[float] = ..., z: _Optional[float] = ...) -> None: ...

class CylindricalCoordinate(_message.Message):
    __slots__ = ["r", "theta", "z"]
    R_FIELD_NUMBER: _ClassVar[int]
    THETA_FIELD_NUMBER: _ClassVar[int]
    Z_FIELD_NUMBER: _ClassVar[int]
    r: float
    theta: float
    z: float
    def __init__(self, r: _Optional[float] = ..., theta: _Optional[float] = ..., z: _Optional[float] = ...) -> None: ...

class Quaternion(_message.Message):
    __slots__ = ["x", "y", "z", "w"]
    X_FIELD_NUMBER: _ClassVar[int]
    Y_FIELD_NUMBER: _ClassVar[int]
    Z_FIELD_NUMBER: _ClassVar[int]
    W_FIELD_NUMBER: _ClassVar[int]
    x: float
    y: float
    z: float
    w: float
    def __init__(self, x: _Optional[float] = ..., y: _Optional[float] = ..., z: _Optional[float] = ..., w: _Optional[float] = ...) -> None: ...

class Plane(_message.Message):
    __slots__ = ["point", "normal"]
    POINT_FIELD_NUMBER: _ClassVar[int]
    NORMAL_FIELD_NUMBER: _ClassVar[int]
    point: Vec3
    normal: Vec3
    def __init__(self, point: _Optional[_Union[Vec3, _Mapping]] = ..., normal: _Optional[_Union[Vec3, _Mapping]] = ...) -> None: ...

class Quad(_message.Message):
    __slots__ = ["pose", "size"]
    POSE_FIELD_NUMBER: _ClassVar[int]
    SIZE_FIELD_NUMBER: _ClassVar[int]
    pose: SE3Pose
    size: float
    def __init__(self, pose: _Optional[_Union[SE3Pose, _Mapping]] = ..., size: _Optional[float] = ...) -> None: ...

class Ray(_message.Message):
    __slots__ = ["origin", "direction"]
    ORIGIN_FIELD_NUMBER: _ClassVar[int]
    DIRECTION_FIELD_NUMBER: _ClassVar[int]
    origin: Vec3
    direction: Vec3
    def __init__(self, origin: _Optional[_Union[Vec3, _Mapping]] = ..., direction: _Optional[_Union[Vec3, _Mapping]] = ...) -> None: ...

class SE2Pose(_message.Message):
    __slots__ = ["position", "angle"]
    POSITION_FIELD_NUMBER: _ClassVar[int]
    ANGLE_FIELD_NUMBER: _ClassVar[int]
    position: Vec2
    angle: float
    def __init__(self, position: _Optional[_Union[Vec2, _Mapping]] = ..., angle: _Optional[float] = ...) -> None: ...

class SE2Velocity(_message.Message):
    __slots__ = ["linear", "angular"]
    LINEAR_FIELD_NUMBER: _ClassVar[int]
    ANGULAR_FIELD_NUMBER: _ClassVar[int]
    linear: Vec2
    angular: float
    def __init__(self, linear: _Optional[_Union[Vec2, _Mapping]] = ..., angular: _Optional[float] = ...) -> None: ...

class SE2VelocityLimit(_message.Message):
    __slots__ = ["max_vel", "min_vel"]
    MAX_VEL_FIELD_NUMBER: _ClassVar[int]
    MIN_VEL_FIELD_NUMBER: _ClassVar[int]
    max_vel: SE2Velocity
    min_vel: SE2Velocity
    def __init__(self, max_vel: _Optional[_Union[SE2Velocity, _Mapping]] = ..., min_vel: _Optional[_Union[SE2Velocity, _Mapping]] = ...) -> None: ...

class SE3Pose(_message.Message):
    __slots__ = ["position", "rotation"]
    POSITION_FIELD_NUMBER: _ClassVar[int]
    ROTATION_FIELD_NUMBER: _ClassVar[int]
    position: Vec3
    rotation: Quaternion
    def __init__(self, position: _Optional[_Union[Vec3, _Mapping]] = ..., rotation: _Optional[_Union[Quaternion, _Mapping]] = ...) -> None: ...

class SE3Velocity(_message.Message):
    __slots__ = ["linear", "angular"]
    LINEAR_FIELD_NUMBER: _ClassVar[int]
    ANGULAR_FIELD_NUMBER: _ClassVar[int]
    linear: Vec3
    angular: Vec3
    def __init__(self, linear: _Optional[_Union[Vec3, _Mapping]] = ..., angular: _Optional[_Union[Vec3, _Mapping]] = ...) -> None: ...

class Wrench(_message.Message):
    __slots__ = ["force", "torque"]
    FORCE_FIELD_NUMBER: _ClassVar[int]
    TORQUE_FIELD_NUMBER: _ClassVar[int]
    force: Vec3
    torque: Vec3
    def __init__(self, force: _Optional[_Union[Vec3, _Mapping]] = ..., torque: _Optional[_Union[Vec3, _Mapping]] = ...) -> None: ...

class FrameTreeSnapshot(_message.Message):
    __slots__ = ["child_to_parent_edge_map"]
    class ParentEdge(_message.Message):
        __slots__ = ["parent_frame_name", "parent_tform_child"]
        PARENT_FRAME_NAME_FIELD_NUMBER: _ClassVar[int]
        PARENT_TFORM_CHILD_FIELD_NUMBER: _ClassVar[int]
        parent_frame_name: str
        parent_tform_child: SE3Pose
        def __init__(self, parent_frame_name: _Optional[str] = ..., parent_tform_child: _Optional[_Union[SE3Pose, _Mapping]] = ...) -> None: ...
    class ChildToParentEdgeMapEntry(_message.Message):
        __slots__ = ["key", "value"]
        KEY_FIELD_NUMBER: _ClassVar[int]
        VALUE_FIELD_NUMBER: _ClassVar[int]
        key: str
        value: FrameTreeSnapshot.ParentEdge
        def __init__(self, key: _Optional[str] = ..., value: _Optional[_Union[FrameTreeSnapshot.ParentEdge, _Mapping]] = ...) -> None: ...
    CHILD_TO_PARENT_EDGE_MAP_FIELD_NUMBER: _ClassVar[int]
    child_to_parent_edge_map: _containers.MessageMap[str, FrameTreeSnapshot.ParentEdge]
    def __init__(self, child_to_parent_edge_map: _Optional[_Mapping[str, FrameTreeSnapshot.ParentEdge]] = ...) -> None: ...

class Box2(_message.Message):
    __slots__ = ["size"]
    SIZE_FIELD_NUMBER: _ClassVar[int]
    size: Vec2
    def __init__(self, size: _Optional[_Union[Vec2, _Mapping]] = ...) -> None: ...

class Box2WithFrame(_message.Message):
    __slots__ = ["box", "frame_name", "frame_name_tform_box"]
    BOX_FIELD_NUMBER: _ClassVar[int]
    FRAME_NAME_FIELD_NUMBER: _ClassVar[int]
    FRAME_NAME_TFORM_BOX_FIELD_NUMBER: _ClassVar[int]
    box: Box2
    frame_name: str
    frame_name_tform_box: SE3Pose
    def __init__(self, box: _Optional[_Union[Box2, _Mapping]] = ..., frame_name: _Optional[str] = ..., frame_name_tform_box: _Optional[_Union[SE3Pose, _Mapping]] = ...) -> None: ...

class Box3(_message.Message):
    __slots__ = ["size"]
    SIZE_FIELD_NUMBER: _ClassVar[int]
    size: Vec3
    def __init__(self, size: _Optional[_Union[Vec3, _Mapping]] = ...) -> None: ...

class Box3WithFrame(_message.Message):
    __slots__ = ["box", "frame_name", "frame_name_tform_box"]
    BOX_FIELD_NUMBER: _ClassVar[int]
    FRAME_NAME_FIELD_NUMBER: _ClassVar[int]
    FRAME_NAME_TFORM_BOX_FIELD_NUMBER: _ClassVar[int]
    box: Box3
    frame_name: str
    frame_name_tform_box: SE3Pose
    def __init__(self, box: _Optional[_Union[Box3, _Mapping]] = ..., frame_name: _Optional[str] = ..., frame_name_tform_box: _Optional[_Union[SE3Pose, _Mapping]] = ...) -> None: ...

class Matrix(_message.Message):
    __slots__ = ["rows", "cols", "values"]
    ROWS_FIELD_NUMBER: _ClassVar[int]
    COLS_FIELD_NUMBER: _ClassVar[int]
    VALUES_FIELD_NUMBER: _ClassVar[int]
    rows: int
    cols: int
    values: _containers.RepeatedScalarFieldContainer[float]
    def __init__(self, rows: _Optional[int] = ..., cols: _Optional[int] = ..., values: _Optional[_Iterable[float]] = ...) -> None: ...

class SE3Covariance(_message.Message):
    __slots__ = ["matrix", "yaw_variance", "cov_xx", "cov_xy", "cov_xz", "cov_yx", "cov_yy", "cov_yz", "cov_zx", "cov_zy", "cov_zz"]
    MATRIX_FIELD_NUMBER: _ClassVar[int]
    YAW_VARIANCE_FIELD_NUMBER: _ClassVar[int]
    COV_XX_FIELD_NUMBER: _ClassVar[int]
    COV_XY_FIELD_NUMBER: _ClassVar[int]
    COV_XZ_FIELD_NUMBER: _ClassVar[int]
    COV_YX_FIELD_NUMBER: _ClassVar[int]
    COV_YY_FIELD_NUMBER: _ClassVar[int]
    COV_YZ_FIELD_NUMBER: _ClassVar[int]
    COV_ZX_FIELD_NUMBER: _ClassVar[int]
    COV_ZY_FIELD_NUMBER: _ClassVar[int]
    COV_ZZ_FIELD_NUMBER: _ClassVar[int]
    matrix: Matrix
    yaw_variance: float
    cov_xx: float
    cov_xy: float
    cov_xz: float
    cov_yx: float
    cov_yy: float
    cov_yz: float
    cov_zx: float
    cov_zy: float
    cov_zz: float
    def __init__(self, matrix: _Optional[_Union[Matrix, _Mapping]] = ..., yaw_variance: _Optional[float] = ..., cov_xx: _Optional[float] = ..., cov_xy: _Optional[float] = ..., cov_xz: _Optional[float] = ..., cov_yx: _Optional[float] = ..., cov_yy: _Optional[float] = ..., cov_yz: _Optional[float] = ..., cov_zx: _Optional[float] = ..., cov_zy: _Optional[float] = ..., cov_zz: _Optional[float] = ...) -> None: ...

class PolyLine(_message.Message):
    __slots__ = ["points"]
    POINTS_FIELD_NUMBER: _ClassVar[int]
    points: _containers.RepeatedCompositeFieldContainer[Vec2]
    def __init__(self, points: _Optional[_Iterable[_Union[Vec2, _Mapping]]] = ...) -> None: ...

class Polygon(_message.Message):
    __slots__ = ["vertexes"]
    VERTEXES_FIELD_NUMBER: _ClassVar[int]
    vertexes: _containers.RepeatedCompositeFieldContainer[Vec2]
    def __init__(self, vertexes: _Optional[_Iterable[_Union[Vec2, _Mapping]]] = ...) -> None: ...

class PolygonWithExclusions(_message.Message):
    __slots__ = ["inclusion", "exclusions"]
    INCLUSION_FIELD_NUMBER: _ClassVar[int]
    EXCLUSIONS_FIELD_NUMBER: _ClassVar[int]
    inclusion: Polygon
    exclusions: _containers.RepeatedCompositeFieldContainer[Polygon]
    def __init__(self, inclusion: _Optional[_Union[Polygon, _Mapping]] = ..., exclusions: _Optional[_Iterable[_Union[Polygon, _Mapping]]] = ...) -> None: ...

class Circle(_message.Message):
    __slots__ = ["center_pt", "radius"]
    CENTER_PT_FIELD_NUMBER: _ClassVar[int]
    RADIUS_FIELD_NUMBER: _ClassVar[int]
    center_pt: Vec2
    radius: float
    def __init__(self, center_pt: _Optional[_Union[Vec2, _Mapping]] = ..., radius: _Optional[float] = ...) -> None: ...

class Area(_message.Message):
    __slots__ = ["polygon", "circle"]
    POLYGON_FIELD_NUMBER: _ClassVar[int]
    CIRCLE_FIELD_NUMBER: _ClassVar[int]
    polygon: Polygon
    circle: Circle
    def __init__(self, polygon: _Optional[_Union[Polygon, _Mapping]] = ..., circle: _Optional[_Union[Circle, _Mapping]] = ...) -> None: ...

class Volume(_message.Message):
    __slots__ = ["box"]
    BOX_FIELD_NUMBER: _ClassVar[int]
    box: Vec3
    def __init__(self, box: _Optional[_Union[Vec3, _Mapping]] = ...) -> None: ...

class Bounds(_message.Message):
    __slots__ = ["lower", "upper"]
    LOWER_FIELD_NUMBER: _ClassVar[int]
    UPPER_FIELD_NUMBER: _ClassVar[int]
    lower: float
    upper: float
    def __init__(self, lower: _Optional[float] = ..., upper: _Optional[float] = ...) -> None: ...

class Vec2Value(_message.Message):
    __slots__ = ["x", "y"]
    X_FIELD_NUMBER: _ClassVar[int]
    Y_FIELD_NUMBER: _ClassVar[int]
    x: _wrappers_pb2.DoubleValue
    y: _wrappers_pb2.DoubleValue
    def __init__(self, x: _Optional[_Union[_wrappers_pb2.DoubleValue, _Mapping]] = ..., y: _Optional[_Union[_wrappers_pb2.DoubleValue, _Mapping]] = ...) -> None: ...

class Vec3Value(_message.Message):
    __slots__ = ["x", "y", "z"]
    X_FIELD_NUMBER: _ClassVar[int]
    Y_FIELD_NUMBER: _ClassVar[int]
    Z_FIELD_NUMBER: _ClassVar[int]
    x: _wrappers_pb2.DoubleValue
    y: _wrappers_pb2.DoubleValue
    z: _wrappers_pb2.DoubleValue
    def __init__(self, x: _Optional[_Union[_wrappers_pb2.DoubleValue, _Mapping]] = ..., y: _Optional[_Union[_wrappers_pb2.DoubleValue, _Mapping]] = ..., z: _Optional[_Union[_wrappers_pb2.DoubleValue, _Mapping]] = ...) -> None: ...

class JointArray(_message.Message):
    __slots__ = ["data"]
    DATA_FIELD_NUMBER: _ClassVar[int]
    data: _containers.RepeatedScalarFieldContainer[float]
    def __init__(self, data: _Optional[_Iterable[float]] = ...) -> None: ...
