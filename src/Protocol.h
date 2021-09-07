#ifndef PROTOCOL_H
#define PROTOCOL_H

#define ROBOT_ENABLE    "tv80!"
#define ROBOT_DISABLE   "tv112!"

#define LUA_START(script)       "tv99!p"(script)"!" //  ->"/hanbing/app/Line0.MOV"
#define LUA_STOP        "tv100!"
#define LUA_PAUSE       "tv115!"
#define LUA_CONTINUE    "tv103!"

#define RECORD_JOINT(J_NAME)  "tv111!d"(J_NAME)"!"
#define RECORD_POSE(P_NAME)   "tv110!d"(P_NAME)"!"

#define J1_P          "tv49!"
#define J1_N          "tv113!"
#define J1_Z          "tv33!"

#define J2_P          "tv50!"
#define J2_N          "tv119!"
#define J2_Z          "tv64!"

#define J3_P          "tv51!"
#define J3_N          "tv101!"
#define J3_Z          "tv35!"

#define J4_P          "tv52!"
#define J4_N          "tv114!"
#define J4_Z          "tv36!"

#define J5_P          "tv53!"
#define J5_N          "tv116!"
#define J5_Z          "tv37!"

#define J6_P          "tv54!"
#define J6_N          "tv121!"
#define J6_Z          "tv94!"

#define J7_P          "tv55!"
#define J7_N          "tv117!"
#define J7_Z          "tv38!"

#define CX_P          "tv249!"
#define CX_N          "tv313!"
#define CX_Z          "tv233!"

#define CY_P          "tv250!"
#define CY_N          "tv319!"
#define CY_Z          "tv264!"

#define CZ_P          "tv251!"
#define CZ_N          "tv301!"
#define CZ_Z          "tv235!"

#define CK_P          "tv252!"
#define CK_N          "tv314!"
#define CK_Z          "tv236!"

#define CP_P          "tv253!"
#define CP_N          "tv316!"
#define CP_Z          "tv237!"

#define CS_P          "tv254!"
#define CS_N          "tv321!"
#define CS_Z          "tv294!"

#define CR_P          "tv255!"
#define CR_N          "tv317!"
#define CR_Z          "tv238!"

#define TX_P          "tv349!"
#define TX_N          "tv413!"
#define TX_Z          "tv333!"

#define TY_P          "tv350!"
#define TY_N          "tv419!"
#define TY_Z          "tv364!"

#define TZ_P          "tv351!"
#define TZ_N          "tv401!"
#define TZ_Z          "tv335!"

#define TK_P          "tv352!"
#define TK_N          "tv414!"
#define TK_Z          "tv336!"

#define TP_P          "tv353!"
#define TP_N          "tv416!"
#define TP_Z          "tv337!"

#define TS_P          "tv354!"
#define TS_N          "tv421!"
#define TS_Z          "tv394!"

#define TR_P          "tv355!"
#define TR_N          "tv417!"
#define TR_Z          "tv338!"

#define SET_JNTSPD(S)  "sc1!v"(S)"!" //S是百分比，默认25%
#define SET_CARTSPD(S) "sc2!v"(S)"!"
#define SET_TOOLSPD(S) "sc3!v"(S)"!"

#define GET_JNTSPD  "gc1!"
#define GET_CARTSPD "gc2!"
#define GET_TOOLSPD "gc3!"

#endif // PROTOCOL_H
