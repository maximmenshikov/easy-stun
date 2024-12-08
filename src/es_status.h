#pragma once

typedef enum es_status {
	ES_EOK,
	ES_EPARAMMISSING,
	ES_EPARAMINVALID,
	ES_EPARAMINSUFFICIENT,
	ES_ECONNFAIL,
	ES_EDNSFAIL,
	ES_ESENDFAIL,
	ES_ESKCREATEFAIL,
	ES_ESKBINDFAIL,
	ES_ESKCLOEXECFAIL,
	ES_ESKNONBLOCKFAIL,
} es_status;