import pathlib
import unittest


ROOT = pathlib.Path(__file__).resolve().parents[1]


def read_text(relative_path):
    return (ROOT / relative_path).read_text(encoding="utf-8")


def load_yaml(relative_path):
    import yaml

    return yaml.safe_load(read_text(relative_path))


class HttpContractStaticTests(unittest.TestCase):
    def test_cmake_prefers_available_qt6_prefix_before_qt5_fallback(self):
        cmake = read_text("CMakeLists.txt")

        self.assertIn("ROCOS_VIZ_QT_PREFIX_CANDIDATES", cmake)
        self.assertIn("/opt/Qt/6.6.2/gcc_64", cmake)
        self.assertIn("find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets Network Charts REQUIRED)", cmake)

    def test_http_client_uses_qt_async_networking_not_sync_httplib(self):
        client_h = read_text("src/HttpRobotClient.h")
        client_cpp = read_text("src/HttpRobotClient.cpp")
        combined = client_h + client_cpp

        self.assertIn("QNetworkAccessManager", combined)
        self.assertIn("&QNetworkReply::finished", combined)
        self.assertNotIn("httplib", combined)
        self.assertNotRegex(combined, r"\.Get\s*\(")
        self.assertNotRegex(combined, r"\.Post\s*\(")

    def test_http_client_does_not_access_self_after_user_json_callback(self):
        client_h = read_text("src/HttpRobotClient.h")
        client_cpp = read_text("src/HttpRobotClient.cpp")

        self.assertIn("struct JsonResult", client_h)
        self.assertIn("JsonResult parseStandardResponse", client_h)
        self.assertIn("reply->deleteLater();\n    callback(result.ok, result.data, result.error);", client_cpp)
        self.assertIn("reply->isOpen() ? reply->readAll() : QByteArray()", client_cpp)

    def test_robot_info_accepts_minimal_joint_info_fields(self):
        source = read_text("src/ConnectDialog.cpp")

        self.assertIn('!joint.value("name").isString()', source)
        self.assertNotIn('!joint.value("cnt_per_unit").isDouble()', source)
        self.assertNotIn('!joint.value("torque_per_unit").isDouble()', source)
        self.assertNotIn('!joint.value("ratio").isDouble()', source)
        self.assertNotIn('!joint.value("pos_zero_offset").isDouble()', source)
        self.assertNotIn('!joint.value("user_unit_name").isString()', source)
        self.assertIn('intValue(joint, "zero_offset"', source)
        self.assertIn('stringValue(joint, "unit_name"', source)

    def test_move_response_task_id_is_saved_and_status_can_be_polled(self):
        header = read_text("src/ConnectDialog.h")
        source = read_text("src/ConnectDialog.cpp")
        combined = header + source

        self.assertIn("last_move_task_id_", combined)
        self.assertIn("last_move_status_", combined)
        self.assertIn("queryMoveStatus", combined)
        self.assertIn("/api/move/status", combined)
        self.assertRegex(source, r"task_id")
        self.assertRegex(source, r"result")

    def test_robot_state_accepts_optional_telemetry_fields(self):
        source = read_text("src/ConnectDialog.cpp")

        self.assertIn('orientation.value("w").isDouble()', source)
        self.assertIn('!joint.value("name").isString() || !joint.value("position").isDouble()', source)
        self.assertNotIn('!joint.value("velocity").isDouble()', source)
        self.assertNotIn('!joint.value("acceleration").isDouble()', source)
        self.assertNotIn('!joint.value("load").isDouble()', source)
        self.assertNotIn('!joint.value("status").isString()', source)
        self.assertNotIn('!hardware.value("cycle_time_avg").isDouble()', source)
        self.assertIn('data.value("flange")', source)
        self.assertIn('data.value("tool")', source)
        self.assertIn('data.value("object")', source)
        self.assertIn('data.value("hw_state")', source)
        self.assertIn('numberValue(joint, "torque")', source)
        self.assertIn('numberValue(joint, "load_torque")', source)
        self.assertIn('statusName(joint.value("status"))', source)
        self.assertIn('hardwareTypeName(hardware.value("hw_type"))', source)
        self.assertIn('numberValue(hardware, "current_cycle_time")', source)
        self.assertIn('numberValue(hardware, "min_cycle_time")', source)

    def test_robot_state_read_timeout_allows_ui_thread_jitter(self):
        source = read_text("src/ConnectDialog.cpp")

        self.assertNotIn("setTimeouts(1000, 50, 500)", source)
        self.assertRegex(source, r"setTimeouts\(1000,\s*(?:500|1000|1500|2000),\s*500\)")

    def test_openapi_robot_state_matches_current_controller_schema(self):
        spec = read_text("config/rocos-API.yaml")

        for field in [
            "load_torque",
            "torque",
            "status: { type: integer",
            "flange:",
            "tool:",
            "object:",
            "hw_state:",
            "hw_type",
            "current_cycle_time",
            "min_cycle_time",
            "max_cycle_time",
            "slave_num",
        ]:
            self.assertIn(field, spec)

    def test_openapi_robot_info_matches_current_controller_schema(self):
        spec = read_text("config/rocos-API.yaml")

        self.assertIn("unit_name", spec)
        self.assertIn("zero_offset", spec)

    def test_openapi_get_data_schemas_are_explicit(self):
        spec = load_yaml("config/rocos-API.yaml")
        schemas = spec["components"]["schemas"]

        for schema_name in [
            "RobotEnabledData",
            "RobotInfoData",
            "RobotStateData",
            "RobotModelData",
            "CalibrationResultData",
        ]:
            self.assertIn(schema_name, schemas)

        model_response = spec["paths"]["/api/robot/model"]["get"]["responses"]["200"]["content"]["application/json"]["schema"]
        enabled_response = spec["paths"]["/api/robot/enabled"]["get"]["responses"]["200"]["content"]["application/json"]["schema"]
        calibration_response = spec["paths"]["/api/calibration/result"]["get"]["responses"]["200"]["content"]["application/json"]["schema"]

        self.assertEqual(model_response["allOf"][1]["properties"]["data"]["$ref"], "#/components/schemas/RobotModelData")
        self.assertEqual(enabled_response["allOf"][1]["properties"]["data"]["$ref"], "#/components/schemas/RobotEnabledData")
        self.assertEqual(calibration_response["allOf"][1]["properties"]["data"]["$ref"], "#/components/schemas/CalibrationResultData")

    def test_openapi_matches_simulation_edge_response_shapes(self):
        spec = load_yaml("config/rocos-API.yaml")
        schemas = spec["components"]["schemas"]

        move_data = schemas["MoveResponse"]["allOf"][1]["properties"]["data"]
        async_move = move_data["oneOf"][0]
        nullable_position = schemas["NullableVector3"]["properties"]

        self.assertTrue(move_data["nullable"])
        self.assertEqual(async_move["required"], ["task_id"])
        self.assertTrue(nullable_position["x"]["nullable"])
        self.assertTrue(nullable_position["y"]["nullable"])
        self.assertTrue(nullable_position["z"]["nullable"])

    def test_api_curl_response_record_covers_every_configured_path(self):
        spec = load_yaml("config/rocos-API.yaml")
        record = read_text("docs/API-curl-response-record.md")

        for path in spec["paths"]:
            self.assertIn(path, record)

        self.assertIn("curl --location 'http://127.0.0.1:8080/api/robot/state'", record)
        self.assertIn("Simulation-mode POST checks were executed against the live controller", record)

    def test_model_download_streams_to_file_with_dedicated_timeout(self):
        client_h = read_text("src/HttpRobotClient.h")
        client_cpp = read_text("src/HttpRobotClient.cpp")
        header = read_text("src/ConnectDialog.h")
        source = read_text("src/ConnectDialog.cpp")

        self.assertIn("status != 200", client_cpp)
        self.assertIn("Qt::CaseInsensitive", source)
        self.assertIn("FileCallback", client_h)
        self.assertIn("downloadFile", client_h)
        self.assertIn("binary_timeout_ms_", client_h)
        self.assertIn("QTemporaryFile", client_cpp)
        self.assertIn("&QNetworkReply::readyRead", client_cpp)
        self.assertNotIn("mesh_body", header)
        self.assertTrue(
            "std::thread" in source or "QtConcurrent::run" in source,
            "mesh/config file writes should happen away from the UI callback",
        )
        self.assertIn("writeModelFiles", source)

    def test_model_loader_reports_each_stl_path_and_vtk_read_counts(self):
        model_cpp = read_text("src/Model.cpp")
        link_cpp = read_text("src/Link.cpp")

        self.assertIn("mesh_path", model_cpp)
        self.assertIn("GetNumberOfPoints", link_cpp)
        self.assertIn("GetNumberOfCells", link_cpp)
        self.assertIn("Failed to load mesh", link_cpp)
        self.assertIn("Loaded mesh", link_cpp)


if __name__ == "__main__":
    unittest.main()
